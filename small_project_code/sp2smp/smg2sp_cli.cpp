#include "smg2sp_cli.h"
#include <process.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include "socket.h"
#include "global.h"
#include "protocol.h"
#include "error_define.h"

/*��ʼdeliverģ��*/
int	
start_deliver()
{
	_beginthreadex(NULL, 0, deliver_fun, NULL, 0, NULL);
	
	return 0;
}



/*���һ��deliver*/
int
do_one_deliver()
{
	int						sock, err;
	struct serial			ser;

	if((sock = block_connect(g_option.sp_IP, g_option.sp_port)) < 0){
		printf("����[%s:%d]����[%d:%d]\n",
			g_option.sp_IP, g_option.sp_port, GET_SYS_ERR(sock), GET_USER_ERR(sock));
		return sock;
	}
	printf("����[%s:%d]�ɹ�\n",
		g_option.sp_IP, g_option.sp_port);

	init_serial(ser);		/*���к�*/

	if((err = do_bind(sock, ser)) != 0){
		printf("do_bindʧ��[%d:%d]\n", GET_SYS_ERR(err), GET_USER_ERR(err));
		closesocket(sock);
		return err;
	}

	if((err = do_deliver(sock, ser)) != 0){
		printf("do_deliverʧ��[%d]\n", err);
		closesocket(sock);
		return err;
	}

	if(err = do_unbind(sock, ser) != 0){
		printf("do_unbindʧ��[%d]\n", err);
		closesocket(sock);
		return err;
	}

	printf("�׽���[%d]����deliver�ɹ�\n", sock);
	closesocket(sock);
	return 0;
}

/*��ʼ����*/
unsigned int __stdcall
deliver_fun(void *in)
{
	int					err;
	char				buf[256];

	printf("�����������deliver��Ϣ��:\n");
	while((err = read(0, buf, sizeof(buf))) > 0){
		if(memcmp(CREATE_DELIVER_MSG, buf, strlen(CREATE_DELIVER_MSG)) == 0){
			printf("��ʼ����һ��deliver\n");
			if((err = do_one_deliver()) != 0){
				printf("deliver��Ϣʧ��[%d]\n", err);
			}
		}
		else{
			printf("���ܱ�ʶ������\n");
		}
		printf("�������ֿ�����deliver��Ϣ��:\n");
	}
	
	printf("********************fuck, in scanf_thread\n");
	return 0;
}