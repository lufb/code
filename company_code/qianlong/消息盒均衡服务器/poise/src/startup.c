/**
 * src/startup.c
 *	平台启动模块的时候会调用本函数
 *
 *	2012-09-20 - 首次创建
 *		            LUFUBO
 */

#include "lock.h"
#include "request.h"
#include "poise.h"
#include "thread.h"
#include "cleanup.h"
#include "config.h"
#include "version.h"
	

/**
 *	startup - 模块启动函数
 *
 *	return
 *		0		成功
 *		!0		失败
 */
CTYPENAMEFN DLLEXPORT int SPSTDCALL startup(void)
{
	/**
	 *	平台在进行了协议接口后调用的函数.在本函数中动态库函数可以完成自己的初始化动作等操作
	 */
	const char					*_va = "均衡服务器";
	struct _msg_ops				*msg_ops = &(mbi_sp_interface->msg_ops);
	struct _util_ops			*util_ops = &(mbi_sp_interface->util_ops);
	struct _thread_ops			*ths  =		&(mbi_sp_interface->thread_ops);
	int							iError;

	util_ops->write_log(demo_module_no, LOG_TYPE_INFO, LOG_LEVEL_GENERAL,
	#ifdef _DEBUG
		"消息盒均衡服务器D[V%3.2f B%03d]",
	#else
		"消息盒均衡服务器R[V%3.2f B%03d]",
	#endif
		(float)(poise_version()>>16) / 100, poise_version() & 0xFFFF );

	/**	初始化配置信息*/
	config_init();

	iError = instance(util_ops->nr_links);
	if(0 != iError){
		util_ops->write_log(demo_module_no,
			LOG_TYPE_ERROR,
			LOG_LEVEL_DEVELOPERS,
			"%s 初始化失败,均衡服务器退出", _va);
		return iError;
	}
	
	iError = installDataMsg();
	if(iError)
	{
		util_ops->write_log(demo_module_no,
			LOG_TYPE_ERROR,
			LOG_LEVEL_DEVELOPERS,
			"%s 注册数据处理函数错误,均衡服务器退出", _va);
		cleanup();
		return iError;
	}
	
	iError = installLinkMsg();
	if(iError)
	{
		util_ops->write_log(demo_module_no,
			LOG_TYPE_ERROR,
			LOG_LEVEL_DEVELOPERS,
			"%s 注册链路处理函数错误,均衡服务器退出", _va);
		cleanup();
		return iError;
	}
	
	//创建一个线程，负责向连到到均衡上的代理和升级发送[128,1]协议
	#define _BROAD_128_1_THREAD_		"broad_128_1_thread"
	
	m_pThread = ths->create(0, broadcast128_1_thread, NULL, _BROAD_128_1_THREAD_, \
		strlen(_BROAD_128_1_THREAD_), 0, NULL);
	if(m_pThread == NULL)
	{
		util_ops->write_log(demo_module_no,
			LOG_TYPE_ERROR,
			LOG_LEVEL_DEVELOPERS,
			"%s 创建广播128_1协议出错,均衡服务器退出", _va);
		cleanup();
		return -11;
	}
	return 0;
}

int installDataMsg()
{
	struct _msg_ops				*msg_ops = &(mbi_sp_interface->msg_ops);
	struct _util_ops			*util_ops = &(mbi_sp_interface->util_ops);
	struct _thread_ops			*ths  =		&(mbi_sp_interface->thread_ops);
	struct data_msg_fn_descr	data_fn;
	int							iError;

	/**
	 *	向服务平台注册134_51协议（代理报告固有信息）处理函数
	 */
	data_fn.do_data_msg_fn	= do_request134_51;
	iError = msg_ops->install_data_msg(
					demo_module_no,
					_NOSWAP_MODE, 
					134, 51,			
					&data_fn);
	if(iError)		return -1;

		/**
	 *	向服务平台注册136_51协议（升级报告固有信息）处理函数
	 */
	data_fn.do_data_msg_fn	= do_request136_51;
	iError = msg_ops->install_data_msg(
					demo_module_no,
					_NOSWAP_MODE, 
					136, 51,			
					&data_fn);
	if(iError)		return -2;

	/**
	 *	向服务平台注册137_51协议（客户端请求列表）处理函数
	 */
	data_fn.do_data_msg_fn	= do_request137_51;
	iError = msg_ops->install_data_msg(
					demo_module_no,
					_NOSWAP_MODE, 
					137, 51,			
					&data_fn);
	if(iError)		return -3;

	/**
	 *	向服务平台注册134_153协议（代理报告状态信息）处理函数
	 */
	data_fn.do_data_msg_fn	= do_request134_153;
	iError = msg_ops->install_data_msg(
					demo_module_no,
					_NOSWAP_MODE, 
					134, 153,			
					&data_fn);
	if(iError)		return -4;

	/**
	 *	向服务平台注册136_153协议（升级报告状态信息）处理函数
	 */
	data_fn.do_data_msg_fn	= do_request136_153;
	iError = msg_ops->install_data_msg(
					demo_module_no,
					_NOSWAP_MODE, 
					136, 153,			
					&data_fn);
	if(iError)		return -5;

	/**
	 *	向服务平台注册134_152协议（代理注销）处理函数
	 */
	data_fn.do_data_msg_fn	= do_request134_152;
	iError = msg_ops->install_data_msg(
					demo_module_no,
					_NOSWAP_MODE, 
					134, 152,			
					&data_fn);
	if(iError)		return -6;

	/**
	 *	向服务平台注册136_152协议（升级注销）处理函数
	 */
	data_fn.do_data_msg_fn	= do_request136_152;
	iError = msg_ops->install_data_msg(
					demo_module_no,
					_NOSWAP_MODE, 
					136, 152,			
					&data_fn);
	if(iError)		return -7;

	/**
	 *	向服务平台注册134_154协议（代理报状态）处理函数
	 */
	data_fn.do_data_msg_fn	= do_request134_154;
	iError = msg_ops->install_data_msg(
					demo_module_no,
					_NOSWAP_MODE, 
					134, 154,			
					&data_fn);
	if(iError)		return -8;

	/**
	 *	向服务平台注册136_154协议（升级报状态）处理函数
	 */
	data_fn.do_data_msg_fn	= do_request136_154;
	iError = msg_ops->install_data_msg(
					demo_module_no,
					_NOSWAP_MODE, 
					136, 154,			
					&data_fn);
	if(iError)		return -9;

	return 0;
}

int installLinkMsg()
{
	struct link_msg_fn_descr	link_fn;
	struct _msg_ops				*msg_ops = &(mbi_sp_interface->msg_ops);
	int							iError;
	/**
	 *	向服务平台注册一个链接关闭,连接的消息处理函数
	 */
	link_fn.do_link_msg_fn	=	do_linkmsg;

	iError = msg_ops->install_link_msg(demo_module_no, &link_fn);
	if(iError)		
		return -10;

	return 0;
}