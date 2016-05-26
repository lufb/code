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
	/*��װͷ*/
	head->msg_len = htonl(msg_len);
	head->msg_id = htonl((unsigned int)SGIP_REPORT);
	memcpy(&(head->msg_serial), &ser, sizeof(head->msg_serial));
	
	/*��װ��*/
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

/*���÷���ֵ*/
void
try_send_report(int sock, struct serial &ser,struct report_list &rep_list)
{
	struct msg_report	rep_msg;
	int					err;

	if(del_report(rep_msg, rep_list) == 0){/*��report����*/
		if((err = send_report(sock, ser, &rep_msg)) != 0)
			g_log.writeLog(LOG_TYPE_ERROR,
				"reportʱ���׽���[%d]��������[%d :%d]",
				sock, GET_SYS_ERR(err), GET_USER_ERR(err));
	}
}

int
recv_report(int sock)
{
	char				rcv_buff[1024] = {0};
	struct msg_head		*phead = (struct msg_head*)rcv_buff;
	struct response		*prsp = (struct response*)(rcv_buff + sizeof(struct msg_head));
	size_t				rcv_size = sizeof(struct msg_head) + sizeof(struct response);/* ������Ӧ���յ����ݴ�С */
	int					err;

	//��ͷ
	if((err = rcv_non_block(sock, rcv_buff, sizeof(struct msg_head))) != 0)
		return err;

	if(ntohl(phead->msg_id) != SGIP_REPORT_RESP)
		return BUILD_ERROR(0, E_RSP);

	if(ntohl(phead->msg_len) != rcv_size)
		return BUILD_ERROR(0, E_HEAD_SIZE);

	//����
	if((err = rcv_non_block(sock, 
		rcv_buff+sizeof(struct msg_head), 
		sizeof(struct response))) != 0)
		return err;

	if(prsp->res_code != 0)
		g_log.writeLog(LOG_TYPE_ERROR,
		"�׽���[%d]�յ���report�Ļ�Ӧ�У�������[%d]",
		sock, prsp->res_code);

	char			tmp[8] = {0};
	if(memcmp(prsp->reserv, tmp, sizeof(prsp->reserv)) != 0)
		g_log.writeLog(LOG_TYPE_ERROR,
		"�׽���[%d]�յ���report�Ļ�Ӧ�У��������ݲ�ȫΪ0",
		sock);

	return 0;
}

/*report�߳�,*/
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
		printf("report, ����[%s:%d]����[%d]\n", g_option.sp_IP, g_option.sp_port, srv_sock);
		Sleep(150);
		goto begin;
	}
	g_log.writeLog(LOG_TYPE_INFO, "report�׽���[%d]����[%s:%d]�ɹ�", 
		srv_sock, g_option.sp_IP, g_option.sp_port);

	init_serial(ser);		/*���к�*/

	if((err = do_bind(srv_sock, ser)) != 0){
		g_log.writeLog(LOG_TYPE_ERROR,
			"reportʱ��do_bindʧ��[%d:%d]", GET_SYS_ERR(err), GET_USER_ERR(err));
		closesocket(srv_sock);
		Sleep(150);
		goto begin;
	}
	g_log.writeLog(LOG_TYPE_INFO, "report�׽���[%d]bind�ɹ�", 
		srv_sock);

	begin = time(NULL);
		
	setnonblocking(srv_sock);	//����Ϊ�첽
	set_sock_buf(srv_sock, 1024*128);	//�շ���������С
	
	while(1){
		FD_ZERO(&fd_read);
		FD_ZERO(&fd_write);
		FD_SET(srv_sock, &fd_read);
		FD_SET(srv_sock, &fd_write);
		
		err = select(0, &fd_read, &fd_write, NULL, &tv);
		if(err > 0){
			begin = time(NULL);
			if(FD_ISSET(srv_sock, &fd_read)){	/*�ɶ�*/
				if((err = recv_report(srv_sock)) != 0){
					g_log.writeLog(LOG_TYPE_ERROR,
						"report�߳���,�׽���[%d]�����ݷ�������[%d: %d]",
						srv_sock, GET_SYS_ERR(err), GET_USER_ERR(err));
					closesocket(srv_sock);
					goto begin;
				}
			}
			
			if(FD_ISSET(srv_sock, &fd_write)){	/*��д*/
				try_send_report(srv_sock, ser, g_rep_list);	/*���Է�report*/
			}
		}else if(err == 0){/*��ʱ*/
			now = time(NULL);
			if(now - begin > (int)g_option.smg2sp_timeout){
				g_log.writeLog(LOG_TYPE_ERROR, "�׽���[%d]��ʱ", srv_sock);
				closesocket(srv_sock);
				goto begin;	/*ȥ���½�������*/
			}
		}else{assert(0);}
		Sleep(15);//��ֹCPU����
	}
	
	
	return 0;
		
#if 0	
	struct msg_report		*report = (struct msg_report *)in;
	int						sock, err;
	struct serial			ser;
	
	assert(report != NULL);
	if((sock = block_connect(g_option.sp_IP, g_option.sp_port)) < 0){
		g_log.writeLog(LOG_TYPE_ERROR,
			"reportʱ������[%s:%d]����[%d]",
			g_option.sp_IP, g_option.sp_port, sock);
		free(report);
		return sock;
	}

	g_log.writeLog(LOG_TYPE_INFO,
		"reportʱ���׽���[%d]����[%s:%d]�ɹ�",
			 sock, g_option.sp_IP, g_option.sp_port);
	
	init_serial(ser);		/*���к�*/
	
	if((err = do_bind(sock, ser)) != 0){
		g_log.writeLog(LOG_TYPE_ERROR,
			"reportʱ��do_bindʧ��[%d:%d]", GET_SYS_ERR(err), GET_USER_ERR(err));
		free(report);
		closesocket(sock);
		return err;
	}
	
	if((err = do_report(sock, ser, report)) != 0){
		g_log.writeLog(LOG_TYPE_ERROR,
			"reportʱ��do_reportʧ��[%d]", err);
		free(report);
		closesocket(sock);
		return err;
	}
	
	if(err = do_unbind(sock, ser) != 0){
		printf("do_unbindʧ��[%d]\n", err);
		free(report);
		closesocket(sock);
		return err;
	}
	
	g_log.writeLog(LOG_TYPE_INFO,
			"reportʱ���׽���[%d] report�ɹ�",
			 sock);

	free(report);
	closesocket(sock);
	return 0;
#endif
}