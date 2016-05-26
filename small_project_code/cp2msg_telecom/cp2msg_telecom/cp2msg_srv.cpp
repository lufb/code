#include <process.h>
#include <assert.h>
#include <time.h>
#include "cp2msg_srv.h"
#include "sock_op.h"
#include "global.h"
#include "error_define.h"
#include "protocol.h"
#include "local_define.h"
#include "pro_interface.h"


int	
start_srv_mode()
{
	int					lis_sock;
	
	if((lis_sock = create_listen(LOCAL_PORT)) < 0)
		return lis_sock;
	
	_beginthreadex(NULL, 0, smg_listen, (void *)lis_sock, 0, NULL);
	
	return 0;
}

/*
	打印链路出错信息，并关闭链路
*/
void
clean_link(int sock, int err_code)
{
	g_log.writeLog(LOG_TYPE_INFO, 
		"套接字[%d]链路发生错误码[%d:%d]", 
		sock, GET_SYS_ERR(err_code), 
		GET_USER_ERR(err_code));

	closesocket(sock);
}

/*开始监听*/
unsigned int __stdcall
smg_listen(void *in)
{
	int					lis_sock = (int)in;
	struct sockaddr_in	client;
	int					addr_size = sizeof(client);
	int					cli_sock;
	
	listen(lis_sock, 5);
	g_log.writeLog(LOG_TYPE_INFO, "启动监听端口[%d]成功，开始监听", LOCAL_PORT);
	while(1){
		cli_sock = accept(lis_sock, (struct sockaddr *)&client, &addr_size);
		if(cli_sock == INVALID_SOCKET)
			return BUILD_ERROR(_OSerrno(), 0);
		g_log.writeLog(LOG_TYPE_INFO,
			"通过监听端口[%d] 从[%s:%d] 收到socket[%d]",
			LOCAL_PORT, inet_ntoa(client.sin_addr),
			ntohs(client.sin_port), cli_sock);
		if(g_cli_queue.Insert(cli_sock, inet_ntoa(client.sin_addr), ntohs(client.sin_port)))
			_beginthreadex(NULL, 0, srv_thread, (void *)cli_sock, 0, NULL);
		else{
			g_log.writeLog(LOG_TYPE_ERROR,
				"向客户端列表添加数据失败");
			clean_link(cli_sock, BUILD_ERROR(0, E_FULL_QUE));
		}
	}
	
	return 0;
}

void
head_ntol(tagMsgHead &head)
{
	head.PacketLength = ntohl(head.PacketLength);
	head.RequestID = ntohl(head.RequestID);
	head.SequenceID = ntohl(head.SequenceID);
}

void
head_hton(tagMsgHead &head)
{
	head.PacketLength = htonl(head.PacketLength);
	head.RequestID = htonl(head.RequestID);
	head.SequenceID = htonl(head.SequenceID);
}

void
build_login_rsp(const tagLogin *p_login, tagLoginResp *login_rsp)
{
	assert(login_rsp != NULL);
	login_rsp->Status = htonl(PRO_SUCESS);
	strncpy(login_rsp->AuthenticatorServer, "need cal md5", sizeof(login_rsp->AuthenticatorServer));
#if 0
	if(strncmp(p_login->ClientID, LOGIN_ID, sizeof(p_login->ClientID)) != 0 &&
		strncmp(p_login->AuthenticatorClient, LOGIN_AUTH, sizeof(p_login->AuthenticatorClient)) != 0){
		login_rsp->Status = htonl(LOGIN_ERR_AUTH);	/* 认证出错	*/
		memset(login_rsp->AuthenticatorServer, 0, sizeof(login_rsp->AuthenticatorServer));
	}else{
		login_rsp->Status = htonl(PRO_SUCESS);
		/* fixme 这里要算md5值	*/
		strncpy(login_rsp->AuthenticatorServer, "need cal md5", sizeof(login_rsp->AuthenticatorServer));
	}
#endif
	/* fixme 这里版本号我填的最大值	*/
	login_rsp->ServerVersion = (unsigned char)-1;
}

int
do_login(const char *rcv_body, size_t body_size, char *rsp_body)
{
	if(body_size != sizeof(tagLogin))
		return BUILD_ERROR(0, E_ERR_SIZE);

	build_login_rsp((const tagLogin*)rcv_body, (tagLoginResp*)rsp_body);

	return sizeof(tagLoginResp);
}

void
build_submit_rsp(tagSubmitRsp *rsp, const char *msgid)
{
	//rsp->Status = htonl(PRO_SUCESS);
	rsp->Status = htonl(12);

	memset(rsp->MsgID, 0, sizeof(rsp->MsgID));
	strncpy(rsp->MsgID, msgid, sizeof(rsp->MsgID));
}

int
SubMitIsOk(const char *rcv_body, size_t body_size)
{
	return 0;
}

void
addReport(const char *msgid, size_t msgid_size, int sock)
{
	if(g_deliver_que.Insert(1, msgid, sock) == false){
		assert(0);
		g_log.writeLog(LOG_TYPE_ERROR, "添加deliver队列失败");
	}
}

