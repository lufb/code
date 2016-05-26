#include <process.h>
#include <assert.h>
#include <time.h>
#include "sp2smg_srv.h"
#include "option.h"
#include "socket.h"
#include "global.h"
#include "error_define.h"
#include "protocol.h"


int	
start_listen()
{
	int					lis_sock;
	
	if((lis_sock = create_listen(g_option.smg_bind_port)) < 0)
		return lis_sock;
	printf("���ذ󶨶˿�[%d]�ɹ�\n", g_option.smg_bind_port);

	_beginthreadex(NULL, 0, smg_listen, (void *)lis_sock, 0, NULL);

	return 0;
}

/*��ʼ����*/
unsigned int __stdcall
smg_listen(void *in)
{
	int					lis_sock = (int)in;
	struct sockaddr_in	client;
	int					addr_size = sizeof(client);
	int					cli_sock;

	listen(lis_sock, 5);

	while(1){
		cli_sock = accept(lis_sock, (struct sockaddr *)&client, &addr_size);
		if(cli_sock == INVALID_SOCKET)
			return BUILD_ERROR(_OSerrno(), 0);
		g_log.writeLog(LOG_TYPE_INFO,
			"ͨ�������˿�[%d] ��[%s:%d] �յ�socket[%d]",
			g_option.smg_bind_port, inet_ntoa(client.sin_addr),
					ntohs(client.sin_port), cli_sock);

		/*�����߳������������*/
		_beginthreadex(NULL, 0, srv_thread, (void *)cli_sock, 0, NULL);
	}

	return 0;
}



unsigned int __stdcall 
srv_thread(void * in)
{
	int					sock = (int)in;
	assert(sock > 0);
	int					last_time = time(NULL);
	int					now_time;
	struct timeval		tv;
	fd_set				fd_read;
	int					err;

	setnonblocking(sock);	/*�����׽��ַ�����*/
	set_sock_buf(sock, 1024*64);/*�����׽��ֻ�������С*/

	tv.tv_sec = 1;
	tv.tv_usec = 0;

	while(1){
		FD_ZERO(&fd_read);
		FD_SET((unsigned int)sock, &fd_read);

		err = select(0, &fd_read, NULL, NULL, &tv);
		if(err == 0){	/*û����,��ʱ*/
			now_time = time(NULL);
			if(now_time - last_time > (int)g_option.sp2smg_timeout){	/*��·��ʱ*/
				err = BUILD_ERROR(0, E_LINK_TIMEOUT);
				clean_link(sock, err);
				return err;
			}
		}else if(err > 0){
			last_time = time(NULL);
			if(FD_ISSET(sock, &fd_read)){
				err = rcv_msg2rsp(sock);
				if(err != 0){
					clean_link(sock, err);
					return err;
				}
			}else{assert(0);}
		}else{assert(0);}	/*��Ӧ�õ�����*/
	}

	
	return 0;
}