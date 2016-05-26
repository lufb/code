/************************************************************************/
/* �ļ���:                                                              */
/*			interface/gcCliInterface.h									*/
/* ����:																*/
/*			����GcClient��Ӧ�ӿ�										*/
/* ����˵��:															*/
/*			�ýӿ��Ǵ���ǰGcClient���ƹ�����							*/
/* ��Ȩ:																*/
/*			CopyRight @¬���� <1164830775@qq.com>						*/
/* �޸���ʷ��															*/
/*			2013-11-25	¬����		����								*/
/*			                                                            */
/************************************************************************/


#ifndef _GC_CLIENT_INTERFACE_H_
#define _GC_CLIENT_INTERFACE_H_


#pragma pack(1)

//�ȵ��ú���GC_ComCmd type = 0xFF ���� �������� �� ����ģ��
//���� StartWork(unsigned char ucRunModel ) ��������ģʽ����ʼ�����õ�ʱ�򲻴��ļ���ȡ��
//���ú���GC_ComCmd type = 0xFE ���� ��ɫͨ�������� ��ַ�Ͷ˿� ���µ� ����
enum
{
	RM_NETWORK			= 0,	// Ϊ����ģʽ���Լ�����DLL,ʹ���Լ��������ļ�����������˿�
	RM_NETWORK_CONFIG	= 1,	// Ϊ����ģʽ���콢�����DLL���콢���������ã������Լ��������ļ�����������˿�
	//...
};


//Э�̴���ͷ
typedef struct
{
	char						szFlag[4];		// QLGC	
	unsigned short				usSize;			// ��������ݳ���	
	unsigned char				ucApplyType;	// ��������		lufubo���ע�� 1 ��ͨ�İ���2 ί�а�
}tagQLGCProxy_Apply;

//ucApplyType = 0  ���������Ϣ ����Ϊ0
//���ش���ͻ�����Ӧ�ó��򷵻ذ汾��Ϣ��������Ϣ, tagQLGCProxy_Info
typedef struct
{
	unsigned char				ucType;			// ������֤����
	unsigned short				usVER;			// �汾
	unsigned char				CheckCode;		// У����
} tagQLGCProxy_Info;

//ucApplyType = 1������֤��Ϣ
//����tagQLGCProxy_Apply| tagQLGCProxy_Certify
typedef struct
{
	unsigned char				ucType;					// ������֤����
	char						strUserName[32];		// �û�����
	char						strUserPassword[16];	// �û�����
	char						strIPAddress[32];		// Ŀ�������IP
	unsigned short				usPort;					// Ŀ��������˿�
	unsigned char				CheckCode;
} tagQLGCProxy_Certify;

//���ط�����֤�����ͬ������������������
//tagQLGCProxy_Apply| tagQLGCProxy_CertifyRet
typedef struct
{
	unsigned char				ucReply;				// ����CODE
	unsigned char				CheckCode;
}tagQLGCProxy_CertifyRet;

//����ucReply>0,��ʾ��������״��
//�ɹ���ucReply=0

//tangj ֧��ί�д���,����Э��
//ucApplyType = 2������֤��Ϣί�д�
//����tagQLGCProxy_Apply| tagQLGCProxy_TradeCertify
typedef struct
{
	unsigned char				ucType;					// ������֤����
	char						strUserName[32];		// �û�����
	char						strUserPassword[16];	// �û�����
	char						strIPAddress[32];		// Ŀ�������IP
	unsigned short				usPort;					// Ŀ��������˿�
	unsigned char				CheckCode;
} tagQLGCProxy_TradeCertify;

//���ط�����֤�����ͬ������������������
//tagQLGCProxy_Apply| tagQLGCProxy_CertifyRet
typedef struct
{
	unsigned char				ucReply;				// ����CODE
	unsigned char				CheckCode;
}tagQLGCProxy_TradeCertifyRet;

//2009-08-11 ����
//////////////////////////////////////////////////////////////////////////
typedef struct _tagGcPCOption
{
	unsigned short						sMaxLinkCount;						//���֧�ֵ���������
	unsigned short						sChannelMaxLinkCount;				//ͨ�����֧�ֵ���������
	unsigned short						sLinkTimeOut;						//���ӳ�ʱʱ��[��]
	unsigned long						lRecvBufSize;						//���ջ����С[�ܡ���λ���ֽ�]
	unsigned long						lSendBufSize;						//���ͻ����С[�ܡ���λ���ֽ�]
	unsigned short						sSendQueueCount;					//���Ͷ�������
	unsigned short						sListenCount;						//�������г���
	unsigned short						sResponseTime;						//Ӧ���ʹ���
	unsigned short						sMinCompressSize;					//��Сѹ������
	unsigned short						sLimitSendPercent;					//���Ʒ��Ͱٷֱ�	
	unsigned short						sSrvThreadCount;					//�����߳�����
	unsigned short						sSrvCoreThreadCount;				//���Ĵ����߳�����
	
	_tagGcPCOption()//�콢�����,���û�����
	{
		sMaxLinkCount			= 100;
		sChannelMaxLinkCount	= 1;
		sLinkTimeOut			= 120;
		lRecvBufSize			= 2048000;
		lSendBufSize			= 2048000;
		sSendQueueCount			= 100;
		sListenCount			= 100;
		sResponseTime			= 3;
		sMinCompressSize		= 100;
		sLimitSendPercent		= 50;
		sSrvThreadCount			= 4;
		sSrvCoreThreadCount		= 8;	
	};
} tagGcPCOption;

enum
{
	CT_SET_OPTION	= 0xFF,
	CT_SET_IP_PORT	= 0xFE,
	CT_SHOW_CONFIG	= 0xFD		// ��ӡ����	
};

//////////////////////////////////////////////////////////////////////////
// �콢�����,����ģʽ,��Ҫ�������ⲿ����
//ComCmd(char cType,void* pIn,void* pOut)

//1.	��������
//cType = CT_SET_OPTION
//����ṹΪtagQLGCProxy_OptionIn*
typedef struct
{
	tagGcPCOption				stSrv;		//����ģ������
	tagGcPCOption				stClt;		//�ͻ���ģ������	
} tagQLGCProxy_OptionIn;
//����ṹΪ��
//����ֵ>0 ��ʾ�ɹ�


//2.	������ɫͨ�������� (����Ż�����ͨ), ���ú����ٴε��� StartServer����������Ч
//cType = CT_SET_IP_PORT
//����ṹΪtagQLGCProxy_OptionIn*
typedef struct
{
	char						szIP[255+1];	//IP��ַ��������
	unsigned short				usPort;			//�˿�
} tagQLGCProxy_IPPortIn;




//lufb 2013-12-22����Э��Begin for GcC֧�ִ�������GcS
typedef struct
{
	char						strMagic[8];			//��Э�������־,CDYFSUPP
	unsigned char				ucType;					// ������֤����
	bool                    	bAuth;					//�Ƿ���Ҫ��֤��־
	char						strUserName[64];		// �û�����
	char						strUserPassword[64];	// �û�����
	char						strAgentAddress[256];	//���������Ip
	unsigned short				usAgentPort;			//����������˿�
	char						strIPAddress[256];		// Ŀ�������IP
	unsigned short				usPort;					// Ŀ��������˿�
	unsigned char				CheckCode;				//������
} tagNewQLGCProxy_Certify;
//lufb 2013-12-22����Э��End for GcC֧�ִ�������GcS



#pragma pack()


#endif