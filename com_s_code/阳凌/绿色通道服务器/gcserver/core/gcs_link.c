/**
 *	core/gcs_link.c
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */

#include "native.h"
#include "typedef.h"
#include "gcs_sock.h"
#include "gcs_errno.h"
#include "gcs_link.h"
#include "gcs_page.h"
#include "gcs_http.h"
#include "gcs_iocp.h"
#include "gcs_parser.h"
#include "gcs_conn.h"
#include "gcs_config.h"
#include "gcs_smif.h"
#include "asm-x86/atomic.h"
#include "bits/gcs_cnt64.h"
#include "gcs_mon.h"
#include "gcs_wt.h"
#include "gcs_main.h"

#ifndef lint
static const char rcsid[] _U_ =
	"@(#) $Id: core/gcs_link.c,"
	"v 1.00 2013/10/17 11:38:40 yangling Exp $ (LBL)";
#endif

static char *_log_tags[] = {"server", "client"};

/**
 *	LINK的统计数据
 */
static struct link_stat {
	unsigned short			nr_server;		/*	当前用于server的连接数 linkno_s_pool lock控制*/
	unsigned short			nr_client;		/*	当前用于client的连接数 linkno_s_pool lock控制*/
	cnt64_t					srv_received;	/*	当前server接收的字节数*/
	cnt64_t					srv_sent;		/*	当前server发送的字节数*/
	cnt64_t					srv_committed;	/*	当前server提交的字节数*/
	cnt64_t					cli_received;	/*	当前client接收的字节数*/
	cnt64_t					cli_sent;		/*	当前client发送的字节数*/
	cnt64_t					cli_committed;	/*	当前client提交的字节数*/
}__link_stat_object__;
#define	lnk_stat_object()	(&__link_stat_object__)	

/**
 *	LINKNO 项
 */
static struct linkno_s {
	struct list_head		list;			/*	链节点*/
	unsigned short			inc;			/*	自增数*/
	unsigned short			padding;		/*	保留*/
};

/**
 *	LINKNO 项资源池
 */
static struct linkno_s_pool {
	struct list_head		free_head;		/*	空闲LINKNO链表头*/
	struct linkno_s		*	base;			/*	LINKNO资源缓冲区*/
	pthread_mutex_t			lock;			/*	锁*/
	unsigned short			nr_links;		/*	最大的资源数*/
	unsigned short			cur_using;		/*	当前正在使用的*/
	unsigned int			bitmap[2048];	/*	位图掩码*/
}__linkno_s_pool_object__;
#define	lnk_pool_object()	(&__linkno_s_pool_object__)

/**
 *	LINK 槽表
 */
static struct link_slot_tab {
	struct link_slot	*	slot;			/*	LINK槽表缓冲区*/
	struct link_buff	*	buff;			/*	LINK缓冲区缓冲区*/
	unsigned short			nr_links;		/*	最大的资源数*/
	unsigned short			padding;		/*	保留*/
}__link_slot_tab_object__;
#define	lnktable_object()	(&__link_slot_tab_object__)

/**
 *	槽表加锁
 */
#define	lock_link_slot(_slt_)					\
	do {										\
		pthread_mutex_lock(&((_slt_)->lock));	\
	} while(0)

/**
 *	槽表解锁
 */
#define	unlock_link_slot(_slt_)					\
	do {										\
		pthread_mutex_unlock(&((_slt_)->lock));	\
	} while(0)

/**
 *	非法的LINKNO
 */
#define	INVALID_LINKNO	0xFFFFFFFF

/**
 *	过期的LINKNO吗 真=是 假=否
 */
#define	is_overdue_linkno(a, b) ((a) != (b))

#define linkno_index(_a_)		\
	((unsigned short)((_a_) & 0xFFFF))

/**
 *	_close_connection_logtrace - 关闭连接的日志
 *
 *	@slot:		LINK槽
 *
 *	return
 *		无
 */
static void
_close_connection_logtrace(IN struct link_slot *slot)
{
	char	_nm[256];

	if(ref_config_object()->log_level >= LOG_LEVEL_SUPPORT)
		write_log(GCS_MODULE_ID,
			LOG_TYPE_INFO, LOG_LEVEL_SUPPORT,
			"Nothing left to send, closing %s connection %s",
			_log_tags[slot->passive],
			ne_inet_ntopx(slot->ipv4, slot->port, _nm, sizeof(_nm)));
}

/**
 *	_free_connection_logtrace - 释放连接的日志
 *
 *	@passive:	主动/被动(FOR_SERVER/FOR_CLIENT)
 *	@ipv4:		IP地址(big endian)
 *	@port:		端口信息(big endian)
 *
 *	return
 *		无
 */
static void
_free_connection_logtrace(IN unsigned int passive,
	IN unsigned int ipv4, IN unsigned short port)
{
	char	_nm[256];

	if(ref_config_object()->log_level >= LOG_LEVEL_SUPPORT) {
		passive = passive & 0x1;
		
		write_log(GCS_MODULE_ID,
			LOG_TYPE_INFO, LOG_LEVEL_SUPPORT,
			"%s connection %s closed",
			_log_tags[passive],
			ne_inet_ntopx(ipv4, port, _nm, sizeof(_nm)));
	}	
}

/**
 *	ref_link_slot - 根据LINKNO引用LINK槽表
 *
 *	@link_no:		LINKNO
 *
 *	return
 *		NULL		失败
 *		!NULL		成功
 */
static INLINE_FN
struct link_slot *
ref_link_slot(IN linkno_t link_no)
{
	struct link_slot_tab	*obj = lnktable_object();
	unsigned short			idx;
	
	idx = linkno_index(link_no);

	if(idx >= obj->nr_links)
		return NULL;

	return obj->slot + idx;
}

/**
 *	build_linkno - 构建一个LINKNO
 *
 *	@ptr:		LINKNO项
 *
 *	return
 *		LINKNO
 */
static INLINE_FN linkno_t
build_linkno(IN struct linkno_s *ptr)
{
	struct linkno_s_pool	*obj = lnk_pool_object();
	unsigned int			hi = ptr->inc++;
	unsigned int			lo = ptr - obj->base;
	linkno_t				v;

	v = ((hi << 16) & 0xFFFF0000) | (lo & 0xFFFF);

	SET_ID(obj->bitmap, lo);

	return v;
}

static int
do_http_headers_cbk(
	IN void *, IN struct http_parser *);

static int
do_http_bodyctx_cbk(
	IN void *, IN struct http_parser *,
	IN void *, size_t);

static int
do_wt_pkg1_cbk(IN void *);

static int
do_wt_pkg2_cbk(IN void *);

/**
 *	init_link_slot - 初始化LINK槽
 *
 *	@slot:		LINK槽
 *
 *	return
 *		无
 */
static void
init_link_slot(IN struct link_slot *slot)
{
	struct link_slot_tab	*obj = lnktable_object();
	unsigned int			pos = slot - obj->slot;
	struct http_parser		*parser = &(slot->parser);
	struct wt_parser		*wtp = &(slot->wtp);

	pthread_mutex_init(&(slot->lock), NULL);
	slot->link_no	= INVALID_LINKNO;
	slot->peer_no	= INVALID_LINKNO;
	slot->s			= INVALID_SOCKET;
	http_parser_init(parser, slot,
		do_http_headers_cbk, do_http_bodyctx_cbk);
	wt_parser_init(wtp, slot,
		do_wt_pkg1_cbk, do_wt_pkg2_cbk);

	slot->rx_buff	= obj->buff + (pos * 2 + 1);
	slot->tx_buff	= obj->buff + (pos * 2);
}

