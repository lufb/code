#include "protocol.h"
#include <stddef.h>
#include <process.h>
#include <assert.h>
#include <time.h>
#include "sp2smg_srv.h"
#include "socket.h"
#include "global.h"
#include "error_define.h"
#include "create_rand.h"
#include "report.h"





int
rcv_head(int sock, struct msg_head* head)
{
	int						err;

	err = rcv_non_block(sock, (char *)head, sizeof(struct msg_head));
	if(err != 0)
		return err;
	
	return 0;
}

void
pr_usr_name(struct msg_bind *bind)
{
	char				tmpbuf[256] = {0};
	
	memcpy(tmpbuf, 
		bind + offsetof(struct msg_bind, login_name), 
		sizeof(bind->login_name));

	g_log.writeLog(LOG_TYPE_ERROR,
		"username[%s]", tmpbuf);
}

void
pr_usr_pass(struct msg_bind *bind)
{
	char				tmpbuf[256] = {0};
	
	memcpy(tmpbuf, 
		bind + offsetof(struct msg_bind, login_pass), 
		sizeof(bind->login_pass));
	
	g_log.writeLog(LOG_TYPE_ERROR,
		"passwd[%s]", tmpbuf);
}

/*校验保留字节是否全是0*/
int	
check_resver(const char * reserv, size_t len)
{
#define RESERV_LEN		8
	assert(len == RESERV_LEN);
	char		res[RESERV_LEN] = {0};

	assert(len == RESERV_LEN);
	if(memcmp(reserv, res, RESERV_LEN) != 0)
		return BUILD_ERROR(0, E_RESERV);
#undef 	RESERV_LEN

	return 0;
}


int
check_bind(const struct msg_bind *msg_bind,  char *rsp_err)
{
	int						err;

	if(memcmp(msg_bind->login_name, g_option.sp2smg_usrname, sizeof(msg_bind->login_name)) != 0){
		*rsp_err = (char)1;	/*用户名或者密码出错*/
		g_log.writeLog(LOG_TYPE_ERROR, "用户名不正确[%d]", BUILD_ERROR(0, E_USERNAME));
		return 0;		/* 假成功 */
	}

	if(memcmp(msg_bind->login_pass, g_option.sp2smp_usrpass, sizeof(msg_bind->login_pass)) != 0){
		*rsp_err = (char)1;	/*用户名或者密码出错*/
		g_log.writeLog(LOG_TYPE_ERROR, "密码不正确[%d]", BUILD_ERROR(0, E_USERNAME));
		return 0;		/* 假成功 */
	}

	if((err = check_resver(msg_bind->reserv, sizeof(msg_bind->reserv))) != 0){
		*rsp_err = (char)5;	/*协议数据不符合标准*/
		return err;
	}

	return 0;
}

/*校验字符串每个字符在[min, max]之间，0除外*/
int
check_text(const char *p, size_t size, char min, char max)
{
	for(size_t i = 0; i < size; ++i)
		if(p[i] != 0)
			if(p[i] < min || p[i] > max)
				return BUILD_ERROR(0, E_TEXT);

	return 0;
}

