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
		printf("������־ģ��ʧ�ܣ�������ֹ\n");
		return err;
	}
	g_log.writeLog(LOG_TYPE_INFO, "��־ģ�������ɹ�");

	if((err = start_srv_mode()) != 0){
		g_log.writeLog(LOG_TYPE_ERROR,
			"����srvģ��ʧ��[%u:%u]",
			GET_SYS_ERR(err), GET_USER_ERR(err));
		return err;
	}
	g_log.writeLog(LOG_TYPE_INFO, "����srvģ��ɹ�");

	if((err = start_deliver_mode()) != 0){
		g_log.writeLog(LOG_TYPE_ERROR,
			"����DELIVERģ��ʧ��[%u:%u]",
			GET_SYS_ERR(err), GET_USER_ERR(err));
		return err;
	}
	g_log.writeLog(LOG_TYPE_INFO, "����DELIVERģ��ɹ�");

	/* �����߳��������������Deliver */
	while((err = getc(stdin)) != EOF){
		if(g_cli_queue.GetRandSock(err) == true)
			if(g_deliver_que.Insert(0, NULL, err) != true)
				assert(0);
	}

	return 0;
}