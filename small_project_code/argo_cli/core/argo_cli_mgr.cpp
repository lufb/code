#include <assert.h>
#include "argo_cli_mgr.h"
#include "argo_sock.h"
#include "argo_global.h"
#include "argo_error.h"
#include "argo_pro.h"
#include "argo_ckcrc16.h"
#include "argo_base.h"
#include "MFile.h"
#include "mask_decom.h"
#include "zlib_compress.h"

#ifdef LINUXCODE
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#endif

#define SELECT_TIME_OUT				1			//	select �ĳ�ʱʱ��

/** ����һ��timeval��ʱ��*/
#define INIT_TIMEVAL(_timeval_, _timeout_)		\
	do{											\
	_timeval_.tv_sec = _timeout_;				\
	_timeval_.tv_usec = 0;						\
	}while(0)

MDealPro::MDealPro()
{
}

MDealPro::~MDealPro()
{

}

void MDealPro::Init()
{

}

void MDealPro::FillArgoBae(argo_base *base)
{
	Global_argo_base.heart_times = base->heart_times;
	Global_argo_base.recon_times = base->recon_times;
	if(Global_argo_base.recon_times == 0)
		Global_argo_base.recon_times = 1;
}

int MDealPro::FilllArgoOption(argo_hello *option, unsigned int op_count)
{
	unsigned int				i;
	sc_option					*op;

	for(i = 0; i < op_count; ++i){
		argo_hello		*cur_op = option + i;
		op = (sc_option*)malloc(sizeof(sc_option));
		if(op == NULL)
			return BUILD_ERROR(E_OS, E_MALLOC);
		
		memset(op, 0, sizeof(*op));
		strncpy(op->sc_op.file_name, cur_op->file_name, sizeof(op->sc_op.file_name)-1);
		dos2unix_path(op->sc_op.file_name, strlen(op->sc_op.file_name));
		op->sc_op.file_type = cur_op->file_type;
		op->sc_op.old_del = cur_op->old_del;
		Global_Log.writeLog(LOG_TYPE_INFO, "�յ��������ļ�������Ϣ[file_name:%s][file_type:%d][old_del:%d]",
			op->sc_op.file_name, cur_op->file_type, cur_op->old_del);
		list_add_tail(&(op->list_node), &Global_sc_op_head);
	}

	return 0;
}

//ɾ���ļ�
bool MDealPro::TryDeleteFile( char *strFile, int iTryTimes)
{
	int iTry = 0;
	
	while( iTry < iTryTimes  )
	{
		if( MFile::DeleteFile( strFile) < 0 )
		{
			Sleep(500);
			iTry++;
		}
		
		return (true);
	}
	
	return (false);
}

//	 ����ĳ�ļ���Ŀ¼�ṹ
int MDealPro::TryCreateDirIfNeed( const char* filename)
{
	char						fileDir[MAX_PATH]={0};
	char*						pDst;
	int							errorcode;

	pDst = strrchr(filename, '/');
	strncpy(fileDir, filename, pDst - filename);

	if( MFile::IsExist( fileDir) == true )
		return 0;
	
	if( ( errorcode = MFile::CreateDirectoryTree( MString( fileDir ) ))  <0 )
	{
		Global_Log.writeLog( LOG_TYPE_ERROR, 0, "����Ŀ¼[%s]ʧ��\n",fileDir );
		return BUILD_ERROR(_OSerrno(), E_CRT_DIR);
	}
	
	return 0;
}

int	MDealPro::_Write_File(char *file_name, unsigned int modify_time, unsigned char *file_buf, unsigned int buf_size)
{
	int						err;
	MFile					mf;
	
	err = TryCreateDirIfNeed(file_name);
	if(err)
		return err;
	
	if(!MFile::IsExist(file_name))
		mf.Create(file_name);
	err = mf.OpenWrite(file_name);
	if(err != 1){
		Global_Log.writeLog(LOG_TYPE_ERROR, "���ļ�[%s]����[%d]", file_name, err);
		return BUILD_ERROR(_OSerrno(), E_OPEN_WRITE);
	}
	
	err = mf.Write((const char *)file_buf, buf_size);
	if(err != buf_size){
		mf.Close();
		MFile::DeleteFile(file_name);
		Global_Log.writeLog(LOG_TYPE_ERROR, "д�ļ�[%s]����[%d]", file_name, err);
		return BUILD_ERROR(_OSerrno(), E_WRITE_WRITE);
	}
	mf.SetFileLength(buf_size);
	//	�����ļ�ʱ��
	if( ( err = mf.SetUpdateDateTime( MDateTime(modify_time) )) <0 )
	{
		mf.Close();
		Global_Log.writeLog(LOG_TYPE_ERROR, "�����ļ�[%s]ʱ�����[%d]", file_name, err);
		return BUILD_ERROR(_OSerrno(), E_FILE_TIME);
	}
	
	return 0;
}

