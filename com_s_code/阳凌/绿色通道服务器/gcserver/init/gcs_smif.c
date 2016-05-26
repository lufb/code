/**
 *	init/gcs_smif.c
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
#include "pthread.h"
#include "event.h"
#include "native.h"
#include "crypto/crc16.h"
#include "gcs_sock.h"
#include "gcs_errno.h"
#include "gcs_smif.h"
#include "gcs_main.h"
#include "gcs_version.h"
#include "gcs_ctrl.h"
#include "gcs_config.h"

#ifndef lint
static const char rcsid[] _U_ =
	"@(#) $Id: init/gcs_smif.c,"
	"v 1.00 2013/11/01 09:33:40 yangling Exp $ (LBL)";
#endif

#pragma pack(1)
struct _4X_HEADER {
	unsigned char		magic;			/*	协议签名*/
	unsigned int		body_size:13;	/*	数据体的长度*/
	unsigned int		flags:3;		/*	数据包状态*/
	unsigned int		checksum:16;	/*	校验码 CRC16*/
	unsigned char		main;			/*	数据包主类型*/
	unsigned char		child;			/*	数据包辅助类型*/
	unsigned short		seqno;			/*	请求编号*/
	short				err_code;		/*	错误号*/
};

static struct proc_info {				/*	1-3	进程信息*/
	int 				pid;			/*	pid*/
	char				description[64];/*	进程描述*/
};

static struct module_info {				/*	模块信息*/
	unsigned short		module_id;		/*	模块编号*/
	unsigned short		module_type;	/*	模块类型*/
	unsigned short		parent_id;		/*	父模块编号*/
	char				module_name[32];/*	模块名称*/
	char				module_descr[40];/*	模块描述*/
};

static struct module_status_header {	/*	1-18模块服务状态*/
	int					pid;			/*	pid*/
	unsigned short		module_id;		/*	模块编号*/
	char				status;			/*	模块状态*/
};

static struct module_verbose_status_header {/*	1-7模块状态信息*/
	unsigned short		module_id;		/*	模块编号*/
	int 				pid;			/*	pid*/
	size_t				offset;			/*	状态偏移*/
	unsigned long		size;			/*	状态长度*/
};

static struct log_header {				/*	1-8 日志头*/
	unsigned char		log_type;		/*	日志类别*/
	unsigned long		log_level;		/*	日志等级*/
	unsigned long		log_date;		/*	日志日期*/
	unsigned long		log_time;		/*	日志时间*/
	unsigned short		module_id;		/*	模块编号*/
	int					pid;			/*	pid*/
};

static struct cmd_header {				/*	1-10指令头*/
	union {
		unsigned short	finish_flags;	/*	本次指令完成标志*/
		unsigned short	module_id;		/*	模块编号*/
	}_un_less1;
	unsigned int		cmd_no;			/*	指令序号*/
};
#pragma pack()

static struct smconn {
	unsigned short		errors;			/*	出现错误的次数*/
	int					pid;			/*	进程号*/
	int					s;				/*	SOCKET*/
	unsigned int		ip;				/*	IP地址*/
	unsigned short		port;			/*	端口*/
	char				first;			/*	首次标志*/
	char				status;			/*	链路状态*/
	size_t				size;			/*	数据大小*/
	struct futex_event	event;			/*	事件*/
	pthread_mutex_t		lock;			/*	锁*/
	char				*buffer;		/*	数据指针*/
} conn2sm;
typedef struct smconn	smconn_t;

/**
 *	__txdata - 发送网络数据
 *
 *	@conn:			连接句柄
 *	@buffer:		发送的数据
 *	@size:			发送的数据大小
 *
 *	return
 *		>=		0	成功
 *		<		0	失败
 */
static int
__txdata(IN smconn_t *conn, IN char *buffer, IN size_t size)
{
	int rc;
	int	tx = 0;
	
	while(tx < (int)size) {
		rc = send(conn->s, buffer + tx, size - tx, 0);
		if(rc < 0)
			return gen_errno(_OSerrno(), GCEOS);
		tx += rc;
	}
	
	return size;
}

/*
 *	_txdata - 发送网络数据
 *
 *	@conn:			连接句柄
 *	@buffer:		发送的数据
 *	@size:			发送的数据大小
 *
 *	return
 *		>=		0	成功
 *		<		0	失败
 */