/**
 *	init_link_stat - LINK的统计初始化
 *
 *	return
 *		无
 */
static void
init_link_stat()
{
	struct link_stat *obj = lnk_stat_object();

	memset(obj, 0, sizeof(struct link_stat));
}

/**
 *	init_link_buff - 初始化LINK缓冲区
 *
 *	@buff:		LINK缓冲区
 *
 *	return
 *		无
 */
static void
init_link_buff(IN struct link_buff *buff)
{
	struct link_slot_tab	*obj = lnktable_object();

	/*	奇数=接收模式 偶数=发送模式*/
	//buff->page		= NULL;
	//buff->mtime		= 0;
	//buff->cur_size	= 0;
	//buff->lmt_size	= 0;
	INIT_LIST_HEAD(&(buff->head));
	buff->flags |= ((buff - obj->buff) % 2);
}

/**
 *	init_link_slot_tab - 初始化LINK槽表
 *
 *	@nr_links:		总的连接数
 *
 *	return
 *		0			成功
 *		!0			失败
 */
static int
init_link_slot_tab(IN unsigned short nr_links)
{
	int						i;
	size_t					size;
	struct link_slot_tab	*obj = lnktable_object();
	struct link_slot		*slot;
	struct link_buff		*buff;

	memset(obj, 0, sizeof(struct link_slot_tab));
	obj->nr_links = nr_links;

	size = nr_links * sizeof(struct link_slot);
	obj->slot = zmalloc(size);
	if(IS_NULL(obj->slot))
		return gen_errno(0, GCENOMEM);
	else
		slot = obj->slot;

	size = nr_links * 2 * sizeof(struct link_buff);
	obj->buff = zmalloc(size);
	if(IS_NULL(obj->buff))
		return gen_errno(0, GCENOMEM);
	else
		buff = obj->buff;

	for(i = 0; i < nr_links*2; i++)
		init_link_buff(buff++);
	
	for(i = 0; i < nr_links; i++)
		init_link_slot(slot++);

	return 0;
}

/**
 *	init_linkno_pool - 初始化LINKNO池
 *
 *	@nr_links:		总的LINKNO数
 *
 *	return
 *		0			成功
 *		!0			失败
 */
static int
init_linkno_pool(IN unsigned short nr_links)
{
	int						i;
	size_t					size;
	struct linkno_s_pool	*obj = lnk_pool_object();
	struct linkno_s			*ptr;

	memset(obj, 0, sizeof(struct linkno_s_pool));

	obj->cur_using	= 0;
	INIT_LIST_HEAD(&(obj->free_head));
	pthread_mutex_init(&(obj->lock), NULL);
	obj->nr_links = nr_links;
	memset(obj->bitmap, 0, sizeof(obj->bitmap));

	size = nr_links * sizeof(struct linkno_s);
	obj->base = zmalloc(size);
	if(IS_NULL(obj->base))
		return gen_errno(0, GCENOMEM);
	
	ptr = obj->base;
	for(i = 0; i < nr_links; i++, ptr++)
		list_add_tail(&(ptr->list), &(obj->free_head));

	return 0;
}

/**
 *	global_link_module_init - 全局初始化LINK模块
 *
 *	return
 *		!0			失败
 *		0			成功
 */
int
global_link_module_init()
{
	int					rc;
	unsigned short		nr_links;
	struct gcs_config	*config = ref_config_object();

	nr_links	= config->nr_links;
	nr_links	*= 2;

	init_link_stat();

	rc = init_linkno_pool(nr_links);
	if(rc)
		return rc;

	return init_link_slot_tab(nr_links);
}

/**
 *	new_linkno - 构建一个LINKNO
 *
 *	@passive:			申请原因
 *
 *	return
 *		INVALID_LINKNO	出错
 *		otherwise		LINKNO
 */
static unsigned int new_linkno(IN int passive)
{
	struct linkno_s_pool	*obj = lnk_pool_object();
	pthread_mutex_t			*lck = &(obj->lock);
	struct list_head		*hdr = &(obj->free_head);
	struct list_head		*lst;
	struct linkno_s			*ptr;
	linkno_t				link_no;

	pthread_mutex_lock(lck);
	if(!list_empty(hdr)) {
		lst = hdr->next; list_del(lst);
		ptr = list_entry(lst, struct linkno_s, list);
		link_no = build_linkno(ptr);
		if(passive == FOR_SERVER)
			__link_stat_object__.nr_server++;
		else
			__link_stat_object__.nr_client++;

		obj->cur_using++;
	} else
		link_no = INVALID_LINKNO;
	pthread_mutex_unlock(lck);

	return link_no;
}

/**
 *	peek_linkno - 偷窥是否还有可用的LINKNO资源
 *
 *	return
 *		!0		还有
 *		0		没有
 */
static int peek_linkno()
{
	struct linkno_s_pool	*obj = lnk_pool_object();
	pthread_mutex_t			*lck = &(obj->lock);
	struct list_head		*hdr = &(obj->free_head);
	int						rc;

	pthread_mutex_lock(lck);
	rc = list_empty(hdr);
	pthread_mutex_unlock(lck);

	return !rc;
}

/**
 *	del_linkno - 删除一个LINKNO
 *
 *	@passive:		申请原因
 *	@linkno:		LINKNO
 *
 *	return
 *		无
 */
static void del_linkno(IN int passive, IN linkno_t linkno)
{
	struct linkno_s_pool	*obj = lnk_pool_object();
	pthread_mutex_t			*lck = &(obj->lock);
	struct list_head		*hdr = &(obj->free_head);
	struct linkno_s			*ptr;
	unsigned short			idx = linkno_index(linkno);

	if(idx >= obj->nr_links)
		return;

	pthread_mutex_lock(lck);
	if(IS_SET_ID(obj->bitmap, idx)) {
		ptr = obj->base + idx;
		list_add_tail(&(ptr->list), hdr);
		UNSET_ID(obj->bitmap, idx);
		if(passive == FOR_SERVER)
			__link_stat_object__.nr_server--;
		else
			__link_stat_object__.nr_client--;

		obj->cur_using--;
	}
	pthread_mutex_unlock(lck);
}

/**
 *	gcs_peek_link - 偷窥是否还有可用的LINKNO资源
 *
 *	return
 *		!0		还有
 *		0		没有
 */
int gcs_peek_link()
{
	return peek_linkno();
}

/**
 *	ll_iocp_send - 低级IOCP发送LowLevel
 *
 *	@slot:			LINK槽
 *
 *	return:
 *		0 成功
 *		!0失败
 */
static int
ll_iocp_send(IN struct link_slot *slot)
{
	WSABUF				wsabuf;
	unsigned long		size = 0;
	struct link_buff	*tx = slot->tx_buff;
	struct link_stat	*stat = lnk_stat_object();
	
	wsabuf.buf	= tx->page->buffer;
	wsabuf.len	= tx->page->size;
	tx->mtime	= time(NULL);
	
	memset(&(tx->iocp_arg), 0, sizeof(OVERLAPPED));
	if(WSASend(slot->s, &wsabuf, 1, &size, 0, 
		(OVERLAPPED *)tx, 0) && GetLastError() != WSA_IO_PENDING)
		return gen_errno(_OSerrno(), GCEOS);
	else {
		if(slot->passive == FOR_SERVER)
			CNT64_T_ADD(&(stat->srv_committed), wsabuf.len);
		else
			CNT64_T_ADD(&(stat->cli_committed), wsabuf.len);
		return 0;
	}
}

