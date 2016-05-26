/**
 *	core/gcs_conn.c
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
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
	struct list_head		list;		/*	���ڵ�*/
	unsigned int			ip;			/*	��ַ*/
	linkno_t				launcher;	/*	������������*/
	unsigned short			action;		/*	������GcType*/
	unsigned short			port;		/*	�˿�*/
	int						s;			/*	SOCKET������*/
	unsigned short			_unitno;	/*	�첽��Ϣ(��Ԫ��)*/
	unsigned short			_tunnel;	/*	�첽��Ϣ(�����)*/
};

static struct conn_request_queue {
	struct list_head		queue_head;	/*	��������ͷ*/
	struct list_head		free_head;	/*	��������ͷ*/
	pthread_mutex_t			qlck;		/*	������*/
	pthread_mutex_t			flck;		/*	������*/
	struct conn_descr		*buff;		/*	����������������*/
	struct futex_event		event;		/*	�¼����*/

	/**	�����ָ������ͳ������*/
	unsigned short			nr_useable;	/*	��ʹ�������� flck*/
	unsigned short			nr_queuing;	/*	���ڶ����е� qlck*/
	atomic_t				nr_pending;	/*	���������е�*/

	cnt64_t					nr_succ;	/*	�ɹ���*/
	cnt64_t					nr_fail;	/*	ʧ�ܵ�*/
	cnt64_t					nr_abort;	/*	�쳣��*/
}__conn_req_queue_object__;
#define	crq_object()	(&__conn_req_queue_object__)

static struct poll_header {
	struct list_head		head;
	unsigned short			nr_poll;
};
#define	poll_queue_header(_ph_)	(&((_ph_)->head))

/**
 *	alloc_connect_descriptor - ����һ������������
 *
 *	return
 *		NULL		ʧ��
 *		!NULL		�ɹ�
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
 *	free_connect_descriptor - �黹һ������������
 *
 *	@descr:		����������
 *
 *	return
 *		��
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
 *	connect_request_enqueue - Ͷ��һ���������������������
 *
 *	@rq:			��������������
 *
 *	return
 *		0			�ɹ�
 *		!0			ʧ��
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
 *	get_connect_request - ��������������л�ȡһ������
 *
 *	return
 *		NULL		������
 *		!NULL		��������������
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
 *	do_success - �������ӳɹ�
 *
 *	@rq:		������
 *	@code:		�������
 *
 *	return
 *		��
 */
static void
do_success(IN struct conn_descr *rq, IN int code)
{
	int		rc;
	struct conn_request_queue	*obj = crq_object();

	UNREFERENCED_PARAMETER(code);

	/**	�������ó�����SOCKET*/
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
 *	do_failure - ��������ʧ��
 *
 *	@rq:		������
 *	@code:		�������
 *
 *	return
 *		��
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
 *	poll_queue_init - ��ʼ����ȡͷ
 *
 *	@ph:		��ȡ����ͷ
 *
 *	return
 *		��
 */
static void
poll_queue_init(IN struct poll_header *ph)
{
	INIT_LIST_HEAD(&(ph->head)); ph->nr_poll = 0;
}

/**
 *	poll_enqueue - ��ȡ��������
 *
 *	@ph:		��ȡ����ͷ
 *	@rq:		����Ԫ��
 *
 *	return
 *		��
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
 *	poll_dequeue - ��ȡ���г���
 *
 *	@ph:		��ȡ����ͷ
 *	@rq:		����Ԫ��
 *
 *	return
 *		��
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
 *	poll_queue_full - ��ȡ����������?
 *
 *	@ph:		��ȡ����ͷ
 *
 *	return
 *		!0		��
 *		0		����
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
 *	poll_queue_empty - ��ȡ���пյ���?
 *
 *	@ph:		��ȡ����ͷ
 *
 *	return
 *		!0		��
 *		0		����
 */
static int
poll_queue_empty(IN struct poll_header *ph)
{
	return list_empty(&(ph->head));
}

/**
 *	do_connect - ������������ main loop
 *
 *	@ph:			��ȡ����ͷ
 *	@event:			�¼����
 *
 *	return
 *		��
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
	

	/**	��ʼ���ӵ��̹߳���*/
	for(;;) {
		nothing = 0;

		if(!poll_queue_full(ph)) {
			/*	�����Ӷ��л�ȡһ����������*/
			request = connect_request_dequeue();
			if(IS_NOT_NULL(request)) {
				nothing++;
				
				/**	�첽����*/
				rc = ne_asyn_connect(request->ip, request->port, &(request->s));
				if(rc < 0) {
					/**	ʧ��*/
					do_failure(request, gen_errno(_OSerrno(), GCEOS));
				} else if(rc == 0) {
					/**	ֱ�ӳɹ�,�����ϲ�����*/
					do_success(request, 0);
				} else {
					/**	need select test*/
					poll_enqueue(ph, request);
				}
			}
		}

		/**	select��������Ҫ������Ŀ��*/
		if(!poll_queue_empty(ph)) {
			nothing++;

			FD_ZERO(&wrfds);
			/**	��������Ҫ�����׽��ּ���*/
			list_for_each(list, poll_queue_header(ph)) {
				request = list_entry(list, struct conn_descr, list);
				FD_SET((unsigned int)request->s, &wrfds);
			}

			tv.tv_sec	= 0;
			tv.tv_usec	= 50 * 1000;
			/**	������е��׽���������*/
			if(select(0, NULL, &wrfds, NULL, &tv) > 0) {
				/**	������������⵽*/
				list_for_each_safe(list, safe, poll_queue_header(ph)) {
					request = list_entry(list, struct conn_descr, list);
					if(FD_ISSET(request->s, &wrfds))
					{
						poll_dequeue(ph, request);
						do_success(request, 0);
					}
				}
			} else {
				/**	û���κ�����������⵽*/
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

		/**	û�¿���*/
		if(!nothing) {
			gcs_thread_sleep(gth);
			futex_event_timedwait(event, INFINITE);
			gcs_thread_wake(gth);
		}
	}
}

/**
 *	connect_routine - �����߳�
 *
 *	@arg:		�̲߳���
 *
 *	return
 *		NULL	������
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
 *	make_conn_module_ctrl_buffer - ����connģ��Ŀ���̨�������
 *
 *	@buffer:		������
 *	@size:			���������ֵ
 *
 *	return
 *		��
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
 *	global_conn_module_init - ȫ�����Ӵ���ģ���ʼ��
 *
 *	return
 *		0			�ɹ�
 *		!0			ʧ��
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
 *	gcs_aio_commit_connect - ��ɫͨ������һ���첽����������
 *
 *	@addr:		��ַ��Ϣ(big endian)
 *	@port:		�˿���Ϣ(big endian)
 *	@launcher:	������
 *	@_unitno:	�첽��Ϣ(��Ԫ��)
 *	@_tunnel:	�첽��Ϣ(�����)
 *	@_action:	�첽��Ϣ(������)
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
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



