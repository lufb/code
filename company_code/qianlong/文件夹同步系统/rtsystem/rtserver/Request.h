/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		Request.h
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RTϵͳ����������ģ��
	History:		
*********************************************************************************************************/
#ifndef __REQUEST_H__
#define __REQUEST_H__

#include "../common/commstruct.h"
#include "MEngine.hpp"
#include "Process.h"

//������ṹ, ���Դ���Ҳ���Դ���
typedef struct
{
	//����������д
	unsigned char			ChildType;			//������
	const void *			InBuf;				//���󻺳���
	unsigned long			InSize;				//����������

	int						nCurFrameNo;
	int						IsComplete;
	int						ApplyOffset;			//��������,������������
	__int64					ApplyTimeOffset;	//�´�ʱ��ƫ��

} tagRequestParam;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Request
{
public:
	Request();
	virtual ~Request();
	tagCommFun_SendResponse		*	m_SendResponse;					//����Ӧ����Ϣ
	tagCommFun_SendErrorMsg		*	m_SendErrorMsg;					//���ʹ�����Ϣ
public:
	int						Instance( tagSrvUnit_PlatInterface * In);
	void					Release();
	int						DispatchRequest( unsigned char cMarket, unsigned long lLinkNo, unsigned short sListenPort, unsigned long lIPAddr, unsigned short sPort, unsigned long lAddtionData, unsigned char cMainType, unsigned char cChildType,unsigned short sRequestCode, const void *	lpInBuf, int InSize	 );
	static int				OnLink( tagSrvComm_LinkMsg * pLinkMsg );
	static int				OnRequest( tagSrvComm_LinkMsg * pLinkMsg, char * pInBuf, unsigned short wInSize );	
	static int  			OnIdleFun0();
	static int  			OnIdleFun1();
	static int  			OnIdleFun2();
	static int  			OnIdleFun3();
	static int  			OnIdleFun4();
	int						GetRequestParam( tagRequestParam * );
	int						GetRequestData( tagRequestParam *, void *, int & nLen );
	int						GetRequestData_Cfg( tagRequestParam *, void *, int & nLen );
	int						GetRequestData_Login( tagRequestParam *, void *, int & nLen );
	int						GetRequestData_ListInfo( tagRequestParam *, void *, int & nLen );
	int						GetRequestData_iniList( tagRequestParam *, void *, int & nLen );
	int						GetRequestData_UpdateList( tagRequestParam *, void *, int & nLen );
	int						GetRequestData_File( tagRequestParam *, void *, int & nLen );
	int						GetRequestData_FileInfo( tagRequestParam *, void *, int & nLen );
	int						SendFrame( unsigned long LinkNo, const tagComm_FrameHead * MainHead, unsigned char FrameCnt, unsigned char FrameNo, const void * Buf, unsigned short Size, bool bCompress ) const;
	int						SendErrorFrame(unsigned long LinkNo, const tagComm_FrameHead * MainHead, short shErrCode, const char* szErr ) const;
};










#endif//__REQUEST_H__