int	MDealPro::Write_File(char *file_name, unsigned int modify_time, unsigned char *file_buf, unsigned int buf_size)
{
	int						err;
	char					abs_file_name[FILE_NAME_SIZE];

	sprintf(abs_file_name, "%s/%s", Global_Loc_Option.GetMain(), file_name);
	err = TryCreateDirIfNeed(abs_file_name);
	if(err)
		return err;

	if(!MFile::IsExist(abs_file_name)){
		char					abs_file_name_tmp[FILE_NAME_SIZE];

		sprintf(abs_file_name_tmp, "%s~~TMP~~", abs_file_name);
		err = _Write_File(abs_file_name_tmp, modify_time, file_buf, buf_size);
		if(err){
			MFile::DeleteFile(abs_file_name_tmp);
			return err;
		}
		//����
		err = MFile::RenameFileName(abs_file_name_tmp, abs_file_name);
		if(err != 1){
			MFile::DeleteFile(abs_file_name_tmp);
			Global_Log.writeLog(LOG_TYPE_ERROR, "�������ļ�[%s]->[%s]ʧ��",
				abs_file_name_tmp, abs_file_name);
			return BUILD_ERROR(_OSerrno(), E_RENAME_FILE);
		}
		
		return 0;
	}else{
		return _Write_File(abs_file_name, modify_time, file_buf, buf_size);
	}
}


int	MDealPro::DealCpData_MKTD(argo_all_head *head, unsigned char *file_buf, unsigned int buf_size)
{
	int							err;
	local_file_node				*pNode = Global_Loc_File.Find(head->file_name, FILE_TYPE_MKTD);

	err = CpCheck(head, file_buf, buf_size);
	if(err)
		return err;

	//	д��������
	err = Write_File(head->file_name, head->modify_time, file_buf, buf_size);
	if(err)
		return err;

	if(pNode == NULL){	//�½�����
		return  Global_Loc_File.Add2(head, file_buf);
	}else{	//��������
		return Global_Loc_File.UpNode(head, file_buf, buf_size, pNode );
	}
}

int	MDealPro::DealCpData_FJY(argo_all_head *head, unsigned char *file_buf, unsigned int buf_size)
{
	int							err;

	err = CpCheck(head, file_buf, buf_size);
	if(err)
		return err;
	return Write_File(head->file_name, head->modify_time, file_buf, buf_size);
}

int MDealPro::CpCheck(argo_all_head	*head, unsigned char *buf, unsigned int buf_size)
{
	unsigned char			tmp_hash[HASH_SIZE];

	if(head->file_size != buf_size){
		Global_Log.writeLog(LOG_TYPE_ERROR, "�ļ�[%s]ȫ����ʱ��Э��ͷ�������ݴ�С[%u]���ļ���С[%u]��һ��",
			head->file_name, head->file_size, buf_size);
		return BUILD_ERROR(0, E_FILE_SIZE);
	}
	
	get_hash(tmp_hash, buf, buf_size);
	if(memcmp(head->hash_new, tmp_hash, HASH_SIZE)){
		Global_Log.writeLog(LOG_TYPE_ERROR, "�ļ�[%s]ȫ����ʱ��Э��ͷ����hash[%-20.20s]���ļ�hash[%-20.20s]��һ��",
			head->file_name, head->hash_new, tmp_hash);
		return BUILD_ERROR(0, E_FILE_SIZE);
	}

	return 0;
}

int	MDealPro::DealCpData(unsigned char *buf, unsigned int buf_size)
{
	int				err;
	argo_all_head	*cp = (argo_all_head*)(buf+sizeof(argo_head));
	if(buf_size < sizeof(argo_head) + sizeof(argo_all_head))
		return BUILD_ERROR(0, E_MINHEAD_SIZE);

	dos2unix_path(cp->file_name, my_min(strlen(cp->file_name), FILE_NAME_SIZE));
	switch(cp->file_type)
	{
	case FILE_TYPE_MKTD:
		err = DealCpData_MKTD(cp, 
			buf+sizeof(argo_head) + sizeof(argo_all_head), 
			buf_size - sizeof(argo_head) - sizeof(argo_all_head));
		if(err != 0)
			return err;
		break;
	case FILE_FJY:
		err = DealCpData_FJY(cp, 
			buf+sizeof(argo_head) + sizeof(argo_all_head), 
			buf_size - sizeof(argo_head) - sizeof(argo_all_head));
		if(err != 0)
			return err;
		break;
	default:
		Global_Log.writeLog(LOG_TYPE_ERROR, "�ļ�[%s]��ȫ�������Ͳ���ȷ[%d]", cp->file_name, cp->file_type);
		return BUILD_ERROR(0, E_UN_FILETYPE);
	}
#ifdef _DEBUG
	Global_Log.writeLog(LOG_TYPE_DEBUG, "�յ��ļ�[%s]��ȫ����[%u]", cp->file_name, buf_size - sizeof(argo_head) - sizeof(argo_all_head));
#endif
	return 0;
}