/*
	校验submit包
	返回值：
		== 0	链路正常
		< 0		校验失败，带错误码
		> 0		该包需要report
*/
int	
check_submit(const char *data,  char *rsp_err, size_t size)
{
	assert(size > sizeof(struct msg_submit));
	struct msg_submit  *submit = (struct msg_submit *)data;
	int					err;
	const char				*tmp = NULL;

	if(submit->sub_usr_count < 1 || submit->sub_usr_count > 100){
		g_log.writeLog(LOG_TYPE_ERROR, 
			"接收短信的手机数量[%d]", submit->sub_usr_count);
		return BUILD_ERROR(0, E_CHECK);
	}

	if((err = check_resver(data+size-8, 8)) != 0)
		return BUILD_ERROR(0, E_CHECK);
		
	/*开始校验文本内容为0到9的*/
	tmp = data + offsetof(struct msg_submit, sub_corp_id);
	if((err = check_text(tmp, 5, '0', '9')) != 0)
		return err;

	tmp = data + offsetof(struct msg_submit, sub_freevalue);
	if((err = check_text(tmp, 6, '0', '9')) != 0)
		return err;

	tmp = data + offsetof(struct msg_submit, sub_giv_value);
	if((err = check_text(tmp, 6, '0', '9')) != 0)
		return err;

	tmp = data + offsetof(struct msg_submit, sub_agent_flag);
	if(tmp[0] != 0 && tmp[0] != 1)
		return BUILD_ERROR(0, E_CHECK);

	tmp = data + offsetof(struct msg_submit, sub_morelatetomt);
	if(tmp[0] != 0 && tmp[0] != 1 && tmp[0] != 2 && tmp[0] != 3)
		return BUILD_ERROR(0, E_CHECK);
	
	tmp = data + offsetof(struct msg_submit, sub_msg_type);
	if(tmp[0] != 0)
		return BUILD_ERROR(0, E_CHECK);

	unsigned int msg_len = ntohl(((struct msg_submit*)data)->sub_msg_len);
	if(msg_len != size-sizeof(struct msg_submit)-8)
		return BUILD_ERROR(0, E_MSG_LEN);

	tmp = data + offsetof(struct msg_submit, sub_report_flag);
	if(tmp[0] != 0 && tmp[0] != 1 && tmp[0] != 2 && tmp[0] != 3)
		return BUILD_ERROR(0, E_CHECK);

	if(tmp[0] == 1 || tmp[0] == 3)			/*要状态报告(只模拟消息执行成功)，出错时的状态回报暂不添加*/
		return 1;


	return 0;
}

int 
rcv_bind(int sock, const struct msg_head *head,  char *rsp_err)
{
	assert(ntohl(head->msg_id) == SGIP_BIND);
	assert(ntohl(head->msg_len) == sizeof(struct msg_head) + sizeof(struct msg_bind));

	struct msg_bind		bind_msg;
	int					err;

	if((err = rcv_non_block(sock, (char *)&bind_msg, sizeof(bind_msg))) != 0)
		return err;

	if((err = check_bind(&bind_msg, rsp_err)) != 0)
		return err;
	
	return 0;
}

/*
	收unbind数据
	== 0	成功(即没得数据可收)
	!= 0	失败(链路中还有数据)
*/
int
rcv_unbind(int sock, const struct msg_head *head,  char *rsp_err)
{
	assert(ntohl(head->msg_id) == SGIP_UNBIND);
	assert(ntohl(head->msg_len) == sizeof(struct msg_head));
	
	*rsp_err = (char)0;
	return 0;
}


/*由submit消息填充report消息*/
void
fill_report(const struct msg_head *head, const struct msg_submit* sub, struct msg_report *report)
{
	//unsigned char	state = (unsigned char)creat_rand(0, 2);	/*随机生成状态，发送出错状态不模拟*/

	memmove(&report->report_serial, &(head->msg_serial), sizeof(report->report_serial));
	report->report_type = (unsigned char)0;	/*submit的报告*/
	memmove(report->report_usr_num, &(sub->sub_usr_num), sizeof(report->report_usr_num));
	report->report_state = (unsigned char)0;/*只可能是成功*/
	
	report->report_errcode = (unsigned char)0;/*只可能是成功*/

	memset(report->report_reserver, 0, sizeof(report->report_reserver));
}

// int
// add_report(const struct msg_head *head, struct msg_submit* sub)
// {
// 	struct msg_node		msg;		/*需要添加的消息*/
// 	int					err;
// 
// 	/*填头*/
// 	msg.head.msg_len = sizeof(struct msg_head) + sizeof(struct msg_report);
// 	msg.head.msg_id = SGIP_REPORT;
// 	memmove(&msg.head.msg_serial, &(head->msg_serial), sizeof(head->msg_serial));
// 
// 	/*填体report消息体*/
// 	fill_report(head, sub, msg.report);
// 	
// 	/*加到消息队列*/
// 	if((err = add_msg(msg)) < 0)
// 		return err;
// 
// 	return 0;
// }



