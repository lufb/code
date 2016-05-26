/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		Request.h
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统服务器请求模块
	History:		
*********************************************************************************************************/
#ifndef __REQUEST_H__
#define __REQUEST_H__

#include "../common/commstruct.h"
#include "MEngine.hpp"
#include "Process.h"

//请求处理结构, 可以传入也可以传出
typedef struct
{
	//由主函数填写
	unsigned char			ChildType;			//子类型
	const void *			InBuf;				//请求缓冲区
	unsigned long			InSize;				//缓冲区长度

	int						nCurFrameNo;
	int						IsComplete;
	int						ApplyOffset;			//请求类型,记忆请求类型
	__int64					ApplyTimeOffset;	//下次时间偏移

} tagRequestParam;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Request
{
public:
	Request();
	virtual ~Request();
	tagCommFun_SendResponse		*	m_SendResponse;					//发送应答消息
	tagCommFun_SendErrorMsg		*	m_SendErrorMsg;					//发送错误消息
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

