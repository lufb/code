/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		Request.cpp
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统服务器请求模块
	History:		
*********************************************************************************************************/
#include "Request.h"
#include "Global.h"
#include "filedata.h"
//#include "DirTranInc/DirTranMgr.h"

#define MAX_FRAME_SEND		10
#define MAX_DATA_FRAME_SIZE 8000

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Request::Request()
{}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Request::~Request()
{}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Request::OnIdleFun0()
{
	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Request::OnIdleFun1()
{
	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Request::OnIdleFun2()
{
	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Request::OnIdleFun3()
{
	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Request::OnIdleFun4()
{
	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Request::Instance( tagSrvUnit_PlatInterface * pIn)
{
	Release();

	m_SendResponse = pIn->SendResponse;
	m_SendErrorMsg = pIn->SendErrorMsg;	

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Request::Release()
{
	m_SendResponse = NULL;
	m_SendErrorMsg = NULL;	
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int	Request::SendFrame( unsigned long LinkNo, const tagComm_FrameHead * MainHead, unsigned char FrameCnt, unsigned char FrameNo, const void * Buf, unsigned short Size, bool bCompress ) const
{
	return  m_SendResponse( MainHead->cMarket, LinkNo, MainHead->cMainType, MainHead->cChildType, MainHead->sRequestCode, FrameCnt, FrameNo, (char *)Buf, Size, bCompress );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int	Request::SendErrorFrame(unsigned long LinkNo, const tagComm_FrameHead * MainHead, short shErrCode, const char* szErr ) const
{
	int							nLen = 0;
	
	if( szErr )
	{
		nLen = strlen( szErr );
	}
	return m_SendErrorMsg( MainHead->cMarket, LinkNo, MainHead->cMainType, MainHead->cChildType, MainHead->sRequestCode, shErrCode, szErr, nLen );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int	Request::OnRequest( tagSrvComm_LinkMsg * pLinkMsg, char * pInBuf, unsigned short nInSize )	
{
	int						nRetVal = 1;

	if( pLinkMsg == NULL )
	{
		return 0;
	}

	switch( pLinkMsg->cMessageType )
	{
		//数据到达//请求数据
	case SRVCOMM_LINKMSG_DATA:
		if(pLinkMsg->cChildType >= 230 && pLinkMsg->cChildType <= 236)
		{
			nRetVal = Global_DirTran.DealRequest(pLinkMsg, pInBuf, nInSize);
// 			if(Global_DirTran.GetSrvStatus() == hadNotInit)//服务器还没初始化
// 			{
// 				tagComm_FrameHead	tagFrameHead = {0};
// 				
// 				tagFrameHead.cMarket = pLinkMsg->cMarket;
// 				tagFrameHead.cMainType = pLinkMsg->cMainType;
// 				tagFrameHead.cChildType = pLinkMsg->cChildType;
// 				tagFrameHead.sRequestCode = pLinkMsg->sRequestCode;
// 				tagFrameHead.sDataSize = 0;
// 				tagFrameHead.sErrorCode = -1001;
// 				Global_Request.SendErrorFrame( pLinkMsg->lLinkNo, &tagFrameHead, -1001, "" );
// 				break;
// 			}else{
// 				//进入文件夹传输的处理
// 				nRetVal = Global_DirTran.DealRequest(pLinkMsg, pInBuf, nInSize);//
// 			}
		}
		else{
			nRetVal =  Global_Request.DispatchRequest( pLinkMsg->cMarket, pLinkMsg->lLinkNo, pLinkMsg->sListenPort, pLinkMsg->lIPAddr, pLinkMsg->sPort, \
				pLinkMsg->lAddtionData, pLinkMsg->cMainType, pLinkMsg->cChildType, pLinkMsg->sRequestCode, pInBuf, nInSize );
		}
		break;
	
		
	//新连接到达
	case SRVCOMM_LINKMSG_NEWLINK:
		break;
		//客户端主动关闭
	case SRVCOMM_LINKMSG_CLIENTCLOSE:
		//服务器主动关闭
	case SRVCOMM_LINKMSG_SERVERCLOSE:
		//连接关闭
		//通知删除该用户的注册推送
		break;
	default:
		break;
	}

	return nRetVal;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int	Request::DispatchRequest( unsigned char cMarket, unsigned long lLinkNo, unsigned short sListenPort, unsigned long lIPAddr, unsigned short sPort,
								  unsigned long lAddtionData, unsigned char cMainType, unsigned char cChildType,unsigned short sRequestCode, 
								  const void *	lpInBuf, int InSize	 )
{
	int					nRetVal = -1;
	tagRequestParam		tagParam = {0};
	char				tempbuf[MAX_DATA_FRAME_SIZE];
	int					nTemp = 0;
	tagComm_FrameHead	tagFrameHead = {0};

	tagFrameHead.cMarket = '#';
	tagFrameHead.cMainType = cMainType;
	tagFrameHead.cChildType = cChildType;
	tagFrameHead.sRequestCode = sRequestCode;


	tagParam.ChildType = cChildType;
	tagParam.InBuf = lpInBuf;
	tagParam.InSize = InSize;
	tagParam.IsComplete = 0;

	if( (nRetVal = GetRequestParam( &tagParam ) ) < 0 )
	{
		SendErrorFrame( lLinkNo, &tagFrameHead, -300, "请求参数错误" );
		return -2;
	}

	for(tagParam.nCurFrameNo = 0; tagParam.nCurFrameNo < MAX_FRAME_SEND; tagParam.nCurFrameNo++)
	{
		nTemp = sizeof(tempbuf);
		if( (nRetVal = GetRequestData( &tagParam, tempbuf, nTemp ) ) < 0 )
		{
			SendErrorFrame( lLinkNo, &tagFrameHead, -302, "取数据失败" );
			break;
		}
		
		if( nTemp >  MAX_DATA_FRAME_SIZE )
		{
			assert(false);//tangj 报告数据组合超长错误
			SendErrorFrame( lLinkNo, &tagFrameHead, -302, "组合数据超长" );
			break;
		}
		
		if( (nRetVal = SendFrame( lLinkNo, &tagFrameHead, MAX_FRAME_SEND, tagParam.nCurFrameNo, tempbuf, nTemp , 1) ) < 0 )
		{
			SendErrorFrame( lLinkNo, &tagFrameHead, -302, "发送数据失败" );
			
			break;
		}

		if(tagParam.IsComplete)
			break;
	}

	return nRetVal;
}

int Request::OnLink( tagSrvComm_LinkMsg * pLinkMsg ){
	return 1;
}

int	Request::GetRequestParam( tagRequestParam * pparam)
{
	int					nRetVal = -1;
	if( pparam == NULL )
	{
		return 0;
	}
	
	switch( pparam->ChildType )
	{
	case 64:
		break;		

	case 65:
		{
			printf("5");
			if(pparam->InSize < sizeof(tagListInfoRequest))
				return -1;
		}
		break;

	case 66:
		{
			printf("6");
			if(pparam->InSize < sizeof(tagListRequst))
				return -1;
			tagListRequst * pRequest = (tagListRequst *)pparam->InBuf;
			pparam->ApplyOffset = pRequest->nSerial;
		}
		break;

	case 67:
		{
			printf("7");
			if(pparam->InSize < sizeof(tagListRequst))
				return -1;
			tagListRequst * pRequest = (tagListRequst *)pparam->InBuf;
			pparam->ApplyOffset = pRequest->nSerial;
		}
		break;

	case 68:
		{
			printf("8");
			if(pparam->InSize < sizeof(tagFileRequst))
				return -1;
			tagFileRequst * pRequest = (tagFileRequst *)pparam->InBuf;
			pparam->ApplyTimeOffset = pRequest->stOffsetTime;
			pparam->ApplyOffset = pRequest->nOffset;
		}
		break;

	case 69:
		{
			printf("9");
			if(pparam->InSize < sizeof(tagInfoRequst))
				return -1;
		}
		break;

	case 70:
		{
			printf("0");
			if(pparam->InSize < sizeof(tagCfgRequest))
				return -1;
		}
		break;
		
	default:
		TraceLog(LOG_ERROR_NORMAL,"请求","取请求信息失败[暂不支持的子协议号(%d)]", pparam->ChildType );
		return( -1 );
	}
	
	return 1;
}
int	Request::GetRequestData( tagRequestParam * pparam, void * InBuf, int & nLen )
{
	int					nRetVal = -1;
	if( pparam == NULL )
	{
		return 0;
	}
	
	switch( pparam->ChildType )
	{
	case 64:
		nRetVal = GetRequestData_Login( pparam, InBuf, nLen );
		break;
	case 65:
		nRetVal = GetRequestData_ListInfo( pparam, InBuf, nLen );
		break;
	case 66:
		nRetVal = GetRequestData_iniList( pparam, InBuf, nLen );
		break;
	case 67:
		nRetVal = GetRequestData_UpdateList( pparam, InBuf, nLen );
		break;
	case 68:
		nRetVal = GetRequestData_File( pparam, InBuf, nLen );
		break;
	case 69:
		nRetVal = GetRequestData_FileInfo( pparam, InBuf, nLen );
		break;
	case 70:
		nRetVal = GetRequestData_Cfg( pparam, InBuf, nLen );
		break;
		
	default:
		TraceLog(LOG_ERROR_NORMAL,"请求","取请求数据失败[暂不支持的子协议号(%d)]", pparam->ChildType );
		return( -1 );
	}
	
	return nRetVal;
}

int	Request::GetRequestData_Login( tagRequestParam *pParam, void *buf, int & nLen )
{
	return 1;
}

int	Request::GetRequestData_Cfg( tagRequestParam *pParam, void *buf, int & nLen )
{
	tagCfgResponse * pstResponse;
	int	datalen = nLen - sizeof(tagCfgResponse);
	int	ret;
	
	pstResponse = (tagCfgResponse *)buf;
	ret = Global_DataIO.GetCfg((char*)buf + sizeof(tagCfgResponse), datalen, pstResponse);
	pParam->IsComplete = 1;
	nLen = datalen + sizeof(tagCfgResponse);
	
	return ret;
}

int	Request::GetRequestData_ListInfo( tagRequestParam *pParam, void *buf, int & nLen )
{
	tagListInfoRequest * pstInfo;
	tagListInfoResponse * pstResponse;
	int	datalen = nLen - sizeof(tagListInfoResponse);
	int ret;

	pstInfo = (tagListInfoRequest*)pParam->InBuf;
	pstResponse = (tagListInfoResponse *)buf;

	pstResponse->nClassID = pstInfo->nClassID;
	pstResponse->nMarketID = pstInfo->nMarketID;
	pParam->IsComplete = 1;

	ret = Global_DataIO.GetListInfo(pstResponse, (char*)buf + sizeof(tagListInfoResponse), datalen);
	nLen = sizeof(tagListInfoResponse) + datalen;
	return ret;
}

int	Request::GetRequestData_iniList( tagRequestParam *pParam, void *buf, int & nLen )
{
	tagListRequst * pstRequest;
	tagListResponse	* pstResponse;
	int ret;
	int	datalen = nLen - sizeof(tagListResponse);
	pstRequest = (tagListRequst*)pParam->InBuf;
	pstResponse = (tagListResponse*)buf;
	pstResponse->nClassID = pstRequest->nClassID;
	pstResponse->nMarketID = pstRequest->nMarketID;
	pstResponse->nListCookie = pstRequest->nListCookie;
	pstResponse->nNextSerial = pParam->ApplyOffset;
	ret = Global_DataIO.GetInitFileList((char*)buf + sizeof(tagListResponse), datalen, pstResponse);

	pParam->ApplyOffset = pstResponse->nNextSerial;
	nLen = sizeof(tagListResponse) + datalen;

	if(pstResponse->nFlag < 0)
	{
		pParam->IsComplete = 1;
		return 1;
	}

	if(pstResponse->nFlag & FLAG_LAST_PACKET)
	{
		pParam->IsComplete = 1;
	}

	if((MAX_FRAME_SEND - 1) == pParam->nCurFrameNo)
	{
		pstResponse->nFlag |= FLAG_NEED_REQUEST;
	}

	return ret;
}


int	Request::GetRequestData_UpdateList( tagRequestParam *pParam, void *buf, int & nLen )
{
	tagListRequst * pstRequest;
	tagListResponse	* pstResponse;
	int ret;
	int	datalen = nLen - sizeof(tagListResponse);
	pstRequest = (tagListRequst*)pParam->InBuf;
	pstResponse = (tagListResponse*)buf;
	pstResponse->nClassID = pstRequest->nClassID;
	pstResponse->nMarketID = pstRequest->nMarketID;
	pstResponse->nListCookie = pstRequest->nListCookie;
	pstResponse->nNextSerial = pParam->ApplyOffset;
	ret = Global_DataIO.GetUpdataFileList((char*)buf + sizeof(tagListResponse), datalen, pstResponse);
	
	pParam->ApplyOffset = pstResponse->nNextSerial;
	nLen = sizeof(tagListResponse) + datalen;

	if(pstResponse->nFlag < 0)
	{
		pParam->IsComplete = 1;
		return 1;
	}

	if(pstResponse->nFlag & FLAG_LAST_PACKET)
	{
		pParam->IsComplete = 1;
	}

	if((MAX_FRAME_SEND - 1) == pParam->nCurFrameNo)
	{
		pstResponse->nFlag |= FLAG_NEED_REQUEST;
	}

	return ret;
}


int	Request::GetRequestData_File( tagRequestParam *pParam, void *buf, int & nLen )
{
	tagFileRequst * pstRequest;
	tagFileResponse	* pstResponse;
	MString			stFileName;
	int ret;
	int	datalen = nLen - sizeof(tagFileResponse);

	pstRequest = (tagFileRequst*)pParam->InBuf;
	stFileName = (char*)pParam->InBuf + sizeof(tagFileRequst);
	pstResponse = (tagFileResponse*)buf;

	pstResponse->nClassID = pstRequest->nClassID;
	pstResponse->nMarketID = pstRequest->nMarketID;
	pstResponse->nOffset = pParam->ApplyOffset;
	pstResponse->stOffsetTime = pParam->ApplyTimeOffset;
	pstResponse->nFileSzie = pstRequest->nFileSzie;
	pstResponse->stUpdateTime = pstRequest->stUpdateTime;
	pstResponse->nSerial = pstRequest->nSerial;
	ret = Global_DataIO.GetData((char*)buf + sizeof(tagFileResponse), datalen, pstResponse, stFileName);
	pParam->ApplyOffset = pstResponse->nNextOffset;
	pParam->ApplyTimeOffset = pstResponse->stNextDateTime;
	nLen = datalen + sizeof(tagFileResponse);

	if(pstResponse->nFlag < 0)
	{
		pParam->IsComplete = 1;
		return 1;
	}

	if(pstResponse->nFlag & FLAG_LAST_PACKET)
	{
		pParam->IsComplete = 1;
	}

	if((MAX_FRAME_SEND - 1) == pParam->nCurFrameNo)
	{
		pstResponse->nFlag |= FLAG_NEED_REQUEST;
	}

	return ret;
}

int	Request::GetRequestData_FileInfo( tagRequestParam *pParam, void *buf, int & nLen )
{
	tagInfoRequst * pstRequest;
	tagInfoResponse	* pstResponse;
	MString			stFileName;
	
	pstRequest = (tagInfoRequst*)pParam->InBuf;
	stFileName = (char*)pParam->InBuf + sizeof(tagInfoRequst);
	pstResponse = (tagInfoResponse*)buf;
	
	pstResponse->nClassID = pstRequest->nClassID;
	pstResponse->nMarketID = pstRequest->nMarketID;
	pstResponse->nSerial = pstRequest->nSerial;
	pstResponse->nFileSzie = pstRequest->nNeedHead;
	pParam->IsComplete = 1;
	nLen = sizeof(tagInfoResponse);
	return Global_DataIO.GetInfo(pstResponse, stFileName);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

