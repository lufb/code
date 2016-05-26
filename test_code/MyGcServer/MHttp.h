//--------------------------------------------------------------------------------------------------------------------------
//��Ԫ���ƣ�
//��Ԫ˵����
//�������ڣ�
//������Ա��
//--------------------------------------------------------------------------------------------------------------------------

#ifndef __MHTTP_H__
#define __MHTTP_H__

//#include "./gcbase.h"

//user HTTP_MAX_FRAME_SIZE  10240	

class MHttp
{
public:
	MHttp();
	 ~MHttp();
	static int vir_JudgePacket(char * buf, int size);

	// ֱ�ӷ���head ������
	static int GetHead(char* pBuf,int nSize,/*out*/ char* pHead, /*out*/ char* pRet, int nRetMaxLen);

	// ȷ��HTTPʵ�����ݵ���ʼλ�úͳ���
	static int GetDataSize(char* pBuf,int nSize, int nLen,  int &nStart, int &nRetSize);
	
	//����ͷ������
	static int SetData(char* pOutBuf,int nSize,char* szQlData, int nQlDataSize);

	//���ͷ����Ϣ
	static int AddHead(char* pOutBuf,int nSize,char* p_AddHead,  char* p_AddHeadData, char* pSpl);

	static int Instance(bool bIsClient);

	static bool m_bIsClient;

	static char* StrStr(char* pBuf, int nSize, char* pSearch);
	
public:
	// ��ȡЭ������
	static int GetType(char* pBuf,int nSize) ;
	enum
	{		
		TP_CHANNEL	=	0,	// ����ͨ��
		TP_UNIT,			// �½���Ԫ����
		TP_HEART,			// ������
		TP_DATA,			// ���ݵ���		
		TP_CLOSE,			// �ر�����
		TP_OPTION,			// ������ȡ
		TP_HELLOWORLD,		// ���
		TP_TRADEUNIT,		// �½�ί�е�Ԫ���� //tangj ֧��ί�д���,����Э��
	};
		
	// �½�ͨ��
	static int BuildChannel(char* pOutBuf,int nOutBufLen,  short usSrvChanSerial, unsigned short usChannelSerial, int nErrorCode);
	
	static int GetChannel(char* pInBuf,int nInBufSize, short &usSrvChanSerial, unsigned short &usChannelSerial,int &nErrorCode);

	// ������Ԫ����
	static int BuildUnit(char* pOutBuf,int nOutBufLen, unsigned short usUnitSerial, unsigned long ulLinkNo,
		const char* szIP, unsigned short usPort,int nErrorCode);

	//tangj ֧��ί�д���,����Э��
	static int BuildTradeUnit(char* pOutBuf,int nOutBufLen, unsigned short usUnitSerial, unsigned long ulLinkNo, 
		const char* szIP, unsigned short usPort, int nErrorCode);

	
	static int GetUnit(char* pInBuf,int nInBufSize, unsigned short &usUnitSerial, unsigned long &ulLinkNo, 
		char* szIP, unsigned short &usPort, int &nErrorCode);

	//tangj ֧��ί�д���,����Э��
	static int GetTradeUnit(char* pInBuf,int nInBufSize, unsigned short &usUnitSerial, unsigned long &ulLinkNo, 
		char* szIP, unsigned short &usPort, int &nErrorCode);

	// ����������
	static int BuildHeart(char* pOutBuf,int nOutBufLen);

	
	// ���ݷ���
	static int BuildData(char* pOutBuf,int nOutBufLen, unsigned short usUnitSerial, unsigned long ulLinkNo, char* pInBuf, int pInSize);
	
	static int GetData(char* pInBuf,int nInBufSize, unsigned short &usUnitSerial, unsigned long &ulLinkNo,int &nDataStart, int &nDataSize);// �п�������=0
	
	// �Ͽ�����
	static int BuildClose(char* pOutBuf,int nOutBufLen, unsigned short usUnitSerial, unsigned long ulLinkNo);
	
	static int GetClose(char* pInBuf,int nInBufSize, unsigned short &usUnitSerial, unsigned long &ulLinkNo);
	
	// ���
	static int BuildHelloWorld(char* pInBuf,int nInBufSize);

	static int BuildOption(char* pOutBuf,int nOutBufLen, unsigned short ulMaxChannleCount=0, unsigned short ulMaxChannleLink=0,int nErrorCode=0);
	static int GetOption(char* pInBuf,int nInBufSize, unsigned short &ulMaxChannleCount, unsigned short &ulMaxChannleLink,int &nErrorCode);

private:


};

#endif