static int
_txdata(IN smconn_t *conn, IN char *buffer, IN size_t size)
{
	int	rc;

	pthread_mutex_lock(&(conn->lock));
	rc = __txdata(conn, buffer, size);
	pthread_mutex_unlock(&(conn->lock));
	
	return rc;
}

/**
 *	tx_heartbeat - 向ServiceManager 发送心跳包
 *
 *	return
 *		>=		0	成功
 *		<		0	失败
 */
static int
tx_heartbeat()
{
	char	buffer[_4X_PKG_SIZE_];
	struct _4X_HEADER	*_4xhdr;
	
	_4xhdr	= (struct _4X_HEADER *)buffer;
	
	_4xhdr->magic = '#';
	_4xhdr->body_size = 0;
	_4xhdr->flags = 0;
	_4xhdr->checksum = 0;
	_4xhdr->main = 0;
	_4xhdr->child = 0;
	_4xhdr->seqno = 40002;
	_4xhdr->err_code = 0;
	
	return _txdata(&conn2sm, buffer, sizeof(struct _4X_HEADER));
}

/*
 *	repair_link - 修复链路
 *
 *	说明:
 *	同样发送(1,3)协议,但是它有点区别是利用strDescription字段
 *	设置的是0x80,那么就表示是更新LinkNo
 *	ServiceManager相应的协议中会有这个方面的处理
 *
 *	return
 *		>=		0	成功
 *		<		0	失败
 */
static int
repair_link()
{
	char	buffer[_4X_PKG_SIZE_];
	size_t offset_tab[] = {
		0,
			sizeof(struct _4X_HEADER),
			sizeof(struct _4X_HEADER) + sizeof(struct proc_info)
	};
	
	struct _4X_HEADER	*_4xhdr;
	struct module_info	*module;
	struct proc_info	*proc;
	
	_4xhdr	= (struct _4X_HEADER *)		(buffer + offset_tab[0]);
	proc	= (struct proc_info *)		(buffer + offset_tab[1]);
	module	= (struct module_info *)	(buffer + offset_tab[2]);
	
	proc->pid = conn2sm.pid;
	memset(proc->description, 0x80, sizeof(proc->description));
	
	_4xhdr->magic = '#';
	_4xhdr->body_size = sizeof(struct module_info) +sizeof(struct proc_info);
	_4xhdr->flags = 0;
	_4xhdr->checksum = crc16((char *)proc, _4xhdr->body_size, 0);
	_4xhdr->main = 1;
	_4xhdr->child= 3;
	_4xhdr->seqno= 0;
	_4xhdr->err_code = 0;
	
	return _txdata(&conn2sm, buffer,
		sizeof(struct _4X_HEADER) + _4xhdr->body_size);
}

/**
 *	register_module - 向ServiceManager 注册模块
 *
 *	@module_id:		模块ID
 *
 *	return
 *		>=		0	成功
 *		<		0	失败
 */
static int
register_module(IN int module_id)
{
	char	buffer[_4X_PKG_SIZE_];
	size_t offset_tab[] = {
		0,
		sizeof(struct _4X_HEADER),
		sizeof(struct _4X_HEADER) + sizeof(struct proc_info)
	};

	struct _4X_HEADER	*_4xhdr;
	struct module_info	*module;
	struct proc_info	*proc;
	
	memset(buffer, 0, sizeof(buffer));
	
	_4xhdr	= (struct _4X_HEADER *)		(buffer + offset_tab[0]);
	proc	= (struct proc_info *)		(buffer + offset_tab[1]);
	module	= (struct module_info *)	(buffer + offset_tab[2]);
	
	
	proc->pid = conn2sm.pid;
	{
		char	_ver[256];
		_snprintf(proc->description,
			sizeof(proc->description), "%s %s",
			GCSERVER_NAME, global_public_version_str(_ver, sizeof(_ver)));
	}
	module->module_id	= (unsigned short)module_id;
	module->module_type = 0xF101;
	module->parent_id	= 0xFFFF;
	strcpy(module->module_name, GCSERVER_NAME);
	strcpy(module->module_descr, GCSERVER_NAME);
	
	_4xhdr->magic = '#';
	_4xhdr->body_size = sizeof(struct module_info) +sizeof(struct proc_info);
	_4xhdr->flags = 0;
	_4xhdr->checksum = crc16((char *)proc, _4xhdr->body_size, 0);
	_4xhdr->main = 1;
	_4xhdr->child= 3;
	_4xhdr->seqno= 0;
	_4xhdr->err_code = 0;
	
	return _txdata(&conn2sm, buffer,
		sizeof(struct _4X_HEADER) + _4xhdr->body_size);
}

