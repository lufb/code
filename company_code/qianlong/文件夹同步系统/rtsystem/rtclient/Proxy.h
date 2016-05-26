/*
		文件：Proxy.h
		功能：代理客户端
		时间：2007.5.11
		作者：luozn
		版权: 上海乾隆高科技版权所有

		修改纪录
		修改日期		修改人员	修改内容和原因
*/
#ifndef __PROXY_H__
#define __PROXY_H__

#include "MEngine.hpp"
#include "gcproxy.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class MProxy
{
public:
	enum ProxyType{ SOCKS4PROXY, SOCKS5PROXY, HTTP1_1PROXY, QL_USBPROXY };
public:
	MProxy();
	virtual  ~MProxy();
	virtual int Instance(int	SvrSerial) = 0;
	virtual ProxyType GetProxyType() const = 0;
	SOCKET GetSocket() const;
protected:
	int ConnectToProxy( const char * address, unsigned short port );
	int SendToProxy( const char * Inbuf, int nSize );
	int	LoadCfg(int	SvrSerial);
	void Release();
protected:
	SOCKET					m_socket;
	tagGetProxyRes			m_Proxy;
	MString					m_SrvIP;
	unsigned short			m_SrvPort;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class MProxySock5 : public MProxy
{

public:
	MProxySock5();
	~MProxySock5();
	virtual int Instance(int	SvrSerial);
	ProxyType GetProxyType() const;
private:
	int SendSock5Request0();
	int RecvSock5Answer0();
	int	SendSock5Request1();
	int RecvSock5Answer1();
	int SendCmd();
	int RecvCmd();
private:
	char			m_Socket2Type;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class MProxySock4 : public MProxy
{

public:
	MProxySock4();
	~MProxySock4();
	virtual int Instance(int	SvrSerial);
	ProxyType GetProxyType() const;
private:
	int SendCmd();
	int RecvCmd();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class MProxyHttp : public MProxy
{
public:
	MProxyHttp();
	~MProxyHttp();
	virtual int Instance(int	SvrSerial);
	ProxyType GetProxyType() const;
private:
	int SendRequest();
	int RecvRequest();
	int Base64Encode( const char * Inbuf, char * Outbuf, int len );
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class MProxyUsb : public MProxy
{
private:
#pragma pack(1)
	struct					VerHead
	{
		unsigned char		Flag;
		unsigned char		Ver;
		unsigned long		SLength;
		unsigned char		Type;
		unsigned short		DLength;
		unsigned char		CheckCode;
	};

	struct					CerHead
	{
		unsigned char		Method;
		unsigned char		UsernameLen;
		unsigned char		PasswdLen;
		unsigned char		CheckCode;
	};

	struct					CerReply
	{
		unsigned char		Reply;
		unsigned char		CheckCode;
	};

	struct					ConHead
	{
		in_addr				DesIp;
		unsigned short		DesPort;
		unsigned char		CheckCode;
	};
#pragma pack()
public:
	MProxyUsb();
	virtual ~MProxyUsb();
	virtual int Instance(int	SvrSerial);
	ProxyType GetProxyType() const;
private:
	int SendRequest0();
	int RecvRequest0();
	int SendRequest1();
	int RecvRequest1();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class MProxyGC : public MProxy
{
public:
	MProxyGC();
	virtual ~MProxyGC();
	virtual int Instance(int	SvrSerial);
	ProxyType GetProxyType() const;
private:
	int SendRequest0();
	int RecvRequest0();
	int SendRequest1();
	int RecvRequest1();
};

#endif