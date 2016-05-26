/**
 *	base/gcs_parser.c
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */

#include "features.h"
#include "list.h"
#include "native.h"
#include "typedef.h"
#include "gcs_parser.h"
#include "gcs_errno.h"
#include "gcs_smif.h"
#include "gcs_config.h"

#ifndef lint
static const char rcsid[] _U_ =
	"@(#) $Id: base/gcs_parser.c,"
	"v 1.00 2013/10/28 17:12:40 yangling Exp $ (LBL)";
#endif

#define	LF_CHAR	'\n'

#define	IS_LF_CHAR(_c_)	((_c_) == LF_CHAR)

/**
 *	分隔字符掩码表
 */
static unsigned int sep_map[8] ={0, 0, 0, 0, 0, 0, 0, 0};

/**
 *	是分隔字符吗? TRUE - 是 FALSE - 否
 */
#define	is_separate_char(c)	\
			(IS_SET_ID(sep_map, ((unsigned char)(c))))

struct virtual_stack {
	char				*base;		/*	栈基指针*/
	size_t				size;		/*	栈的大小*/
};
typedef struct virtual_stack stack_t;

/**
 *	初始化虚拟栈
 */
#define	virtual_stack_init(_vs_)	\
	do {memset((_vs_), 0, sizeof(stack_t));}while(0)

/**
 *	TRUE - (非空栈) FALSE - (空栈)
 */
#define	virtual_stack_done(_vs_)	\
		((_vs_->size) ? 1 : 0)

/**
 *	复制虚拟栈的数据到单词描述符
 */
#define	virtual_stack_copy(_wp_, _vs_)		\
	do {									\
		(_wp_)->word = (_vs_)->base;		\
		(_wp_)->size = (_vs_)->size;		\
		virtual_stack_init((_vs_));			\
	}while(0)

/**
 *	虚拟栈压栈
 */
#define	virtual_stack_push(_vs_, _ptr_)		\
	do {									\
		if((_vs_)->size == 0)				\
			(_vs_)->base = (char*)(_ptr_);	\
		(_vs_)->size++;						\
	}while(0)


/**
 *	是结束单词吗? TRUE - 是 FALSE - 否
 */
#define	IS_EOS(_wp_) \
	((_wp_)->size == 1 && *((_wp_)->word) == LF_CHAR)


#define	line_parser_reset(_wtab_)			\
	do {									\
		list_splice(&((_wtab_)->used),		\
					&((_wtab_)->free));		\
		INIT_LIST_HEAD(&((_wtab_)->used));	\
	}while(0)

#define	line_parser_startup	line_parser_reset
#define	line_parser_cleanup	line_parser_reset

struct lr_action {
	char		*action;
	size_t		length;
	int			(*fn_ptr)(IN struct word_table *wtab,
						IN struct http_parser *parser);
};

#define	status_switch_to(_parser_, _status_) \
	do {(_parser_)->status = (_status_);}while(0)

#define	http_parser_relay(_parser_)					\
	do {											\
		(_parser_)->action = (unsigned short)-1;	\
		(_parser_)->length = 0;						\
	}while(0)

static int
do_connect_action(
	IN struct word_table *,
	IN struct http_parser *);

static int
do_channo_action(
	IN struct word_table *,
	IN struct http_parser *);

static int
do_unitno_action(
	IN struct word_table *,
	IN struct http_parser *);

static int
do_tunnel_action(
	IN struct word_table *,
	IN struct http_parser *);

static int
do_length_action(
	IN struct word_table *,
	IN struct http_parser *);

static int
do_gctype_action(
	IN struct word_table *,
	IN struct http_parser *);

static struct lr_action request_line[] =
{
	{"Connect",			0, do_connect_action},
};

static struct lr_action headers_line[] = 
{
	{"GCCHNO",			0, do_channo_action},
	{"GCUNNO",			0, do_unitno_action},
	{"GCLNKNO",			0, do_tunnel_action},
	{"Content-Length",	0, do_length_action},
	{"GCType",			0, do_gctype_action},
};

/**
 *	gen_separate_map - 创建分割符掩码表
 *
 *	return
 *		无
 */
