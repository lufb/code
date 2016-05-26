#ifndef _OPT_DIR_TRAN_NET_DATA_H_
#define _OPT_DIR_TRAN_NET_DATA_H_

#include "OptStructDef.h"

//�����д��������ͳһ��һ�ֽڶ���
#pragma pack(1)



//230Э�飺��ȡ������״̬
//����tagComm_FrameHead
//Ӧ��tagComm_FrameHead | tagServSupport
typedef struct
{
	enum SrvStatus			m_eSrvStatus;//�Ż����RT�����ֶα�ʾ��������״̬
} tagServSupport;

//231Э�飺��ȡ�������������
//����tagComm_FrameHead
//Ӧ��tagComm_FrameHead | tagSrvMaxFrame
typedef struct
{
	unsigned int 			m_uiSrvMaxFrame;
} tagSrvMaxFrame;

//232Э�飺���������ļ�
//����tagComm_FrameHead | tagResponseIni
//Ӧ��tagComm_FrameHead | tagResponseIni | ResponseContent
typedef struct
{
	unsigned char			usTotalDir;//�����ж��ٸ�Ŀ¼
} tagResponseIni;

typedef struct
{
	unsigned char			m_uszIndex;							//��Ŀ¼��Ϣ���ڵ��±�
	unsigned char 			m_uszUseFileCrc32;					//�Ƿ������ļ����ݵ�CRC32У��
	char					m_szPath[MAX_PATH];					//path��Ŀ¼, ������mainpath����logs/		#define MAX_PATH 260
	char					m_szSrcPath[MAX_PATH];				//srcpath������û������Ϊ��
	char					m_szSrvMainPath[MAX_PATH];			//SRV�µ�mainpathĿ¼
	char					m_szName[MAX_PATH];					//ͬ���ļ������ֻ���ͨ���
	char					m_szInfo[MAX_PATH];					//
	unsigned int			m_uiCupdateCycle;					//�ͻ���������������б��ʱ����
	char					m_szSyncMode;						//��Ԥ��
	char					m_szUpdateMode;						//��Ԥ��	
} tagRT_ResponseContent;

//233Э�飺����ĳ��Ŀ¼����
//����tagComm_FrameHead | tagRequestDirAttr
//Ӧ��tagComm_FrameHead | tagResponseDirAttr
typedef struct{
	unsigned char			m_ucIndex;			// ��Ŀ¼���±�ֵ
	char					m_cName[MAX_PATH];	// Ŀ¼��
}tagRequestDirAttr;

typedef struct{
	unsigned long			m_ulCRC32;				//Ŀ¼У����
	unsigned long			m_ulFileAndDirTotalNum;	//Ŀ¼�µ��ļ�����Ŀ¼�ܸ�����
}tagResponseDirAttr;


//234Э�飺��Ŀ¼������ĳ�������ļ����ԣ��ڴ��У�
//����tagComm_FrameHead | tagRequestFileAttr
//Ӧ��tagComm_FrameHead | tagResponseFileAttr
typedef struct {
	unsigned char			m_ucIndex;				//Ŀ¼�±�
	unsigned char			m_ucSwitchTime;			//�����л������������������ж�ָ���Ƿ�Ϸ�
	char					m_cName[MAX_PATH];		//Ŀ¼��
	unsigned long			ulInSrvMemAddr;			//ָ�룬���ȡ�ĵ�ַ����ʼ��Ϊ0
	tagResponseDirAttr		m_sToken;				//���ƣ����������ж�ָ���Ƿ�Ϸ���
}tagRequestFileAttr;

typedef struct {
	unsigned long			ulInSrvMemAddr;			//ָ�룬�´�ȡ�ĵ�ַ������λ���Ա�����Ŀ¼������ͨ�ļ�
	tagNodeAttr				m_sAttr;				//�ļ�����Ŀ¼�����Խڵ㣬ע�⣬�˴�tagNodeAttrҲ��һ�ֽڶ����˵�
	char					m_cName[MAX_PATH];		//��������Ŀ¼���ļ���������Ŀ¼����
	unsigned char			m_ucSwitchTime;			//�����л�����
}tagResponseFileAttr;


//235Э�飺�ͻ���������ĳ���ļ�ʱ����ĳ���ļ����ԣ������У�
//����tagComm_FrameHead | tagRequestFileAttrInDisk
//Ӧ��tagComm_FrameHead | tagResponseFileAttrInDisk
typedef struct {
	unsigned char			m_ucIndex;				//Ŀ¼�±�
	char					m_cName[MAX_PATH];		//�ļ���
}tagRequestFileAttrInDisk;

typedef struct {
	tagFileAttr				m_sFileAttr;			//�ļ��ڴ����е�����	
}tagResponseFileAttrInDisk;

//236Э�飺�ͻ�������ĳ���ļ�����
//����tagComm_FrameHead | tagRequestFileData
//Ӧ��tagComm_FrameHead | tagRequestFileData | �ļ�����
typedef struct
{
	unsigned char			m_ucCompressType;		//ѹ������
	unsigned char			m_uszDirIndex;			//����Ŀ¼���±�ֵ
	char					m_szPath[MAX_PATH];		//�ļ���
	unsigned long			m_ulOffset;				//�ļ�ƫ��
	unsigned long			m_ulTransSize;			//��ƫ�ƴ���ʼ���͵��ļ���С��������0XFFFFFFFF,��ʾֱ���ļ�ĩβ	
	tagFileAttr				m_sFileAttr;			//�ļ�����
} tagRequestFileData;




#pragma pack()

#endif