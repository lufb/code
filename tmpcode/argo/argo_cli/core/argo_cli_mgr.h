#ifndef _ARGO_CLI_MGR_H_
#define _ARGO_CLI_MGR_H_

#include "zlib_compress.h"
#include "argo_pro.h"
#include "argo_option.h"
#include "list.h"
#include <time.h>
#include "MThread.h"



#define		MAX_BDE_SIZE		8192			/*	单个BDE包的最大长度		*/

typedef struct _pro_ctx{
	unsigned char			*pro_buf;
	unsigned int			pro_buf_size;	
	unsigned int			pro_buf_used;
}pro_ctx;

typedef struct _local_file_node{
	char					file_name[FILE_NAME_SIZE];
	unsigned int			type;					
	unsigned int			file_time;
	unsigned int			file_size;
	unsigned char			file_cur_hash[HASH_SIZE];
	unsigned char			*file_buf;
	struct list_head		list_node;
}local_file_node;

class MDealPro{
public:
							MDealPro();
							~MDealPro();

	void					Init();
	int						DealProFunc(unsigned char *buf, unsigned int buf_size);
	int						CheckSoftVer(argo_head *p_head);
	int						DealSrvHeart(argo_head *p_head);
	void					FillArgoBae(argo_base *base);
	int						FilllArgoOption(argo_hello *option, unsigned int op_count);
	int						FilllOption(unsigned char *buf, unsigned int buf_size);
	int						DealCpData(unsigned char *buf, unsigned int buf_size);
	int						DealUpData(unsigned char *buf, unsigned int buf_size);
	int						DealCpData_MKTD(argo_all_head *head, unsigned char *buf, unsigned int buf_size);
	int						DealCpData_FJY(argo_all_head *head, unsigned char *buf, unsigned int buf_size);
	int						TryCreateDirIfNeed( const char* filename);
	int						_Write_File(char *file_name, unsigned int modify_time, unsigned char *file_buf, unsigned int buf_size);
	int						Write_File(char *file_name, unsigned int modify_time, unsigned char *file_buf, unsigned int buf_size);
	int						CpCheck(argo_all_head	*head, unsigned char *buf, unsigned int buf_size);
	int						UpMemory(local_file_node *pNode, unsigned char *buf);
	int						ChkMemIsZero(unsigned char *buf, unsigned int size);
};

class MDealBDE{
public:
							MDealBDE();
							~MDealBDE();
							
	void					Init();
	int						DealOnceBDE(int sock);
	int						RcvOnceBDE(int sock);
	void					ChkBdeVer(bde_head	*head);
	int						ChkCrc();
	int						ChkBDE();
	int						CpToProBuf();
	int						HadRcvedOnceBDE();

private:
	int						ZlibDecomCp(unsigned char *in_buf, unsigned int in_size);
	int						UnComCp(unsigned char *in_buf, unsigned int in_size);

private:
	BDE_STATUS				status;
	bde_head				*p_head;
	bool					had_chk_bde;
	//	下面是bde的缓冲区
	unsigned char			once_bde_buf[MAX_BDE_SIZE];
	unsigned int			once_bde_size;
	

	MDealPro				deal_pro;	
};


class MArgoCliMgr{
public:
							MArgoCliMgr();
							~MArgoCliMgr();
	void					Init();
	void					ReInit();
	//void					Stop();
	//int						start_mainthread();
	int						upload_local();
	int						build_local();	
	int						up_local();

	int						build_local_mktd(sc_option				*pNode);	
	int						build_local_fjy(sc_option				*pNode);
	int						is_corect_time(unsigned int t, unsigned int old_del);
	int						trySendHeart();
	int						main_func(char *ip, unsigned short port);
	void					close_sock();
	int						ArgoFun();


private:
	void					set_select_timeout(int time_out);
	time_t					get_cur_time();
private:
	int						sock;
	MDealBDE				deal_bde;	
	struct timeval			tv;						/** the timeout of select*/
	time_t					rcv_t;					//	收到包的最新时间
	time_t					snd_t;					//	发包的最新时间
};

class MCom_Ctx{
public:
							MCom_Ctx();
							~MCom_Ctx();

	int						Com_Data(unsigned char *buf, unsigned int size);
	int						Com_Init();
	void					Com_Destroy();

	void					Set0();
	void					Set1();
	bool					Get0();
	bool					Get1();
	void					ResetSta();

private:
	bool					had_init;
	MDeCompress				de_compress;
	bool					sta[2];
};

#endif