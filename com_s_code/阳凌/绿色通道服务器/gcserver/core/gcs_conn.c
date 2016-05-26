/**
 *	core/gcs_conn.c
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
#include "event.h"
#include "native.h"
#include "gcs_link.h"
#include "gcs_conn.h"
#include "gcs_sock.h"
#include "gcs_errno.h"
#include "gcs_http.h"
#include "gcs_config.h"
#include "gcs_smif.h"
#include "gcs_main.h"
#include "asm-x86/atomic.h"
#include "bits/gcs_cnt64.h"

#ifndef lint
static const char rcsid[] _U_ =
	"@(#) $Id: core/gcs_conn.c,"
	"v 1.00 2013/10/29 11:01:40 yangling Exp $ (LBL)";
#endif

static struct conn_descr {
	struct list_head		list;		/*	链节点*/
	unsigned int			ip;			/*	地址*/
	linkno_t				launcher;	/*	连接请求发起者*/
	unsigned short			action;		/*	动作码GcType*/
	unsigned short			port;		/*	端口*/
	int						s;			/*	SOCKET描述符*/
	unsigned short			_unitno;	/*	异步信息(单元号)*/
	unsigned short			_tunnel;	/*	异步信息(隧道号)*/
};

static struct conn_request_queue {
	struct list_head		queue_head;	/*	队列链表头*/
	struct list_head		free_head;	/*	空闲链表头*/
	pthread_mutex_t			qlck;		/*	队列锁*/
	pthread_mutex_t			flck;		/*	空闲锁*/
	struct conn_descr		*buff;		/*	连接描述符缓冲区*/
	struct futex_event		event;		/*	事件句柄*/

	/**	下面的指标用于统计数据*/
	unsigned short			nr_useable;	/*	可使的请求项 flck*/
	unsigned short			nr_queuing;	/*	正在队列中的 qlck*/
	atomic_t				nr_pending;	/*	正在连接中的*/

	cnt64_t					nr_succ;	/*	成功的*/
	cnt64_t					nr_fail;	/*	失败的*/
	cnt64_t					nr_abort;	/*	异常的*/
}__conn_req_queue_object__;
#define	crq_object()	(&__conn_req_queue_object__)

static struct poll_header {
	struct list_head		head;
	unsigned short			nr_poll;
};
#define	poll_queue_header(_ph_)	(&((_ph_)->head))

/**
 *	alloc_connect_descriptor - 分配一个连接描述符
 *
 *	return
 *		NULL		失败
 *		!NULL		成功
 */
static struct conn_descr *
alloc_connect_descriptor()
{
	struct conn_request_queue	*obj = crq_object();
	pthread_mutex_t				*lck = &(obj->flck);
	struct list_head			*hdr= &(obj->free_head);
	struct list_head			*tmp;

	pthread_mutex_lock(lck);
	if(list_empty(hdr))
		tmp = NULL;
	else {
		tmp = hdr->next;
		list_del(tmp);
		obj->nr_useable--;
	}
	pthread_mutex_unlock(lck);

	if(tmp)
		return list_entry(tmp, struct conn_descr, list);
	else
		return NULL;
}

/**
 *	free_connect_descriptor - 归还一个连接描述符
 *
 *	@descr:		连接描述符
 *
 *	return
 *		无
 */
static void
free_connect_descriptor(IN struct conn_descr *descr)
{
	struct conn_request_queue	*obj = crq_object();
	pthread_mutex_t				*lck = &(obj->flck);
	struct list_head			*hdr = &(obj->free_head);
	struct list_head			*lst =&(descr->list);

	pthread_mutex_lock(lck);
	list_add_tail(lst, hdr);
	obj->nr_useable++;
	pthread_mutex_unlock(lck);
}

/**
 *	connect_request_enqueue - 投递一个连接请求到连接请求队列
 *
 *	@rq:			连接请求描述符
 *
 *	return
 *		0			成功
 *		!0			失败
 */