static void
gen_separate_map()
{
	int	i, len;
	unsigned char *seps = (unsigned char*)"\r :";

	len = strlen((char*)seps);
	for(i = 0; i < len; i++)
		SET_ID(sep_map, seps[i]);
}

/**
 *	lr_action_compare - 比较函数
 *
 *	@elem1:		参数一
 *	@elem2:		参数二
 *
 *	>	0		参数一大于参数二
 *	==	0		参数一等于参数二
 *	<	0		参数一小于参数二
 */
static int
lr_action_compare(const void *elem1, const void *elem2)
{
	struct lr_action *v1 = (struct lr_action*)elem1;
	struct lr_action *v2 = (struct lr_action*)elem2;

	if(v1->length == v2->length)
		return memicmp(v1->action, v2->action, v1->length);
	else
		return (int)((int)v1->length - (int)v2->length);
}

/**
 *	lr_action_init - left to right 分析表初始化
 *
 *	return
 *		无
 */
static void
lr_action_init()
{
	int		i;

	for(i = 0; i < DINUM(request_line); i++)
		request_line[i].length =
				strlen(request_line[i].action);

	for(i = 0; i < DINUM(headers_line); i++)
		headers_line[i].length =
				strlen(headers_line[i].action);

	qsort(request_line, DINUM(request_line),
		sizeof(struct lr_action), lr_action_compare);

	qsort(headers_line, DINUM(headers_line),
		sizeof(struct lr_action), lr_action_compare);
}

/**
 *	local_word_table_init - 单词表初始化(每个线程仅调用一次)
 *
 *	@wtab:			单词表
 *
 *	return
 *		无
 */
void
local_word_table_init(IN struct word_table *wtab)
{
	int		i;
	word_t	*wp = wtab->words;

	INIT_LIST_HEAD(&wtab->used);
	INIT_LIST_HEAD(&wtab->free);

	for(i = 0; i < NR_WORDS; i++,wp++)
		list_add_tail(&wp->list, &wtab->free);
}

/**
 *	global_parser_module_init - 解析器全局初始化(仅调用一次)
 *
 *	return
 *		无
 */
void
global_parser_module_init()
{
	lr_action_init();
	gen_separate_map();
}

/**
 *	alloc_word - 申请一个单词描述符
 *
 *	@wtab:			单词表
 *
 *	return
 *		!NULL		单词描述符
 *		NULL		没有可用的单词描述符
 */
static word_t*
alloc_word(IN struct word_table *wtab)
{
	struct list_head *free = &wtab->free;
	struct list_head *used = &wtab->used;
	struct list_head *temp = free->next;

	if(list_empty(free))
		return NULL;
	else {
		list_del(temp);
		list_add_tail(temp, used);
		return list_entry(temp, word_t, list);
	}
}

/**
 *	shift_word - 移进一个单词描述符
 *
 *	@wtab:			单词表
 *
 *	return
 *		!NULL		单词描述符
 *		NULL		没有可用的单词描述符
 */
static word_t*
shift_word(IN struct word_table *wtab)
{
	struct list_head *free = &wtab->free;
	struct list_head *used = &wtab->used;
	struct list_head *temp = used->next;

	if(list_empty(used))
		return NULL;
	else {
		list_del(temp);
		list_add_tail(temp, free);
		return list_entry(temp, word_t, list);
	}
}

/**
 *	tail_word - 查看单词表中的最后一个单词
 *
 *	@wtab:			单词表
 *
 *	return
 *		!NULL		单词描述符
 *		NULL		没有可用的单词描述符
 */
static word_t*
tail_word(IN struct word_table *wtab)
{
	struct list_head *used = &wtab->used;
	struct list_head *temp = used->prev;

	if(list_empty(used))
		return NULL;
	else
		return list_entry(temp, word_t, list);
}

/**
 *	do_line_lex - 行词法分析
 *
 *	@wtab:			单词表
 *	@line:			数据缓冲区
 *	@size:			数据缓冲区的大小
 *
 *	return
 *		<	0		失败
 *		==	0		不是一个完整的行
 *		>	0		本行的字节数
 */