int MDealPro::ChkMemIsZero(unsigned char *buf, unsigned int size)
{
	unsigned int				i;

	for(i = 0; i < size; ++i){
		if(buf[i])
			return BUILD_ERROR(0, E_MASK_NO_ZERO);
	}

	return 0;
}

int	MDealPro::UpMemory(local_file_node *pNode, 
					   unsigned char *buf)
{
	argo_mask		*p_mask = (argo_mask*)buf;
	unsigned char	*p_mask_data = buf+sizeof(argo_mask);
	argo_dic_dc		*p_dc = NULL;
	unsigned int	mask_size = 0;
	unsigned int	i;
	int				err;
	unsigned int	change_size;
	
	for(i = 0; i < LEVELS; ++i){
		mask_size += p_mask->size[i];
	}
	p_dc = (argo_dic_dc *)(buf+sizeof(argo_mask)+mask_size);
	p_dc->swap();		//	note: ��ת
	change_size = *((unsigned int *)(buf+sizeof(argo_mask)+mask_size+sizeof(argo_dic_dc)));
	err = mask_decom(p_dc, pNode, p_mask, p_mask_data, buf+sizeof(argo_mask)+mask_size+sizeof(unsigned int)+sizeof(argo_dic_dc), change_size);
	if(err)
		return err;
	
	return ChkMemIsZero(p_mask_data, mask_size);
}


int	MDealPro::DealUpData(unsigned char *buf, unsigned int buf_size)	
{
	int							err;
	argo_up_head				*head;
	local_file_node				*pNode;
	unsigned char				tmp_hash[HASH_SIZE];

	if(buf_size < sizeof(argo_head) + sizeof(argo_up_head))
		return BUILD_ERROR(0, E_MINHEAD_SIZE);

	head = (argo_up_head*)(buf+sizeof(argo_head));
	dos2unix_path(head->file_name, my_min(FILE_NAME_SIZE, strlen(head->file_name)));
	pNode = Global_Loc_File.Find(head->file_name, FILE_TYPE_MKTD);

	if(head->file_type != FILE_TYPE_MKTD){
		Global_Log.writeLog(LOG_TYPE_ERROR, "�ݲ�֧���ļ���[%s]����[%d]����������", head->file_name, head->file_type);
		return BUILD_ERROR(0, E_UN_FILETYPE);
	}

	if(pNode == NULL){
		Global_Log.writeLog(LOG_TYPE_ERROR, "���ڴ���û�ҵ��ļ���[%s]���͵������Ϣ", head->file_name);
		return BUILD_ERROR(0, E_ABORT1);
	}

	if(pNode->file_size != head->file_size){
		Global_Log.writeLog(LOG_TYPE_ERROR, "�ļ�[%s]�ڴ��еĴ�С[%u]���յ��Ĵ�С[%u]���ȣ����ܽ�����������", 
			head->file_name, pNode->file_size, head->file_size);
		return BUILD_ERROR(0, E_ABORT1);
	}

	if(memcmp(pNode->file_cur_hash, head->hash_old, HASH_SIZE)){
		Global_Log.writeLog(LOG_TYPE_ERROR, "�ļ�[%s]�ڴ��е�hash[%-20.20s]Э���е���hash[%-20.20s]���ȣ����ܽ�����������", 
			head->file_name, pNode->file_cur_hash, head->hash_old);
		return BUILD_ERROR(0, E_ABORT1);
	}

#ifdef _DEBUG
	Global_Log.writeLog(LOG_TYPE_DEBUG, "�յ��ļ�[%s]����������С[%d]", pNode->file_name, buf_size-sizeof(argo_head)-sizeof(argo_up_head)-sizeof(argo_dic_dc));
#endif
	err = UpMemory(pNode, buf+sizeof(argo_head)+sizeof(argo_up_head));
	if(err)
		return err;

	get_hash(tmp_hash, pNode->file_buf, pNode->file_size);
	if(memcmp(tmp_hash, head->hash_new, HASH_SIZE)){
		Global_Log.writeLog(LOG_TYPE_ERROR, "�ļ�[%s]�ڴ��и��º�hash[%-20.20s]Э���еķ���������hash[%-20.20s]���ȣ���������ʧ��", 
			head->file_name, tmp_hash, pNode->file_cur_hash);
		return BUILD_ERROR(0, E_UP_HASH);
	}

	err =  Write_File(head->file_name, head->modify_time, pNode->file_buf, pNode->file_size);
	if(err)
		return err;

	memmove(pNode->file_cur_hash, head->hash_new, HASH_SIZE);
	pNode->file_time = head->modify_time;

	return 0;
}