/**
 *	ll_iocp_recv - 低级IOCP接收LowLevel
 *
 *	@slot:			LINK槽
 *
 *	return:
 *		0 成功
 *		!0失败
 */
static int
ll_iocp_recv(IN struct link_slot *slot)
{
	WSABUF				wsabuf;
	unsigned long		size = 0;
	unsigned long		flags = 0;
	struct link_buff	*rx = slot->rx_buff;

	if(IS_NULL(rx->page)) {
		rx->page = getpage(FOX_RX_USE);
		if(IS_NULL(rx->page))
			return gen_errno(0, GCENOBUFS);
	}
	
	wsabuf.len	= MPAGE_SIZE - rx->page->size;
	wsabuf.buf	= rx->page->buffer + rx->page->size;
	memset(&(rx->iocp_arg), 0, sizeof(OVERLAPPED));

	if(wsabuf.len == 0)
		return gen_errno(0, GCEINVAL);
	
	if(WSARecv(slot->s, &wsabuf, 1,&size, &flags,
		(WSAOVERLAPPED *)rx, 0) && GetLastError() != WSA_IO_PENDING)
	{
		putpage(rx->page, FOX_RX_USE); rx->page = NULL;
		return gen_errno(_OSerrno(), GCEOS);
	}
	else
		return 0;
}

/**
 *	ll_iocp_bind - 低级IOCP绑定LowLevel
 *
 *	@slot:			LINK槽
 *
 *	return:
 *		0 成功
 *		!0失败
 */
static int
ll_iocp_bind(IN struct link_slot *slot)
{
	struct iocp_model	*model = ref_iocp_model_object();
	
	if(!CreateIoCompletionPort(
			(HANDLE)slot->s, model->handle,
			slot->link_no, model->nr_wk_ths))
		return gen_errno(_OSerrno(), GCEOS);
	else
		return 0;
}

/**
 *	_logical_close_link 逻辑关闭一个LINK
 *
 *	@slot:		LINK槽
 *	@link_no:	LINKNO
 *
 *	return
 *		0		成功
 *		!0		失败
 */
static int
_logical_close_link(
	IN struct link_slot *slot, IN linkno_t link_no)
{
	struct link_buff	*tx = slot->tx_buff;

	if(is_overdue_linkno(
		slot->link_no, link_no))
		return gen_errno(0, GCELINKNO);

	/*	如果正在发送中,仅仅设置延迟关闭标志*/
	if(IS_NOT_NULL(tx->page)) {
		slot->defer_close = GCS_TRUE;
		return 0;
	}

	if(slot->s != INVALID_SOCKET) {
		_close_connection_logtrace(slot);
		if(ref_config_object()->rst_close)
			ne_fast_close(slot->s);
		else
			ne_close(slot->s);
		slot->s = INVALID_SOCKET;
	}
	return 0;
}

/**
 *	gcs_logical_close_link 逻辑关闭一个LINK
 *
 *	@link_no:	LINKNO
 *
 *	return
 *		0		成功
 *		!0		失败
 */
int
gcs_logical_close_link(IN linkno_t link_no)
{
	int					rc;
	struct link_slot	*slot;

	slot = ref_link_slot(link_no);
	if(IS_NULL(slot))
		return gen_errno(0, GCEINVAL);
	
	lock_link_slot(slot);
	rc = _logical_close_link(slot, link_no);
	unlock_link_slot(slot);
	
	return rc;
}

/**
 *	_hard_close_link 硬关闭一个LINK
 *
 *	@slot:		LINK槽
 *	@link_no:	LINKNO
 *
 *	return
 *		0		成功
 *		!0		失败
 */
static int
_hard_close_link(
	IN struct link_slot *slot, IN linkno_t link_no)
{
	if(is_overdue_linkno(
		slot->link_no, link_no))
		return gen_errno(0, GCELINKNO);

	if(slot->s != INVALID_SOCKET) {
		_close_connection_logtrace(slot);
		if(ref_config_object()->rst_close)
			ne_fast_close(slot->s);
		else
			ne_close(slot->s);
		slot->s = INVALID_SOCKET;
	}

	return 0;
}

/**
 *	gcs_hard_close_link 硬关闭一个LINK
 *
 *	@link_no:	LINKNO
 *
 *	return
 *		0		成功
 *		!0		失败
 */
int
gcs_hard_close_link(IN linkno_t link_no)
{
	int					rc;
	struct link_slot	*slot;

	slot = ref_link_slot(link_no);
	if(IS_NULL(slot))
		return gen_errno(0, GCEINVAL);
	
	lock_link_slot(slot);
	rc = _hard_close_link(slot, link_no);
	unlock_link_slot(slot);
	
	return rc;
}

/**
 *	write_tx_queue - 写发送队列数据
 *
 *	@tx:		LINK发送缓冲区
 *	@buffer:	数据缓冲区
 *	@size:		数据缓冲区大小
 *
 *	return
 *		>=	0	成功写入的字节数
 *		<	0	失败
 */
static int
write_tx_queue(
	IN struct link_buff *tx, IN void *buffer, IN size_t size)
{
	struct list_head	*head	= &(tx->head);
	struct list_head	*prev	= head->prev;
	struct page_head	*page;
	int					rc = 0;
	size_t				written = 0;
	
	if(tx->cur_size + size > tx->lmt_size)
		return gen_errno(0, GCEQFULL);

	if(!list_empty(head)) {	/*	队列不空*/
		page = list_entry(prev, struct page_head, list);
		rc = pgwrite(page, page->size, buffer, size);
		if(rc < 0)
			return rc;
		else
			written = rc;
	}

	while(size > written) {
		page = getpage(FOR_TX_USE);
		if(IS_NULL(page))
			return gen_errno(0, GCENOBUFS);

		rc = pgwrite(page, page->size,
				(char *)buffer + written, size - written);
		if(rc > 0) {
			list_add_tail(&(page->list), head); written += rc;
		} else {
			putpage(page, FOR_TX_USE);
			return rc;
		}
	}

	tx->cur_size += written;

	return size;
}

/**
 *	read_tx_queue - 读发送队列数据
 *
 *	@tx:		LINK发送缓冲区
 *
 *	return
 *		>	0	成功读入的字节数
 *		==	0	队列中无任何数据
 *		<	0	失败
 */
static int
read_tx_queue(
	IN struct link_buff *tx)
{
	struct list_head	*head	= &(tx->head);
	struct list_head	*temp	= head->next;
	struct page_head	*page;

	if(IS_NULL(tx->page))
		return gen_errno(0, GCEINVAL);

	putpage(tx->page, FOR_TX_USE); tx->page = NULL;

	/*	发送队列中还有数据吗?*/
	if(list_empty(head))
		return 0;

	page = list_entry(temp, struct page_head, list);
	list_del(temp);

	tx->page	= page;
	tx->cur_size-= page->size;

	return page->size;
}

