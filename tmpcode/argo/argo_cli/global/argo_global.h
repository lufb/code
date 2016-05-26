#ifndef _ARGO_GLOBAL_H_
#define _ARGO_GLOBAL_H_
#include "argo_log.h"
#include "list.h"
#include "argo_option.h"
#include "argo_cli_mgr.h"
#include "argo_pro.h"

int							try_realloc_pro_buf();

class MLocalFileMgr{
public:
								MLocalFileMgr();
								~MLocalFileMgr();
	void						Init();
	void						Destroy();
	int							Add(sc_option	*pNode, char *abs_name, char *fjy_name);
	int							Add2(argo_all_head *head, unsigned char *file_buf);
	int							UpNode(argo_all_head *head, unsigned char *file_buf, 
										unsigned int buf_size, local_file_node *node);
	int							UpInfo(int sock);
	int							SplitBDE(int sock, unsigned char	com_type);
	int							SendHeart(int sock);
	local_file_node*			Find(const char *file_name, unsigned char type);
private:
	struct list_head			head;	
	bde_head					bde_data;
	unsigned int				min_bde_size;
};




extern MWriteLog			Global_Log;			//写日志
extern struct list_head		Global_sc_op_head;	//服务器下发下来的配置信息
extern argo_base			Global_argo_base;	//服务器下发下来的配置信息
extern MArgoCliMgr			Glboal_Mgr;			//主控
extern MOptin				Global_Loc_Option;
extern MLocalFileMgr		Global_Loc_File;
extern pro_ctx				Global_Pro_Ctx;		//处理协议的缓冲区
extern MCom_Ctx				Global_Com_Ctx;		//解压上下文
extern unsigned char		Global_Main_Ver;		//客户端主版本号
extern unsigned char		Global_Child_Ver;		//客户端子版本号
extern unsigned char		Global_BDE_Main;		//BDE主版本号
extern unsigned char		Global_BDE_Child;		//BDE主版本号






#endif