static int
do_line_lex(IN struct word_table *wtab, IN const void *line, IN size_t size)
{
	stack_t			_local;
#define	read_charactor(_p_)	(*((unsigned char *)(_p_)))
	unsigned char	ch;
	unsigned char	*ptr = (unsigned char*)line;
	stack_t			*stack = &_local;
	word_t			*wp;
	int				exit = 0;

	virtual_stack_init(stack);
	for(;ptr - (unsigned char*)line < (int)size && (!exit); ptr++) {
		ch = read_charactor(ptr);
		if(is_separate_char(ch)) {	/*	是分隔字符*/
			if(virtual_stack_done(stack)) {
				wp = alloc_word(wtab);
				if(IS_NULL(wp))
					return gen_errno(GCEPARSE, GCENORES);
				else
					virtual_stack_copy(wp, stack);
			}
		} else {					/*	不是分隔字符*/
			if(IS_LF_CHAR(ch))
				exit = 1;
			virtual_stack_push(stack, ptr);
		}
	}

	if(virtual_stack_done(stack)) {
		wp = alloc_word(wtab);
		if(IS_NULL(wp))
			return gen_errno(GCEPARSE, GCENORES);
		else
			virtual_stack_copy(wp, stack);
	}

	wp = tail_word(wtab);
	if(IS_NULL(wp))
		return 0;

	if(IS_EOS(wp))
		return ptr - (unsigned char*)line;
	else
		return 0;
}

/**
 *	do_body_lex - BODY行词法分析
 *
 *	@wtab:			单词表
 *	@parser:		HTTP解析器
 *	@line:			数据缓冲区
 *	@size:			数据缓冲区的大小
 *
 *	return
 *		<	0		失败
 *		==	0		不是一个完整的行
 *		>	0		本行的字节数
 */
static int
do_body_lex(IN struct word_table *wtab,
	IN struct http_parser *parser, IN const void *line, IN size_t size)
{
	word_t	*wp;
	size_t	length;

	length = (size > parser->length) ?
				parser->length : size;

	parser->length -= length;

	wp = alloc_word(wtab);
	if(IS_NULL(wp))
		return gen_errno(GCEPARSE, GCENORES);;

	wp->word = (char*)line;
	wp->size = length;

	return length;
}

/**
 *	do_connect_action - 处理connect标签动作
 *
 *	@wtab:			单词表
 *	@parser:		HTTP解析器
 *
 *	return
 *		!0			失败
 *		0			成功
 */
static int
do_connect_action(
	IN struct word_table *wtab, IN struct http_parser *parser)
{
	char				addr[MAX_DOMAIN_LENGTH + 1];
	char				port[MAX_DOMAIN_LENGTH + 1];
	struct sockaddr_in	sa;
	word_t				*wp;
	struct hostent		*host;
	struct gcs_nat		*nat;
	char				*ipv4_addr;

	/**	移进地址信息*/
	wp = shift_word(wtab);
	if(IS_NULL(wp))
		return gen_errno(GCEPARSE, GCEMISSING_IP);

	memset(addr, 0, sizeof(addr));
	strncpy(addr, wp->word,
		min(wp->size, MAX_DOMAIN_LENGTH));

	/**	移进端口信息*/
	wp = shift_word(wtab);
	if(IS_NULL(wp))
		return gen_errno(GCEPARSE, GCEMISSING_PORT);
	
	memset(port, 0, sizeof(port));
	strncpy(port, wp->word,
		min(wp->size, MAX_DOMAIN_LENGTH));
	
	parser->port = (unsigned short)strtoul(port, NULL, 0);
	parser->port = htons(parser->port);

	/**	查找本地的NAT路由表*/
	nat = lookup_nat_rules(addr, parser->port);
	if(IS_NOT_NULL(nat)) {
		if(ref_config_object()->log_level >= LOG_LEVEL_SUPPORT)
			write_log(GCS_MODULE_ID,
				LOG_TYPE_INFO, LOG_LEVEL_SUPPORT,
				"found nat rule %s:%d --> %s:%d",
				addr, ntohs(parser->port),
				nat->dst_addr, ntohs(nat->dst_port));
		ipv4_addr		= nat->dst_addr;
		parser->port	= nat->dst_port;
	} else
		ipv4_addr		= addr;

	parser->ip = inet_addr(ipv4_addr);
	if(parser->ip == INADDR_NONE) {
		host = gethostbyname(ipv4_addr);
		if(host) {
			memcpy((char *)&sa.sin_addr,(char *)host->h_addr,host->h_length);
			parser->ip = sa.sin_addr.S_un.S_addr;
		} else {
			write_log(GCS_MODULE_ID,
				LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
				"dns error: could not resolve address"
				"(0x%08X)(%s)", gen_errno(_OSerrno(), GCEOS), ipv4_addr);
		}
	}

	return 0;
}