/**
 *	_iocp_send - iocp 发送数据
 *
 *	@slot:			LINK槽
 *	@link_no:		LINKNO
 *	@buffer:		数据缓冲区
 *	@size:			数据缓冲区的大小
 *
 *	return
 *		>	0		成功
 *		==	0		保留(不可能返回)
 *		<	0		失败
 */
static int
_iocp_send(
	IN struct link_slot *slot,
	IN linkno_t link_no,
	IN void *buffer, IN size_t size)
{
	int					rc;
	struct link_buff	*tx = slot->tx_buff;

	if(is_overdue_linkno(
		slot->link_no, link_no))
		return gen_errno(0, GCELINKNO);

	if(slot->defer_close)
		return size;	/*	已经延迟关闭,一个假的成功返回*/

	/*	正在发送中吗*/
	if(IS_NOT_NULL(tx->page))
		return write_tx_queue(tx, buffer, size);

	/*	没有正在发送中*/
	tx->page = getpage(FOR_TX_USE);
	if(IS_NULL(tx->page))
		return gen_errno(0, GCENOBUFS);

	rc = pgwrite(tx->page, tx->page->size, buffer, size);
	if(rc < 0)
		return rc;
	else if(rc == 0)
		return gen_errno(0, GCEABORT);
	else {
		if(size > (size_t)rc) {	/*	还有遗留数据,写入队列中*/
			rc = write_tx_queue(tx,
				(char*)buffer + rc, size - rc);
			if(rc < 0)
				return rc;
			else if(rc == 0)
				return gen_errno(0, GCEABORT);
		} else if(size == (size_t)rc) {
		} else
			return gen_errno(0, GCEABORT);
	}

	assert(rc > 0);

	rc = ll_iocp_send(slot);
	if(rc)
		return rc;
	else
		return size;
}

/**
 *	gcs_aio_commit_send - GCS 异步IO 提交发送
 *
 *	@link_no:		LINKNO
 *	@buffer:		数据缓冲区
 *	@size:			数据缓冲区的大小
 *
 *	return
 *		>	0		成功
 *		==	0		保留(不可能返回)
 *		<	0		失败
 */
int
gcs_aio_commit_send(IN linkno_t link_no,
	IN void *buffer, IN size_t size)
{
	struct link_slot	*slot;
	int					rc;

	if(size == 0)
		return gen_errno(0, GCEINVAL);

	slot = ref_link_slot(link_no);
	if(IS_NULL(slot))	
		return gen_errno(0, GCEINVAL);
	
	lock_link_slot(slot);
	rc = _iocp_send(slot, link_no, buffer, size);
	unlock_link_slot(slot);

	return rc;
}

/**
 *	_iocp_send_continue - 继续发送数据
 *
 *	@slot:			LINK槽
 *	@link_no:		LINKNO
 *
 *	return
 *		>	0		成功
 *		==	0		成功(队列无数据再发送)
 *		<	0		失败
 *
 *	remark
 *		本接口用于WSASend成功消息被递达后
 */
static int
_iocp_send_continue(
	IN struct link_slot *slot, IN linkno_t link_no)
{
	int					rc;

	if(is_overdue_linkno(
		slot->link_no, link_no))
		return gen_errno(0, GCELINKNO);

	rc = read_tx_queue(slot->tx_buff);
	if(rc < 0)
		return rc;
	else if(rc == 0) {	/**	send queue has no more data been sent*/
		if(slot->defer_close)	/*	延迟关闭标志已设置*/
			return _hard_close_link(slot, link_no);
		else
			return rc;
	} else {
		int	sent = rc;
		rc = ll_iocp_send(slot);
		if(rc)
			return rc;
		else
			return sent;
	}
}

/**
 *	iocp_send_continue - 继续发送数据
 *
 *	@link_no:		LINKNO
 *	@size:			成功发送的字节
 *
 *	return
 *		>	0		成功
 *		==	0		成功(队列无数据再发送)
 *		<	0		失败
 *
 *	remark
 *		本接口用于WSASend成功消息被递达后
 */
static int
iocp_send_continue(IN linkno_t link_no, IN size_t size)
{
	struct link_stat	*stat = lnk_stat_object();
	struct link_slot	*slot;
	int					rc;
	
	slot = ref_link_slot(link_no);
	if(IS_NULL(slot))	
		return gen_errno(0, GCEINVAL);

	if(slot->passive == FOR_SERVER)
		CNT64_T_ADD(&(stat->srv_sent), size);
	else
		CNT64_T_ADD(&(stat->cli_sent), size);

	lock_link_slot(slot);
	rc = _iocp_send_continue(slot, link_no);
	unlock_link_slot(slot);

	return rc;
}

/**
 *	gcs_aio_complete_sent - GCS 异步IO 成功完成发送
 *
 *	@link_no:		LINKNO
 *	@size:			成功发送的字节
 *
 *	return
 *		无
 */
void
gcs_aio_complete_sent(IN linkno_t link_no, IN size_t size)
{
	if(iocp_send_continue(link_no, size) < 0)
		gcs_hard_close_link(link_no);
}

/**
 *	_switch_client_link_2_transfering - 切换CLIENT LINK进入传输状态
 *
 *	@cli_slot:		客服端的LINK槽
 *	@srv_slot:		服务器的LINK槽
 *	@client:		客服端的LINKNO
 *	@server:		服务器的LINKNO
 *	@buffer:		要发送的数据
 *	@size:			要发送的数据的大小
 *
 *	return
 *		0			成功
 *		!0			失败
 *
 *	remark
 *		本操作完成映射关系的建立 CLIENT-->SERVER关系
 *	包括连接成功建立的消息响应给CLIENT
 */
static int
_switch_client_link_2_transfering(
	IN struct link_slot *cli_slot,
	IN struct link_slot *srv_slot,
	IN linkno_t client, IN linkno_t server,
	IN char *buffer, IN size_t size)
{
	int		rc;

	UNREFERENCED_PARAMETER(srv_slot);

	if(is_overdue_linkno(
		cli_slot->link_no, client))
		return gen_errno(0, GCELINKNO);

	/**	为客服端的LINK槽设置对等的SERVER LINKNO*/
	cli_slot->peer_no = server;

	/*切换成正在传输状态*/
	cli_slot->status = S_TRANSFERRING;
	rc = _iocp_send(cli_slot, client, buffer, size);
	if(rc < 0) {
		cli_slot->status = S_OPENING_FWD;	/*	状态退回*/
		return rc;
	} else if(rc == 0) {
		cli_slot->status = S_OPENING_FWD;	/*	状态退回*/
		return gen_errno(0, GCEABORT);
	} else
		return 0;
}

/**
 *	switch_client_link_2_transfering - 切换CLIENT LINK进入传输状态
 *
 *	@action:		动作码(GcType)
 *	@client:		客服端的LINKNO
 *	@server:		服务器的LINKNO
 *
 *	return
 *		0			成功
 *		!0			失败
 *
 *	remark
 *		本操作完成映射关系的建立 CLIENT-->SERVER关系
 *	包括连接成功建立的消息响应给CLIENT
 */
