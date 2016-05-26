#include "argo_global.h"
#include "argo_error.h"
#include "argo_ckcrc16.h"
#include "argo_sock.h"
#include "MFile.h"
#include "argo_base.h"
#include "zlib_compress.h"

MArgoCliMgr				Glboal_Mgr;			//主控
MWriteLog				Global_Log;			//写日志
struct list_head		Global_sc_op_head;	//服务器下发下来的配置信息
argo_base				Global_argo_base;	//服务器下发下来的配置信息
MOptin					Global_Loc_Option;
MLocalFileMgr			Global_Loc_File;
pro_ctx					Global_Pro_Ctx;		//处理协议的缓冲区
MCom_Ctx				Global_Com_Ctx;		//解压上下文
unsigned char			Global_Main_Ver = 0;		//客户端主版本号
unsigned char			Global_Child_Ver = 0;		//客户端子版本号
unsigned char			Global_BDE_Main	= 0;		//BDE主版本号
unsigned char			Global_BDE_Child= 0;		//BDE主版本号



int try_realloc_pro_buf()
{
	unsigned char			*p_realloc;
	
	if(Global_Pro_Ctx.pro_buf_size - Global_Pro_Ctx.pro_buf_used < CHUNK){
		p_realloc = (unsigned char *)realloc(Global_Pro_Ctx.pro_buf, Global_Pro_Ctx.pro_buf_size + CHUNK);
		if(p_realloc == NULL){
			free(Global_Pro_Ctx.pro_buf);
			Global_Pro_Ctx.pro_buf = NULL;
			Global_Pro_Ctx.pro_buf_size = 0;
			return BUILD_ERROR(E_OS, E_REALLOC);
		}
		Global_Pro_Ctx.pro_buf = p_realloc;
		Global_Pro_Ctx.pro_buf_size += CHUNK;
	}
	
	
	return 0;
}

MLocalFileMgr::MLocalFileMgr()
{
	INIT_LIST_HEAD(&head);
	memset(&bde_data, 0, sizeof(bde_data));
	bde_data.main = Global_BDE_Main;
	bde_data.child = Global_BDE_Child;
	min_bde_size = sizeof(bde_data)-2;
}

MLocalFileMgr::~MLocalFileMgr()
{

}

void MLocalFileMgr::Init()
{
	INIT_LIST_HEAD(&head);
}
void MLocalFileMgr::Destroy()
{
	struct list_head				*pList, *pSafe;	
	local_file_node					*pNode;
	
	list_for_each_safe(pList, pSafe, &head){
		pNode = list_entry(pList, local_file_node, list_node);
		assert(pNode != NULL);
		if(pNode->file_buf != NULL)
			free(pNode->file_buf);
		list_del(pList);
		free(pNode);
	}
}

int	MLocalFileMgr::Add(sc_option	*pNode, char *abs_name, char *fjy_name)
{
	MFile				mf;
	int					err;
	local_file_node		*ptmp = (local_file_node*)malloc(sizeof(local_file_node));
	if(ptmp == NULL){
		return BUILD_ERROR(E_OS, E_MALLOC);
	}

	memset(ptmp, 0, sizeof(local_file_node));
	if(fjy_name){
		strncpy(ptmp->file_name, fjy_name, sizeof(ptmp->file_name)-1);
	}else{
		strncpy(ptmp->file_name, pNode->sc_op.file_name, sizeof(ptmp->file_name)-1);
	}
	ptmp->type = pNode->sc_op.file_type;
	ptmp->file_size = MFile::GetFileSizeByStat(abs_name);
	ptmp->file_buf = (unsigned char *)malloc(ptmp->file_size);
	if(ptmp->file_buf == NULL){
		free(ptmp);
		err = BUILD_ERROR(E_OS, E_MALLOC);
		goto END;
	}
	
	mf.OpenRead(abs_name);
	err = mf.Read((char *)(ptmp->file_buf), ptmp->file_size);
	if((unsigned int)err != ptmp->file_size){
		Global_Log.writeLog(LOG_TYPE_ERROR, "读文件[%s][%d]发生错误[%d]", abs_name, ptmp->file_size, err);
		err = BUILD_ERROR(_OSerrno(), E_READ_FILE);
		goto END;
	}

	get_hash(ptmp->file_cur_hash, ptmp->file_buf, ptmp->file_size);
	list_add_tail(&(ptmp->list_node), &head);
	err = 0;

END:
	if(fjy_name && ptmp && ptmp->file_buf){
		free(ptmp->file_buf);
		ptmp->file_buf = NULL;
	}
	return err;
}