/**
 *	smif_refresh_module_status - 向ServiceManager 刷新模块的服务状态
 *
 *	@module_id:		模块ID
 *	@c:				状态
 *
 *	return
 *		>=		0	成功
 *		<		0	失败
 */
int
smif_refresh_module_status(IN int module_id, IN char c)
{
	char	buffer[_4X_PKG_SIZE_];
	size_t offset_tab[] = {
		0,
		sizeof(struct _4X_HEADER)
	};
	struct _4X_HEADER			*_4xhdr;
	struct module_status_header	*status_hdr;

	_4xhdr		= (struct _4X_HEADER *)			(buffer + offset_tab[0]);
	status_hdr	= (struct module_status_header *)(buffer + offset_tab[1]);

	status_hdr->module_id = (unsigned short)module_id;
	status_hdr->pid = conn2sm.pid;
	status_hdr->status = c;

	_4xhdr->magic = '#';
	_4xhdr->body_size = sizeof(struct module_status_header);
	_4xhdr->flags = 0;
	_4xhdr->checksum = crc16((char *)status_hdr, _4xhdr->body_size, 0);
	_4xhdr->main = 1;
	_4xhdr->child = 18;
	_4xhdr->seqno = 40006;
	_4xhdr->err_code = 0;

	return _txdata(&conn2sm, buffer,
		sizeof(struct _4X_HEADER) + _4xhdr->body_size);
}

/**
 *	smif_refresh_module_verbose_status - 向ServiceManager 刷新模块的状态信息
 *
 *	@module_id:		模块ID
 *	@si:			状态缓冲区
 *	@size:			状态缓冲区大小
 *
 *	return
 *		>=		0	成功
 *		<		0	失败
 */
int
smif_refresh_module_verbose_status(
	IN int module_id, IN char *si, IN size_t size)
{
	char	buffer[_4X_PKG_SIZE_];
	size_t offset_tab[] = {
		0,
		sizeof(struct _4X_HEADER),
		sizeof(struct _4X_HEADER) + 
					sizeof(struct module_verbose_status_header)
	};
	struct _4X_HEADER					*_4xhdr;
	struct module_verbose_status_header	*status_hdr;
	char								*status_info;

	_4xhdr		= (struct _4X_HEADER *)	(buffer + offset_tab[0]);
	status_hdr	= (struct module_verbose_status_header *)
										(buffer + offset_tab[1]);
	status_info	= (char*)				(buffer + offset_tab[2]);

	memcpy(status_info, si, size);
	status_hdr->module_id = (unsigned short)module_id;
	status_hdr->offset = 0;
	status_hdr->pid = conn2sm.pid;
	status_hdr->size = size;

	_4xhdr->magic = '#';
	_4xhdr->body_size = sizeof(struct module_verbose_status_header) + size;
	_4xhdr->flags = 0;
	_4xhdr->checksum = crc16((char *)status_hdr, _4xhdr->body_size, 0);
	_4xhdr->main = 1;
	_4xhdr->child = 7;
	_4xhdr->seqno = 40002;
	_4xhdr->err_code = 0;

	return _txdata(&conn2sm, buffer,
		sizeof(struct _4X_HEADER) + _4xhdr->body_size);
}

/**
 *	push_log - 向ServiceManager 推送日志
 *
 *	@module_id:		模块ID
 *	@log_type:		日志类型
 *	@log_level:		日志等级
 *	@log:			日志内容
 *	@logsz:			日志内容大小
 *
 *	return
 *		>=		0	成功
 *		<		0	失败
 */