static int
switch_client_link_2_transfering(IN unsigned short action,
					IN linkno_t client, IN linkno_t server)
{
	char				package[MAX_HTTP_PKG_SIZE];
	int					rc;
	struct link_slot	*cli_slot;
	struct link_slot	*srv_slot;

	cli_slot = ref_link_slot(client);
	if(IS_NULL(cli_slot))
		return gen_errno(0, GCELINKNO);

	srv_slot = ref_link_slot(server);
	if(IS_NULL(srv_slot))
		return gen_errno(0, GCEINVAL);

	rc = build_open_package(action,
			cli_slot->parser.unitno,
			cli_slot->parser.tunnel,
			0, package, sizeof(package));
	if(rc < 0)
		return rc;
	else if(rc == 0)
		return gen_errno(0, GCEABORT);

	lock_link_slot(cli_slot);
	rc = _switch_client_link_2_transfering(
		cli_slot, srv_slot, client, server, package, rc);
	unlock_link_slot(cli_slot);

	return rc;
}

/**
 *	copy_unitno_and_tunnel - 复制unitno和tunnel值
 *
 *	@srv_slot:			SERVER的LINK槽
 *	@cli_slot:			CLIENT的LINK槽
 *
 *	return
 *		无
 *
 *	remark
 *		注意本函数未做任何的加锁操作,因为从逻辑上
 *	我们可以保证它,注意它仅仅在alloc_link_for_server
 *	被调用.仔细理解所有的逻辑.你就知道为什么不用加锁
 */
static void
copy_unitno_and_tunnel(
	IN struct link_slot *srv_slot,
	IN struct link_slot *cli_slot)
{
	if(IS_NOT_NULL(cli_slot) &&
		IS_NOT_NULL(srv_slot)) {
		srv_slot->parser.unitno = cli_slot->parser.unitno;
		srv_slot->parser.tunnel = cli_slot->parser.tunnel;
	}
}

/**
 *	reset_link_buff - 重置link缓冲区
 *
 *	@buff:		LINK缓冲区
 *	@t:			当前UNIX时间
 *	@purpose:	目的
 *
 *	return
 *		无
 */
static void
reset_link_buff(
	IN struct link_buff *buff, IN time_t t, IN int purpose)
{
	struct list_head *head = &(buff->head);
	struct list_head *list;
	struct list_head *n;
	struct page_head *pg;
	
	list_for_each_safe(list, n, head) {
		pg = list_entry(list, struct page_head, list);
		putpage(pg, purpose);
	}
	
	INIT_LIST_HEAD(head);

	if(IS_NOT_NULL(buff->page)) {
		putpage(buff->page, purpose); buff->page = NULL;
	}

	buff->mtime = t;
	buff->cur_size = 0;
	buff->lmt_size = ref_config_object()->bytes_per_link;
}

/**
 *	reset_link_slot - 重置LINK槽
 *
 *	@slot:		LINK槽
 *	@link_no:	LINKNO
 *	@peer_no:	对等LINKNO
 *	@s:			SOCKET描述符
 *	@passive:	被动/主动
 *	@status:	状态
 *	@ipv4:		目标IP地址(big endian)
 *	@port:		目标端口(big endian)
 *	@from:		来至端口(big endian)
 *
 *	return
 *		无
 */
static void
reset_link_slot(IN struct link_slot *slot,
	IN linkno_t link_no, IN linkno_t peer_no,
	IN int s, IN int passive, IN int status,
	IN unsigned int ipv4, IN unsigned short port,
	IN unsigned short from)
{
	struct http_parser	*parser = &(slot->parser);
	struct wt_parser	*wtp	= &(slot->wtp);

	slot->link_no		= link_no;
	if(slot->s != INVALID_SOCKET) {
		_close_connection_logtrace(slot);
		if(ref_config_object()->rst_close)
			ne_fast_close(slot->s);
		else
			ne_close(slot->s);
	}
	slot->peer_no		= peer_no;
	slot->s				= s;
	slot->passive		= passive & 0x1;
	slot->defer_close	= 0; 
	slot->status		= status & 0x3;
	slot->ipv4			= ipv4;
	slot->port			= port;
	slot->lsnr			= from;
	http_parser_reset(parser);
	wt_parser_reset(wtp);
}


/**
 *	reset_link - 重置LINK
 *
 *	@slot:		LINK槽
 *	@link_no:	LINKNO
 *	@peer_no:	对等LINKNO
 *	@s:			SOCKET描述符
 *	@passive:	被动/主动
 *	@status:	状态
 *	@ipv4:		目标IP地址(big endian)
 *	@port:		目标端口(big endian)
 *	@from:		来至端口(big endian)
 *
 *	return
 *		无
 */
static void reset_link(IN struct link_slot *slot,
	IN linkno_t link_no, IN linkno_t peer_no,
	IN int s, IN int passive, IN int status,
	IN unsigned int ipv4, IN unsigned short port,
	IN unsigned short from)
{
	time_t t = time(NULL);

	reset_link_slot(slot, link_no, peer_no,
		s, passive, status, ipv4, port, from);

	reset_link_buff(slot->rx_buff, t, FOX_RX_USE);
	reset_link_buff(slot->tx_buff, t, FOR_TX_USE);
}

/**
 *	alloc_link_for_server - 分配一个SERVER链路
 *
 *	@peer_no:		对等LINKNO
 *	@s:				SOCKET描述符
 *	@action:		动作码(GcType)
 *	@ipv4:			目标IP地址(big endian)
 *	@port:			目标端口(big endian)
 *
 *	return
 *		0		成功
 *		!0		失败
 */
int
alloc_link_for_server(IN linkno_t peer_no,
	IN int s, IN unsigned short action,
	IN unsigned int ipv4, IN unsigned short port)
{
	int					rc;
	linkno_t			link_no;
	struct link_slot	*slot;
	
	link_no = new_linkno(FOR_SERVER);
	if(link_no == INVALID_LINKNO) {
		gcs_logical_close_link(peer_no);
		return gen_errno(0, GCENORES);
	}
	
	slot = ref_link_slot(link_no);
	
	lock_link_slot(slot);
	reset_link(slot, link_no, peer_no,
		s, FOR_SERVER, S_TRANSFERRING, ipv4, port, 0);
	unlock_link_slot(slot);

	rc = ll_iocp_bind(slot);
	if(rc) {
		del_linkno(FOR_SERVER, link_no);
		gcs_logical_close_link(peer_no);
		return rc;
	}

	/**	复制CLIENT的LINK槽中http_parser引擎中的
		unitno 和 tunnel 数据过来*/
	copy_unitno_and_tunnel(slot, ref_link_slot(peer_no));

	/**	切换CLIENT LINK进入数据传输状态*/
	rc = switch_client_link_2_transfering(action, peer_no, link_no);
	if(rc) {
		del_linkno(FOR_SERVER, link_no);
		gcs_logical_close_link(peer_no);
		return rc;
	}
	
	rc = ll_iocp_recv(slot);
	if(rc) {
		del_linkno(FOR_SERVER, link_no);
		gcs_logical_close_link(peer_no);
		return rc;
	} else
		return 0;
}

/**
 *	alloc_link_for_client - 分配一个CLIENT链路
 *
 *	@s:				SOCKET描述符
 *	@ipv4:			目标IP地址(big endian)
 *	@port:			目标端口(big endian)
 *	@from:			来至端口(big endian)
 *
 *	return
 *		0		成功
 *		!0		失败
 */
