/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		Communicate.cpp
	Author:			lufubo6
	Create:			2012.12.25
	
	Description:	RT通讯模块（仅文件夹传输才用了）
	History:		
*********************************************************************************************************/


#ifndef _COMMNICATE_H_20121226
#define _COMMNICATE_H_20121226

#include "MEngine.hpp"
#include <time.h>
#include <windows.h>

#ifndef WIN32
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif



/**********************************************************************
类名		MCommunicateBaseIO
功能		通讯基类
作者		LUFUBO created	20130104
历史
**********************************************************************/
class MCommunicateBaseIO
{
public:
								MCommunicateBaseIO();
	virtual						~MCommunicateBaseIO();
	void						Instance();
	void						ns_startup();
	void 						ns_cleanup();
	int 						ns_connect2(unsigned int ip, unsigned short port);
	int 						ns_connect(const char *ip, unsigned short port);
	int 						ns_recv(int s, void *buffer, size_t size);
	int							ns_send(int s, void *buffer, size_t size);
	void 						ns_close(int s);
	int 						ns_create();
	int 						ns_bind(int s, unsigned short port);
	int							ns_set_recv_timeout(int s, unsigned int timeout);
	
	//以下的函数是为了方便而实现的
	void						ns_set_socket(SOCKET s);
	const SOCKET				ns_get_socket() const;
	int							ns_set_currentserverpos(unsigned short);
	const unsigned short		ns_get_currentserverpos() const;
	int							ns_set_currentserveradd(char *add, unsigned size);
	const char* const			ns_get_currentserveradd() const;
	void 						ns_set_currentserverport(unsigned short port);
	const unsigned short		ns_get_currentserverport() const;
	void						ns_set_connectflag(bool flag);
	const	bool				ns_get_connectflag() const;
	
private:
	SOCKET						m_ClientSocket;
	char						m_strCurrentServerAdd[80];	//当前连接服务器地址,在链路出错后会连下一个地址
	unsigned short				m_sCurrentServerPort;		//当前连接服务器端口,在链路出错后会连下一个地址
	unsigned short				m_sCurrentServerPos;	
	bool						m_bConnectFlg;
};

/**********************************************************************
类名		M4XCommunicateIO
功能		4X通讯类
作者		LUFUBO created	20121226
历史
**********************************************************************/
class M4XCommunicateIO: public MCommunicateBaseIO
{
public:
								M4XCommunicateIO();
	virtual						~M4XCommunicateIO();
	int							Create4XHead(tagComm_FrameHead* head, char *body, size_t bodySize, unsigned char market, 
 											unsigned char main, unsigned char child, short errorCode);
	bool						RecvPacIsRight(char* buffer, size_t bufferSize, unsigned char main, unsigned char chile);
};


#endif