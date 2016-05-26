/**
 *	init/gcs_main.c
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */

#include "typedef.h"
#include "gcs_lsnr.h"
#include "gcs_conn.h"
#include "gcs_iocp.h"
#include "gcs_link.h"
#include "gcs_sock.h"
#include "gcs_config.h"
#include "gcs_smif.h"
#include "gcs_parser.h"
#include "gcs_page.h"
#include "gcs_main.h"
#include "gcs_version.h"
#include "gcs_ctrl.h"
#include "gcs_mon.h"

#ifdef WIN32
#ifdef _DEBUG
#pragma comment(lib, "crashdump/Debug/crashdump.lib")
#else
#pragma comment(lib, "crashdump/Release/crashdump.lib")
#endif
#endif

#ifndef lint
static const char rcsid[] _U_ =
	"@(#) $Id: init/gcs_main.c,"
	"v 1.00 2013/10/28 17:08:40 yangling Exp $ (LBL)";
#endif

#define	gcs_version_show()								\
	do {												\
		char	v[256];									\
		write_log(GCS_MODULE_ID,						\
			LOG_TYPE_INFO, LOG_LEVEL_GENERAL,			\
			"%s version: %s",							\
			GCSERVER_NAME,								\
			global_public_version_str(v, sizeof(v)));	\
	}while(0)

static struct gcs_main {
#define	NR_GCS_THREAD		2048
	time_t					startup;			/*	����ʱ��*/
	unsigned short			nr_threads;			/*	�߳�����*/
	unsigned short			padding;			/*	����*/
	struct gcs_thread		threads[NR_GCS_THREAD];
}__gcs_main_object__;
#define	gcs_main_object()		(&__gcs_main_object__)

/**
 *	make_ts_ctrl_buffer - ����ts����Ŀ���̨�������
 *
 *	@buffer:		������
 *	@size:			���������ֵ
 *
 *	return
 *		��
 */
void
make_ts_ctrl_buffer(
	IN char *buffer, IN size_t size)
{
	struct gcs_main	*obj = gcs_main_object();
	int				i;
	int				circle = min(NR_GCS_THREAD, obj->nr_threads);
	int				sleeping	= 0;
	int				running		= 0;
	int				zombie		= 0;
	int				unknown		= 0;

	for(i = 0; i < circle; i++) {
		switch(obj->threads[i].status)
		{
		case GCS_THREAD_SLEEPING:
			sleeping++;
			break;
		case GCS_THREAD_RUNNING:
			running++;
			break;
		case GCS_THREAD_ZOMBIE:
			zombie++;
			break;
		default:
			unknown++;
			break;
		}
	}

	_snprintf(buffer, size,
		"Threads: %u total, %u running, "
		"%u sleeping, %u zombie, %u unknown\n",
		obj->nr_threads, running,
		sleeping, zombie, unknown);
}

/**
 *	make_dts_ctrl_buffer - ����dts����Ŀ���̨�������
 *
 *	@buffer:		������
 *	@size:			���������ֵ
 *
 *	return
 *		��
 */
void
make_dts_ctrl_buffer(
	IN char *buffer, IN size_t size)
{
	char			stat;
	struct gcs_main	*obj = gcs_main_object();
	int				i;
	int				circle = min(NR_GCS_THREAD, obj->nr_threads);
	int				rc = 0;
	/*	MOCK:HACK ����ӡ200���߳�*/
	circle = min(circle, 200);

	for(i = 0; i < circle; i++) {
		switch(obj->threads[i].status) {
		case GCS_THREAD_SLEEPING:
			stat = 'S';
			break;
		case GCS_THREAD_RUNNING:
			stat = 'R';
			break;
		case GCS_THREAD_ZOMBIE:
			stat = 'Z';
			break;
		default:
			stat = 'N';
			break;
		}

		rc += _snprintf(buffer + rc, size - rc,
				"%8.8s %8d %c\n",
				obj->threads[i].name,
				obj->threads[i].thread_id,
				stat);
	}
}

/**
 *	refresh_main_mon - ˢ��main�������
 *
 *	@mon:		������ݽṹ
 *
 *	return
 *		��
 */
static void
refresh_main_mon(IN struct gcs_mon *mon)
{
	struct gcs_main	*obj = gcs_main_object();
	int				i;
	int				circle = min(NR_GCS_THREAD, obj->nr_threads);
	unsigned short	sleeping	= 0;
	unsigned short	running		= 0;
	unsigned short	zombie		= 0;
	unsigned short	unknown		= 0;
	
	for(i = 0; i < circle; i++) {
		switch(obj->threads[i].status)
		{
		case GCS_THREAD_SLEEPING:
			sleeping++;
			break;
		case GCS_THREAD_RUNNING:
			running++;
			break;
		case GCS_THREAD_ZOMBIE:
			zombie++;
			break;
		default:
			unknown++;
			break;
		}
	}

	mon->version		= global_public_version_int();
	mon->nr_sleeping	= sleeping;
	mon->nr_running		= running;
	mon->nr_zombie		= zombie;
	mon->nr_unknown		= unknown;
	mon->nr_total		= obj->nr_threads;
	mon->uptime			= time(NULL) - obj->startup;
}

