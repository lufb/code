/**
 * src/consult.c
 *
 *	DEMO模块: 被平台调用的接口
 *
 *	平台在调用startup函数之前会调用的函数
 *	平台与模块进行参数协商,用于平台校验是否决定调用本模块
 *
 *	2012-09-17 - 首次创建
 *		            阳凌, <yl.tienon@gmal.com>
 */

#include "request.h"
#include "version.h"
#include <stdio.h>

/**
 *	consult - 协商接口
 *
 *	@mbi_sp:[in]		平台的接口信息(包括平台提供的所有的操作)
 *	@module_no:[in]		平台分配给动态库的模块编号(该编号需要动态库保存,打印日志的时候,刷新状态等操作的时候需要)
 *	@module:[out]		模块的静态基本信息(平台会把这些注册给服务管理和监控器来最终显示)
 *	@verify:[out]		返回当时编译动态库所使用的平台的一些数据结构的版本号(平台会进行主版等校验,决定是否兼容)
 *
 *	无返回值
 *
 *
 *	Note:
 *		该函数导出给服务平台的接口函数,平台会在首次LoadDll加载后调用,并且只会调用一次
 *		主要完成的功能包括
 *
 *		把平台的操作接口mosp_interface 传递给动态库
 *		把平台分配给动态库的模块编号传输给动态库,便于日志打印的操作
 *
 *		动态库需要把自己的静态的基本信息reg_module,以及自己当时编译的时候使用的平台的接口版本号返回给平台
 *		平台需要根据主版号来决定是否继续调用动态库,避免接口的差异导致一些问题.
 */
CTYPENAMEFN DLLEXPORT void SPSTDCALL consult(
	IN struct mosp_interface *mbi_if, IN unsigned short module_no,
	OUT struct reg_module *module, OUT struct mosp_features* verify)
{
	mbi_sp_interface		= mbi_if;
	demo_module_no			= module_no;
	verify->if_magic		= MOSP_INTERFACE_MAGIC;
	verify->msg_magic		= MOSP_MSG_HEADER_MAGIC;
	verify->version			= MOSP_RUN_VERSION;

	module->parent_id		= 0;						/*	父模块ID,固定是0*/
	module->version			= poise_version();			/*	V1.00 B000*/
	module->module_type		= 0x2803;					/*	模块类型,找总部申请的*/
	module->log_level		= LOG_LEVEL_GENERAL;		/*	一开始使用的日志的等级*/

	strncpy(
		module->image_name,
		"poise.dll",
		sizeof(module->image_name));

	strncpy(
		module->module_name,
		"消息盒均衡服务器",
		sizeof(module->module_name));

	_snprintf(
		module->module_descr,
		sizeof(module->module_descr),
	#ifdef _DEBUG
		"消息盒均衡服务器D[V%3.2f B%03d]",
	#else
		"消息盒均衡服务器R[V%3.2f B%03d]",
	#endif
		(float)(module->version>>16) / 100, module->version & 0xFFFF );
}