static int
push_log(IN int module_id, IN unsigned char log_type,
	IN unsigned char log_level, IN char *log, IN size_t logsz)
{
	char	buffer[_4X_PKG_SIZE_];
	size_t offset_tab[] = {
		0,
		sizeof(struct _4X_HEADER),
		sizeof(struct _4X_HEADER) + sizeof(struct log_header)
	};
	struct _4X_HEADER	*_4xhdr;
	struct log_header	*log_hdr;
	char				*ptr;
	time_t				cur_time	= time(NULL);
	struct tm			_tmbuf = {0};
	struct tm		*	_tm			= localtime(&cur_time);
	unsigned int		_date, _time;

	if(_tm == NULL)
		_tm = &_tmbuf;

	_date = (_tm->tm_year + 1900) * 10000 + (_tm->tm_mon + 1) * 100 + _tm->tm_mday;
	_time = _tm->tm_hour * 10000 + _tm->tm_min * 100 + _tm->tm_sec;

	_4xhdr	= (struct _4X_HEADER *)(buffer + offset_tab[0]);
	log_hdr	= (struct log_header *)(buffer + offset_tab[1]);
	ptr		= (char*)			(buffer + offset_tab[2]);

	memcpy(ptr, log, logsz);
	log_hdr->module_id = (unsigned short)module_id;
	log_hdr->pid = conn2sm.pid;
	log_hdr->log_date = _date;
	log_hdr->log_time = _time;
	log_hdr->log_level = log_level;
	log_hdr->log_type = log_type;

	_4xhdr->magic = '#';
	_4xhdr->body_size = sizeof(struct log_header) + logsz;
	_4xhdr->flags = 0;
	_4xhdr->checksum = crc16((char *)log_hdr, _4xhdr->body_size, 0);
	_4xhdr->main = 1;
	_4xhdr->child = 8;
	_4xhdr->seqno = 40001;
	_4xhdr->err_code = 0;

	return _txdata(&conn2sm, buffer,
		sizeof(struct _4X_HEADER) + _4xhdr->body_size);
}

static unsigned short color[LOG_TYPE_ALARM + 1] =
{
	FOREGROUND_GREEN | FOREGROUND_BLUE,
	FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	FOREGROUND_RED | FOREGROUND_GREEN,
	FOREGROUND_RED,
	FOREGROUND_RED
};

/**
 *	write_log - 向 SM 推送日志(接口函数)
 *
 *	@moduile:	模块ID,值是在第一次做接口协商的时候平台分配的
 *	@log_type:	日志类型
 *	@log_level: 日志等级
 *	@fmt:		变参数(类型 printf族函数)
 *
 *	return
 *		无返回值
 */
void
write_log(
	IN int module_id, IN unsigned char log_type,
	IN unsigned char log_level, IN const char *fmt, ...)
{
	int		rc;
	va_list	ap;
	char	buffer[_4X_PKG_DATA_SIZE_];

	va_start(ap, fmt);
	rc = _vsnprintf(buffer, 
		_4X_PKG_DATA_SIZE_ - sizeof(struct log_header), fmt, ap);
	va_end(ap);

	if(rc < 0)
		rc = _4X_PKG_DATA_SIZE_ - sizeof(struct log_header);

	push_log(module_id, log_type, log_level, buffer, rc);

#ifdef _DEBUG
	{
		char	fmt[32];

		if(log_type > LOG_TYPE_ALARM)
			log_type = LOG_TYPE_ALARM;

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color[log_type]);
		sprintf(fmt, "%%%d.%ds\n", rc, rc);
		printf(fmt, buffer);
	}
#endif
}

/**
 *	forward_ctl_cmd_result - 向ServiceManager 转发控制命令的结果
 *
 *	@finish_flags:		转发包标志 (> 0 final pkg == 0 中间包)
 *	@cmd_no:			命令序号
 *	@result:			包体报文
 *	@size:				包体报文大小
 *
 *	return
 *		>=		0	成功
 *		<		0	失败
 */
static int
forward_ctl_cmd_result(IN int finish_flags,
	IN unsigned int cmd_no, IN char *result, IN size_t size)
{
	char	buffer[_4X_PKG_SIZE_];
	size_t offset_tab[] = {
		0,
		sizeof(struct _4X_HEADER),
		sizeof(struct _4X_HEADER) + sizeof(struct cmd_header)
	};
	struct _4X_HEADER	*_4xhdr;
	struct cmd_header	*cmd_hdr;
	char				*ptr;

	_4xhdr	= (struct _4X_HEADER *)(buffer + offset_tab[0]);
	cmd_hdr	= (struct cmd_header *)(buffer + offset_tab[1]);
	ptr		= (char*)			(buffer + offset_tab[2]);

	memcpy(ptr, result, size);
	cmd_hdr->_un_less1.finish_flags = (unsigned short)finish_flags;
	cmd_hdr->cmd_no = cmd_no;

	_4xhdr->magic = '#';
	_4xhdr->body_size = sizeof(struct cmd_header) + size;
	_4xhdr->flags = 0;
	_4xhdr->checksum = crc16((char *)cmd_hdr, _4xhdr->body_size, 0);
	_4xhdr->main = 1;
	_4xhdr->child = 10;
	_4xhdr->seqno = 40003;
	_4xhdr->err_code = 0;

	return _txdata(&conn2sm, buffer,
		sizeof(struct _4X_HEADER) + _4xhdr->body_size);
}

