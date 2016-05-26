#include "smg2sp_cli.h"
#include <process.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include "socket.h"
#include "global.h"
#include "protocol.h"
#include "error_define.h"

/*开始deliver模块*/
int	
start_deliver()
{
	_beginthreadex(NULL, 0, deliver_fun, NULL, 0, NULL);
	
	return 0;
}



/*完成一次deliver*/
int
do_one_deliver()
{
	int						sock, err;
	struct serial			ser;

	if((sock = block_connect(g_option.sp_IP, g_option.sp_port)) < 0){
		printf("连接[%s:%d]出错[%d:%d]\n",
			g_option.sp_IP, g_option.sp_port, GET_SYS_ERR(sock), GET_USER_ERR(sock));
		return sock;
	}
	printf("连接[%s:%d]成功\n",
		g_option.sp_IP, g_option.sp_port);

	init_serial(ser);		/*序列号*/

	if((err = do_bind(sock, ser)) != 0){
		printf("do_bind失败[%d:%d]\n", GET_SYS_ERR(err), GET_USER_ERR(err));
		closesocket(sock);
		return err;
	}

	if((err = do_deliver(sock, ser)) != 0){
		printf("do_deliver失败[%d]\n", err);
		closesocket(sock);
		return err;
	}

	if(err = do_unbind(sock, ser) != 0){
		printf("do_unbind失败[%d]\n", err);
		closesocket(sock);
		return err;
	}

	printf("套接字[%d]处理deliver成功\n", sock);
	closesocket(sock);
	return 0;
}

/*开始监听*/
unsigned int __stdcall
deliver_fun(void *in)
{
	int					err;
	char				buf[256];

	printf("现在你可生成deliver消息了:\n");
	while((err = read(0, buf, sizeof(buf))) > 0){
		if(memcmp(CREATE_DELIVER_MSG, buf, strlen(CREATE_DELIVER_MSG)) == 0){
			printf("开始发送一次deliver\n");
			if((err = do_one_deliver()) != 0){
				printf("deliver消息失败[%d]\n", err);
			}
		}
		else{
			printf("不能标识的命令\n");
		}
		printf("现在你又可生成deliver消息了:\n");
	}
	
	printf("********************fuck, in scanf_thread\n");
	return 0;
}