int	MLocalFileMgr::UpNode(argo_all_head *head, unsigned char *file_buf, 
						unsigned int buf_size, local_file_node *node)
{
	if(buf_size > node->file_size){
		unsigned char *tmp = (unsigned char *)realloc(node->file_buf, buf_size);
		
		if(tmp == NULL){
			free(node->file_buf);
			node->file_buf = NULL;
			return BUILD_ERROR(0, E_MALLOC);
		}
		node->file_buf = tmp;
	}
	memmove(node->file_buf, file_buf, buf_size);
	memmove(node->file_cur_hash, head->hash_new, HASH_SIZE);
	node->file_time = head->modify_time;
	node->file_size = buf_size;

	return 0;
}


int	MLocalFileMgr::Add2(argo_all_head *head, unsigned char *file_buf)
{
	local_file_node		*ptmp = (local_file_node*)malloc(sizeof(local_file_node));
	if(ptmp == NULL)
		return BUILD_ERROR(E_OS, E_MALLOC);
	
	memset(ptmp, 0, sizeof(local_file_node));
	strncpy(ptmp->file_name, head->file_name, sizeof(ptmp->file_name)-1);
	ptmp->type = head->file_type;
	ptmp->file_size = head->file_size;
	ptmp->file_time = head->modify_time;
	ptmp->file_buf = (unsigned char *)malloc(head->file_size);
	memmove(ptmp->file_cur_hash, head->hash_new, HASH_SIZE);
	if(ptmp->file_buf == NULL){
		free(ptmp);
		return BUILD_ERROR(E_OS, E_MALLOC);
	}

	memmove(ptmp->file_buf, file_buf, head->file_size);
	return 0;
}


int	MLocalFileMgr::SendHeart(int sock)
{
	char					tmp[1024];
	
	memset(tmp, 0, sizeof(tmp));
	argo_head	*a_head = (argo_head*)(tmp+sizeof(bde_data));
	bde_head	*p_bde = (bde_head*)tmp;
	a_head->pro = PRO_CS_00;
	bde_data.bde_type = BDE_BDE;
	bde_data.size = sizeof(argo_head);
	memmove(tmp, &bde_data, sizeof(bde_data));
	p_bde->crc = argo_ckcrc16_calc(tmp+2, sizeof(bde_data)+sizeof(argo_head)-2);
	
	return MSocket::sendDataNonBlock(sock, tmp, sizeof(bde_data)+sizeof(argo_head));
}

local_file_node* MLocalFileMgr::Find(const char *file_name, unsigned char type)
{
	struct list_head				*pList;
	local_file_node					*pNode;

	list_for_each(pList, &head)
	{
		pNode = list_entry(pList, local_file_node, list_node);
		if(strcmp(file_name, pNode->file_name) == 0 &&
			pNode->type == type){
			return pNode;
		}
	}

	return NULL;
}