/**
 *	global_main_module_init - ��ģ���ʼ��
 *
 *	return
 *		��
 */
static void
global_main_module_init()
{
	struct gcs_main	*obj = gcs_main_object();

	memset(obj, 0, sizeof(struct gcs_main));

	obj->startup = time(NULL);
}

/**
 *	alloc_gcs_thread_object - ����һ��GCS�̶߳���
 *
 *	@name:		����
 *
 *	return
 *		NULL	ʧ��
 *		!NULL	�ɹ�
 *
 *	remark
 *		û�м���,��Ϊʵ���в�����race-condition���
 */
struct gcs_thread*
alloc_gcs_thread_object(IN char *name)
{
	struct gcs_main		*obj = gcs_main_object();
	unsigned short		idx;
	struct gcs_thread	*gth;
	int					len;

	if(obj->nr_threads >= NR_GCS_THREAD)
		return NULL;

	idx = obj->nr_threads++;

	gth = obj->threads + idx;

	len = strlen(name);

	strncpy(gth->name, name,
			min(len, sizeof(gth->name) - 1));

	return gth;
}

/**
 *	local_gcs_thread_init - �̳߳�ʼ��(ÿ���߳̽�����һ��)
 *
 *	@gth:		�߳�������
 *
 *	return
 *		��
 */
void
local_gcs_thread_init(IN struct gcs_thread *gth)
{
	gth->thread_id	= GetCurrentThreadId();
	gth->handler	= GetCurrentThread();
	gcs_thread_wake(gth);
}

extern	int _stdcall DbgInstance(int, int);

/**
 *	main - ������ ���
 *
 *	return
 *		��
 */
int main()
{
	int					rc;

	DbgInstance(0, 0);

	gcs_mon_module_init();

	global_main_module_init();

	global_ctrl_command_init();

	ne_startup();

	/**	��ʼ������������ͨѶ��ģ��*/
	global_smif_module_init();

	gcs_version_show();

	/**	��ʼ��ҳģ��*/
	rc = global_page_module_init();
	if(rc) {
		write_log(GCS_MODULE_ID,
			LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
			"An error(0x%08X) occurred while "
			"calling global_page_module_init", rc);
		return rc;
	}

	/**	��ʼ������*/
	rc = global_config_init();
	if(rc) {
		write_log(GCS_MODULE_ID,
			LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
			"An error(0x%08X) occurred while "
			"calling global_config_init", rc);
		return rc;
	}

	/**	��ʼ��HTTP������ģ��*/
	global_parser_module_init();

	/**	��ʼ��CONNģ��*/
	rc = global_conn_module_init();
	if(rc) {
		write_log(GCS_MODULE_ID,
			LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
			"An error(0x%08X) occurred while "
			"calling global_conn_module_init", rc);
		return rc;
	}

	/**	��ʼ��LINKģ��*/
	rc = global_link_module_init();
	if(rc) {
		write_log(GCS_MODULE_ID,
			LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
			"An error(0x%08X) occurred while "
			"calling global_link_module_init", rc);
		return rc;
	}

	/**	��ʼ��IOCPģ��*/
	rc = global_iocp_module_init();
	if(rc) {
		write_log(GCS_MODULE_ID,
			LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
			"An error(0x%08X) occurred while "
			"calling global_iocp_module_init", rc);
		return rc;
	}

	/**	��ʼ������ģ��*/
	rc = global_lsnr_module_init();
	if(rc) {
		write_log(GCS_MODULE_ID,
			LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
			"An error(0x%08X) occurred while "
			"calling global_lsnr_module_init", rc);
		return rc;
	}

	smif_refresh_module_status(GCS_MODULE_ID, GCS_TRUE);

	write_log(GCS_MODULE_ID,
			LOG_TYPE_INFO, LOG_LEVEL_GENERAL,
			"all modules been successfully initialized. "
			"now gcserver is working");

	for(;;) {
		refresh_main_mon(ref_gcs_mon_object());
		refresh_page_mon(ref_gcs_mon_object());
		refresh_link_mon(ref_gcs_mon_object());
		smif_refresh_module_verbose_status(GCS_MODULE_ID,
			(char *)ref_gcs_mon_object(), sizeof(struct gcs_mon));
		check_all_connection_timeout();
		noarch_sleep(5000);
	}
	

	return 0;
}