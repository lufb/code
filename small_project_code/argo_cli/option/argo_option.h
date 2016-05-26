#ifndef _PRO_OPTION_H_
#define _PRO_OPTION_H_

#include "argo_pro.h"
#include "list.h"



//	�������·�������������Ϣ�ڵ�
typedef struct _sc_option{
	argo_hello			sc_op;
	struct list_head	list_node;
}sc_option;


class MOptin{
public:		
								MOptin();
								~MOptin();
	int							LoadOption();
	char*						GetMain();
	char*						GetTmpDir();
	char*						GetIP();
	unsigned short				GetPort();
private:
	char						op_main[FILE_NAME_SIZE];	//�ͻ��˵���Ŀ¼
	char						op_tmpdir[FILE_NAME_SIZE];//��������Ŀ¼
	char						IP[256];				//������IP
	unsigned short				port;					//�������˿�
	char						ini_name[FILE_NAME_SIZE];
};

#endif