int MDealPro::FilllOption(unsigned char *buf, unsigned int buf_size)
{
	int				err;
	int				count;

	if(buf_size < sizeof(argo_head) + sizeof(argo_base))
		return BUILD_ERROR(0, E_MINHEAD_SIZE);

	argo_base *base_info= (argo_base*)(buf+sizeof(argo_head));
#ifdef _DEBUG
	Global_Log.writeLog(LOG_TYPE_INFO, "�յ��ķ���������������Ϣ[heart_times:%u][recon_times:%u]", 
		base_info->heart_times, base_info->recon_times);
#endif
	FillArgoBae(base_info);

	assert(buf_size >= sizeof(argo_head) + sizeof(argo_base));
	assert((buf_size - sizeof(argo_head) - sizeof(argo_base))%sizeof(argo_hello) == 0);
	
	argo_hello *hello = (argo_hello*)(buf+sizeof(argo_head) + sizeof(argo_base));
	count = (buf_size - sizeof(argo_head) + sizeof(argo_base))/sizeof(argo_hello);
	if(count == 0)
		return BUILD_ERROR(0, E_OPTION_00);

	if((err = FilllArgoOption(hello, count)) != 0)
		return err;

	return 0;
}

int	MDealPro::CheckSoftVer(argo_head *p_head)
{
	if(p_head->main != Global_Main_Ver){
		Global_Log.writeLog(LOG_TYPE_ERROR, "���������汾��[%u]��ͻ������汾��[%u]���ȣ������ݣ������˳�", p_head->main, Global_Main_Ver);
		exit(0);
	}

	return 0;
}

int	MDealPro::DealProFunc(unsigned char *buf, unsigned int buf_size)
{
	int							err;

	if(buf_size < sizeof(argo_head))
		return BUILD_ERROR(0, E_MINHEAD_SIZE);
	
	argo_head				*p_head = (argo_head*)buf;
	switch(p_head->pro)
	{
	case PRO_CS_00:		//	�ͻ��˵���������������
	case PRO_CS_01:		//	�ͻ��˵����������ļ���Ϣ��
		Global_Log.writeLog(LOG_TYPE_ERROR, "�ͻ����յ�����������͵�Э���[%u]", p_head->pro);
		return BUILD_ERROR(0, E_PRO_DIR); 
	case PRO_SC_O0:		//	���������ͻ��˵�������
		//	������ֱ������
#ifdef _DEBUG
		Global_Log.writeLog(LOG_TYPE_DEBUG, "�յ��������·���������");
#endif
		break;
	case PRO_SC_OP:		//	���������ͻ��˵����ð�
		CheckSoftVer(p_head);//У�����ļ�����
		err = FilllOption(buf, buf_size);
		if(err)
			return 0;
		Global_Log.writeLog(LOG_TYPE_DEBUG, "�շ���������������Ϣ���");
		err = Glboal_Mgr.upload_local();
		if(err)
			return err;
		Global_Log.writeLog(LOG_TYPE_INFO, "�ϴ�ȫ�������ļ�hash�ɹ�");
		break;
	case PRO_SC_AD:		//	���������ͻ��˵�������
		err = DealUpData(buf, buf_size);
		if(err)
			return err;
		break;
	case PRO_SC_AL:		//	���������ͻ��˵�ȫ����
		err = DealCpData(buf, buf_size);
		if(err)
			return err;
		break;
	default:
		Global_Log.writeLog(LOG_TYPE_ERROR, "�յ�δ֪Э���[%u]", p_head->pro);
		return BUILD_ERROR(0, E_UN_PROTYPE);
	}

	//	�ɹ�����0
	return 0;
}



//////////////////////////////////////////////////////////////////////////


MDealBDE::MDealBDE()
{
	had_chk_bde = false;
	Init();
}

MDealBDE::~MDealBDE()
{

}

void MDealBDE::Init()
{
	status = BDE_00;
	p_head = NULL;
	once_bde_size = 0;

	Global_Pro_Ctx.pro_buf_used = 0;
	deal_pro.Init();
}

int MDealBDE::ChkCrc()
{
	bde_head				*head = (bde_head*)once_bde_buf;

	unsigned short			crc = argo_ckcrc16_calc(once_bde_buf+2, once_bde_size-2);
	if(crc != head->crc){
		Global_Log.writeLog(LOG_TYPE_ERROR, "CRC �յ���[%u]!=�������[%u]", head->crc, crc);
		return BUILD_ERROR(0, E_CRC);
	}
	return 0;
}