static int
connect_request_enqueue(IN struct conn_descr *rq)
{
	struct conn_request_queue	*obj = crq_object();
	pthread_mutex_t				*lck = &(obj->qlck);
	struct list_head			*hdr= &(obj->queue_head);
	struct list_head			*lst= &(rq->list);
	struct futex_event			*event=&(obj->event);

	pthread_mutex_lock(lck);
	list_add_tail(lst, hdr);
	obj->nr_queuing++;
	pthread_mutex_unlock(lck);

	if(futex_event_post(event))
		return gen_errno(_OSerrno(), GCEOS);
	else
		return 0;
}

/**
 *	get_connect_request - 从连接请求队列中获取一个请求
 *
 *	return
 *		NULL		无请求
 *		!NULL		连接请求描述符
 */
static struct conn_descr*
connect_request_dequeue()
{
	struct conn_request_queue	*obj = crq_object();
	pthread_mutex_t				*lck = &(obj->qlck);
	struct list_head			*hdr= &(obj->queue_head);
	struct list_head			*tmp;

	pthread_mutex_lock(lck);
	if(list_empty(hdr))
		tmp = NULL;
	else {
		tmp = hdr->next;
		list_del(tmp);
		obj->nr_queuing--;
	}
	pthread_mutex_unlock(lck);

	if(tmp)
		return list_entry(tmp, struct conn_descr, list);
	else
		return NULL;
}

/**
 *	do_success - 处理连接成功
 *
 *	@rq:		请求项
 *	@code:		错误代码
 *
 *	return
 *		无
 */
static void
do_success(IN struct conn_descr *rq, IN int code)
{
	int		rc;
	struct conn_request_queue	*obj = crq_object();

	UNREFERENCED_PARAMETER(code);

	/**	重新设置成阻塞SOCKET*/
	ne_setblocking(rq->s);

	rc = alloc_link_for_server(
		rq->launcher, rq->s, rq->action, rq->ip, rq->port);
	if(rc) {
		char	_nm[256];
		ne_fast_close(rq->s); rq->s = INVALID_SOCKET;
		write_log(GCS_MODULE_ID,
			LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
			"An error(0x%08X) occurred while "
			"allocating a link resource for "
			"server connection to %s", rc,
			ne_inet_ntopx(rq->ip, rq->port, _nm, sizeof(_nm)));

		CNT64_T_INC(&(obj->nr_abort));
	} else {
		char	_nm[256];

		if(ref_config_object()->log_level >= LOG_LEVEL_SUPPORT)
			write_log(GCS_MODULE_ID,
				LOG_TYPE_INFO, LOG_LEVEL_SUPPORT,
				"forwarded connection to %s is established",
				ne_inet_ntopx(rq->ip, rq->port, _nm, sizeof(_nm)));

		CNT64_T_INC(&(obj->nr_succ));
	}

	free_connect_descriptor(rq);
}

/**
 *	do_failure - 处理连接失败
 *
 *	@rq:		请求项
 *	@code:		错误代码
 *
 *	return
 *		无
 */
static void
do_failure(IN struct conn_descr *rq, IN int code)
{
	char	package[MAX_HTTP_PKG_SIZE];
	struct conn_request_queue	*obj = crq_object();
	int		rc;

	rc = build_open_package(rq->action,
			rq->_unitno, rq->_tunnel,
			code, package, sizeof(package));
	if(rc > 0) {
		gcs_aio_commit_send(rq->launcher, package, rc);
		gcs_logical_close_link(rq->launcher);
	}

	ne_fast_close(rq->s); rq->s = INVALID_SOCKET;

	{
		char	_nm[256];

		write_log(GCS_MODULE_ID,
			LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
			"An error(0x%08X) occurred while "
			"connecting to %s", code,
			ne_inet_ntopx(rq->ip, rq->port, _nm, sizeof(_nm)));
	}
	

	free_connect_descriptor(rq);

	CNT64_T_INC(&(obj->nr_fail));
}

/**
 *	poll_queue_init - 初始化拉取头
 *
 *	@ph:		拉取队列头
 *
 *	return
 *		无
 */
static void
poll_queue_init(IN struct poll_header *ph)
{
	INIT_LIST_HEAD(&(ph->head)); ph->nr_poll = 0;
}

/**
 *	poll_enqueue - 拉取队列入列
 *
 *	@ph:		拉取队列头
 *	@rq:		队列元素
 *
 *	return
 *		无
 */