/**
 *	smif_ctrl_cmd_print - 向 SM 转发控制命令的结果(非最后包)
 *
 *	@cmd_no:	命令序号
 *	@fmt:		变参数(类型 printf族函数)
 *
 *	return
 *		无
 */
void
smif_ctrl_cmd_print(
	IN unsigned int cmd_no, IN const char *fmt, ...)
{
	int		rc;
	va_list	ap;
	char	buffer[_4X_PKG_DATA_SIZE_];

	va_start(ap, fmt);
	rc = _vsnprintf(buffer, 
		_4X_PKG_DATA_SIZE_ - sizeof(struct cmd_header), fmt, ap);
	va_end(ap);

	if(rc < 0)
		rc = _4X_PKG_DATA_SIZE_ - sizeof(struct cmd_header);

	forward_ctl_cmd_result(0, cmd_no, buffer, rc);
}

/**
 *	smif_ctrl_cmd_println - 向 SM 转发控制命令的结果(最后包)
 *						
 *	@cmd_no:	命令序号
 *	@fmt:		变参数(类型 printf族函数)
 *
 *	return
 *		无
 */
void
smif_ctrl_cmd_println(
	IN unsigned int cmd_no, IN const char *fmt, ...)
{
	int		rc;
	char	buffer[_4X_PKG_DATA_SIZE_];
	va_list	ap;

	va_start(ap, fmt);
	rc = _vsnprintf(buffer, 
		_4X_PKG_DATA_SIZE_ - sizeof(struct cmd_header), fmt, ap);
	va_end(ap);

	if(rc < 0)
		rc = _4X_PKG_DATA_SIZE_ - sizeof(struct cmd_header);

	forward_ctl_cmd_result(1, cmd_no, buffer, rc);
}

/**
 *	conn_init - 连接参数的初始化
 *
 *	@conn:			连接句柄
 *
 *	return
 *		无
 */
static void
conn_init(IN smconn_t *conn)
{
	char	smport[CONFIG_VALUE_LENGTH];

	memset(smport, 0, sizeof(smport));
	load_key_value("../servicemanager.ini", "COMM",
		"LISTENPORT", "6001", smport, sizeof(smport));

	memset(conn, 0, sizeof(smconn_t));

	conn->s		= INVALID_SOCKET;
	conn->size	= 0;
	conn->status= 0;
	conn->port	= htons((unsigned short)atol(smport));
	conn->ip	= inet_addr("127.0.0.1");
	conn->first	= 0;
	conn->buffer= NULL;
	conn->pid	= GetCurrentProcessId();
	conn->errors= 0;
	futex_event_init(&(conn->event));
	pthread_mutex_init(&(conn->lock), NULL);
}

/**
 *	conn_reset - 连接参数重置
 *
 *	@conn:			连接句柄
 *
 *	return
 *		无
 */
static void
conn_reset(IN smconn_t *conn)
{
	conn->status= 1;
}

/**
 *	conn_close - 连接关闭,并且设置状态
 *
 *	@conn:			连接句柄
 *
 *	return
 *		无
 */
static void
conn_close(IN smconn_t *conn)
{
	conn->status = 0;	/*	this status is very important*/
	if(conn->s != INVALID_SOCKET) {
		ne_fast_close(conn->s);
		conn->s = INVALID_SOCKET;
	}
	conn->errors++;
}

/**
 *	__calc_token - 计算参数的令牌
 */