int MDealBDE::ChkBDE()
{
	BDE_STATUS			rcv_sta = (BDE_STATUS)(p_head->bde_type);//�յ���������
	
	switch(rcv_sta)	
	{
	case BDE_00:
		assert(0);
		return BUILD_ERROR(0, E_UNBDETYPE2);
	case BDE_E:
		if(status != BDE_D && status != BDE_BD){
			Global_Log.writeLog(LOG_TYPE_ERROR, "BDE״̬��[%u]->[%u]������",status, rcv_sta);
			return BUILD_ERROR(0, E_UNBDETYPE1);
		}
		Global_Com_Ctx.Set1();
		break;
	case BDE_D:
		if(status != BDE_B && status != BDE_BD && status != BDE_D){
			Global_Log.writeLog(LOG_TYPE_ERROR, "BDE״̬��[%u]->[%u]������",status, rcv_sta);
			return BUILD_ERROR(0, E_UNBDETYPE1);
		}
		break;
	case BDE_DE:
		if(status != BDE_D && status != BDE_BD){
			Global_Log.writeLog(LOG_TYPE_ERROR, "BDE״̬��[%u]->[%u]������",status, rcv_sta);
			return BUILD_ERROR(0, E_UNBDETYPE1);
		}
		Global_Com_Ctx.Set1();
		break;
	case BDE_B:
		if(status != BDE_00){
			Global_Log.writeLog(LOG_TYPE_ERROR, "BDE״̬��[%u]->[%u]������",status, rcv_sta);
			return BUILD_ERROR(0, E_UNBDETYPE1);
		}
		Global_Com_Ctx.Set0();
		break;
	case BDE_BE:
		return BUILD_ERROR(0, E_UNBDETYPE1);		
	case BDE_BD:
		if(status != BDE_00){
			Global_Log.writeLog(LOG_TYPE_ERROR, "BDE״̬��[%u]->[%u]������",status, rcv_sta);
			return BUILD_ERROR(0, E_UNBDETYPE1);
		}
		Global_Com_Ctx.Set0();
		break;
	case BDE_BDE:
		if(status != BDE_00 && status != BDE_BDE){
			Global_Log.writeLog(LOG_TYPE_ERROR, "BDE״̬��[%u]->[%u]������",status, rcv_sta);
			return BUILD_ERROR(0, E_UNBDETYPE1);
		}
		Global_Com_Ctx.Set0();
		Global_Com_Ctx.Set1();
		break;
	default:
		assert(0);
		return BUILD_ERROR(0, E_UNBDETYPE);
	}
	
	//	����״̬��״̬��Ǩ
	status = rcv_sta;

	return 0;
}

void MDealBDE::ChkBdeVer(bde_head	*head)
{
	if(head->main != Global_BDE_Main){
		Global_Log.writeLog(LOG_TYPE_ERROR, "������BDE���汾��[%u]�����ڿͻ��˵�BDE���汾��[%d],�����˳�", 
			head->main != Global_BDE_Main);
		exit(0);
	}
	had_chk_bde = true;
}

int MDealBDE::RcvOnceBDE(int sock)
{
	int					err;
	
	//	SETP1:	��BEDͷ
	err = MSocket::rcvDataNonBlock(sock, (char *)once_bde_buf, sizeof(bde_head));
	if(err != 0)
		return err;

	if(!had_chk_bde){
		ChkBdeVer((bde_head*)once_bde_buf);
	}

	p_head = (bde_head *)once_bde_buf;

	if(sizeof(bde_head) + p_head->size > MAX_BDE_SIZE){
		Global_Log.writeLog(LOG_TYPE_ERROR, "BDE��СԽ��[���ֵ:%u][BDEͷ:%u][BDE��:%u]", 
			MAX_BDE_SIZE, sizeof(bde_head), p_head->size);
		return BUILD_ERROR(0, E_MAX_BDE_SIZE);
	}
	
	//	SETP2:	��BED��
	err = MSocket::rcvDataNonBlock(sock, (char *)(once_bde_buf+sizeof(bde_head)), p_head->size);
	if(err != 0)
		return err;
	once_bde_size = sizeof(bde_head) + p_head->size;

	return 0;
}

int	MDealBDE::HadRcvedOnceBDE()
{
	if(	status == BDE_E ||
		status == BDE_DE||
		status == BDE_BDE){
		return 1;
	}
	return 0;
}

int MDealBDE::ZlibDecomCp(unsigned char *in_buf, unsigned int in_size)
{
	int					err;

	err =  Global_Com_Ctx.Com_Data(in_buf, in_size);
	if(err){
		Global_Log.writeLog(LOG_TYPE_ERROR, "��ѹ���ݳ���[%d]", err);
		return BUILD_ERROR(0, E_UNCOMPRESS);
	}

	return 0;
}

int MDealBDE::UnComCp(unsigned char *in_buf, unsigned int in_size)
{
	int					err;
	//	��֤������CHUNK���пռ�
	err = try_realloc_pro_buf();
	if(err)
		return err;
	memmove(Global_Pro_Ctx.pro_buf + Global_Pro_Ctx.pro_buf_used, in_buf, in_size);
	Global_Pro_Ctx.pro_buf_used += in_size;

	return 0;
}


//	������BDE�����ݿ���Э�黺����
int	MDealBDE::CpToProBuf()
{
	assert(Global_Pro_Ctx.pro_buf_size % CHUNK == 0);
	assert(Global_Pro_Ctx.pro_buf_size >= Global_Pro_Ctx.pro_buf_used);
	
	assert(p_head != NULL);
	switch(p_head->com_type)
	{
	case 0x00:			//	��ѹ��
		return UnComCp(once_bde_buf+sizeof(bde_head), once_bde_size-sizeof(bde_head));
	case 0x01:			//	zlibѹ��
		return ZlibDecomCp(once_bde_buf+sizeof(bde_head), once_bde_size-sizeof(bde_head));
	default:
		Global_Log.writeLog(LOG_TYPE_ERROR, "�յ�δ֪���͵�ѹ������[%d]", p_head->com_type);
		return  BUILD_ERROR(0, E_UN_COMTYPE);
	}
}

