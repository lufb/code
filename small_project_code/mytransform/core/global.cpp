#include "global.h"
#include "mlog.h"
#include "error.h"
#include "msocket.h"


MWriteLog				global_log;
MOption					global_option;


/**
 *	mytrans		-		中转数据
 *
 *	@argc	:		[in]		传入参数个数
 *	@argv	:		[in]		参数列表
 *	return
 *		==0			成功
 *		!=0			失败
 */
int mytrans(int argc, char *argv[])
{
	int						rc;

	if((rc = global_log.initLog("./", true, "mytransform", ".txt")) < 0){
		return BUILD_ERROR(0, E_LOG);
	}
	global_log.writeLog(LOG_TYPE_INFO, "日志模块启动成功");

	if((rc = MSocket::initNetWork()) != 0){
		return BUILD_ERROR(_OSerrno(), E_NETWORKENV);
	}
	global_log.writeLog(LOG_TYPE_INFO, "初始化网络环境成功");

	if((rc = global_option.init(argc, argv)) != 0)
		return rc;





	return 0;
}