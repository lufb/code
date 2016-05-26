#include  "socket.h"
#include "interface.h"
#include <stdio.h>
#include <process.h>
#include <windows.h>
#include "error_define.h"
#include <assert.h>



#define SUBMIT_MSG			"HELLO THIS IS SP"
#define USER_NAME			"sp2smg_usr"
#define USER_PASS			"sp2smg_pass"
#define SMG_IP				"127.0.0.1"
#define SMG_PORT			8888
#define SP_PORT				9999


unsigned int __stdcall
client_thread(void *in)
{
	int						sock, err;
	char					rcv_buf[1024];
	char					snd_buf[1024];
	size_t					snd_size;
	size_t					rcv_size;
	struct msg_head			*head= (struct msg_head*)snd_buf;
	
	while(1){
		if(( sock = block_connect(SMG_IP, SMG_PORT)) < 0){
			err = sock;
			goto ret;
		}
		//bind
		snd_size = sizeof(struct msg_head) + sizeof(struct msg_bind);
		memset(snd_buf, 0, sizeof(snd_buf));
		head->msg_id = htonl(SGIP_BIND);
		head->msg_len = htonl(snd_size);
		
		struct msg_bind  *bind = (struct msg_bind *)(snd_buf+sizeof(struct msg_head));
		strncpy(bind->login_name, USER_NAME, sizeof(bind->login_name));
		strncpy(bind->login_pass, USER_PASS, sizeof(bind->login_pass));
		bind->login_type = (unsigned char)1;
		memset(bind->reserv, 0, sizeof(bind->reserv));

		if(send(sock, snd_buf, snd_size, 0) !=  snd_size){
			err = BUILD_ERROR(_OSerrno(), 0);
			goto ret;
		}

		//bind_rsp
		rcv_size = sizeof(struct msg_head)+sizeof(struct response);
		if(recv(sock, rcv_buf, rcv_size, 0) != rcv_size){
			err = BUILD_ERROR(_OSerrno(), 0);
			goto ret;
		}
		{
			struct msg_head			*h = (struct msg_head*)rcv_buf;
			assert(ntohl(h->msg_id) == SGIP_BIND_RESP);
			assert(ntohl(h->msg_len) == sizeof(struct msg_head)+sizeof(struct response));
		}

		for(int i = 0; i < 100; ++i){//发100次需要report的submit
			//submit,需要带report
			snd_size = sizeof(struct msg_head) + sizeof(struct msg_submit)+strlen(SUBMIT_MSG)+8;
			memset(snd_buf, 0, sizeof(snd_buf));
			head->msg_id = htonl(SGIP_SUBMIT);
			head->msg_len = htonl(snd_size);
			
			struct msg_submit  *submit = (struct msg_submit *)(snd_buf+sizeof(struct msg_head));
			submit->sub_usr_count = 50;
			strncpy(submit->sub_corp_id, "55555", 5);
			strncpy(submit->sub_freevalue, "55555", 5);
			strncpy(submit->sub_giv_value, "55555", 5);
			submit->sub_agent_flag = 0;
			submit->sub_pri = 6;
			submit->sub_report_flag = 1;
			submit->sub_msg_len = htonl(strlen(SUBMIT_MSG));
			//加消息体
			memcpy(snd_buf+sizeof(struct msg_head) + sizeof(struct msg_submit),
				SUBMIT_MSG, strlen(SUBMIT_MSG));
			memset(snd_buf+sizeof(struct msg_head) + sizeof(struct msg_submit)+strlen(SUBMIT_MSG),
				0, 8);
			
			if(send(sock, snd_buf, snd_size, 0) !=  snd_size){
				err = BUILD_ERROR(_OSerrno(), 0);
				goto ret;
			}
			
			//submit_rsp
			rcv_size = sizeof(struct msg_head)+sizeof(struct response);
			if(recv(sock, rcv_buf, rcv_size, 0) != rcv_size){
				err = BUILD_ERROR(_OSerrno(), 0);
				goto ret;
			}
			{
				struct msg_head			*h = (struct msg_head*)rcv_buf;
				assert(ntohl(h->msg_id) == SGIP_SUBMIT_RESP);
				assert(ntohl(h->msg_len) == sizeof(struct msg_head)+sizeof(struct response));
			}
		}
		
		//send unbind
		snd_size = sizeof(struct msg_head);
		memset(snd_buf, 0, sizeof(snd_buf));
		head->msg_id = htonl(SGIP_UNBIND);
		head->msg_len = htonl(snd_size);

		if(send(sock, snd_buf, snd_size, 0) != snd_size){
			err = BUILD_ERROR(_OSerrno(), 0);
			goto ret;
		}
		
		//recv unbind rsp
		rcv_size = sizeof(struct msg_head);
		if(recv(sock, rcv_buf, rcv_size, 0) != rcv_size){
			err = BUILD_ERROR(_OSerrno(), 0);
			goto ret;
		}
		{
			struct msg_head			*h = (struct msg_head*)rcv_buf;
			assert(ntohl(h->msg_id) == SGIP_UNBIND_RESP);
			assert(ntohl(h->msg_len) == sizeof(struct msg_head));
		}

		printf("套接字[%d]完成所有动作\n", sock);
		break;		
	}
	
ret:
	PRINT_ERR_NO(err);

	return 0;
}


