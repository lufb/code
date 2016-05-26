#include "report.h"
#include "global.h"
#include "socket.h"
#include "protocol.h"
#include <assert.h>
#include "error_define.h"
#include <process.h>
#include <time.h>

int
send_report(int sock, struct serial &ser, struct msg_report *r)
{
	char						buf[1024] = {0};
	struct msg_head				*head = (struct msg_head*)buf;
	struct msg_report			*report = (struct msg_report*)(buf+sizeof(struct msg_head));
	unsigned int				msg_len;
	
	msg_len = sizeof(struct msg_head)+
		sizeof(struct msg_report);
	/*组装头*/
	head->msg_len = htonl(msg_len);
	head->msg_id = htonl((unsigned int)SGIP_REPORT);
	memcpy(&(head->msg_serial), &ser, sizeof(head->msg_serial));
	
	/*组装体*/
	memcpy(report, r, sizeof(struct msg_report));
	if(send(sock, buf, msg_len, 0) != (int)msg_len)
		return BUILD_ERROR(_OSerrno(), E_BLOCK_SEND);
	
	return 0;
}


int
start_report()
{
	_beginthreadex(NULL, 0, report_func, NULL, 0, NULL);
	
	return 0;
}

/*不用返回值*/
void
try_send_report(int sock, struct serial &ser,struct report_list &rep_list)
{
	struct msg_report	rep_msg;
	int					err;

	if(del_report(rep_msg, rep_list) == 0){/*有report数据*/
		if((err = send_report(sock, ser, &rep_msg)) != 0)
			g_log.writeLog(LOG_TYPE_ERROR,
				"report时，套接字[%d]发生错误[%d :%d]",
				sock, GET_SYS_ERR(err), GET_USER_ERR(err));
	}
}

int
recv_report(int sock)
{
	char				rcv_buff[1024] = {0};
	struct msg_head		*phead = (struct msg_head*)rcv_buff;
	struct response		*prsp = (struct response*)(rcv_buff + sizeof(struct msg_head));
	size_t				rcv_size = sizeof(struct msg_head) + sizeof(struct response);/* 理论上应该收的数据大小 */
	int					err;

	//收头
	if((err = rcv_non_block(sock, rcv_buff, sizeof(struct msg_head))) != 0)
		return err;

	if(ntohl(phead->msg_id) != SGIP_REPORT_RESP)
		return BUILD_ERROR(0, E_RSP);

	if(ntohl(phead->msg_len) != rcv_size)
		return BUILD_ERROR(0, E_HEAD_SIZE);

	//收体
	if((err = rcv_non_block(sock, 
		rcv_buff+sizeof(struct msg_head), 
		sizeof(struct response))) != 0)
		return err;

	if(prsp->res_code != 0)
		g_log.writeLog(LOG_TYPE_ERROR,
		"套接字[%d]收到的report的回应中，错误码[%d]",
		sock, prsp->res_code);

	char			tmp[8] = {0};
	if(memcmp(prsp->reserv, tmp, sizeof(prsp->reserv)) != 0)
		g_log.writeLog(LOG_TYPE_ERROR,
		"套接字[%d]收到的report的回应中，保留数据不全为0",
		sock);

	return 0;
}

/*report线程,*/
unsigned int __stdcall
report_func(void *in)
{
	int					srv_sock;
	int					err;
	struct timeval		tv;	
	fd_set				fd_read, fd_write;
	int					begin, now;
	struct serial		ser;
	
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	
begin:
	if((srv_sock = block_connect(g_option.sp_IP, g_option.sp_port)) < 0){
		printf("report, 连接[%s:%d]出错[%d]\n", g_option.sp_IP, g_option.sp_port, srv_sock);
		Sleep(150);
		goto begin;
	}
	g_log.writeLog(LOG_TYPE_INFO, "report套接字[%d]连接[%s:%d]成功", 
		srv_sock, g_option.sp_IP, g_option.sp_port);

	init_serial(ser);		/*序列号*/

	if((err = do_bind(srv_sock, ser)) != 0){
		g_log.writeLog(LOG_TYPE_ERROR,
			"report时，do_bind失败[%d:%d]", GET_SYS_ERR(err), GET_USER_ERR(err));
		closesocket(srv_sock);
		Sleep(150);
		goto begin;
	}
	g_log.writeLog(LOG_TYPE_INFO, "report套接字[%d]bind成功", 
		srv_sock);

	begin = time(NULL);
		
	setnonblocking(srv_sock);	//设置为异步
	set_sock_buf(srv_sock, 1024*128);	//收发缓冲区大小
	
	while(1){
		FD_ZERO(&fd_read);
		FD_ZERO(&fd_write);
		FD_SET(srv_sock, &fd_read);
		FD_SET(srv_sock, &fd_write);
		
		err = select(0, &fd_read, &fd_write, NULL, &tv);
		if(err > 0){
			begin = time(NULL);
			if(FD_ISSET(srv_sock, &fd_read)){	/*可读*/
				if((err = recv_report(srv_sock)) != 0){
					g_log.writeLog(LOG_TYPE_ERROR,
						"report线程中,套接字[%d]收数据发生错误[%d: %d]",
						srv_sock, GET_SYS_ERR(err), GET_USER_ERR(err));
					closesocket(srv_sock);
					goto begin;
				}
			}
			
			if(FD_ISSET(srv_sock, &fd_write)){	/*可写*/
				try_send_report(srv_sock, ser, g_rep_list);	/*尝试发report*/
			}
		}else if(err == 0){/*超时*/
			now = time(NULL);
			if(now - begin > (int)g_option.smg2sp_timeout){
				g_log.writeLog(LOG_TYPE_ERROR, "套接字[%d]超时", srv_sock);
				closesocket(srv_sock);
				goto begin;	/*去重新建立连接*/
			}
		}else{assert(0);}
		Sleep(15);//防止CPU过高
	}
	
	
	return 0;
		
#if 0	
	struct msg_report		*report = (struct msg_report *)in;
	int						sock, err;
	struct serial			ser;
	
	assert(report != NULL);
	if((sock = block_connect(g_option.sp_IP, g_option.sp_port)) < 0){
		g_log.writeLog(LOG_TYPE_ERROR,
			"report时，连接[%s:%d]出错[%d]",
			g_option.sp_IP, g_option.sp_port, sock);
		free(report);
		return sock;
	}

	g_log.writeLog(LOG_TYPE_INFO,
		"report时，套接字[%d]连接[%s:%d]成功",
			 sock, g_option.sp_IP, g_option.sp_port);
	
	init_serial(ser);		/*序列号*/
	
	if((err = do_bind(sock, ser)) != 0){
		g_log.writeLog(LOG_TYPE_ERROR,
			"report时，do_bind失败[%d:%d]", GET_SYS_ERR(err), GET_USER_ERR(err));
		free(report);
		closesocket(sock);
		return err;
	}
	
	if((err = do_report(sock, ser, report)) != 0){
		g_log.writeLog(LOG_TYPE_ERROR,
			"report时，do_report失败[%d]", err);
		free(report);
		closesocket(sock);
		return err;
	}
	
	if(err = do_unbind(sock, ser) != 0){
		printf("do_unbind失败[%d]\n", err);
		free(report);
		closesocket(sock);
		return err;
	}
	
	g_log.writeLog(LOG_TYPE_INFO,
			"report时，套接字[%d] report成功",
			 sock);

	free(report);
	closesocket(sock);
	return 0;
#endif
}