#include <stdio.h>
#include "global.h"
#include "error_define.h"
#include "create_rand.h"
#include "socket.h"
#include "sp2smg_srv.h"
#include "smg2sp_cli.h"
#include "report.h"


/*�����ʼ��*/
int
init_sp2smg(void)
{
	int					err;
	
	if((err = g_log.initLog("./", true, "sp2smg", "log")) < 0)	/*������־ģ��*/
		return err;
	
	if((err = load_option()) != 0)								/*���������ļ�*/
		return err;
	
	print_option(g_option);										/*��ӡ���õ���־*/
	init_rand();												/*��ʼ�����������*/
	init_network();												/*��ʼ�����绷��*/
	

	return 0;
}

/*
	ģ�����������
*/
int
start_sp2smg(void)
{
	int						err;

	if((err = init_sp2smg()) != 0)	
		return err;
	
	if((err = start_listen()) != 0)/*��������ģ��*/
		return err;

	if((err = start_deliver()) != 0)/*����deliverģ��*/
		return err;

	if((err = start_report()) != 0) /*����reportģ��*/
		return err;

	return 0;
}

int
main(void)
{
	int				err;
	
	if((err = start_sp2smg()) != 0){
		printf("��������ʧ��:");
		PRINT_ERR_NO(err);
		g_log.writeLog(LOG_TYPE_ERROR, 
			"��������ʧ��[%d]", err);

		return err;
	}else{
		printf("���������ɹ�\n");
		g_log.writeLog(LOG_TYPE_INFO, 
			"���������ɹ�");
	}
	
	while(1){
		Sleep(1000);
	}
	
	return 0;
}