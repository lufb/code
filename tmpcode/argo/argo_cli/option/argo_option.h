#ifndef _PRO_OPTION_H_
#define _PRO_OPTION_H_

#include "argo_pro.h"
#include "list.h"



//	服务器下发下来的配置信息节点
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
	char						op_main[FILE_NAME_SIZE];	//客户端的主目录
	char						op_tmpdir[FILE_NAME_SIZE];//本地隐藏目录
	char						IP[256];				//服务器IP
	unsigned short				port;					//服务器端口
	char						ini_name[FILE_NAME_SIZE];
};

#endif
