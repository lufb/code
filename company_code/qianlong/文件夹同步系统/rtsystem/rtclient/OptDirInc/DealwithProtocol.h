#ifndef _DEALWITH_PROTOCOL_H_20121226
#define _DEALWITH_PROTOCOL_H_20121226

#include "MEngine.hpp"
#include "Communicate.h"
#include "assert.h"
#include "../Global.h"
#include "../../common/OptDirTranNetData.h"
#include "../../common/OptBaseDef.h"


/**********************************************************************
����		MDirTranBase
����		��Ҫ�����Э����صĶ���
����		LUFUBO created	20121226
��ʷ
**********************************************************************/
class MDirTranProtocolBase: public M4XCommunicateIO{
public:
							MDirTranProtocolBase(unsigned char main, unsigned char child, unsigned char market);
	virtual					~MDirTranProtocolBase();
	unsigned char			GetMain();
	unsigned char			GetChild();
	unsigned char			GetMarket();

	int						DealWithProtocol(void* inParam = NULL, void* outParam = NULL);										
	
	int						Send(int s, char* buffer, size_t size);						//�첽��	
	int						Receive(int s, char* buffer, size_t size);					//�첽��
//�����Ϊ�˷��㣬�����Ƿ�װ��
	char					m_szSendBuffer[COMM_MAX_FRAME_SIZE];
	char					m_szReceiveBuffer[COMM_MAX_FRAME_SIZE];
public:
	virtual int				_CreatePackage(void* param = NULL) = 0;				//������Ҫʵ�ֵĽӿ�
	virtual int				_DoWithPackage(size_t size, void* outParam = NULL) = 0;					//������Ҫʵ�ֵĽӿ�
	virtual int				_DealWithErrorCode(short errCode) = 0;				//������Ҫʵ�ֵĽӿ�
	virtual bool			_HadDone(void* param = NULL) = 0;				//������Ҫʵ�ֵĽӿ�
	
private:
	unsigned char			m_ucMainType;				//���ݰ�������
	unsigned char			m_ucChildType;				//���ݰ���������
	unsigned char			m_ucMarket;					//�汾�ţ�Ҳ������Ϊͬ����ǣ���Ŀǰ����Ϊ'#'��Ҳ������ץȡ���ݰ�ʱ���з���	
};

/**********************************************************************
����		MProtocol10_240
����		��Ҫ�����Э��10_240
����		LUFUBO created	20121226
��ʷ
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
����		MProtocol10_240
����		��Ҫ�����Э��10_240
����		LUFUBO created	20121226
��ʷ
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
����		MProtocol10_232
����		��Ҫ�����Э��10_232
����		LUFUBO created	20121226
��ʷ
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
	tagResponseIni				m_stRequestIni;				//10_241Э��Ӧ���Ķ���

};


/**********************************************************************
����		MProtocol10_233
����		��Ҫ�����Э��10_233
����		LUFUBO created	20121226
��ʷ
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
����		MProtocol10_234
����		��Ҫ�����Э��10_234
����		LUFUBO created	20121226
��ʷ
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
����		MProtocol10_235
����		��Ҫ�����Э��10_235
����		LUFUBO created	20121226
��ʷ
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
����		MProtocol10_236
����		��Ҫ�����Э��10_236
����		LUFUBO created	20121226
��ʷ
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