/**
 *	do_channo_action - 处理GCCHNO标签动作
 *
 *	@wtab:			单词表
 *	@parser:		HTTP解析器
 *
 *	return
 *		!0			失败
 *		0			成功
 */
static int
do_channo_action(
	IN struct word_table *wtab, IN struct http_parser *parser)
{
	char	val[256];
	word_t	*wp;

	if(parser->bit3)	/*	加快处理速度*/
		return 0;

	wp = shift_word(wtab);
	if(IS_NULL(wp))
		return gen_errno(GCEPARSE, GCEMISSING_CHANNO);

	memset(val, 0, sizeof(val));
	strncpy(val, wp->word,
		min(wp->size, sizeof(val) - 1));

	parser->channo = (unsigned short)strtoul(val, NULL, 0);

	return 0;
}

/**
 *	do_unitno_action - 处理GCUNNO标签动作
 *
 *	@wtab:			单词表
 *	@parser:		HTTP解析器
 *
 *	return
 *		!0			失败
 *		0			成功
 */
static int
do_unitno_action(
	IN struct word_table *wtab, IN struct http_parser *parser)
{
	char	val[256];
	word_t	*wp;

	if(parser->bit1)	/*	加快处理速度*/
		return 0;

	wp = shift_word(wtab);
	if(IS_NULL(wp))
		return gen_errno(GCEPARSE, GCEMISSING_UNITNO);

	memset(val, 0, sizeof(val));
	strncpy(val, wp->word,
		min(wp->size, sizeof(val) - 1));

	parser->unitno = (unsigned short)strtoul(val, NULL, 0);

	return 0;
}

/**
 *	do_tunnel_action - 处理GCLNKNO标签动作
 *
 *	@wtab:			单词表
 *	@parser:		HTTP解析器
 *
 *	return
 *		!0			失败
 *		0			成功
 */
static int
do_tunnel_action(
	IN struct word_table *wtab, IN struct http_parser *parser)
{
	char	val[256];
	word_t	*wp;

	if(parser->bit2)	/*	加快处理速度*/
		return 0;

	wp = shift_word(wtab);
	if(IS_NULL(wp))
		return gen_errno(GCEPARSE, GCEMISSING_TUNNEL);
	
	memset(val, 0, sizeof(val));
	strncpy(val, wp->word,
		min(wp->size, sizeof(val) - 1));
	
	parser->tunnel = (unsigned short)strtoul(val, NULL, 0);
	
	return 0;
}

/**
 *	do_length_action - 处理Content-Length标签动作
 *
 *	@wtab:			单词表
 *	@parser:		HTTP解析器
 *
 *	return
 *		!0			失败
 *		0			成功
 */
static int
do_length_action(
	IN struct word_table *wtab, IN struct http_parser *parser)
{
	char	val[256];
	word_t	*wp;
	
	wp = shift_word(wtab);
	if(IS_NULL(wp))
		return gen_errno(GCEPARSE, GCEMISSING_LENGTH);
	
	memset(val, 0, sizeof(val));
	strncpy(val, wp->word,
		min(wp->size, sizeof(val) - 1));
	
	parser->length = (unsigned short)strtoul(val, NULL, 0);
	
	return 0;
}

/**
 *	do_gctype_action - 处理GCType标签动作
 *
 *	@wtab:			单词表
 *	@parser:		HTTP解析器
 *
 *	return
 *		!0			失败
 *		0			成功
 */