int
do_submit(const char *rcv_body, size_t body_size, char *rsp_body, int sock)
{
	int					err;

	if(body_size < sizeof(tagSubMit_1) + sizeof(tagSubMit_2) + sizeof(tagSubMit_3)){
		assert(0);
		return BUILD_ERROR(0, E_ERR_SIZE);
	}

	if((err = SubMitIsOk(rcv_body, body_size)) != 0)
		g_log.writeLog(LOG_TYPE_ERROR, "收到的submit包异常[%u:%u]", GET_SYS_ERR(err), GET_USER_ERR(err));

	char	msgid[10];
	build_msgid(msgid, sizeof(msgid));
	build_submit_rsp((tagSubmitRsp*)rsp_body, msgid);

	tagSubMit_1 *pSubmit1 = (tagSubMit_1 *)rcv_body;
	if(pSubmit1->NeedReport)	/* fixme 这里要查看是否要状态报告 */
		addReport(msgid, sizeof(msgid), sock);

	return sizeof(tagSubmitRsp);
}

int
do_deliver_rsp(const char *rcv_body, size_t body_size)
{
	if(body_size != sizeof(tagDeliverRsp))
		return BUILD_ERROR(0, E_ERR_SIZE);

	tagDeliverRsp *p_rsp = (tagDeliverRsp*)rcv_body;
	unsigned int	status = ntohl(p_rsp->Status);
	if(status != PRO_SUCESS)
		g_log.writeLog(LOG_TYPE_ERROR, "收到的deliverrsp状态码为[%u]", p_rsp);

	return 0;
}

int
do_active(const char *rcv_body, size_t body_size)
{
	if(body_size != 0)
		return BUILD_ERROR(0, E_ERR_SIZE);
	
	return 0;
}

int
do_exit(const char *rcv_body, size_t body_size)
{
	if(body_size != 0)
		return BUILD_ERROR(0, E_ERR_SIZE);
	
	return 0;
}


int
rcv_msg2rsp(int sock)
{
	tagMsgHead				msgHead;
	char					body[8192];	/* FIXME: 从现有协议来看，8192的数据体是足够的,后面不用校验体大小是否大于8192*/
	char					rsp[8192];	/* FIXME: 从现有协议来看，8192的数据体是足够的,后面不用校验体大小是否大于8192*/
	unsigned int			body_size;
	int						err;

	if((err = rcv_non_block(sock, (char *)&msgHead, sizeof(msgHead))) != 0){
		g_log.writeLog(LOG_TYPE_ERROR, "套接字[%d]收协议头出错[%u:%u]", sock, GET_SYS_ERR(err), GET_USER_ERR(err));
		return err;
	}
	head_ntol(msgHead);	
	assert(msgHead.PacketLength >= sizeof(tagMsgHead));
	body_size = msgHead.PacketLength-sizeof(tagMsgHead);
	assert(body_size <= sizeof(body));

	if((err = rcv_non_block(sock, body, body_size)) != 0){
		g_log.writeLog(LOG_TYPE_ERROR, "套接字[%d]收协议体出错[%u:%u]", sock, GET_SYS_ERR(err), GET_USER_ERR(err));
		return err;
	}
	
	/* 以下switch分支语句的err，>=0表示需要返回体的大小，<0 表示出错，对应error_define.h的错误码*/
	switch(msgHead.RequestID)
	{
		case LOGIN:
			err = do_login(body, body_size, rsp+sizeof(tagMsgHead));
			msgHead.RequestID = LONGIN_RSP;
			break;
		case SUBMIT:
			err = do_submit(body, body_size, rsp+sizeof(tagMsgHead), sock);
			msgHead.RequestID = SUBMIT_RSP;
			break;
		case DELIVER_RSP:
			return do_deliver_rsp(body, body_size);
			break;
		case ACTIVE_TEST:
			err = do_active(body, body_size);
			msgHead.RequestID = ACTIVE_TEST_RSP;
			break;
		case EXIT:
			err = do_exit(body, body_size);
			msgHead.RequestID = EXIT_RSP;
			break;
		default:
			g_log.writeLog(LOG_TYPE_ERROR, "srv模块收到未知协议号[%0x]", msgHead.RequestID);
			return BUILD_ERROR(0, E_ERR_PRO);
	}
	
	assert(err >= 0);
	msgHead.PacketLength = sizeof(tagMsgHead)+err;
	head_hton(msgHead);
	memcpy(rsp, &msgHead, sizeof(msgHead));
	if((err = send_nonblock(sock, rsp, sizeof(tagMsgHead)+err)) != 0)
		g_log.writeLog(LOG_TYPE_ERROR, "套接字回应协议出错[%u :%u]", GET_SYS_ERR(err), GET_USER_ERR(err));
	
	return err;
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
	
	setnonblocking(sock);			/*设置套接字非阻塞*/
	set_sock_buf(sock, SOCK_SND2RCV_BUFF_SIZE);	/*设置套接字缓冲区大小*/
	
	tv.tv_sec = SELECT_TV_TIMEOUT;
	tv.tv_usec = 0;
	
	while(1){
		FD_ZERO(&fd_read);
		FD_SET((unsigned int)sock, &fd_read);
		
		err = select(0, &fd_read, NULL, NULL, &tv);
		if(err == 0){	/*没数据,超时*/
			now_time = time(NULL);
			if(now_time - last_time > TIME_OUT){	/*链路超时*/
				err = BUILD_ERROR(0, E_LINK_TIMEOUT);
			}
		}else if(err > 0){
			last_time = time(NULL);
			if(FD_ISSET(sock, &fd_read)){
				err = rcv_msg2rsp(sock);
			}else{assert(0);}
		}else{assert(0);}	/*不应该的流程*/

		/* 检测应用层是否需要断开链路 */
		if(err != 0){
			assert(g_cli_queue.Del(sock) == true);
			clean_link(sock, err);
			return err;
		}
	}
	
	assert(0);
	return 0;
}