static void
poll_enqueue(
	IN struct poll_header *ph,
	IN struct conn_descr *rq)
{
	struct conn_request_queue	*obj = crq_object();

	atomic_inc(&(obj->nr_pending));

	list_add_tail(&(rq->list), &(ph->head)); ph->nr_poll++;
}

/**
 *	poll_dequeue - 拉取队列出列
 *
 *	@ph:		拉取队列头
 *	@rq:		队列元素
 *
 *	return
 *		无
 */
static
void poll_dequeue(
	IN struct poll_header *ph,
	IN struct conn_descr *rq)
{
	struct conn_request_queue	*obj = crq_object();

	atomic_dec(&(obj->nr_pending));

	list_del(&(rq->list)); ph->nr_poll--;
}

/**
 *	poll_queue_full - 拉取队列满了吗?
 *
 *	@ph:		拉取队列头
 *
 *	return
 *		!0		满
 *		0		不满
 */
static int
poll_queue_full(IN struct poll_header *ph)
{
#define NR_SELECT_COUNTER	64
	if(ph->nr_poll < NR_SELECT_COUNTER)
		return 0;
	else
		return 1;
}

/**
 *	poll_queue_empty - 拉取队列空的吗?
 *
 *	@ph:		拉取队列头
 *
 *	return
 *		!0		空
 *		0		不空
 */
static int
poll_queue_empty(IN struct poll_header *ph)
{
	return list_empty(&(ph->head));
}

/**
 *	do_connect - 处理连接请求 main loop
 *
 *	@ph:			拉取队列头
 *	@event:			事件句柄
 *
 *	return
 *		无
 */
static
void do_connect(
	IN struct poll_header *ph,
	IN struct futex_event *event,
	IN struct gcs_thread *gth)
{
	fd_set					wrfds;
	struct timeval			tv;
	int						nothing;
	int						rc;
	struct conn_descr		*request;
	struct list_head		*list, *safe;
	

	/**	开始连接的线程工作*/
	for(;;) {
		nothing = 0;

		if(!poll_queue_full(ph)) {
			/*	从连接队列获取一个连接请求*/
			request = connect_request_dequeue();
			if(IS_NOT_NULL(request)) {
				nothing++;
				
				/**	异步连接*/
				rc = ne_asyn_connect(request->ip, request->port, &(request->s));
				if(rc < 0) {
					/**	失败*/
					do_failure(request, gen_errno(_OSerrno(), GCEOS));
				} else if(rc == 0) {
					/**	直接成功,理论上不可能*/
					do_success(request, 0);
				} else {
					/**	need select test*/
					poll_enqueue(ph, request);
				}
			}
		}

		/**	select链表有需要检测的项目吗*/
		if(!poll_queue_empty(ph)) {
			nothing++;

			FD_ZERO(&wrfds);
			/**	把所有需要检测的套节字加入*/
			list_for_each(list, poll_queue_header(ph)) {
				request = list_entry(list, struct conn_descr, list);
				FD_SET((unsigned int)request->s, &wrfds);
			}

			tv.tv_sec	= 0;
			tv.tv_usec	= 50 * 1000;
			/**	检测所有的套节子描述符*/
			if(select(0, NULL, &wrfds, NULL, &tv) > 0) {
				/**	有描述符被检测到*/
				list_for_each_safe(list, safe, poll_queue_header(ph)) {
					request = list_entry(list, struct conn_descr, list);
					if(FD_ISSET(request->s, &wrfds))
					{
						poll_dequeue(ph, request);
						do_success(request, 0);
					}
				}
			} else {
				/**	没有任何描述符被检测到*/
				list_for_each_safe(list, safe, poll_queue_header(ph)) {
					request = list_entry(list, struct conn_descr, list);
					rc = ne_getsock_errno(request->s);
					if(rc) {
						poll_dequeue(ph, request);
						do_failure(request, gen_errno(rc, GCEOS));
					}
				}
			}
		}

		/**	没事可做*/
		if(!nothing) {
			gcs_thread_sleep(gth);
			futex_event_timedwait(event, INFINITE);
			gcs_thread_wake(gth);
		}
	}
}

/**
 *	connect_routine - 连接线程
 *
 *	@arg:		线程参数
 *
 *	return
 *		NULL	无意义
 */