int
alloc_link_for_client(
	IN int s, IN unsigned int ipv4,
	IN unsigned short port, IN unsigned short from)
{
	int					rc;
	linkno_t			link_no;
	struct link_slot	*slot;

	link_no = new_linkno(FOR_CLIENT);
	if(link_no == INVALID_LINKNO)
		return gen_errno(0, GCENORES);

	slot = ref_link_slot(link_no);

	lock_link_slot(slot);
	reset_link(slot, link_no, INVALID_LINKNO,
		s, FOR_CLIENT, S_WAITING_CHAN, ipv4, port, from);
	unlock_link_slot(slot);

	rc = ll_iocp_bind(slot);
	if(rc) {
		del_linkno(FOR_CLIENT, link_no);
		return rc;
	}

	rc = ll_iocp_recv(slot);
	if(rc) {
		del_linkno(FOR_CLIENT, link_no);
		return rc;
	} else
		return 0;
}

/**
 *	free_link - 释放一个LINK槽
 *
 *	@link_no:		LINKNO
 *
 *	return
 *		无
 */
static void
free_link(IN linkno_t link_no)
{
	struct link_slot	*slot;
	linkno_t			peer_no;

	unsigned int		passive;
	unsigned int		ipv4;
	unsigned short		port;

	slot = ref_link_slot(link_no);

#ifdef _DEBUG
	assert(slot != NULL);
#endif
	
	/*	slot->passive尽量防止统计错误*/
	lock_link_slot(slot);
	peer_no = slot->peer_no;
	passive = slot->passive;
	ipv4	= slot->ipv4;
	port	= slot->port;
	reset_link(slot, INVALID_LINKNO, INVALID_LINKNO,
		INVALID_SOCKET, slot->passive, S_WAITING_CHAN, 0, 0, 0);
	unlock_link_slot(slot);

	del_linkno(passive, link_no);

	_free_connection_logtrace(
		passive, ipv4, port);

	/*	关闭对等链路*/
	gcs_logical_close_link(peer_no);
}

/**
 *	gcs_free_link - 释放一个LINK槽
 *
 *	@link_no:		LINKNO
 *
 *	return
 *		无
 */
void gcs_free_link(IN linkno_t link_no)
{
	free_link(link_no);
}

/**
 *	do_gca_hello - do HELLO 命令
 *
 *	@slot:		LINK槽
 *	@parser:	HTTP解析器
 *
 *	return
 *		0		成功
 *		!0		失败
 */
static int
do_gca_hello(
	IN struct link_slot *slot, IN struct http_parser *parser)
{
	char	package[MAX_HTTP_PKG_SIZE];
	int		rc;

	if(slot->status != S_WAITING_CHAN)
	{
		char	_nm[256];//MOCK:HACK test point
		write_log(GCS_MODULE_ID,
			LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
			"detect illegal command sequence attack: "
			"this behavior might cause of lost data. "
			"closing via listen port %d connection %s "
			"(S: S_WAITING_CHAN) status: %d",
			ntohs(slot->lsnr),
			ne_inet_ntopx(
				slot->ipv4, slot->port, _nm, sizeof(_nm)),
			slot->status);
		return gen_errno(0, GCELOGIC);
	}

	parser->bit3 = 1;				/*	加快HTTP解析速度*/

	rc = build_hello_package(0, parser->channo,
						0, package, sizeof(package));
	if(rc < 0)
		return rc;
	else if(rc == 0)
		return gen_errno(0, GCEABORT);

	rc = gcs_aio_commit_send(slot->link_no, package, rc);
	if(rc < 0)
		return rc;
	else if(rc == 0)
		return gen_errno(0, GCEABORT);
	else {
		slot->status = S_WAITING_FWD;/*	切换状态到等待OPEN命令*/
		return 0;
	}
}

/**
 *	do_gca_open - do open 命令
 *
 *	@slot:		LINK槽
 *	@parser:	HTTP解析器
 *	@action:	动作码
 *
 *	return
 *		0		成功
 *		!0		失败
 */
static int
do_gca_open(IN struct link_slot *slot,
	IN struct http_parser *parser, IN unsigned short action)
{
	int		rc;

	if(slot->status != S_WAITING_FWD)
	{
		char	_nm[256];//MOCK:HACK test point
		write_log(GCS_MODULE_ID,
			LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
			"detect illegal command sequence attack: "
			"this behavior might cause of lost data. "
			"closing via listen port %d connection %s "
			"(S: S_WAITING_FWD) status: %d",
			ntohs(slot->lsnr),
			ne_inet_ntopx(
				slot->ipv4, slot->port, _nm, sizeof(_nm)),
			slot->status);
		return gen_errno(0, GCELOGIC);
	}

	if(parser->ip == INADDR_NONE || parser->ip == 0) {
		return gen_errno(0, GCEDNS);
	}

	slot->status = S_OPENING_FWD;	/*	切换状态到等待转发连接建立*/
	rc = gcs_aio_commit_connect(parser->ip, parser->port,
							slot->link_no, parser->unitno,
								parser->tunnel, action);
	if(rc) {
		char	_nm[256];

		slot->status = S_WAITING_FWD;	/*	状态退回*/

		write_log(GCS_MODULE_ID,
			LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
			"An error(0x%08X) occurred while "
			"calling gcs_aio_commit_connect(%s)", rc,
			ne_inet_ntopx(parser->ip,
				parser->port, _nm, sizeof(_nm)));
		return rc;
	} else {
		char	_nm[256];

		if(ref_config_object()->log_level >= LOG_LEVEL_SUPPORT)
			write_log(GCS_MODULE_ID,
				LOG_TYPE_INFO, LOG_LEVEL_SUPPORT,
				"Opening forwarded connection to %s",
				ne_inet_ntopx(parser->ip,
					parser->port, _nm, sizeof(_nm)));
	}

	parser->bit1 = 1;				/*	加快HTTP解析速度*/
	parser->bit2 = 1;				/*	加快HTTP解析速度*/

	return 0;
}

/**
 *	do_gca_transfer - do transfer 命令
 *
 *	@slot:		LINK槽
 *	@parser:	HTTP解析器
 *
 *	return
 *		0		成功
 *		!0		失败
 */
static int
do_gca_transfer(
	IN struct link_slot *slot, IN struct http_parser *parser)
{
	UNREFERENCED_PARAMETER(parser);

	if(slot->status != S_TRANSFERRING)
	{
		char	_nm[256];//MOCK:HACK test point
		write_log(GCS_MODULE_ID,
			LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
			"detect illegal command sequence attack: "
			"this behavior might cause of lost data. "
			"closing via listen port %d connection %s "
			"(S: S_TRANSFERRING) status: %d",
			ntohs(slot->lsnr),
			ne_inet_ntopx(
				slot->ipv4, slot->port, _nm, sizeof(_nm)),
			slot->status);
		return gen_errno(0, GCELOGIC);
	}
	else
		return 0;
}

/**
 *	do_wt_pkg1_cbk - 处理委托第一个包的回调
 *
 *	@user:		用户数据(LINK槽)
 *
 *	return
 *		0		成功
 *		!0		失败
 */
static int
do_wt_pkg1_cbk(IN void *user)
{
	char	package[MAX_HTTP_PKG_SIZE];
	int		rc;
	struct link_slot *slot = user;
	struct http_parser *parser = &(slot->parser);

	/*	返回响应包给GCC*/
	rc = build_wt_package1(
		parser->unitno, parser->tunnel,
		package, sizeof(package));
	if(rc < 0)
		return rc;
	else if(rc == 0)
		return gen_errno(0, GCEABORT);

	rc = gcs_aio_commit_send(slot->link_no, package, rc);
	if(rc < 0)
		return rc;
	else if(rc == 0)
		return gen_errno(0, GCEABORT);
	else
		return 0;
}