static char *__calc_token(char *s, const char *delim,
							char **save_ptr)
{
# define __rawmemchr strchr
  char *token;

  if (s == NULL)
    s = *save_ptr;

  /* Scan leading delimiters.  */
  s += strspn (s, delim);
  if (*s == '\0')
    {
      *save_ptr = s;
      return NULL;
    }

  /* Find the end of the token.  */
  token = s;
  s = strpbrk (token, delim);
  if (s == NULL)
    /* This token finishes the string.  */
    *save_ptr = __rawmemchr (token, '\0');
  else
    {
      /* Terminate the token and make *SAVE_PTR point past it.  */
      *s = '\0';
      *save_ptr = s + 1;
    }
  return token;
}

static char*   __builtin_trimright(char* s)
{
    register    i, k;

    /*  输入为空则直接返回*/
	if(!s) return NULL;
	i = strlen(s) - 1;
	k = i;
	while(0 <= i && (' ' == s[i] || 9 == s[i]) || 10 == s[i] || 13 == s[i])
		i--;

	if(i == k)
		return s;
	else if(0 <= i)
		s[i+1] = 0;
	else
		s[0]   = 0;
	return s;
}

/**
 *	do_exec_cmd - 执行一个脚本命令
 *
 *	@buffer:		一个完整的4X数据包体
 *	@size:			一个完整的4X数据包体
 *
 *	return
 *		无
 */
static void
do_exec_cmd(IN char *buffer, IN size_t size)
{
#define	NR_CTL_CMD_ARG	8192
	char				arg_buff[_4X_PKG_SIZE_];
	char				*argv[NR_CTL_CMD_ARG];
	int					argc = 0;
	struct cmd_header	*cmd_hdr;
	char				*ptr;
	char				*seps = " \t\"";
	char				*lasts = NULL;

	if(size < sizeof(struct cmd_header))
		return;

	cmd_hdr = (struct cmd_header *)buffer;

	memset(argv, 0, sizeof(argv));
	ptr		= (char *)buffer + sizeof(struct cmd_header);
	memset(arg_buff, 0, sizeof(arg_buff));
	memcpy(arg_buff, ptr, size - sizeof(struct cmd_header));
	__builtin_trimright(arg_buff);

#define	__PUSH_ARG(a, b, c) do {(a)[(b)] = c; (b)++;} while(0)
	ptr = __calc_token(arg_buff, seps, &lasts);
	if(ptr) {
		__PUSH_ARG(argv, argc, ptr);
		for(;;) {
			ptr = __calc_token(NULL, seps, &lasts);
			if(ptr)
				__PUSH_ARG(argv, argc, ptr);
			else
				break;
		}
	}

	global_ctrl_command_entry(cmd_hdr->cmd_no, argc, argv);
}

/**
 *	do_conn_recv - 处理连接接收的数据请求
 *
 *	@buffer:		一个完整的4X数据包
 *	@size:			一个完整的4X数据包的大小
 *
 *	return
 *		无
 */
static void
do_data_rq(IN char *buffer, IN size_t size)
{
	struct _4X_HEADER		*_4xhdr;

	_4xhdr = (struct _4X_HEADER *)buffer;

	if(_4xhdr->main == 1) {
		switch(_4xhdr->child) {
		case 3:	/*	模块注册的返回 FIXME*/
			break;
		case 10:/*	请求执行控制命令*/
			do_exec_cmd(buffer + sizeof(struct _4X_HEADER), 
						size - sizeof(struct _4X_HEADER));
			break;
		case 11:/*	服务管理器退出消息*/
			exit(0);
			break;
		case 88:/*	单停*/
			break;
		default:
			break;
		}
	}
}

/**
 *	do_4x_protocol_parse - 处理协议的解析
 *
 *	@conn:			连接句柄
 *
 *	return
 *		!0			失败
 *		0			成功
 */
static int
do_4x_protocol_parse(IN smconn_t *conn)
{
	size_t					off = 0;
	size_t					pkgsz;
	struct _4X_HEADER		*_4xhdr;
	
	for(;;) {
		pkgsz  = conn->size - off;

		/*	数据不够头的长度*/
		if(pkgsz < sizeof(struct _4X_HEADER))
			break;

		_4xhdr = (struct _4X_HEADER *)(conn->buffer + off);
		/*	数据签名不正确*/
		if(_4xhdr->magic != '#')
			return -1;

		/*	长度域错误*/
		if(_4xhdr->body_size > _4X_PKG_DATA_SIZE_)
			return -1;

		/*	包体长度不够*/
		if(_4xhdr->body_size + sizeof(struct _4X_HEADER) > pkgsz)
			break;

		do_data_rq((char*)_4xhdr, _4xhdr->body_size + sizeof(struct _4X_HEADER));

		off += _4xhdr->body_size + sizeof(struct _4X_HEADER);
	}
	
	if(off > 0) {
		conn->size -= off;
		memmove(conn->buffer, conn->buffer + off, conn->size);
	}

	return 0;
}


