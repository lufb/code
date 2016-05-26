/**
 *	core/gcs_lsnr.c
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
#include "native.h"
#include "pthread.h"
#include "list.h"
#include "gcs_sock.h"
#include "gcs_lsnr.h"
#include "gcs_link.h"
#include "gcs_errno.h"
#include "gcs_config.h"
#include "gcs_smif.h"
#include "gcs_main.h"
#include "bits/gcs_cnt64.h"

#ifndef lint
static const char rcsid[] _U_ =
	"@(#) $Id: core/gcs_lsnr.c,"
	"v 1.00 2013/10/29 11:34:40 yangling Exp $ (LBL)";
#endif

static struct lsnr_descr {
	struct list_head		list;		/*	list节点*/
	int						s;			/*	SOCKET描述符*/
	unsigned short			port;		/*	端口号*/
	unsigned short			padding;
};

static struct lsnr_object {
	struct list_head		head;		/*	监听数组链表头*/
	struct lsnr_descr		*base;		/*	数据缓冲区*/

	/**	下面的指标用于统计数据*/
	cnt64_t					nr_succ;	/*	成功的*/
	cnt64_t					nr_fail;	/*	失败的*/
	cnt64_t					nr_abort1;	/*	异常的1*/
	cnt64_t					nr_abort2;	/*	异常的2*/
}_lsnr_object_;
#define	_lsnr_object()	(&_lsnr_object_)

/**
 *	startup_listener - 启动监听
 *
 *	return
 *		0		成功
 *		!0		失败
 */
static int
startup_listener()
{
	int					rc;
	struct lsnr_object	*obj = _lsnr_object();
	struct list_head	*list;
	struct lsnr_descr	*lsnr;
	struct list_head	*head = &(obj->head);
	struct sockaddr_in	sa;

	list_for_each(list, head) {
		lsnr = list_entry(list, struct lsnr_descr, list);
	
		write_log(GCS_MODULE_ID,
			LOG_TYPE_INFO, LOG_LEVEL_GENERAL,
			"attempting to listen port %d", lsnr->port);

		lsnr->s = socket(PF_INET, SOCK_STREAM, 0);
		if(lsnr->s == INVALID_SOCKET) {
			rc = gen_errno(_OSerrno(), GCEOS);
			write_log(GCS_MODULE_ID,
				LOG_TYPE_ERROR, LOG_LEVEL_GENERAL, 
				"An error(0x%08X) occurred while "
				"calling socket", rc);
			return rc;
		}

		if(ref_config_object()->reuseaddr)
		{
			rc = ne_setreuseaddr(lsnr->s);
			if(rc) {
				rc = gen_errno(_OSerrno(), GCEOS);
				write_log(GCS_MODULE_ID,
					LOG_TYPE_ERROR, LOG_LEVEL_GENERAL, 
					"An error(0x%08X) occurred while "
					"calling ne_setreuseaddr", rc);
				return rc;
			}
		}

		sa.sin_family = AF_INET;
		sa.sin_addr.s_addr = htonl(INADDR_ANY);
		lsnr->port = htons(lsnr->port);
		sa.sin_port = lsnr->port;

		if(bind(lsnr->s, (struct sockaddr *)&sa, sizeof(sa))) {
			rc = gen_errno(_OSerrno(), GCEOS);
			write_log(GCS_MODULE_ID,
				LOG_TYPE_ERROR, LOG_LEVEL_GENERAL, 
				"An error(0x%08X) occurred while "
				"calling bind", rc);
			return rc;
		}

		if(listen(lsnr->s, SOMAXCONN)) {
			rc = gen_errno(_OSerrno(), GCEOS);
			write_log(GCS_MODULE_ID,
				LOG_TYPE_ERROR, LOG_LEVEL_GENERAL, 
				"An error(0x%08X) occurred while "
				"calling listen", rc);
			return rc;
		}
	}

	return 0;
}

/**
 *	_accepted_conn_logtrace - 成功接收一个连接日志跟踪
 *
 *	@from:			来至端口(big endian)
 *	@ipv4:			IP地址(big endian)
 *	@port:			端口(big endian)
 *
 *	return
 *		无
 */
static void
_accepted_conn_logtrace(IN unsigned short from,
	IN unsigned int ipv4, IN unsigned short port)
{
	char	_nm[256];

	if(ref_config_object()->log_level >= LOG_LEVEL_SUPPORT)
		write_log(GCS_MODULE_ID,
			LOG_TYPE_INFO, LOG_LEVEL_SUPPORT,
			"via listen port %d accepted a new "
			"connection from %s", ntohs(from),
			ne_inet_ntopx(ipv4, port, _nm, sizeof(_nm)));
}

/**
 *	listen_routine - GCS 监听处理线程
 *
 *	@arg:		线程参数
 *
 *	return
 *		NULL	无意义
 */