int	MDealBDE::DealOnceBDE(int sock)
{
	int					err;

	//	STEP1:	������һ��BDE��
	err = RcvOnceBDE(sock);
	if(err != 0)
		goto END;

	//	STEP2:	У����Ƿ���ȷ
	err = ChkCrc();
	if(err != 0)
		goto END;

	Global_Com_Ctx.ResetSta();
	//	STEP3:	У��BDE״̬
	err = ChkBDE();
	if(err != 0)
		goto END;

	//	TODO
	if(Global_Com_Ctx.Get0() != 0){
		if((err = Global_Com_Ctx.Com_Init()) != 0)
			return err;
	}

	//	STEP4:	�����ݷŵ�Э��������
	err = CpToProBuf();
	if(err != 0)
		goto END;

	//	STEP5:	����һ��Э���ȥ���ݴ���
	if(HadRcvedOnceBDE()){
		err = deal_pro.DealProFunc(Global_Pro_Ctx.pro_buf, Global_Pro_Ctx.pro_buf_used);
		Init();		//	���³�ʼ�����Ա��´�BDE�հ�
		if(err != 0)
			goto END;
	}

END:
	if(Global_Com_Ctx.Get1()){
		Global_Com_Ctx.Com_Destroy();
	}

	return err;
}


//////////////////////////////////////////////////////////////////////////

MArgoCliMgr::MArgoCliMgr()
{
	sock = -1;
	rcv_t = time(NULL);
	snd_t = time(NULL);
}

MArgoCliMgr::~MArgoCliMgr()
{

}

void MArgoCliMgr::Init()
{
	deal_bde.Init();
}

//	����ʱ����Դ�ͷ�
void MArgoCliMgr::ReInit()
{
	
	//	�ͷ���Global_sc_op_headָ��ķ������·���������Ϣ
	do{
		struct list_head				*pList, *pSafe;	
		sc_option						*pNode;

		list_for_each_safe(pList, pSafe, &Global_sc_op_head){
			pNode = list_entry(pList, sc_option, list_node);
			assert(pNode != NULL);
			list_del(pList);
			free(pNode);
		}
	}while(0);
	
	//	�ͷ���Global_Loc_Fileָ���;
	Global_Loc_File.Destroy();
	Global_Pro_Ctx.pro_buf_used = 0;	
	Global_Com_Ctx.Com_Destroy();
	Glboal_Mgr.Init();
	
}

void MArgoCliMgr::set_select_timeout(int time_out)
{
	INIT_TIMEVAL(tv, time_out);
}


