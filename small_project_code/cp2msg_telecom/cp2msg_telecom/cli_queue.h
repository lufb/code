#ifndef _CLI_QUEUE_H_
#define _CLI_QUEUE_H_

#include "list.h"
#include "string.h"
#include "local_define.h"
#include <windows.h>

#define MAX_IP_LEN		64			/* 存储ip地址的长度	*/


typedef struct  {
	int					sock;			/* 对应的套接字	*/
	char				ip[MAX_IP_LEN];	/* 对应的ip		*/
	unsigned short		port;			/* 端口			*/
} tagCliInfo;

typedef struct {
	tagCliInfo			cli_info;
	struct list_head	s_list_node;

	void _init(){
		memset(&cli_info, 0, sizeof(cli_info));
	}
} tagCliListNode;

class MCliQue{
public:
							MCliQue();
							~MCliQue();

	bool					Insert(int sock, char *ip, unsigned short port);
 	bool					Del(int sock);
	bool					GetRandSock(int &sock);
private:
	void					_Init();
	void					_Del2Insert(struct list_head *lhs, struct list_head *rhs);
private:
	struct list_head		s_UsedHead;
	struct list_head		s_FreeHead;
	tagCliListNode			s_Data[CLI_LIST_LEN];
	CRITICAL_SECTION		m_Section;
};

#endif
