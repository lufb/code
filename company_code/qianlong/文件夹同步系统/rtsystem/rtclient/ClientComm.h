/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		ClientComm.h
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RTϵͳ���ջ�ͨ��ģ��
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
	unsigned short			m_usDataBufCurrentSize;	//��ǰ��������Ч���ݳ���
protected:
	static void * __stdcall ClientCommThreadFun( void * pIn );
	//ȡ�����ļ���������ַ������
	bool					CreatLink();
	MString					m_strCurrentServerAdd;//��ǰ���ӷ�������ַ
	unsigned short			m_sCurrentServerPort;//��ǰ���ӷ������˿�
private:
	SOCKET					m_clientsocket;
	bool					m_bConnectFlg;//���ӱ�־

public:
	MCounter				heartBeat; 
	unsigned short			m_sCurrentServerPos;//��ǰѡ��ķ�������ַ�б�λ�ã��������л��ã�
public:
	ClientComm();
	virtual ~ClientComm();
public:
	int						Instance();
	void					Release(void);
public:
	//ȡ���ӱ�־
	bool					GetLinkFlg();
	unsigned short			GetCurretServerPos();
	//ȡ��ǰ���ӵ�IP��ַ�Ͷ˿ں�
	void				    GetCurrentLinkMsg( char * IPAddr,unsigned short *Port );
public:
	int						OnConnectSrvSuc(void);
	void					OnConnectClose(const char * errmsg);
	int						OnRecvData( char * InBuf,const int Insize );
	void					OnTime();
public:
	int  				    SendRequest( unsigned char maintype,		//������
										unsigned char childtype,		//������
										const char * inbuf,				//�������ݻ���
										unsigned short insize,			//�������ݳ���
										unsigned short nRequestNo
									   );
	//�رյ�ǰ����
	void                    CloseLink( bool switchServer );
};

#endif //__CLIENTCOMM_H__

