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




extern MWriteLog			Global_Log;			//д��־
extern struct list_head		Global_sc_op_head;	//�������·�������������Ϣ
extern argo_base			Global_argo_base;	//�������·�������������Ϣ
extern MArgoCliMgr			Glboal_Mgr;			//����
extern MOptin				Global_Loc_Option;
extern MLocalFileMgr		Global_Loc_File;
extern pro_ctx				Global_Pro_Ctx;		//����Э��Ļ�����
extern MCom_Ctx				Global_Com_Ctx;		//��ѹ������
extern unsigned char		Global_Main_Ver;		//�ͻ������汾��
extern unsigned char		Global_Child_Ver;		//�ͻ����Ӱ汾��
extern unsigned char		Global_BDE_Main;		//BDE���汾��
extern unsigned char		Global_BDE_Child;		//BDE���汾��






#endif