/**
 *	routine_with_sm - 手机服务平台与服务管理器通讯过程
 *
 *	@arg:		线程参数
 *
 *	return
 *		无意义
 */
static void *
routine_with_sm(IN void *arg)
{
	char				buffer[_4X_PKG_SIZE_];
	struct gcs_thread	*gth = arg;
	smconn_t			*conn = &conn2sm;
	fd_set				_fdset;
	int					rc;
	struct timeval		tv;

	local_gcs_thread_init(gth);

	conn->buffer = buffer;

	for(;;) {
		if(conn->status)
		{
			/*	链路是活动的*/
			FD_ZERO(&_fdset);
			FD_SET((unsigned int)conn->s, &_fdset);

			tv.tv_sec = 30;
			tv.tv_usec= 0;
			gcs_thread_sleep(gth);
			rc = select(0, &_fdset,  NULL, NULL, &tv); 
			gcs_thread_wake(gth);
			if(rc < 0)
			{
				/*	切断任何与SM的通讯 MOCK:HACK*/
				noarch_sleep(30000);
				continue;
			}
			else if (rc == 0)
			{
				/**	超时了,发送心跳吧*/
				tx_heartbeat();
				continue;
			}

			if(FD_ISSET(conn->s, &_fdset))
			{
				rc = recv(conn->s, 
					conn->buffer + conn->size,
					_4X_PKG_SIZE_ - conn->size, 0);
				if(rc <= 0) {
					conn_close(conn);
					continue;
				}
				else
				{
					conn->size += rc;
				}

				rc = do_4x_protocol_parse(conn);
				if(rc)
				{
					conn_close(conn);
					continue;
				}
			}
		}
		else
		{
#define	NR_SM_ERRORS	1024

			/**	与SM的通讯出现了1024次错误,不再和SM打交道了*/
			/**	这个措施有助于减少SM的SOCKET泄露 MOCK:HACK*/
			if(conn->errors > NR_SM_ERRORS)
			{
				for(;;)
					noarch_sleep(INFINITE);
			}

			/*	链路不是活动的*/
			conn->s = ne_connect2(conn->ip, conn->port);
			if(conn->s > 0)
			{
				ne_setnonblocking(conn->s);
				conn_reset(conn);
			}

			if(conn->s > 0 && conn->first) {
				/*
				 *	不是第一次连接成功,那么就修复链路.
				 *	如果有未注册完的模块继续注册
				 */
				repair_link();
				/*
				 *	这里稍做睡眠,由于SM上设计的缺陷
				 *	本来可以利用reparir_link的返回来继续驱动的
				 *	但是遗憾的是reparir_link 不再驱动了
				 *	为了安全起见,睡眠的意思就是期待SM把修复链路
				 *	的操作已经成功完成了,然后再注册.
				 */
			}

			if(conn->s > 0 && !conn->first) {
				/*	唤醒主线程,表示SM状态OK*/
				futex_event_post(&(conn->event));
				conn->first = 1;
			}
		}
	}

	return NULL;
}

/**
 *	global_smif_module_init - 全局初始化smif模块
 *
 *	return
 *		0		成功
 *		!0		失败
 */
int
global_smif_module_init()
{
	char				name[256];
	pthread_t			thread;
	int					rc;
	struct gcs_thread	*gth;
	smconn_t			*conn = &conn2sm;

	conn_init(conn);

	memset(name, 0, sizeof(name));
	sprintf(name, "SMIF%04d", 0);

	gth = alloc_gcs_thread_object(name);
	if(IS_NULL(gth))
		return gen_errno(0, GCEABORT);

	/**	启动与SM通讯的线程*/
	rc = pthread_create(&thread, NULL,
				routine_with_sm, gth);
	if(rc)
		return rc;
	else {
		futex_event_wait(&(conn->event));
		register_module(GCS_MODULE_ID);
		noarch_sleep(1000);
		return 0;
	}
}