static int
do_gctype_action(
	IN struct word_table *wtab, IN struct http_parser *parser)
{
	char	val[256];
	word_t	*wp;
	
	wp = shift_word(wtab);
	if(IS_NULL(wp))
		return gen_errno(GCEPARSE, GCEMISSING_GCTYPE);
	
	memset(val, 0, sizeof(val));
	strncpy(val, wp->word,
		min(wp->size, sizeof(val) - 1));
	
	parser->action = (unsigned short)strtoul(val, NULL, 0);
	
	return 0;
}

/**
 *	do_request_line_yacc - 处理请求行的语法分析
 *
 *	@first_word:	首单词
 *	@wtab:			单词表
 *	@parser:		HTTP解析器
 *
 *	return
 *		!=0			失败
 *		0			成功
 */
static int
do_request_line_yacc(IN word_t *first_word,
	IN struct word_table *wtab, IN struct http_parser *parser)
{
	struct lr_action	key;
	struct lr_action	*result;

	http_parser_relay(parser);

	key.action = first_word->word;
	key.length = first_word->size;

	result = bsearch(&key, request_line, DINUM(request_line),
				sizeof(struct lr_action), lr_action_compare);
	if(IS_NOT_NULL(result) && IS_NOT_NULL(result->fn_ptr))
		result->fn_ptr(wtab, parser);

	status_switch_to(parser, S_PARSE_HEADERS_LINE);

	return 0;
}

/**
 *	do_headers_line_yacc - 处理HTTP头的语法分析
 *
 *	@first_word:	首单词
 *	@wtab:			单词表
 *	@parser:		HTTP解析器
 *
 *	return
 *		!=0			失败
 *		0			成功
 */
static int
do_headers_line_yacc(IN word_t *first_word,
	IN struct word_table *wtab, IN struct http_parser *parser)
{
	struct lr_action	key;
	struct lr_action	*result;

	key.action = first_word->word;
	key.length = first_word->size;

	result = bsearch(&key, headers_line, DINUM(headers_line),
				sizeof(struct lr_action), lr_action_compare);
	if(IS_NOT_NULL(result) && IS_NOT_NULL(result->fn_ptr))
		result->fn_ptr(wtab, parser);

	if(IS_EOS(first_word)) {
		if(parser->length == 0)
			status_switch_to(parser, S_PARSE_E_BLANK_LINE);
		else
			status_switch_to(parser, S_PARSE_BODYCTX_LINE);

		if(IS_NOT_NULL(parser->headers_cbk))
			return parser->headers_cbk(parser->user, parser);
		else
			return 0;
	} else
		return 0;
}

/**
 *	do_bodyctx_line_yacc - 处理HTTPBODY的语法分析
 *
 *	@first_word:	首单词
 *	@wtab:			单词表
 *	@parser:		HTTP解析器
 *
 *	return
 *		!=0			失败
 *		0			成功
 */
static int
do_bodyctx_line_yacc(IN word_t *first_word,
	IN struct word_table *wtab, IN struct http_parser *parser)
{
	UNREFERENCED_PARAMETER(wtab);

	if(parser->length == 0)
		status_switch_to(parser, S_PARSE_E_BLANK_LINE);

	if(IS_NOT_NULL(parser->bodyctx_cbk) && first_word->size)
		return parser->bodyctx_cbk(parser->user, parser,
						first_word->word, first_word->size);
	else
		return 0;
}

/**
 *	do_e_blank_line_yacc - 处理HTTPBODY 尾行的语法分析
 *
 *	@first_word:	首单词
 *	@wtab:			单词表
 *	@parser:		HTTP解析器
 *
 *	return
 *		!=0			失败
 *		0			成功
 */
static int
do_e_blank_line_yacc(IN word_t *first_word,
	IN struct word_table *wtab, IN struct http_parser *parser)
{
	UNREFERENCED_PARAMETER(wtab);

	if(IS_EOS(first_word)) {
		if(parser->eobl++)
			status_switch_to(parser, S_PARSE_REQUEST_LINE);
		return 0;
	} else
		return gen_errno(GCEPARSE, GCEMISSING_EBLANK);
}

