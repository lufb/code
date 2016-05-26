/**
 *	core/gcs_iocp.c
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
#include "gcs_conn.h"
#include "gcs_errno.h"
#include "gcs_iocp.h"
#include "gcs_parser.h"
#include "gcs_config.h"
#include "gcs_main.h"

#ifndef lint
static const char rcsid[] _U_ =
	"@(#) $Id: core/gcs_iocp.c,"
	"v 1.00 2013/10/29 12:02:40 yangling Exp $ (LBL)";
#endif

static struct iocp_model	__iocp_model_object;
#define	iocp_object()	(&__iocp_model_object)

/**
 *	ref_iocp_model_object - 引用IOCP模型对象
 *
 *	return
 *		无
 */
struct iocp_model *
ref_iocp_model_object()
{
	return iocp_object();
}

/**
 *	iocp_work_routine - IOCP工作线程
 *
 *	@arg:		线程参数
 *
 *	return
 *		NULL	无意义
 */
static void *
iocp_work_routine(IN void *arg)
{
	struct word_table	word_tab;
	struct iocp_model	*obj = ref_iocp_model_object();
	unsigned long		size;
	unsigned long		link_no;
	struct link_buff	*buff;
	int					rc;
	struct gcs_thread	*gth = arg;
	
	local_gcs_thread_init(gth);

	local_word_table_init(&word_tab);

	for(;;)
	{
		gcs_thread_sleep(gth);
		if(GetQueuedCompletionStatus(
				obj->handle, &size, &link_no,
				(OVERLAPPED **)&buff, INFINITE) != 0) {
			gcs_thread_wake(gth);
			if(size == 0) {	/*connection has been gracefully closed*/
				if(IS_RX_MODE(buff))
					gcs_free_link(link_no);
				else
					gcs_hard_close_link(link_no);
			} else {/* successfully received or successfully sent*/
				if(IS_RX_MODE(buff))
					gcs_aio_complete_recv(
						&word_tab, link_no, size);
				else
					gcs_aio_complete_sent(link_no, size);
			}
		} else {	/*	some errors occurred*/
			gcs_thread_wake(gth);
			rc = GetLastError();
			if(rc != WAIT_TIMEOUT &&
				rc != WSA_IO_PENDING && buff != NULL) {
				if(IS_RX_MODE(buff))
					gcs_free_link(link_no);
				else
					gcs_hard_close_link(link_no);
			}
		}
	}

	return NULL;

}

/**
 *	global_iocp_module_init - 全局初始化IOCP模块
 *
 *	return
 *		0		成功
 *		!0		失败
 */
int
global_iocp_module_init()
{
	char				name[32];
	int					i, rc;
	pthread_t			thread;
	struct iocp_model	*obj = iocp_object();
	struct gcs_config	*cfg = ref_config_object();
	struct gcs_thread	*gth;

	memset(obj, 0, sizeof(struct iocp_model));

	obj->nr_wk_ths = cfg->core_ths;

	obj->handle = CreateIoCompletionPort(
		INVALID_HANDLE_VALUE, NULL, 0, obj->nr_wk_ths);
	if(IS_NULL(obj->handle))
		return gen_errno(_OSerrno(), GCEOS);

	for(i = 0; i < obj->nr_wk_ths; i++) {
		memset(name, 0, sizeof(name));
		sprintf(name, "IOCP%04d", i);
		gth = alloc_gcs_thread_object(name);
		if(IS_NULL(gth))
			return gen_errno(0, GCEABORT);

		rc = pthread_create(&thread, NULL, iocp_work_routine, gth);
		if(rc)
			return rc;
	}

	return 0;
}


