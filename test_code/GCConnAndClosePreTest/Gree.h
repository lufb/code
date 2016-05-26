/**
 *	communication/msgbox_green.h
 *
 *	Copyright (C) 2012 yjj  <ayjj_8109@qq.com>
 *
 *	��������Ϣ�пͻ�����ɫͨ��������ݽṹ
 *
 *	�޸���ʷ:
 *
 *	2012-09-27 - �״δ���
 *
 *                     yjj	<ayjj_8109@qq.com>
 */

#ifndef		_MSGBOX_GREEN_H_
#define		_MSGBOX_GREEN_H_

//#include "define.h" 
#include <process.h>
#include <winsock.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <io.h>
#include <Nb30.h> 
#include <map>
#include <vector>
#include <string>
#include <list>
#include <mmsystem.h>

#pragma pack(1)

/**
 * ��������ɫͨ��������ݽṹ
 */
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
		sMaxLinkCount			= 1000;
		sChannelMaxLinkCount	= 1;
		sLinkTimeOut			= 120;
		lRecvBufSize			= 2048000;
		lSendBufSize			= 2048000;
		sSendQueueCount			= 5000;
		sListenCount			= 5000;
		sResponseTime			= 3;
		sMinCompressSize		= 100;
		sLimitSendPercent		= 50;
		sSrvThreadCount			= 4;
		sSrvCoreThreadCount		= 8;

// 		sMaxLinkCount			= 10;
// 		sChannelMaxLinkCount	= 1;
// 		sLinkTimeOut			= 120;
// 		lRecvBufSize			= 102400;
// 		lSendBufSize			= 102400;
// 		sSendQueueCount			= 100;
// 		sListenCount			= 100;
// 		sResponseTime			= 3;
// 		sMinCompressSize		= 100;
// 		sLimitSendPercent		= 50;
// 		sSrvThreadCount			= 2;
// 		sSrvCoreThreadCount		= 2;
	};
} tagGcPCOption;

enum
{
	CT_SET_OPTION	= 0xFF,
	CT_SET_IP_PORT	= 0xFE,
	CT_SHOW_CONFIG	= 0xFD		// ��ӡ����	
};

//�ȵ��ú���GC_ComCmd type = 0xFF ���� �������� �� ����ģ��
//���� StartWork(unsigned char ucRunModel ) ��������ģʽ����ʼ�����õ�ʱ�򲻴��ļ���ȡ��
//���ú���GC_ComCmd type = 0xFE ���� ��ɫͨ�������� ��ַ�Ͷ˿� ���µ� ����
enum
{
	RM_NETWORK			= 0,	// Ϊ����ģʽ���Լ�����DLL,ʹ���Լ��������ļ�����������˿�
	RM_NETWORK_CONFIG	= 1,	// Ϊ����ģʽ���콢�����DLL���콢���������ã������Լ��������ļ�����������˿�
	//...
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

typedef struct
{
	char						szIP[255+1];	//IP��ַ��������
	unsigned short				usPort;			//�˿�
} tagQLGCProxy_IPPortIn;

#pragma pack()

typedef unsigned long   (__stdcall *pGetDllVersion)();
typedef int  (__stdcall *pStartWork)(unsigned char ucRunModel, unsigned short usPort );
typedef int  (__stdcall *pEndWork)();
typedef int  (__stdcall *pConfig)(void * pHwnd);
typedef int  (__stdcall *pSetCallBack)(void * pFunc);
typedef int  (__stdcall *pComCmd)(unsigned char cType,void* p1,void* p2);

class cls_proxy_green
{
public:
	cls_proxy_green();
	~cls_proxy_green();

protected:
	HMODULE					m_gc_module;

	pGetDllVersion			m_gc_GetDllVersion;
	pStartWork				m_gc_StartWork;
	pEndWork				m_gc_EndWork;
	pConfig					m_gc_Config;
	pSetCallBack			m_gc_SetCallBack;
	pComCmd					m_gc_ComCmd;


public:
	/**
	 *	instance		- ��ʼ����ɫͨ��
	 *
	 *	return:
	 *			���ؽ����õļ����˿�
	 */
	unsigned short		instance();

	/**
	 *	release			- �ͷ���ɫͨ��ģ��
	 *
	 */
	void release();
	
	/**
	 *	comcmd			- ��ɫͨ�������������
	 *
	 *	@cType			[in]���ò������ͣ�CT_SET_OPTION��CT_SET_IP_PORT��CT_SHOW_CONFIG��
	 *	@p1				[in]��Ҫ���õĲ���
	 *	@p2				[out]�������
	 *
	 *	return:
	 *			���ؽ����õļ����˿�
	 */
	int	comcmd( IN unsigned char cType, IN void* p1, OUT void* p2 );
};

#endif		// _MSGBOX_GREEN_H_