/**
 *	do_line_yacc - 行语法分析
 *
 *	@wtab:			单词表
 *	@parser:		HTTP解析器
 *
 *	return
 *		!=0			失败
 *		0			成功
 */
static int
do_line_yacc(IN struct word_table *wtab, IN struct http_parser *parser)
{
	int		rc;
	word_t	*wp;

	wp = shift_word(wtab);
	if(wp == NULL)
		return gen_errno(GCEPARSE, GCEABORT);

	switch(parser->status) {
	case S_PARSE_REQUEST_LINE:
		rc = do_request_line_yacc(wp, wtab, parser);
		break;
	case S_PARSE_HEADERS_LINE:
		rc = do_headers_line_yacc(wp, wtab, parser);
		break;
	case S_PARSE_BODYCTX_LINE:
		rc = do_bodyctx_line_yacc(wp, wtab, parser);
		break;
	case S_PARSE_E_BLANK_LINE:
		rc = do_e_blank_line_yacc(wp, wtab, parser);
		break;
	default:
		return -1;
	}

	return rc;
}

/**
 *	word_table_dbg_trace - 单词表调试测试
 *
 *	@wtab:			单词表
 *	@tag:			调试标签
 *
 *	return
 *		无
 */
static void
word_table_dbg_trace(
		IN struct word_table *wtab, const char *tag)
{
#ifdef _DEBUG
	int		used = 0;
	int		free = 0;
	struct list_head *list;
	word_t			*wp;
	char			fmt[32];

	list_for_each(list, &wtab->used) used++;
	list_for_each(list, &wtab->free) free++;

	printf("++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	list_for_each(list, &wtab->used) {
		wp = list_entry(list, word_t, list);
		printf("%4d ---> ", wp->size);
		sprintf(fmt, "[%%%d.%ds]\n", wp->size, wp->size);
		printf(fmt, wp->word);
	}

	printf("%s word_table used: %4d free: %4d\n", tag, used, free);
	printf("++++++++++++++++++++++++++++++++++++++++++++++++++\n");
#endif
}

/**
 *	do_http_parse - 执行HTTP语法解析
 *
 *	@wtab:			单词表
 *	@parser:		HTTP解析器
 *	@buffer:		数据缓冲区
 *	@size:			数据缓冲区的大小
 *
 *	return
 *		>	0		成功解析的字节数
 *		==	0		输入数据不足够解析数据
 *		<	0		解析过程中出现错误
 */
int
do_http_parse(IN struct word_table *wtab,
			IN struct http_parser *parser,
			IN const void *buffer, IN size_t size)
{
	int			rc;
	size_t		deal = 0;
	
	for(;;) {
		line_parser_startup(wtab);

		if(parser->status == S_PARSE_BODYCTX_LINE)
			rc = do_body_lex(wtab, parser,
					(char*)buffer + deal, size - deal);
		else
			rc = do_line_lex(wtab,
					(char*)buffer + deal, size - deal);
	//	word_table_dbg_trace(wtab, "after do_line_lex");
		if(rc < 0)
			return rc;
		else if(rc == 0)
			return deal;
		else
			deal += rc;
		
		rc = do_line_yacc(wtab, parser);
		if(rc)
			return rc;

		line_parser_cleanup(wtab);
	}
	

	return deal;
}

/**
 *	http_parser_init - HTTP解析引擎初始化
 *
 *	@parser:		HTTP解析器
 *	@user:			用户句柄
 *	@headers_cbk:	头回调函数
 *	@bodyctx_cbk:	体回调函数
 *
 *	return
 *		无
 */
void
http_parser_init(
	IN struct http_parser *parser, IN void *user,
	IN int	(*headers_cbk)
				(IN void *, IN struct http_parser *),
	IN int	(*bodyctx_cbk)
				(IN void *, IN struct http_parser *,
				IN void *, size_t))
{
	memset(parser, 0, sizeof(struct http_parser));

	http_parser_reset(parser);

	parser->user		= user;
	parser->headers_cbk	= headers_cbk;
	parser->bodyctx_cbk	= bodyctx_cbk;
}
