/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		ClientComm.h
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统接收机通信模块
	History:		
*********************************************************************************************************/
#ifndef __CLIENTCOMM_H__
#define __CLIENTCOMM_H__

#include "MEngine.hpp"
#include "Queue.h"
#include "Proxy.h"

#define  BUFFERSIZE      (8192*5) 

class ClientComm  
{
public:
	Queue					m_RecQueue;
	MCriticalSection		m_Section;
	MProxy		*			m_stproxy;
protected:
	MThread					m_ClientCommThread;
	char					m_DataBuf[BUFFERSIZE];
	unsigned short			m_usDataBufCurrentSize;	//当前处理缓存有效数据长度
protected:
	static void * __stdcall ClientCommThreadFun( void * pIn );
	//取配置文件服务器地址并连接
	bool					CreatLink();
	MString					m_strCurrentServerAdd;//当前连接服务器地址
	unsigned short			m_sCurrentServerPort;//当前连接服务器端口
private:
	SOCKET					m_clientsocket;
	bool					m_bConnectFlg;//连接标志

public:
	MCounter				heartBeat; 
	unsigned short			m_sCurrentServerPos;//当前选择的服务器地址列表位置（服务器切换用）
public:
	ClientComm();
	virtual ~ClientComm();
public:
	int						Instance();
	void					Release(void);
public:
	//取连接标志
	bool					GetLinkFlg();
	unsigned short			GetCurretServerPos();
	//取当前连接的IP地址和端口号
	void				    GetCurrentLinkMsg( char * IPAddr,unsigned short *Port );
public:
	int						OnConnectSrvSuc(void);
	void					OnConnectClose(const char * errmsg);
	int						OnRecvData( char * InBuf,const int Insize );
	void					OnTime();
public:
	int  				    SendRequest( unsigned char maintype,		//主类型
										unsigned char childtype,		//子类型
										const char * inbuf,				//请求内容缓冲
										unsigned short insize,			//请求内容长度
										unsigned short nRequestNo
									   );
	//关闭当前连接
	void                    CloseLink( bool switchServer );
};

#endif //__CLIENTCOMM_H__