/**
 *	do_wt_pkg2_cbk - 处理委托第二个包的回调
 *
 *	@user:		用户数据(LINK槽)
 *
 *	return
 *		0		成功
 *		!0		失败
 */
static int
do_wt_pkg2_cbk(IN void *user)
{
	char	package[MAX_HTTP_PKG_SIZE];
	int		rc;
	struct link_slot *slot = user;
	struct http_parser *parser = &(slot->parser);

	/*	组织IP报告包给SERVER*/
	rc = build_wt_packagen(
			slot->ipv4, slot->port,
			package, sizeof(package));
	if(rc < 0)
		return rc;
	else if(rc == 0)
		return gen_errno(0, GCEABORT);

	rc = gcs_aio_commit_send(slot->peer_no, package, rc);
	if(rc < 0)
		return rc;
	else if(rc == 0)
		return gen_errno(0, GCEABORT);

	/*	返回响应包给GCC*/
	rc = build_wt_package2(
		parser->unitno, parser->tunnel,
		package, sizeof(package));
	if(rc < 0)
		return rc;
	else if(rc == 0)
		return gen_errno(0, GCEABORT);
	
	rc = gcs_aio_commit_send(slot->link_no, package, rc);
	if(rc < 0)
		return rc;
	else if(rc == 0)
		return gen_errno(0, GCEABORT);
	else
		return 0;
}

/**
 *	do_http_headers_cbk - 处理HTTP头回调
 *
 *	@user:		用户数据(LINK槽)
 *	@parser:	HTTP解析器
 *
 *	return
 *		0		成功
 *		!0		失败
 */
static int
do_http_headers_cbk(
	IN void *user, IN struct http_parser *parser)
{
	int					rc = 0;
	struct link_slot	*slot = user;

	switch(parser->action) {
	case GCA_HELLO:
		rc = do_gca_hello(slot, parser);
		break;
	case GCA_OPEN:
		rc = do_gca_open(slot, parser, GCA_OPEN);
		break;
	case GCA_OPEN2:
		wt_parser_create(&(slot->wtp));	/*	设置WT解析引擎*/
		rc = do_gca_open(slot, parser, GCA_OPEN2);
		break;
	case GCA_TRANSFER:
		rc = do_gca_transfer(slot, parser);
		break;
	case GCA_CLOSE:
		rc = -1;/*	需要关闭链路*/
		break;
	default:
		rc = 0;	/*	不识别的命令,都算成功*/
		break;
	}
	return rc;
}

/**
 *	do_http_bodyctx_cbk - 处理BODY头回调
 *
 *	@user:		用户数据(LINK槽)
 *	@parser:	HTTP解析器
 *	@buffer:	数据缓冲区
 *	@size:		数据缓冲区的大小
 *
 *	return
 *		0		成功
 *		!0		失败
 */
static int
do_http_bodyctx_cbk(
	IN void *user, IN struct http_parser *parser,
	IN void *buffer, size_t size)
{
	int					rc;
	struct link_slot	*slot = user;

	if(slot->wtp.user != slot ||
		slot->wtp.wt_pkg1_cbk != do_wt_pkg1_cbk ||
		slot->wtp.wt_pkg2_cbk != do_wt_pkg2_cbk)
	{
		write_log(GCS_MODULE_ID,
			LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
			"%s oops! An serious wtp error occurred! "
			"0x%08X, 0x%08X, 0x%08X, "
			"0x%08X, 0x%08X, 0x%08X", GCSERVER_NAME,
			slot->wtp.user, slot,
			slot->wtp.wt_pkg1_cbk, do_wt_pkg1_cbk,
			slot->wtp.wt_pkg2_cbk, do_wt_pkg2_cbk);
		return gen_errno(0, GCEABORT);
	}

	if(slot->status == S_TRANSFERRING &&
		parser->action == GCA_TRANSFER)
	{
		/**	wtp是一个黑洞解析引擎 black hold engine*/
		/**
		 *	wtp 解析引擎的返回值是他不能吞入的数据
		 *	不能吞入的数据,就是需要转发的,非WT LINK
		 *	这里总是返回rc=size的.这里是个巧妙的地方
		 */
		rc = do_wt_parse(&(slot->wtp), buffer, size);
		if(rc < 0)
			return rc;
		else if(rc == 0)
			return 0;

		if(size > (size_t)rc) {
			write_log(GCS_MODULE_ID,
				LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
				"%s oops! An serious error occurred! (%d)(%d)",
				GCSERVER_NAME, size, rc);
			return gen_errno(0, GCEABORT);
		}

		/**	仅当进入数据传输状态和动作码是数据传输,才传输*/
		rc = gcs_aio_commit_send(slot->peer_no,
						(char*)buffer + size - rc, rc);
		if(rc < 0)
			return rc;
		else if(rc == 0)
			return gen_errno(0, GCEABORT);
		else
			return 0;
	} else
		return 0;
}

/**
 *	do_client_link_forward - 处理CLIENT链路的转发
 *
 *	@wtab:			单词表
 *	@slot:			LINK槽
 *	@rx:			LINK接收缓冲区
 *
 *	return
 *		>=	0		成功转发的字节数
 *		<	0		失败
 */
static int
do_client_link_forward(
	IN struct word_table *wtab,
	IN struct link_slot *slot,
	IN struct link_buff *rx)
{
	struct http_parser	*parser = &(slot->parser);
	struct page_head	*page = rx->page;

	if(parser->user != slot ||
		parser->bodyctx_cbk != do_http_bodyctx_cbk ||
		parser->headers_cbk != do_http_headers_cbk)
	{
		write_log(GCS_MODULE_ID,
			LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
			"%s oops! An serious http error occurred! "
			"0x%08X, 0x%08X, 0x%08X, "
			"0x%08X, 0x%08X, 0x%08X", GCSERVER_NAME,
			parser->user, slot,
			parser->bodyctx_cbk, do_http_bodyctx_cbk,
			parser->headers_cbk, do_http_headers_cbk);
		return gen_errno(0, GCEABORT);
	}

	return do_http_parse(wtab, parser,
						page->buffer, page->size);
}

/**
 *	do_server_link_forward - 处理SERVER链路的转发
 *
 *	@wtab:			单词表
 *	@slot:			LINK槽
 *	@rx:			LINK接收缓冲区
 *
 *	return
 *		>=	0		成功转发的字节数
 *		<	0		失败
 */
static int
do_server_link_forward(
	IN struct word_table *wtab,
	IN struct link_slot *slot,
	IN struct link_buff *rx)
{
	char	package[MAX_HTTP_PKG_SIZE];
	struct http_parser	*parser = &(slot->parser);
	struct page_head	*page = rx->page;
	int		rc;

	UNREFERENCED_PARAMETER(wtab);

	/**	组织HTTP响应包*/
	rc = build_transfer_package(
			parser->unitno, parser->tunnel,
			page->buffer, page->size, 
			package, sizeof(package));
	if(rc < 0)
		return rc;
	else if(rc == 0)
		return gen_errno(0, GCEABORT);

	/**	转发HTTP响应包*/
	rc = gcs_aio_commit_send(slot->peer_no, package, rc);
	if(rc < 0)
		return rc;
	else if(rc == 0)
		return gen_errno(0, GCEABORT);

	return page->size;
}