unsigned int __stdcall
server_thread(void *in)
{
	int					clisock = (int)in;
	struct msg_head		head;
	size_t				msg_size;
	char				rcv_buf[1024];
	char				snd_buf[1024];
	size_t				snd_size;
	unsigned int		type;

	struct msg_head		*h = (struct msg_head *)snd_buf;
	
	int					err;
	printf("套接字[%d]到达\n", clisock);
	while(1)
	{
		if((err = recv(clisock, (char *)&head, sizeof(head), 0)) != sizeof(head)){
			err = BUILD_ERROR(_OSerrno(), 0);
			goto ret;
		}
		msg_size = ntohl(head.msg_len);
		type = ntohl(head.msg_id);
		assert(msg_size >= sizeof(head));
		msg_size -= sizeof(head);
		if(msg_size != 0){
			if((err = recv(clisock, rcv_buf, msg_size, 0)) != msg_size){
				BUILD_ERROR(_OSerrno(), 0);
				goto ret;
			}
		}
		
	
		if(type == SGIP_BIND){
			//struct msg_bind *bind = (struct msg_bind *)rcv_buf;
			snd_size = sizeof(struct msg_head)+sizeof(struct response);
			struct response	*res = (struct response *)(snd_buf+sizeof(struct msg_head));
			h->msg_id = htonl(SGIP_BIND_RESP);
			memcpy(&(h->msg_serial), &head.msg_serial, sizeof(head.msg_serial));
			h->msg_len = htonl(snd_size);

			res->res_code = (unsigned char)0;
			memset(res->reserv, 0, sizeof(res->reserv));
			
			if(send(clisock, snd_buf, snd_size, 0) != snd_size){
				BUILD_ERROR(_OSerrno(), 0);
				goto ret;
			}

			printf("bind消息处理成功\n");
		}else if(type == SGIP_UNBIND){
			snd_size = sizeof(struct msg_head);

			h->msg_id = htonl(SGIP_UNBIND_RESP);
			memcpy(&(h->msg_serial), &head.msg_serial, sizeof(head.msg_serial));
			h->msg_len = htonl(snd_size);
			
			if(send(clisock, snd_buf, snd_size, 0) != snd_size){
				BUILD_ERROR(_OSerrno(), 0);
				goto ret;
			}
			printf("unbind消息处理成功\n");
		}else if(type == SGIP_DELIVER){
			snd_size = sizeof(struct msg_head)+sizeof(struct response);
			struct response	*res = (struct response *)(snd_buf+sizeof(struct msg_head));
			h->msg_id = htonl(SGIP_DELIVER_RESP);
			memcpy(&(h->msg_serial), &head.msg_serial, sizeof(head.msg_serial));
			h->msg_len = htonl(snd_size);
			
			res->res_code = (unsigned char)0;
			memset(res->reserv, 0, sizeof(res->reserv));
			
			if(send(clisock, snd_buf, snd_size, 0) != snd_size){
				BUILD_ERROR(_OSerrno(), 0);
				goto ret;
			}
			
			printf("deliver消息处理成功\n");
		}else if(type == SGIP_REPORT){
			snd_size = sizeof(struct msg_head)+sizeof(struct response);
			struct response	*res = (struct response *)(snd_buf+sizeof(struct msg_head));
			h->msg_id = htonl(SGIP_REPORT_RESP);
			memcpy(&(h->msg_serial), &head.msg_serial, sizeof(head.msg_serial));
			h->msg_len = htonl(snd_size);
			
			res->res_code = (unsigned char)0;
			memset(res->reserv, 0, sizeof(res->reserv));
			
			if(send(clisock, snd_buf, snd_size, 0) != snd_size){
				BUILD_ERROR(_OSerrno(), 0);
				goto ret;
			}
			printf("report消息处理成功\n");
		}else{assert(0);}

	}
ret:
	PRINT_ERR_NO(err);
	return 0;
}

int
start_listen(void)
{
	int					sock, cli_sock;
	struct sockaddr_in	client;
	int					addr_size = sizeof(client);
	
	if((sock = create_listen(SP_PORT)) <= 0)
		return sock;

	printf("绑定端口[%d]成功\n", SP_PORT);

	listen(sock, 5);
	
	while(1){
		cli_sock = accept(sock, (struct sockaddr *)&client, &addr_size);
		if(cli_sock == INVALID_SOCKET)
			return BUILD_ERROR(_OSerrno(), 0);
						
			/*创建线程来处理该连接*/
		_beginthreadex(NULL, 0, server_thread, (void *)cli_sock, 0, NULL);
	}
}



int main()
{
	init_network();

	_beginthreadex(NULL, 0, client_thread, NULL, 0, NULL);
	start_listen();

	while(1)
	{
		Sleep(1000);
	}

	return 0;
}