/*
	收unbind数据
	== 0	成功
	!= 0	失败
*/
int
rcv_submit(int sock, const struct msg_head *head,  char *rsp_err)
{
	size_t		msg_size = ntohl(head->msg_len);
	assert(msg_size > sizeof(struct msg_head));
	size_t		body_size = msg_size - sizeof(struct msg_head);
	char		*pbuf = NULL;
	int			err = 0;


	assert(ntohl(head->msg_id) == SGIP_SUBMIT);
	assert(msg_size > sizeof(struct msg_head) + sizeof(struct msg_submit));

	if((pbuf = (char *)malloc(msg_size)) == NULL)
		return BUILD_ERROR(0, E_MEM_LEAK);
	
	if((err = rcv_non_block(sock, pbuf, msg_size-sizeof(struct msg_head))) != 0)
		goto ret;

	if((err = check_submit(pbuf, rsp_err, body_size)) != 0){
		if(err == 1){	//要状态报告，假成功
			struct msg_report	report;
			fill_report(head, (struct msg_submit*)pbuf, &report);
			g_log.writeLog(LOG_TYPE_INFO,
				"套接字[%d]需要report消息", sock);

			if((err = add_report(report, g_rep_list)) != 0)
				g_log.writeLog(LOG_TYPE_INFO,
				"套接字[%d]添加report队列失败[%d: %d]",
				sock, GET_SYS_ERR(err), GET_USER_ERR(err));
			//创建线程处理report
			//_beginthreadex(NULL, 0, report_func, (void *)report, 0, NULL);
			err = 0;/*要恢复错误码*/
		}
		goto ret;
	}

ret:
	free(pbuf);
	if(err != 0)
		*rsp_err = 5;/*参数格式出错*/
	return err;
}


int
rcv_body(int sock, const struct msg_head *head,  char *rsp_err)
{
	int						err = 0;

	unsigned int		msg_id = ntohl(head->msg_id);	/*转化为本机字节序*/

	switch(msg_id)
	{
	case SGIP_BIND:
		err = rcv_bind(sock, head, rsp_err);
		break;
	case SGIP_UNBIND:
		err = rcv_unbind(sock, head, rsp_err);
		break;
	case SGIP_SUBMIT:
		err = rcv_submit(sock, head, rsp_err);
		break;
	default:
		g_log.writeLog(LOG_TYPE_ERROR, "SP->SMG不认识的类型[%0x]", msg_id);
		*rsp_err = (char)7;/*消息ID出错*/
		err = BUILD_ERROR(0, E_ERRID);
		break;
	}

	if(err != 0)
		g_log.writeLog(LOG_TYPE_ERROR, 
		"套接字[%d]收类型为[%x]体时错误码[%d:%d]",
		sock, msg_id,
		GET_SYS_ERR(err), GET_USER_ERR(err));

	return err;
}

/*
	回应消息体
	返回值：
		==0	成功
		!=0 失败
*/
int
send_rsp(int sock, char res_err, struct msg_head *h)
{
	char							buf[256] = {0};		/*初始化为0*/
	struct msg_head					*head = (struct msg_head *)buf;
	unsigned int					send_size = sizeof(struct msg_head);
	struct response					*res = (struct response *)(buf+sizeof(struct msg_head));
	int								err;

	memmove(&(head->msg_serial), &(h->msg_serial), sizeof(struct serial));
	res->res_code = res_err;				/*填充错误码*/

	/*更新相应数据*/
	if(ntohl(h->msg_id) == SGIP_UNBIND){
		head->msg_id = htonl(SGIP_UNBIND_RESP);
	}else if(ntohl(h->msg_id) == SGIP_SUBMIT){
		head->msg_id = htonl(SGIP_SUBMIT_RESP);
		send_size += sizeof(struct response);
	}else if(ntohl(h->msg_id) == SGIP_BIND){
		head->msg_id = htonl(SGIP_BIND_RESP);
		send_size += sizeof(struct response);
	}else{assert(0);}

	head->msg_len = htonl(send_size);

	if((err = send_nonblock(sock, buf, send_size)) < 0 )
		return err;



	return 0;
}