static void *
listen_routine(IN void *arg)
{
	fd_set				rdfds;
	struct sockaddr_in	sa;
	int					len;
	int					rc;
	int					s;
	struct lsnr_object	*obj = _lsnr_object();
	struct list_head	*list;
	struct lsnr_descr	*lsnr;
	struct list_head	*head = &(obj->head);
	struct gcs_thread	*gth = arg;

	local_gcs_thread_init(gth);

	for(;;) {
		FD_ZERO(&rdfds);

		list_for_each(list, head) {
			lsnr = list_entry(list, struct lsnr_descr, list);
			FD_SET((unsigned int)lsnr->s, &rdfds);
		}

		gcs_thread_sleep(gth);
		rc = select(0, &rdfds, NULL, NULL, NULL);
		gcs_thread_wake(gth);

		if(rc <= 0) {
			noarch_sleep(1000);	/*	其实我填写的参数,肯定不会返回=0的*/
			continue;
		}

		list_for_each(list, head) {
			lsnr = list_entry(list, struct lsnr_descr, list);
			if(FD_ISSET(lsnr->s, &rdfds)) {
				len = sizeof(struct sockaddr_in);
				s = accept(lsnr->s, (struct sockaddr*)&sa, &len);
				if(s > 0) {
					unsigned int	_ipv4 = sa.sin_addr.s_addr;
					unsigned short	_port = sa.sin_port;

					rc = gcs_peek_link();
					if(rc) {
						rc = alloc_link_for_client(
									s, _ipv4, _port, lsnr->port);
						if(rc) {
							char	_nm[256];
							ne_fast_close(s);
							write_log(GCS_MODULE_ID,
								LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
								"An error(0x%08X) occurred while "
								"allocating a link resource for "
								"client connection from %s", rc,
								ne_inet_ntopx(_ipv4, _port,
												_nm, sizeof(_nm)));

							CNT64_T_INC(&(obj->nr_abort2));
						} else {
							CNT64_T_INC(&(obj->nr_succ));
							_accepted_conn_logtrace(
									lsnr->port, _ipv4, _port);
						}
					} else {	/*	没有资源了*/
						char	_nm[256];
						ne_fast_close(s);
						write_log(GCS_MODULE_ID,
							LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
							"system having no more resources for "
							"accept connection from %s", 
							ne_inet_ntopx(_ipv4, _port, _nm, sizeof(_nm)));

						CNT64_T_INC(&(obj->nr_abort1));
					}
				} else {
					write_log(GCS_MODULE_ID,
						LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
						"An error(0x%08X) occurred while "
						"calling accept", gen_errno(_OSerrno(), GCEOS));

					CNT64_T_INC(&(obj->nr_fail));
				}
			}
		}
	}
}

/**
 *	make_lsnr_module_ctrl_buffer - 构建lsnr模块的控制台命令缓冲区
 *
 *	@buffer:		缓冲区
 *	@size:			缓冲区最大值
 *
 *	return
 *		无
 */
void
make_lsnr_module_ctrl_buffer(
	IN char *buffer, IN size_t size)
{
	struct lsnr_object	*obj = _lsnr_object();

	unsigned __int64 *succ  = (unsigned __int64 *)&(obj->nr_succ);
	unsigned __int64 *fail  = (unsigned __int64 *)&(obj->nr_fail);
	unsigned __int64 *abort1= (unsigned __int64 *)&(obj->nr_abort1);
	unsigned __int64 *abort2= (unsigned __int64 *)&(obj->nr_abort2);
	
	_snprintf(buffer, size,
		"Passive Opens                  = %I64u\n"
		"Failed Connection Attempts     = %I64u\n"
		"Aborted(1) Connection Attempts = %I64u\n"
		"Aborted(2) Connection Attempts = %I64u\n",
		*succ, *fail, *abort1, *abort2);
}

/**
 *	global_lsnr_module_init - 全局初始化LSNR模块
 *
 *	return
 *		0		成功
 *		!0		失败
 */
int
global_lsnr_module_init()
{
	char				name[32];
	struct lsnr_object	*obj = _lsnr_object();
	struct gcs_config	*cfg = ref_config_object();
	struct gcs_thread	*gth;
	size_t				size;
	int					i, rc;
	pthread_t			thread;

	memset(obj, 0, sizeof(struct lsnr_object));

	size = sizeof(struct lsnr_descr) * cfg->nr_lsnrs;
	obj->base = zmalloc(size);
	if(IS_NULL(obj->base))
		return gen_errno(0, GCENOMEM);

	INIT_LIST_HEAD(&(obj->head));

	for(i = 0; i < cfg->nr_lsnrs; i++) {
		obj->base[i].port = cfg->lsnr_port[i];
		list_add_tail(&(obj->base[i].list), &(obj->head));
	}

	rc = startup_listener();
	if(rc)
		return rc;

	memset(name, 0, sizeof(name));
	sprintf(name, "LSNR%04d", 0);
	gth = alloc_gcs_thread_object(name);
	if(IS_NULL(gth))
		return gen_errno(0, GCEABORT);

	return pthread_create(&thread, NULL, listen_routine, gth);
}