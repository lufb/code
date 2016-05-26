#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_
/*
	协议处理部份
*/


#include "interface.h"

#define		DELIVER_MSG				"hello this is SMG"		/*deliver消息的消息体*/

int rcv_msg2rsp(int sock);

int rcv_body(int sock, const struct msg_head *head,  char *rsp_err);
int rcv_head(int sock, struct msg_head* head);


int rcv_bind(int sock, const struct msg_head *head,  char *rsp_err);
int rcv_unbind(int sock, const struct msg_head *head,  char *rsp_err);
int rcv_submit(int sock, const struct msg_head *head,  char *rsp_err);


int	check_resver(const char * reserv, size_t len);
int check_bind(const struct msg_bind *msg_bind,  char *rsp_err);
int	check_submit(const char *msg_submit,  char *rsp_err, size_t size);
int check_text(const char *p,  size_t size, char min, char max);


int send_rsp(int sock,  char res_err, struct msg_head *head);

//int add_report(const struct msg_head *head, struct msg_submit* sub);

void fill_report(const struct msg_head *head, const struct msg_submit* sub, struct msg_report *report);

void pr_usr_pass(struct msg_bind *bind);
void pr_usr_name(struct msg_bind *bind);

void clean_link(int sock, int err_code);


//////////////////////////////////////////////////////////////////////////

/*完成bind及bindrsp*/
int	do_bind(int sock, struct serial &s);
int do_unbind(int sock, struct serial &ser);
int do_deliver(int sock, struct serial &ser);


void init_serial(struct serial &s);


#endif