static void *
connect_routine(IN void *arg)
{
	struct poll_header			poll;
	struct conn_request_queue	*obj = crq_object();
	struct futex_event			*event=&(obj->event);
	struct gcs_thread			*gth = arg;
	
	local_gcs_thread_init(gth);

	poll_queue_init(&poll);

	do_connect(&poll, event, gth);

	return NULL;
}

/**
 *	make_conn_module_ctrl_buffer - 构建conn模块的控制台命令缓冲区
 *
 *	@buffer:		缓冲区
 *	@size:			缓冲区最大值
 *
 *	return
 *		无
 */
void
make_conn_module_ctrl_buffer(
	IN char *buffer, IN size_t size)
{
	struct conn_request_queue	*obj = crq_object();

	unsigned __int64 *succ = (unsigned __int64 *)&(obj->nr_succ);
	unsigned __int64 *fail = (unsigned __int64 *)&(obj->nr_fail);
	unsigned __int64 *abort= (unsigned __int64 *)&(obj->nr_abort);
	
	_snprintf(buffer, size,
		"Useable requests            = %u\n"
		"Queuing requests            = %u\n"
		"Pending requests            = %u\n"
		"Active Opens                = %I64u\n"
		"Failed Connection Attempts  = %I64u\n"
		"Aborted Connection Attempts = %I64u\n",
		obj->nr_useable, obj->nr_queuing,
		atomic_read(&(obj->nr_pending)), *succ, *fail, *abort);
}

/**
 *	global_conn_module_init - 全局连接处理模块初始化
 *
 *	return
 *		0			成功
 *		!0			失败
 */
int
global_conn_module_init()
{
	char						name[32];
	pthread_t					thread;
	struct conn_request_queue	*obj = crq_object();
	struct gcs_config			*cfg = ref_config_object();
	struct gcs_thread			*gth;
	size_t						size;
	int							i, rc;

	memset(obj, 0, sizeof(struct conn_request_queue));

	INIT_LIST_HEAD(&(obj->queue_head));
	INIT_LIST_HEAD(&(obj->free_head));

	pthread_mutex_init(&(obj->qlck), NULL);
	pthread_mutex_init(&(obj->flck), NULL);

	futex_event_init(&(obj->event));

#define	NR_CONNECT_REQUEST	65535
	obj->nr_useable = NR_CONNECT_REQUEST;
	size = NR_CONNECT_REQUEST *
			sizeof(struct conn_descr);
	obj->buff = (struct conn_descr*)zmalloc(size);
	if(IS_NULL(obj->buff))
		return gen_errno(0, GCENOMEM);

	for(i = 0; i < NR_CONNECT_REQUEST; i++)
		list_add_tail(&(obj->buff[i].list), &(obj->free_head));

	for(i = 0; i < cfg->conn_ths; i++) {
		memset(name, 0, sizeof(name));
		sprintf(name, "CONN%04d", i);
		gth = alloc_gcs_thread_object(name);
		if(IS_NULL(gth))
			return gen_errno(0, GCEABORT);

		rc = pthread_create(&thread, NULL, connect_routine, gth);
		if(rc)
			return rc;
	}

	return 0;
}

/**
 *	gcs_aio_commit_connect - 绿色通道发送一个异步的连接请求
 *
 *	@addr:		地址信息(big endian)
 *	@port:		端口信息(big endian)
 *	@launcher:	发起者
 *	@_unitno:	异步信息(单元号)
 *	@_tunnel:	异步信息(隧道号)
 *	@_action:	异步信息(动作码)
 *
 *	return
 *		0		成功
 *		!0		失败
 */
int gcs_aio_commit_connect(
	IN unsigned int ip,
	IN unsigned short port,
	IN linkno_t launcher,
	IN unsigned short _unitno,
	IN unsigned short _tunnel,
	IN unsigned short _action)
{
	struct conn_descr	*rq;

	rq = alloc_connect_descriptor();
	if(IS_NULL(rq))
		return gen_errno(0, GCENORES);

	rq->ip		= ip;
	rq->launcher= launcher;
	rq->port	= port;
	rq->_unitno	= _unitno;
	rq->_tunnel	= _tunnel;
	rq->s		= INVALID_SOCKET;
	rq->action	= _action;

	return connect_request_enqueue(rq);
}