int MLocalFileMgr::SplitBDE(int sock, unsigned char	com_type)
{
	unsigned int		i;
	char				tmp_buf[4096];
	unsigned short		once_deal_size = sizeof(tmp_buf)-sizeof(bde_head);
	bde_head			*p_bde = (bde_head *)tmp_buf;
	char				*p_pro = tmp_buf+sizeof(bde_head);
	unsigned short		count = (unsigned short)Global_Pro_Ctx.pro_buf_used / once_deal_size;
	unsigned short		leave = (unsigned short)Global_Pro_Ctx.pro_buf_used % once_deal_size;
	int					err;
	unsigned int		total = 0;
	
	memset(p_bde, 0, sizeof(bde_head));
	p_bde->main = Global_BDE_Main;
	p_bde->child = Global_BDE_Child;
	if(count == 0 || 
		(count == 1 && leave == 0)){	//一个包
		memmove(p_pro, Global_Pro_Ctx.pro_buf, Global_Pro_Ctx.pro_buf_used);
		p_bde->bde_type = BDE_BDE;
		p_bde->com_type = com_type;
		p_bde->size = (unsigned short)(Global_Pro_Ctx.pro_buf_used);
		p_bde->crc = argo_ckcrc16_calc(tmp_buf+2, Global_Pro_Ctx.pro_buf_used+sizeof(bde_head)-2);
		
		total += Global_Pro_Ctx.pro_buf_used+sizeof(bde_head);
		return MSocket::sendDataNonBlock(sock, tmp_buf, Global_Pro_Ctx.pro_buf_used+sizeof(bde_head));
	}else{								//多包
		for(i = 0; i < count; ++i){
			memmove(p_pro, Global_Pro_Ctx.pro_buf+i*once_deal_size, once_deal_size);
			if(i == 0)
				p_bde->bde_type = BDE_BD;
			else 
				p_bde->bde_type = BDE_D;
			p_bde->com_type = com_type;
			p_bde->size = once_deal_size;
			p_bde->crc = argo_ckcrc16_calc(tmp_buf+2, once_deal_size+sizeof(bde_head)-2);
			
			err = MSocket::sendDataNonBlock(sock, tmp_buf, once_deal_size+sizeof(bde_head));
			if(err)
				return err;
		}
		//尾巴数据发送出去
		if(leave == 0){
			p_bde->bde_type = BDE_E;
		}else{
			p_bde->bde_type = BDE_BD;
			memmove(p_pro, Global_Pro_Ctx.pro_buf+count*once_deal_size, leave);
		}
		p_bde->com_type = com_type;
		p_bde->size = leave;
		p_bde->crc = argo_ckcrc16_calc(tmp_buf+2, sizeof(bde_head)+leave-2);
		
		total +=sizeof(bde_head)+leave;
		printf("total[%d]\n", total);
		return MSocket::sendDataNonBlock(sock, tmp_buf, sizeof(bde_head)+leave);
	}
}

int	MLocalFileMgr::UpInfo(int sock)
{
	int						err;
	struct list_head		*pTmp;
	local_file_node			*pNode;
	argo_head				a_head;
	MCompress				com;
	char					dos_file[FILE_NAME_SIZE];

	Global_Pro_Ctx.pro_buf_used = 0;
	a_head.pro = PRO_CS_01;
	a_head.main = Global_Main_Ver;
	a_head.child = Global_Child_Ver;
	
	err = com.Compress((unsigned char *)&a_head, sizeof(a_head));
	if(err)
		return err;
	
	list_for_each(pTmp, &head){
		pNode = list_entry(pTmp, local_file_node, list_node);
		do{
			memmove(dos_file, pNode->file_name, FILE_NAME_SIZE);
			unix2dos_path(dos_file, my_min(strlen(dos_file), FILE_NAME_SIZE));
			err = com.Compress((unsigned char *)(dos_file), FILE_NAME_SIZE);
			if(err)
				return err;
			err = com.Compress(pNode->file_cur_hash, HASH_SIZE);
			if(err)
				return err;
		}while(0);
#ifdef _DEBUG
		Global_Log.writeLog(LOG_TYPE_INFO, "上传文件[%s]hash值", pNode->file_name);
#endif
	}

	err = com.Final();
	if(err)
		return err;
	return SplitBDE(sock, 0x01);
}