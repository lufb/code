#include <stdio.h>
#include "global.h"
#include "cp2msg_srv.h"
#include "cp2msg_cli.h"
#include "error_define.h"
#include "sock_op.h"
#include <assert.h>







int
main(void)
{
	int					err;

	init_network();
	if((err = g_log.initLog("./", true, "cp2msg", "log")) != 0){
		printf("启动日志模块失败，程序终止\n");
		return err;
	}
	g_log.writeLog(LOG_TYPE_INFO, "日志模块启动成功");

	if((err = start_srv_mode()) != 0){
		g_log.writeLog(LOG_TYPE_ERROR,
			"启动srv模块失败[%u:%u]",
			GET_SYS_ERR(err), GET_USER_ERR(err));
		return err;
	}
	g_log.writeLog(LOG_TYPE_INFO, "启动srv模块成功");

	if((err = start_deliver_mode()) != 0){
		g_log.writeLog(LOG_TYPE_ERROR,
			"启动DELIVER模块失败[%u:%u]",
			GET_SYS_ERR(err), GET_USER_ERR(err));
		return err;
	}
	g_log.writeLog(LOG_TYPE_INFO, "启动DELIVER模块成功");

	/* 在主线程中敲任意键，发Deliver */
	while((err = getc(stdin)) != EOF){
		if(g_cli_queue.GetRandSock(err) == true)
			if(g_deliver_que.Insert(0, NULL, err) != true)
				assert(0);
	}

	return 0;
}