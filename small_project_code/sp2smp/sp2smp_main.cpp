#include <stdio.h>
#include "global.h"
#include "error_define.h"
#include "create_rand.h"
#include "socket.h"
#include "sp2smg_srv.h"
#include "smg2sp_cli.h"
#include "report.h"


/*程序初始化*/
int
init_sp2smg(void)
{
	int					err;
	
	if((err = g_log.initLog("./", true, "sp2smg", "log")) < 0)	/*启动日志模块*/
		return err;
	
	if((err = load_option()) != 0)								/*加载配置文件*/
		return err;
	
	print_option(g_option);										/*打印配置到日志*/
	init_rand();												/*初始化随机数种子*/
	init_network();												/*初始化网络环境*/
	

	return 0;
}

/*
	模拟程序主控制
*/
int
start_sp2smg(void)
{
	int						err;

	if((err = init_sp2smg()) != 0)	
		return err;
	
	if((err = start_listen()) != 0)/*启动监听模块*/
		return err;

	if((err = start_deliver()) != 0)/*启动deliver模块*/
		return err;

	if((err = start_report()) != 0) /*启动report模拟*/
		return err;

	return 0;
}

int
main(void)
{
	int				err;
	
	if((err = start_sp2smg()) != 0){
		printf("程序启动失败:");
		PRINT_ERR_NO(err);
		g_log.writeLog(LOG_TYPE_ERROR, 
			"程序启动失败[%d]", err);

		return err;
	}else{
		printf("程序启动成功\n");
		g_log.writeLog(LOG_TYPE_INFO, 
			"程序启动成功");
	}
	
	while(1){
		Sleep(1000);
	}
	
	return 0;
}