/**
 *	_iocp_recv - IOCP 成功完成接收
 *
 *	@wtab:			单词表
 *	@link_no:		LINKNO
 *	@size:			成功接收的字节
 *
 *	return
 *		0			成功
 *		!0			失败
 */
static int
_iocp_recv(
	IN struct word_table *wtab,
	IN linkno_t link_no, IN size_t size)
{
	int					rc;
	struct link_slot	*slot;
	struct link_buff	*rx;
	struct link_stat	*stat = lnk_stat_object();

	slot = ref_link_slot(link_no);
	if(IS_NULL(slot))
		return gen_errno(0, GCEABORT);
	else
		rx = slot->rx_buff;

	rx->mtime = time(NULL);
	rx->page->size += size;

	if(slot->passive == FOR_SERVER)
		CNT64_T_ADD(&(stat->srv_received), size);
	else
		CNT64_T_ADD(&(stat->cli_received), size);

	if(slot->passive == FOR_CLIENT)
		rc = do_client_link_forward(wtab, slot, rx);
	else
		rc = do_server_link_forward(wtab, slot, rx);

	if(rc > 0) {
		if(rc > MPAGE_SIZE ||
			(size_t)rc > rx->page->size)
		{
			write_log(GCS_MODULE_ID,
				LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
				"%s oops! An serious error occurred "
				"while forwarding data (%d)(%d)(%d)",
				GCSERVER_NAME, rc, rx->page->size, slot->passive);
			return gen_errno(0, GCEABORT);
		}

		rx->page->size -= rc;
		memmove(rx->page->buffer,
			rx->page->buffer + rc, rx->page->size);
		return ll_iocp_recv(slot);
	} else if(rc == 0) {
		if(rx->page->size >= MPAGE_SIZE) {
			char	_nm[256];//MOCK:HACK test point
			write_log(GCS_MODULE_ID,
				LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
				"detect illegal attack: this behavior might "
				"cause of http engine death loop. closing "
				"via listen port %d connection %s", slot->lsnr,
				ne_inet_ntopx(slot->ipv4, slot->port, _nm, sizeof(_nm)));
			return -1;
		} else
			return ll_iocp_recv(slot);	/*	压力测试发现的一个问题,导致LINK没能得到释放*/
	} else
		return rc;
}

/**
 *	gcs_aio_complete_recv - GCS 异步IO 成功完成接收
 *
 *	@wtab:			单词表
 *	@link_no:		LINKNO
 *	@size:			成功接收的字节
 *
 *	return
 *		无
 */
void gcs_aio_complete_recv(
	IN struct word_table *wtab,
	IN linkno_t link_no, IN size_t size)
{
	if(_iocp_recv(wtab, link_no, size))
		free_link(link_no);
}

/**
 *	check_all_connection_timeout - 检查所有的连接的超时
 *
 *	return
 *		无
 */
void
check_all_connection_timeout()
{
	char					_nm[256];
	struct link_slot_tab	*obj = lnktable_object();
	struct link_slot		*slot;
	int						i;
	unsigned int			timeout = ref_config_object()->timeout;
	time_t					curtime = time(NULL);
	linkno_t				link_no;
	unsigned int			ipv4;
	unsigned short			port;
	unsigned short			from;

	if(timeout == 0)
		return;	/*	不做任何的检查*/

	slot = obj->slot;
	for(i = 0; i < obj->nr_links; i++, slot++)
	{
		if(slot->link_no != INVALID_LINKNO &&
			__builtin_is_timeout(curtime,
				slot->rx_buff->mtime, timeout) &&
				slot->status != S_TRANSFERRING)
		{
			/**	加锁进一步进行准确的判断*/
			lock_link_slot(slot);
			if(slot->link_no != INVALID_LINKNO &&
				__builtin_is_timeout(curtime,
					slot->rx_buff->mtime, timeout) &&
					slot->status != S_TRANSFERRING)
			{
				link_no = slot->link_no;
				ipv4	= slot->ipv4;
				port	= slot->port;
				from	= slot->lsnr;
			}
			unlock_link_slot(slot);

			write_log(GCS_MODULE_ID,
				LOG_TYPE_INFO, LOG_LEVEL_GENERAL,
				"connection %s (%d) is timeout. "
				"exceed %d seconds does not convert to TRANSFERRING status",
				ne_inet_ntopx(ipv4, port, _nm, sizeof(_nm)), ntohs(from), timeout);

			gcs_hard_close_link(link_no);
		}
	}
}

/**
 *	make_link_module_ctrl_buffer - 构建link模块的控制台命令缓冲区
 *
 *	@buffer:		缓冲区
 *	@size:			缓冲区最大值
 *
 *	return
 *		无
 */
void
make_link_module_ctrl_buffer(
	IN char *buffer, IN size_t size)
{
	struct linkno_s_pool *obj = lnk_pool_object();
	struct link_stat *stat = lnk_stat_object();
	unsigned __int64 *p1 = (unsigned __int64 *)&(stat->srv_received);
	unsigned __int64 *p2 = (unsigned __int64 *)&(stat->srv_sent);
	unsigned __int64 *p3 = (unsigned __int64 *)&(stat->srv_committed);
	unsigned __int64 *p4 = (unsigned __int64 *)&(stat->cli_received);
	unsigned __int64 *p5 = (unsigned __int64 *)&(stat->cli_sent);
	unsigned __int64 *p6 = (unsigned __int64 *)&(stat->cli_committed);

	_snprintf(buffer, size,
		"--- Statistics for Global Connections ---\n"
		"Current Connections         = %u\n""\n"
		"--- Statistics for Server Connections ---\n"
		"Current Connections         = %u\n"
		"Bytes Received              = %I64u\n"
		"Bytes Sent                  = %I64u\n"
		"Bytes Committed             = %I64u\n""\n"
		"--- Statistics for Client Connections ---\n"
		"Current Connections         = %u\n"
		"Bytes Received              = %I64u\n"
		"Bytes Sent                  = %I64u\n"
		"Bytes Committed             = %I64u\n",
		obj->cur_using,
		stat->nr_server, *p1, *p2, *p3,
		stat->nr_client, *p4, *p5, *p6);
}

/**
 *	refresh_link_mon - 刷新link监控数据
 *
 *	@mon:		监控数据结构
 *
 *	return
 *		无
 */
void
refresh_link_mon(IN struct gcs_mon *mon)
{
	struct link_stat *stat = lnk_stat_object();
	unsigned __int64 *p1 = (unsigned __int64 *)&(stat->srv_received);
	unsigned __int64 *p2 = (unsigned __int64 *)&(stat->srv_sent);
	unsigned __int64 *p3 = (unsigned __int64 *)&(stat->srv_committed);
	unsigned __int64 *p4 = (unsigned __int64 *)&(stat->cli_received);
	unsigned __int64 *p5 = (unsigned __int64 *)&(stat->cli_sent);
	unsigned __int64 *p6 = (unsigned __int64 *)&(stat->cli_committed);

	mon->nr_srv_links	=	stat->nr_server;
	mon->srv_received	=	*p1;
	mon->srv_sent		=	*p2;
	mon->srv_committed	=	*p3;

	mon->nr_cli_links	=	stat->nr_client;
	mon->cli_received	=	*p4;
	mon->cli_sent		=	*p5;
	mon->cli_committed	=	*p6;
}