#ifndef LINUXCODE
void  FileTimeToTime_t(  FILETIME  ft,  time_t  *t  )  
{	
	ULARGE_INTEGER	ui;
	
	ui.LowPart = ft.dwLowDateTime;
	ui.HighPart = ft.dwHighDateTime;
	
	*t = ((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);
}
#endif

//	��һ��time_t���͵�ʱ��ת��Ϊ����1970�������
unsigned int GetDayOfTime(time_t t)
{
	unsigned int one_day_secs = 24*60*60;

	return t/one_day_secs;
}

int MArgoCliMgr::build_local_mktd(sc_option	*pNode)
{
	char					tmp_file[FILE_NAME_SIZE*2+1];

	memset(tmp_file, 0, sizeof(tmp_file));
	sprintf(tmp_file, "%s/%s", Global_Loc_Option.GetMain(), pNode->sc_op.file_name);
	if(MFile::IsExist(tmp_file))
		return Global_Loc_File.Add(pNode, tmp_file, NULL);
	
	return 0;
}

int MArgoCliMgr::is_corect_time(unsigned int t, unsigned int old_del)
{
	int					tmp_old_del;

	tmp_old_del = GetDayOfTime(time(NULL)) - GetDayOfTime(t);
	printf("[%d]", tmp_old_del);
	if(tmp_old_del < 0)
		return 1;

	if((unsigned int)tmp_old_del <= old_del)
		return 1;

	return 0;
}

int MArgoCliMgr::build_local_fjy(sc_option *pNode)
{
#ifndef LINUXCODE
	WIN32_FIND_DATA				stFileInfo;
	time_t						lastModifyTime;
	char						abs_name[1024];
	char						dir_name[1024];//node�е���Ŀ¼�ṹ
	char						fjy_name[1024];
	int							err = 0 ;
	char						*last_sep;

	last_sep = strrchr(pNode->sc_op.file_name, '/');
	if(last_sep){	//	����Ŀ¼
		memmove(dir_name, pNode->sc_op.file_name, last_sep-pNode->sc_op.file_name);
		dir_name[last_sep-pNode->sc_op.file_name] = 0;
	}else{	//	û����Ŀ¼
		dir_name[0] = 0;
	}

	sprintf(abs_name, "%s/%s", Global_Loc_Option.GetMain(), pNode->sc_op.file_name, sizeof(abs_name)-1);
	memset(&stFileInfo, 0, sizeof(stFileInfo));
	HANDLE	pFile = ::FindFirstFile(abs_name, &stFileInfo);
	if(pFile == INVALID_HANDLE_VALUE)
	{
		Global_Log.writeLog(LOG_TYPE_INFO, "Ŀ¼[%s]��ʱ������", abs_name);
		return 0;
	}
	do
	{
		if(stFileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			continue;				
		}
		else
		{
			if(dir_name[0] == 0){
				strncpy(fjy_name, stFileInfo.cFileName, sizeof(fjy_name)-1);
				sprintf(abs_name, "%s/%s", Global_Loc_Option.GetMain(), stFileInfo.cFileName);
			}else{
				sprintf(fjy_name, "%s/%s", dir_name, stFileInfo.cFileName);
				sprintf(abs_name, "%s/%s/%s", Global_Loc_Option.GetMain(), dir_name, stFileInfo.cFileName);
			}
			::FileTimeToTime_t(stFileInfo.ftLastWriteTime,  &lastModifyTime);
			if(!is_corect_time(lastModifyTime, pNode->sc_op.old_del)){
				//MFile::DeleteFile(abs_name);	//�ǵô�
#ifdef _DEBUG
				Global_Log.writeLog(LOG_TYPE_DEBUG, "ɾ���ļ�[%s]", abs_name);
#endif
				continue;
			}
			//ʱ����ƥ��������㣬�ӵ�������
			err = Global_Loc_File.Add(pNode, abs_name, fjy_name);
			if(err)
				break;
		}
	}while(::FindNextFile(pFile, &stFileInfo));
	
	::FindClose(pFile);
	return err;

#else
	//linux����
	struct dirent           *dirp;
	DIR                     *dir;
	struct stat             statbuf;
    char                    path[1024];
	
	sprintf(path, "%s/%s", 
		Global_Loc_Option.GetMain(), 
		MFile::GetPathFromString(pNode->sc_op.file_name).c_str());
	//��Ŀ¼
	if((dir = opendir(path)) == NULL){
		Global_Log.writeLog(LOG_TYPE_ERROR, "��Ŀ¼[%s]������", path);
		return BUILD_ERROR(_OSerrno(), E_FISTFILE);
	}
	
	//����Ŀ¼
	while((dirp = readdir(dir)) != NULL){
		if(strcmp(dirp->d_name, ".") == 0 ||
			strcmp(dirp->d_name, "..") == 0){
			continue;//����
		}
		
		sprintf(path, "%s/%s", pDir, dirp->d_name);
		lstat(path, &statbuf);
		if(S_ISDIR(statbuf.st_mode)){
			scanfdir(path, ucDepth, regu, scanTime, dirHeadSize);
		}else{//�ļ�
			if(inner_match(dirp->d_name, regu)){
				AddToArray(path+dirHeadSize, statbuf.st_mtime, statbuf.st_size);
			}
		}
	}
	
    closedir(dir);
	return 0;	
#endif
}

int	MArgoCliMgr::build_local()
{
	struct list_head		*pTmp;
	sc_option				*pNode;
	int						err;

	list_for_each(pTmp, &Global_sc_op_head)
	{
		pNode = list_entry(pTmp, sc_option, list_node);
		assert(pNode != NULL);
		switch(pNode->sc_op.file_type)
		{
		case FILE_TYPE_MKTD:
			err = build_local_mktd(pNode);
			if(err)
				return err;
			break;
		case FILE_FJY:
			err = build_local_fjy(pNode);
			if(err)
				return err;
			break;
		default:
			Global_Log.writeLog(LOG_TYPE_ERROR, "�ļ���[%s]��֪�����ļ�����[%u]", pNode->sc_op.file_name, pNode->sc_op.file_type);
			return BUILD_ERROR(0, E_UN_FILETYPE);
		}
		
	}

	return 0;
}

int	MArgoCliMgr::up_local()
{
	return Global_Loc_File.UpInfo(sock);
}


int MArgoCliMgr::upload_local()
{
	int				err;

	err = build_local();
	if(err)
		return err;

	err = up_local();
	if(err)
		return err;

	return 0;
}

time_t	MArgoCliMgr::get_cur_time()
{
	return time(NULL);
}

int	MArgoCliMgr::trySendHeart()
{
	int						err;
	
	if(get_cur_time() - snd_t > Global_argo_base.heart_times){
		snd_t = time(NULL);
		err = Global_Loc_File.SendHeart(sock);
		if(err){
			Global_Log.writeLog(LOG_TYPE_ERROR, "����������������[%d]", err);
			return err;
		}
#ifdef _DEBUG
		Global_Log.writeLog(LOG_TYPE_DEBUG, "���Ϸ���������");
#endif
	}

	return 0;
}

int	MArgoCliMgr::is_stoped()
{
	return m_Thread.GetThreadStopFlag() == true ? 1 : 0;
}

void MArgoCliMgr::close_sock()
{
	if(sock != -1)
		MSocket::close_sock(sock);
	sock = -1;
}

int	MArgoCliMgr::main_func(char *ip, unsigned short port)
{
	fd_set					fdRead;					/* �ɶ��׽��ּ���*/
	int						err;
	char					ip_num[256];
	
	/* dnsת�� */
	err = MSocket::dnsToIP(ip, port, ip_num, sizeof(ip_num)-1);
	if( err != 0 ){
		Global_Log.writeLog(LOG_TYPE_DEBUG,
			"��[%s]����DNS����[%d]", 
			ip, err);
		goto END;
	}
	
	/*	STEP1: ����				*/
	if((sock = MSocket::block_connect(ip_num, port)) < 0){
		sock = BUILD_ERROR(_OSerrno(), E_CONNECT);
		Global_Log.writeLog(LOG_TYPE_ERROR, "����[%s][%d]��������[%d]", ip, port, sock);
		goto END;
	}
	Global_Log.writeLog(LOG_TYPE_INFO, "���ӷ�����[%s:%d]�ɹ�", ip, port);
	snd_t = time(NULL);
	while ( m_Thread.GetThreadStopFlag( ) == false ){
		//	���Է�������
		err = trySendHeart();
		if(err)
			goto END;
		
		set_select_timeout(SELECT_TIME_OUT);	//����select��ʱʱ��
		FD_ZERO(&fdRead);
		FD_SET(sock, &fdRead);
		err = select(0, &fdRead, NULL, NULL, &tv);
		if(err > 0)	/* ���׽��ֿɶ� */
		{
			rcv_t = time(NULL);
			err = deal_bde.DealOnceBDE(sock);
			if(err != 0){
				Global_Log.writeLog(LOG_TYPE_ERROR, "����������յ������ݷ�������[%d]", err);
				goto END;
			}
		}else if(err < 0)
		{
			assert(0);
			err = BUILD_ERROR(_OSerrno(), E_ABORT1);
			Global_Log.writeLog(LOG_TYPE_ERROR, "select����[%d]\n", err);
			goto END;
		}
		else{ 
			if(get_cur_time() - rcv_t > Global_argo_base.heart_times * 2){
				Global_Log.writeLog(LOG_TYPE_ERROR, "�ͻ����Ѿ�[%d]û���յ����������������ݣ���Ϊ��������·���Ͽ�����", 
					get_cur_time() - rcv_t);
				err =  BUILD_ERROR(0, E_TIMEOUT_HEART);
				goto END;
			}
		}
	}
END:
	close_sock();
	Global_Log.writeLog(LOG_TYPE_ERROR, "�����˳��Ĵ�����[%u][%u],�Ͽ�������[%s][%d]", 
		GET_SYS_ERR(err), GET_USER_ERR(err), ip, port);
	return 0;
}


void * __stdcall MArgoCliMgr::ArgoThreadFun( void * pIn )
{
	MArgoCliMgr				*classptr =  (MArgoCliMgr * )pIn;

	while(classptr->m_Thread.GetThreadStopFlag() == false)
	{
		classptr->main_func(Global_Loc_Option.GetIP(), Global_Loc_Option.GetPort());
		classptr->ReInit();
		MThread::Sleep(Global_argo_base.recon_times * 1000);
		
	}


	return 0;
	
}
/*	����ֵ��
	0�� �ɹ�
	*/
int	MArgoCliMgr::start_mainthread()
{
	int					err;

	if( ( err = m_Thread.StartThread("�������߳�", ArgoThreadFun,this ) ) != 1 )
	{
		Global_Log.writeLog( LOG_TYPE_ERROR, 0, "����argo�����߳�ʧ��[%s]",MErrorCode::GetErrString( err ).c_str() );
		return BUILD_ERROR(_OSerrno(), E_CR_THREAD);
	}
	
	return 0;
}

MCom_Ctx::MCom_Ctx()
{
	had_init = false;
	sta[0] = false;
	sta[1] = false;
}

MCom_Ctx::~MCom_Ctx()
{
	
}

int	MCom_Ctx::Com_Data(unsigned char *buf, unsigned int size)
{
	return de_compress.Decompress(buf, size);
}

int	MCom_Ctx::Com_Init()
{
	int					err;

	if(!had_init){
		if((err = de_compress.Init()) != 0){
			return BUILD_ERROR(0, E_ABORT1);
		}
		had_init = true;
	}else{
		return BUILD_ERROR(0, E_ABORT1);
	}

	return 0;
}

void	MCom_Ctx::Com_Destroy()
{
	if(had_init)
		de_compress.Destroy();
	had_init = false;
	ResetSta();
}

void MCom_Ctx::Set0()
{
	sta[0] = true;
}

void MCom_Ctx::Set1()
{
	sta[1] = true;
}

bool MCom_Ctx::Get0()
{
	return sta[0];
}
bool MCom_Ctx::Get1()
{
	return sta[1];
}

void MCom_Ctx::ResetSta()
{
	sta[0] = false;
	sta[1] = false;
}