/*
	收协议数据，并回应协议数据
	返回值：
		== 0	成功，链路保持
		!= 0	失败，或者链路要关闭，上级需要关闭
*/
int
rcv_msg2rsp(int sock)
{
	struct msg_head				head;
	int							err;
	char						res_err = 0;
	bool						is_unbind = false;

	memset(&head, 0, sizeof(head));

	if((err = rcv_head(sock, &head)) != 0)
		return err;

	if((err = rcv_body(sock, &head, &res_err)) != 0)
		return err;

	if((err = send_rsp(sock, res_err, &head)) != 0)
		return err;

	if(ntohl(head.msg_id) == SGIP_UNBIND)
		err = BUILD_ERROR(0, E_CLI_UNBIND);
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

// void
// getmytime(unsigned int *des)
// {
// 	struct tm			*newtime;
// 	time_t				now;
// 	
// 	time(&now);
// 	newtime = localtime(&now);
// 	
// 	memset(des, 0, sizeof(unsigned int));
// 	sprintf((char*)des, "%02d%02d%02d%02d%02d", 
// 		newtime->tm_mon+1,
// 		newtime->tm_mday,
// 		newtime->tm_hour,
// 		newtime->tm_min,
// 		newtime->tm_sec);
// 	
// 	//printf("%s\n", (char *)des);
// }

void
init_serial(struct serial &s)
{
	static unsigned int		serial_num = 0;
	
	s.serialnum[0] = htonl((unsigned int)1);
	s.serialnum[1] = htonl(time(NULL));
	s.serialnum[2] = htonl((unsigned int)serial_num);	/*序号依次增大*/
	++serial_num;
}


/*发送bind消息*/
int
send_bind(int sock, struct serial &ser)
{
	char						buf[1024] = {0};
	struct msg_head				*head = (struct msg_head*)buf;
	struct msg_bind				*bind = (struct msg_bind*)(buf+sizeof(struct msg_head));
	unsigned int				msg_len;

	msg_len = sizeof(struct msg_head)+sizeof(struct msg_bind);
	/*组装头*/
	head->msg_len = htonl(msg_len);
	head->msg_id = htonl((unsigned int)SGIP_BIND);
	memcpy(&(head->msg_serial), &ser, sizeof(head->msg_serial));

	/*组装体*/
	bind->login_type = (unsigned char)1;
	strncpy(bind->login_name, g_option.smg2sp_usrname, sizeof(bind->login_name));
	strncpy(bind->login_pass, g_option.smg2sp_usrpass, sizeof(bind->login_pass));
	memset(bind->reserv, 0, sizeof(bind->reserv));

	if(send(sock, buf, msg_len, 0) != (int)msg_len)
		return BUILD_ERROR(_OSerrno(), E_BLOCK_SEND);

	return 0;
}
/*
	== 0 成功
	<0   出错
	>0   服务器回应的错误码
*/
int
rcv_bindrsp(int sock)
{
	char						buff[1024];
	unsigned int				rcv_size = sizeof(struct msg_head) + sizeof(struct response);
	struct msg_head				*head = (struct msg_head *)buff;
	struct response				*resp = (struct response*)(buff + sizeof(struct msg_head));

	if(recv(sock, buff, rcv_size, 0) != (int)rcv_size)
		return BUILD_ERROR(_OSerrno(), E_BLOCK_RECV);

	if(ntohl(head->msg_len) != rcv_size){
		assert(0);
		return BUILD_ERROR(0, E_HEAD_SIZE);
	}

	if(ntohl(head->msg_id) != SGIP_BIND_RESP){
		assert(0);
		return BUILD_ERROR(0, E_RSP);
	}

	if(resp->res_code != 0){
		printf("bind消息回应的错误码[%d]\n", resp->res_code);
		return resp->res_code;
	}

	return 0;	
}




/*完成bind及bindrsp*/
int
do_bind(int sock, struct serial &ser)
{
	int					err;

	if((err = send_bind(sock, ser)) != 0)
		return err;
	
	if((err = rcv_bindrsp(sock)) != 0)
		return err;
	
	return 0;
}

/*发送deliver消息*/
int 
send_deliver(int sock, struct serial &ser)
{
	char						buf[1024] = {0};
	struct msg_head				*head = (struct msg_head*)buf;
	struct msg_deliver			*del = (struct msg_deliver*)(buf+sizeof(struct msg_head));
	char						*pmsg = buf+sizeof(struct msg_head)+sizeof(msg_deliver);
	unsigned int				msg_len;
	
	msg_len = sizeof(struct msg_head)+
				sizeof(struct msg_deliver)+
				strlen(DELIVER_MSG)+8;
	/*组装头*/
	head->msg_len = htonl(msg_len);
	head->msg_id = htonl((unsigned int)SGIP_DELIVER);
	memcpy(&(head->msg_serial), &ser, sizeof(head->msg_serial));
	
	/*组装体*/
	del->dlv_msg_cod = 0;
	del->dlv_msg_len = htonl(strlen(DELIVER_MSG));
	memcpy(pmsg, DELIVER_MSG, strlen(DELIVER_MSG));
	memset(pmsg+strlen(DELIVER_MSG), 0, 8);
	
	if(send(sock, buf, msg_len, 0) != (int)msg_len)
		return BUILD_ERROR(_OSerrno(), E_BLOCK_SEND);
	
	return 0;
}

/*
	== 0 成功
	<0   出错
	>0   服务器回应的错误码
*/
int
rcv_deliver_rsp(int sock)
{
	char						buff[1024];
	unsigned int				rcv_size = sizeof(struct msg_head) + sizeof(struct response);
	struct msg_head				*head = (struct msg_head *)buff;
	struct response				*resp = (struct response*)(buff + sizeof(struct msg_head));

	if(recv(sock, buff, rcv_size, 0) != (int)rcv_size)
		return BUILD_ERROR(_OSerrno(), E_BLOCK_RECV);

	if(ntohl(head->msg_len) != rcv_size){
		assert(0);
		return BUILD_ERROR(0, E_HEAD_SIZE);
	}

	if(ntohl(head->msg_id) != SGIP_DELIVER_RESP){
		assert(0);
		return BUILD_ERROR(0, E_RSP);
	}

	if(resp->res_code != 0){
		printf("deliver消息回应的错误码[%d]\n", resp->res_code);
		return resp->res_code;
	}

	return 0;	
}

int
do_deliver(int sock, struct serial &ser)
{
	int						err;

	if((err = send_deliver(sock, ser)) != 0)
		return err;

	if((err = rcv_deliver_rsp(sock)) != 0)
		return err;

	return 0;
}


/*发送unbind消息*/
int 
send_unbind(int sock, struct serial &ser)
{
	char						buf[1024] = {0};
	struct msg_head				*head = (struct msg_head*)buf;
	unsigned int				msg_len;
	
	msg_len = sizeof(struct msg_head);
	/*组装头*/
	head->msg_len = htonl(msg_len);
	head->msg_id = htonl((unsigned int)SGIP_UNBIND);
	memcpy(&(head->msg_serial), &ser, sizeof(head->msg_serial));
	
	/*没得体*/

	if(send(sock, buf, msg_len, 0) != (int)msg_len)
		return BUILD_ERROR(_OSerrno(), E_BLOCK_SEND);
	
	return 0;
}

int
recv_unbind_rsp(int sock)
{
	char						buff[1024];
	unsigned int				rcv_size = sizeof(struct msg_head);
	struct msg_head				*head = (struct msg_head *)buff;
	
	if(recv(sock, buff, rcv_size, 0) != (int)rcv_size)
		return BUILD_ERROR(_OSerrno(), E_BLOCK_RECV);
	
	if(ntohl(head->msg_len) != rcv_size){
		assert(0);
		return BUILD_ERROR(0, E_HEAD_SIZE);
	}
	
	if(ntohl(head->msg_id) != SGIP_UNBIND_RESP){
		assert(0);
		return BUILD_ERROR(0, E_RSP);
	}
	
	return 0;
}
int
do_unbind(int sock, struct serial &ser)
{
	int						err;

	if((err = send_unbind(sock, ser)) != 0)
		return err;
	
	if((err = recv_unbind_rsp(sock)) != 0)
		return err;

	return 0;
}




