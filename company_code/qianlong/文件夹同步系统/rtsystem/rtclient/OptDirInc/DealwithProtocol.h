#ifndef _DEALWITH_PROTOCOL_H_20121226
#define _DEALWITH_PROTOCOL_H_20121226

#include "MEngine.hpp"
#include "Communicate.h"
#include "assert.h"
#include "../Global.h"
#include "../../common/OptDirTranNetData.h"
#include "../../common/OptBaseDef.h"


/**********************************************************************
类名		MDirTranBase
功能		主要处理跟协议相关的东西
作者		LUFUBO created	20121226
历史
**********************************************************************/
class MDirTranProtocolBase: public M4XCommunicateIO{
public:
							MDirTranProtocolBase(unsigned char main, unsigned char child, unsigned char market);
	virtual					~MDirTranProtocolBase();
	unsigned char			GetMain();
	unsigned char			GetChild();
	unsigned char			GetMarket();

	int						DealWithProtocol(void* inParam = NULL, void* outParam = NULL);										
	
	int						Send(int s, char* buffer, size_t size);						//异步发	
	int						Receive(int s, char* buffer, size_t size);					//异步收
//这儿是为了方便，不考虑封装性
	char					m_szSendBuffer[COMM_MAX_FRAME_SIZE];
	char					m_szReceiveBuffer[COMM_MAX_FRAME_SIZE];
public:
	virtual int				_CreatePackage(void* param = NULL) = 0;				//派生类要实现的接口
	virtual int				_DoWithPackage(size_t size, void* outParam = NULL) = 0;					//派生类要实现的接口
	virtual int				_DealWithErrorCode(short errCode) = 0;				//派生类要实现的接口
	virtual bool			_HadDone(void* param = NULL) = 0;				//派生类要实现的接口
	
private:
	unsigned char			m_ucMainType;				//数据包主类型
	unsigned char			m_ucChildType;				//数据包辅助类型
	unsigned char			m_ucMarket;					//版本号（也可以作为同步标记），目前定义为'#'，也方便在抓取数据包时进行分析	
};

/**********************************************************************
类名		MProtocol10_240
功能		主要处理跟协议10_240
作者		LUFUBO created	20121226
历史
**********************************************************************/
class MProtocol10_230: public MDirTranProtocolBase{
public:
							MProtocol10_230(unsigned char main, unsigned char child, unsigned char market);
	virtual					~MProtocol10_230();

	enum SrvStatus			GetSrvStatus();

private:
	virtual int				_CreatePackage(void* param = NULL);
	virtual int				_DoWithPackage(size_t size, void* outParam = NULL);
	virtual int				_DealWithErrorCode(short errCode);
	virtual bool			_HadDone(void* param = NULL);

private:	
	enum SrvStatus			m_bSrvStatus;
};


/**********************************************************************
类名		MProtocol10_240
功能		主要处理跟协议10_240
作者		LUFUBO created	20121226
历史
**********************************************************************/
class MProtocol10_231: public MDirTranProtocolBase{
public:
							MProtocol10_231(unsigned char main, unsigned char child, unsigned char market);
	virtual					~MProtocol10_231();
	
	unsigned int			GetMaxFrames();
	
private:
	virtual int				_CreatePackage(void* param = NULL);
	virtual int				_DoWithPackage(size_t size, void* outParam = NULL);
	virtual int				_DealWithErrorCode(short errCode);
	virtual bool			_HadDone(void* param = NULL);
private:
	unsigned int			m_uiMaxFrames;
};


/**********************************************************************
类名		MProtocol10_232
功能		主要处理跟协议10_232
作者		LUFUBO created	20121226
历史
**********************************************************************/
class MProtocol10_232: public MDirTranProtocolBase{
public:
							MProtocol10_232(unsigned char main, unsigned char child, unsigned char market);
	virtual					~MProtocol10_232();
	
private:
	virtual int				_CreatePackage(void* param = NULL);
	virtual int				_DoWithPackage(size_t size, void* outParam = NULL);
	virtual int				_DealWithErrorCode(short errCode);
	virtual bool			_HadDone(void* param = NULL);
private:
	tagResponseIni				m_stRequestIni;				//10_241协议应带的东西

};


/**********************************************************************
类名		MProtocol10_233
功能		主要处理跟协议10_233
作者		LUFUBO created	20121226
历史
**********************************************************************/
class MProtocol10_233: public MDirTranProtocolBase{
public:
							MProtocol10_233(unsigned char main, unsigned char child, unsigned char market);
	virtual					~MProtocol10_233();
	
	
private:
	virtual int				_CreatePackage(void* param = NULL);
	virtual int				_DoWithPackage(size_t size, void* outParam = NULL);
	virtual int				_DealWithErrorCode(short errCode);
	virtual bool			_HadDone(void* param = NULL);

};


/**********************************************************************
类名		MProtocol10_234
功能		主要处理跟协议10_234
作者		LUFUBO created	20121226
历史
**********************************************************************/
class MProtocol10_234: public MDirTranProtocolBase{
public:
							MProtocol10_234(unsigned char main, unsigned char child, unsigned char market);
	virtual					~MProtocol10_234();

	
private:
	virtual int				_CreatePackage(void* param = NULL);
	virtual int				_DoWithPackage(size_t size, void* outParam = NULL);
	virtual int				_DealWithErrorCode(short errCode);
	virtual bool			_HadDone(void* param = NULL);
};

/**********************************************************************
类名		MProtocol10_235
功能		主要处理跟协议10_235
作者		LUFUBO created	20121226
历史
**********************************************************************/
class MProtocol10_235: public MDirTranProtocolBase{
public:
							MProtocol10_235(unsigned char main, unsigned char child, unsigned char market);
	virtual					~MProtocol10_235();

	
private:
	virtual int				_CreatePackage(void* param = NULL);
	virtual int				_DoWithPackage(size_t size,void* outParam = NULL);
	virtual int				_DealWithErrorCode(short errCode);
	virtual bool			_HadDone(void* param = NULL);
};


/**********************************************************************
类名		MProtocol10_236
功能		主要处理跟协议10_236
作者		LUFUBO created	20121226
历史
**********************************************************************/
class MProtocol10_236: public MDirTranProtocolBase{
public:
							MProtocol10_236(unsigned char main, unsigned char child, unsigned char market);
	virtual					~MProtocol10_236();
	void					CreateAEmptyfile(char* fileName, time_t time);
	bool					TryDeleteFile( MString& strFile, int iTryTimes = 3);
	bool					TryCreateDirIfNeed( const char* strDirName);
	
private:
	virtual int				_CreatePackage(void* param = NULL);
	virtual int				_DoWithPackage(size_t size, void* outParam = NULL);
	virtual int				_DealWithErrorCode(short errCode);
	virtual bool			_HadDone(void* param = NULL);

};



#endif