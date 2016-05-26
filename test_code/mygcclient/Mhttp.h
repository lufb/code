//--------------------------------------------------------------------------------------------------------------------------
//单元名称：
//单元说明：
//创建日期：
//创建人员：
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

	// 直接返回head 的内容
	static int GetHead(char* pBuf,int nSize,/*out*/ char* pHead, /*out*/ char* pRet, int nRetMaxLen);

	// 确定HTTP实体数据的起始位置和长度
	static int GetDataSize(char* pBuf,int nSize, int nLen,  int &nStart, int &nRetSize);
	
	//设置头部数据
	static int SetData(char* pOutBuf,int nSize,char* szQlData, int nQlDataSize);

	//添加头部信息
	static int AddHead(char* pOutBuf,int nSize,char* p_AddHead,  char* p_AddHeadData, char* pSpl);

	static int Instance(bool bIsClient);

	static bool m_bIsClient;

	static char* StrStr(char* pBuf, int nSize, char* pSearch);
	
public:
	// 获取协议类型
	static int GetType(char* pBuf,int nSize) ;
	enum
	{		
		TP_CHANNEL	=	0,	// 建立通道
		TP_UNIT,			// 新建单元连接
		TP_HEART,			// 心跳包
		TP_DATA,			// 数据到达		
		TP_CLOSE,			// 关闭连接
		TP_OPTION,			// 参数获取
		TP_HELLOWORLD,		// 你好
		TP_TRADEUNIT,		// 新建委托单元连接 //tangj 支持委托代理,新增协议
	};
		
	// 新建通道
	static int BuildChannel(char* pOutBuf,int nOutBufLen,  short usSrvChanSerial, unsigned short usChannelSerial, int nErrorCode);
	
	static int GetChannel(char* pInBuf,int nInBufSize, short &usSrvChanSerial, unsigned short &usChannelSerial,int &nErrorCode);

	// 建立单元连接
	static int BuildUnit(char* pOutBuf,int nOutBufLen, unsigned short usUnitSerial, unsigned long ulLinkNo,
		const char* szIP, unsigned short usPort,int nErrorCode);

	//tangj 支持委托代理,新增协议
	static int BuildTradeUnit(char* pOutBuf,int nOutBufLen, unsigned short usUnitSerial, unsigned long ulLinkNo, 
		const char* szIP, unsigned short usPort, int nErrorCode);

	
	static int GetUnit(char* pInBuf,int nInBufSize, unsigned short &usUnitSerial, unsigned long &ulLinkNo, 
		char* szIP, unsigned short &usPort, int &nErrorCode);

	//tangj 支持委托代理,新增协议
	static int GetTradeUnit(char* pInBuf,int nInBufSize, unsigned short &usUnitSerial, unsigned long &ulLinkNo, 
		char* szIP, unsigned short &usPort, int &nErrorCode);

	// 发送心跳包
	static int BuildHeart(char* pOutBuf,int nOutBufLen);

	
	// 数据发送
	static int BuildData(char* pOutBuf,int nOutBufLen, unsigned short usUnitSerial, unsigned long ulLinkNo, char* pInBuf, int pInSize);
	
	static int GetData(char* pInBuf,int nInBufSize, unsigned short &usUnitSerial, unsigned long &ulLinkNo,int &nDataStart, int &nDataSize);// 有可能数据=0
	
	// 断开连接
	static int BuildClose(char* pOutBuf,int nOutBufLen, unsigned short usUnitSerial, unsigned long ulLinkNo);
	
	static int GetClose(char* pInBuf,int nInBufSize, unsigned short &usUnitSerial, unsigned long &ulLinkNo);
	
	// 你好
	static int BuildHelloWorld(char* pInBuf,int nInBufSize);

	static int BuildOption(char* pOutBuf,int nOutBufLen, unsigned short ulMaxChannleCount=0, unsigned short ulMaxChannleLink=0,int nErrorCode=0);
	static int GetOption(char* pInBuf,int nInBufSize, unsigned short &ulMaxChannleCount, unsigned short &ulMaxChannleLink,int &nErrorCode);

private:


};

#endif
