/****************************************************************************************************
		文件：MHttp.cpp
		功能：请求处理
		时间：2009.5.12
		作者：cuixu
		版权: 上海乾隆高科技版权所有

		修改纪录
		修改日期		修改人员	修改内容和原因
*****************************************************************************************************/
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include <memory.h>
#include <string.h>
#include <stdlib.h>
//////////////////////////////////////////////////////////////////////////
#include "./mhttp.h"

#define STR_RET				"\r\n"
#define STR_DBRET			"\r\n\r\n"
#define STR_GET				"GET"
#define STR_GET_CMD			"GET /?q=%d HTTP/1.1\r\n"	// 防止被CACHE
#define STR_CONNECT			"CONNECT"	
#define STR_CONNECT_CMD		"CONNECT %s:%d HTTP/1.1\r\n"	
#define STR_Accept			"Accept: */*\r\n"
#define	STR_AcceptEncoding	"Accept-Encoding: gzip, deflate\r\n"
#define STR_UserAgent		"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727)\r\n"
#define STR_Connection		"Connection: Keep-Alive\r\n"

#define STR_Host			"Host: www.qianlong.com.cn\r\n"
#define STR_ContentLength	"Content-Length: %d\r\n"
#define STR_ContentLengthStr	"Content-Length:"
#define STR_ContentLengthZero	"Content-Length: 0\r\n"

#define STR_HTTPOK			"HTTP/1.1 200 OK\r\n"
#define STR_Date			"Date: Thu, 26 Jul 2007 14:00:02 GMT\r\n"
#define STR_Server			"Server: Microsoft-IIS/6.0\r\n"
#define STR_Powered			"X-Powered-By: Green Channel Server v0.1\r\n"
#define STR_Cache			"Cache-Control: no-cache\r\n"
#define STR_ContentType		"Content-Type: application/octet-stream\r\n"
#define STR_ContentTypeHtml	"Content-Type: text/html; charset=gb2312\r\n" 
#define STR_SetCookie		"Set-Cookie: %s\r\n"


#define HEAD_Type		"GCType"
#define HEAD_SRVCHAN	"SRVCHAN"
#define HEAD_Channel	"GCCHNO"
#define HEAD_Unit		"GCUNNO"
#define HEAD_LNKNO      "GCLNKNO"
#define HEAD_Error		"GCErCD"

#define HEAD_MAXCHAN		"GCMACHCT"		// 最大通道数
#define HEAD_MAXCHANLINK	"GCMACHLK"		// 通道内最大连接数

bool MHttp::m_bIsClient = true;

int MHttp::Instance(bool bIsClient)
{
	MHttp::m_bIsClient = bIsClient;	
	return 1;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/********************************************************************************************************
	Function:		vir_JudgePacket
	Description:	判断数据包
	Protocol:
	  Input:			
	  Output:			
*********************************************************************************************************/
//下一步考虑,使用定长的HTTP包,提高效率.
int MHttp::vir_JudgePacket(char * buf, int size)
{
	buf[size]=0;
	char szTmp[255];
	char *pFd,*pRet,*pEnd;
	int nCtLen;
//	p = buf;

	memset(szTmp, 0 , 255);
	if( (pFd = strstr(buf,"Content-Length") ) )//找到
	{
	//	pFd+16;  //content-length开始位置
		if( (pRet = strstr(pFd+16,"\r\n")))
		{
			memcpy(szTmp,pFd+16, pRet-(pFd+16));//content-length值写入数组
			nCtLen = atoi(szTmp);
			if(nCtLen>=0)
			{
				if( pEnd = MHttp::StrStr(pRet,size-(pRet-buf),"\r\n\r\n") )
				{
					if( (pEnd + 4 + nCtLen + 4 - buf) > size ) // 数据准备好没有?
					{
						return 0;
					}
					if( memcmp(pEnd + 4 + nCtLen, "\r\n\r\n", 4) == 0 )
					{
						return (pEnd + 4 + nCtLen+ 4)-buf;
					}
					else
					{
						return -1;
					}
				}
			}
		}
	}
	else
	{
		//不包含头信息的情况
		if (pFd = strstr(buf,"\r\n\r\n"))
		{
			if (pRet = strstr(pFd,"\r\n\r\n"))
			{
				return(pRet+4-buf);
			}
			else
			{
				return 0;
			}
		}
		// 未收取完
	}	
	return 0;
}

/********************************************************************************************************
	Function:		AddHead
	Description:	添加头部信息
	Protocol:
	  Input:			
	  Output:			
*********************************************************************************************************/
int MHttp::AddHead(char* pOutBuf,int nSize,
			char* p_AddHead,  char* p_AddHeadData, char* pSpl = NULL)
{
	char            *pS = ": ";	
	strcat(pOutBuf, p_AddHead);
	strcat(pOutBuf, pSpl?pSpl:pS);
	strcat(pOutBuf, p_AddHeadData);
	strcat(pOutBuf, "\r\n");
	return strlen(pOutBuf);
}

/********************************************************************************************************
	Function:		SetData
	Description:	设置数据
	Protocol:
	  Input:			
	  Output:			
*********************************************************************************************************/
int MHttp::SetData(char* pOutBuf,int nSize,char* szQlData, int nQlDataSize)
{
	char            *pS = ": ";	
	int				nHeadEnd;
	char			szTmp[10];
	sprintf(szTmp,"%d",nQlDataSize);
	AddHead(pOutBuf, nSize, "Content-Length", szTmp);
	
	strcat(pOutBuf, "\r\n\r\n"); //头结束
	nHeadEnd = strlen(pOutBuf);
	memcpy(pOutBuf+nHeadEnd, szQlData, nQlDataSize);
	nHeadEnd += nQlDataSize;
	memcpy(pOutBuf+nHeadEnd , "\r\n\r\n", 4);
	pOutBuf[nHeadEnd+4] = 0;
	return nHeadEnd+4;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************************************************
	Function:		GetHead
	Description:	直接返回head 的内容
	Protocol:
	  Input:			
	  Output:			
*********************************************************************************************************/
int MHttp::GetHead(char* pBuf,int nSize,/*out*/ char* pHead, /*out*/ char* pRet, int nRetMaxLen)
{
	char            *pFd,*pEnd;	
	char			chr;
	int				nLen=0;
	pBuf[nSize]=0;	//结束
	if( pFd = strstr(pBuf,pHead ))
	{
		pFd+=strlen(pHead);
		while(1)
		{
			chr= *(++pFd);
			if( chr == ':' || chr == ' ' )
				continue;
			break;
		}
		if( pEnd = strstr(pFd, "\r\n") )
		{
			nLen = pEnd - pFd;
			if( nLen < nRetMaxLen )
			{
				memcpy(pRet, pFd, nLen);
				pRet[nLen]=0;
				return nLen;
			}
		}
	}
	return -1;
}

char* MHttp::StrStr(char* pBuf, int nSize, char* pSearch)
{
	int nPos = 0;
	int nStrLen = strlen(pSearch);
	while(nPos <= (nSize - nStrLen) ) 
	{
		if( memcmp(pBuf+nPos, pSearch, nStrLen) == 0 )
		{
			return pBuf+nPos;
		}
		nPos++;
	}
	return NULL;
}

/********************************************************************************************************
	Function:		GetHead
	Description:	确定HTTP实体数据的起始位置和长度
	Protocol:
	  Input:			
	  Output:			
*********************************************************************************************************/
int MHttp::GetDataSize(char* pBuf,int nSize, int nLen, int &nStart, int &nRetSize)
{
	char            *pFd,*pEnd;
	pBuf[nSize]=0;	//结束
	if( nLen == 0 )
	{
		if( pFd = strstr(pBuf,"\r\n\r\n") )
		{
			pFd+=4;
			nStart = pFd-pBuf;
			if( pEnd = MHttp::StrStr(pFd, nSize-(pFd-pBuf), "\r\n\r\n") )
			{
				nRetSize = pEnd-pFd;
				return nRetSize;
			}
		}		
		return 0;
	}
	//nLen >0
	if( pFd = strstr(pBuf,"\r\n\r\n") )
	{
		pFd+=4;
		nStart = pFd-pBuf;
		nRetSize = nSize-(pFd-pBuf)-4;
		if( nLen == nRetSize ) 
		{
			if(strstr( pFd + nLen, "\r\n\r\n") == (pFd + nLen) )
			{
				return nLen;
			}
			else
			{
				return -1;
			}
		}		
	}
	return -1;
}

/********************************************************************************************************
	Function:		GetType
	Description:	获取协议类型
	Protocol:
	  Input:			
	  Output:			
*********************************************************************************************************/
int MHttp::GetType(char* pBuf,int nSize)
{
	char	szTmp[20];
	if( MHttp::GetHead(pBuf,nSize, HEAD_Type, szTmp, 20) < 0 )
	{
		return -1;
	}
	return atoi(szTmp);
}

#define HEAD_MAXCHAN		"GCMACHCT"		// 最大通道数
#define HEAD_MAXCHANLINK	"GCMACHLK"		// 通道内最大连接数

int MHttp::BuildOption(char* pOutBuf,int nOutBufLen, unsigned short usMaxChannleCount, unsigned short usMaxChannleLink,int nErrorCode)
{
	char	szBuf[100]={0};
	pOutBuf[0]=0;
	
	if( MHttp::m_bIsClient )// 组件请求包
	{
		sprintf(szBuf,STR_GET_CMD,rand());			//GET /?q=随机 HTTP/1.1
		strcat(pOutBuf, szBuf);
		//		strcat(pOutBuf, STR_Accept);			//Accept: **
		//		strcat(pOutBuf, STR_AcceptEncoding);	//Accept-Encoding: gzip, deflate\r\n";
		//		strcat(pOutBuf, STR_UserAgent);			//User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727)
		//		strcat(pOutBuf, STR_Host);				//Host: www.qianlong.com.cn
	}
	else
	{
		strcat(pOutBuf, STR_HTTPOK);			//HTTP/1.1 200 OK		
		//strcat(pOutBuf, STR_Date);		// Date: Thu, 26 Jul 2007 14:00:02 GMT
		//		strcat(pOutBuf, STR_Server);	
		//		strcat(pOutBuf, STR_Powered);
		//		strcat(pOutBuf, STR_Cache);
		//		strcat(pOutBuf, STR_ContentType);	
	}
	sprintf(szBuf,"%s: %d\r\n", HEAD_Type, MHttp::TP_OPTION);
	strcat(pOutBuf, szBuf);
	sprintf(szBuf,"%s: %d\r\n", HEAD_MAXCHAN, usMaxChannleCount);
	strcat(pOutBuf, szBuf);
	sprintf(szBuf,"%s: %d\r\n", HEAD_MAXCHANLINK, usMaxChannleLink);
	strcat(pOutBuf, szBuf);
	sprintf(szBuf,"%s: %d\r\n", HEAD_Error, nErrorCode);
	strcat(pOutBuf, szBuf);	
	
	strcat(pOutBuf, STR_Connection);		//Connection: Keep-Alive	
	strcat(pOutBuf, STR_ContentLengthZero);
	strcat(pOutBuf, STR_RET);
	strcat(pOutBuf, STR_DBRET);
	return strlen(pOutBuf);
}
int MHttp::GetOption(char* pInBuf,int nInBufSize, unsigned short &usMaxChannleCount, unsigned short &usMaxChannleLink,int &nErrorCode)
{
	unsigned char  ucType;
	char			szTmp[30];
	
	if( MHttp::GetHead(pInBuf, nInBufSize, HEAD_Type, szTmp, 30) < 0 )
	{
		return -2;
	}
	ucType = atoi(szTmp);
	if( ucType != MHttp::TP_OPTION)
	{
		return -3;
	}
	if( MHttp::GetHead(pInBuf, nInBufSize, HEAD_MAXCHAN, szTmp, 30) < 0 )
	{
		return -4;
	}
	usMaxChannleCount = atol(szTmp);
	if( MHttp::GetHead(pInBuf, nInBufSize, HEAD_MAXCHANLINK, szTmp, 30) < 0 )
	{
		return -5;
	}
	usMaxChannleLink = atol(szTmp);
	if( MHttp::GetHead(pInBuf, nInBufSize, HEAD_Error, szTmp, 30) < 0 )
	{
		return -7;
	}
	nErrorCode = atoi(szTmp);	
	return 1;
}


/********************************************************************************************************
	Function:		BuildChannel
	Description:	建立通道连接信息
	Protocol:
	  Input:
		参数 PC,CHANNEL,UNIT , 向上的目标IP,错误号 :如 资源满, 向上连接错误,	usSrvChanSerial 为之前使用的服务那边的通道号
	  Output:			
*********************************************************************************************************/

int MHttp::BuildChannel(char* pOutBuf,int nOutBufLen, short usSrvChanSerial, unsigned short usChannelSerial, int nErrorCode)
{
	char	szBuf[100]={0};
	pOutBuf[0]=0;
	
	if( MHttp::m_bIsClient )// 组件请求包
	{
		sprintf(szBuf,STR_GET_CMD,rand());			//GET /?q=随机 HTTP/1.1
		strcat(pOutBuf, szBuf);
//		strcat(pOutBuf, STR_Accept);			//Accept: **
//		strcat(pOutBuf, STR_AcceptEncoding);	//Accept-Encoding: gzip, deflate\r\n";
//		strcat(pOutBuf, STR_UserAgent);			//User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727)
//		strcat(pOutBuf, STR_Host);				//Host: www.qianlong.com.cn
	}
	else
	{
		strcat(pOutBuf, STR_HTTPOK);			//HTTP/1.1 200 OK		
		//strcat(pOutBuf, STR_Date);		// Date: Thu, 26 Jul 2007 14:00:02 GMT
//		strcat(pOutBuf, STR_Server);	
//		strcat(pOutBuf, STR_Powered);
//		strcat(pOutBuf, STR_Cache);
//		strcat(pOutBuf, STR_ContentType);	
	}
	sprintf(szBuf,"%s: %d\r\n", HEAD_Type, MHttp::TP_CHANNEL);
	strcat(pOutBuf, szBuf);
	sprintf(szBuf,"%s: %d\r\n", HEAD_SRVCHAN, usSrvChanSerial);
	strcat(pOutBuf, szBuf);
	sprintf(szBuf,"%s: %d\r\n", HEAD_Channel, usChannelSerial);
	strcat(pOutBuf, szBuf);
	sprintf(szBuf,"%s: %d\r\n", HEAD_Error, nErrorCode);
	strcat(pOutBuf, szBuf);	

	strcat(pOutBuf, STR_Connection);		//Connection: Keep-Alive	
	strcat(pOutBuf, STR_ContentLengthZero);
	strcat(pOutBuf, STR_RET);
	strcat(pOutBuf, STR_DBRET);
	return strlen(pOutBuf);
}


/********************************************************************************************************
	Function:		GetChannel
	Description:	获取单元连接信息
	Protocol:
	  Input:			
	  Output:			
*********************************************************************************************************/
int MHttp::GetChannel(char* pInBuf,int nInBufSize, short &usSrvChanSerial, unsigned short &usChannelSerial, int &nErrorCode)
{
	unsigned char  ucType;
	char			szTmp[30];

	if( MHttp::GetHead(pInBuf, nInBufSize, HEAD_Type, szTmp, 30) < 0 )
	{
		return -2;
	}
	ucType = atoi(szTmp);
	if( ucType != MHttp::TP_CHANNEL)
	{
		return -3;
	}
	if( MHttp::GetHead(pInBuf, nInBufSize, HEAD_SRVCHAN, szTmp, 30) < 0 )
	{
		return -4;
	}
	usSrvChanSerial = atoi(szTmp);
	if( MHttp::GetHead(pInBuf, nInBufSize, HEAD_Channel, szTmp, 30) < 0 )
	{
		return -5;
	}
	usChannelSerial = atoi(szTmp);
	if( MHttp::GetHead(pInBuf, nInBufSize, HEAD_Error, szTmp, 30) < 0 )
	{
		return -7;
	}
	nErrorCode = atoi(szTmp);	
	return 1;
}


/********************************************************************************************************
	Function:		BuildUnit
	Description:	新建单元连接
	Protocol:
	  Input:			
				usUnitSerial 单元号 , 向上的目标IP,错误号 :如 资源满, 向上连接错误, nErrorCode = 0 成功
	  Output:			
*********************************************************************************************************/
int MHttp::BuildUnit(char* pOutBuf,int nOutBufLen, unsigned short usUnitSerial, unsigned long ulLinkNo, const char* szIP, unsigned short usPort, int nErrorCode)
{
	char	szBuf[100]={0};
	pOutBuf[0]=0;
	
//	if( MHttp::m_bIsClient )// 组件请求包
//	{
//		sprintf(szBuf,STR_CONNECT_CMD, szIP, usPort);			//GET /?q=随机 HTTP/1.1
//		strcat(pOutBuf, szBuf);		
//		strcat(pOutBuf, STR_Accept);			//Accept: **
//		strcat(pOutBuf, STR_AcceptEncoding);	//Accept-Encoding: gzip, deflate\r\n";
//		strcat(pOutBuf, STR_UserAgent);			//User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727)
//		strcat(pOutBuf, STR_Host);				//Host: www.qianlong.com.cn
//	}
//	else
	{
		strcat(pOutBuf, STR_HTTPOK);			//HTTP/1.1 200 OK		
		//strcat(pOutBuf, STR_Date);		// Date: Thu, 26 Jul 2007 14:00:02 GMT
//		strcat(pOutBuf, STR_Server);	
//		strcat(pOutBuf, STR_Powered);
//		strcat(pOutBuf, STR_Cache);
//		strcat(pOutBuf, STR_ContentType);	
	}
	sprintf(szBuf,"%s: %d\r\n", HEAD_Type, MHttp::TP_UNIT);
	strcat(pOutBuf, szBuf);
	sprintf(szBuf,"%s: %d\r\n", HEAD_Unit, usUnitSerial);
	strcat(pOutBuf, szBuf);
	sprintf(szBuf,"%s: %d\r\n", HEAD_LNKNO, ulLinkNo);
	strcat(pOutBuf, szBuf);
	sprintf(szBuf,"%s: %d\r\n", HEAD_Error, nErrorCode);
	strcat(pOutBuf, szBuf);	

	strcat(pOutBuf, STR_Connection);		//Connection: Keep-Alive	
	strcat(pOutBuf, STR_ContentLengthZero);
	strcat(pOutBuf, STR_RET);
	strcat(pOutBuf, STR_DBRET);
	return strlen(pOutBuf);
}

//tangj 支持委托代理,新增协议
int MHttp::BuildTradeUnit(char* pOutBuf,int nOutBufLen, unsigned short usUnitSerial, unsigned long ulLinkNo, const char* szIP, unsigned short usPort, int nErrorCode)
{
	char	szBuf[100]={0};
	pOutBuf[0]=0;
	
	if( MHttp::m_bIsClient )// 组件请求包
	{
		sprintf(szBuf,STR_CONNECT_CMD, szIP, usPort);			//GET /?q=随机 HTTP/1.1
		strcat(pOutBuf, szBuf);		
		//		strcat(pOutBuf, STR_Accept);			//Accept: **
		//		strcat(pOutBuf, STR_AcceptEncoding);	//Accept-Encoding: gzip, deflate\r\n";
		//		strcat(pOutBuf, STR_UserAgent);			//User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727)
		//		strcat(pOutBuf, STR_Host);				//Host: www.qianlong.com.cn
	}
	else
	{
		strcat(pOutBuf, STR_HTTPOK);			//HTTP/1.1 200 OK		
		//strcat(pOutBuf, STR_Date);		// Date: Thu, 26 Jul 2007 14:00:02 GMT
		//		strcat(pOutBuf, STR_Server);	
		//		strcat(pOutBuf, STR_Powered);
		//		strcat(pOutBuf, STR_Cache);
		//		strcat(pOutBuf, STR_ContentType);	
	}
	sprintf(szBuf,"%s: %d\r\n", HEAD_Type, MHttp::TP_TRADEUNIT);
	strcat(pOutBuf, szBuf);
	sprintf(szBuf,"%s: %d\r\n", HEAD_Unit, usUnitSerial);
	strcat(pOutBuf, szBuf);
	sprintf(szBuf,"%s: %d\r\n", HEAD_LNKNO, ulLinkNo);
	strcat(pOutBuf, szBuf);
	sprintf(szBuf,"%s: %d\r\n", HEAD_Error, nErrorCode);
	strcat(pOutBuf, szBuf);	
	
	strcat(pOutBuf, STR_Connection);		//Connection: Keep-Alive	
	strcat(pOutBuf, STR_ContentLengthZero);
	strcat(pOutBuf, STR_RET);
	strcat(pOutBuf, STR_DBRET);
	return strlen(pOutBuf);
}

//tangj 支持委托代理,新增协议
int MHttp::GetTradeUnit(char* pInBuf,int nInBufSize, unsigned short &usUnitSerial, unsigned long &ulLinkNo, char* szIP, unsigned short &usPort, int &nErrorCode)
{
	unsigned char  ucType;
	char			szTmp[255];
	int				nLen=0;	
	char			szPort[20];
	char*			pFd;
	
	pInBuf[nInBufSize]=0;
	
// 	if( MHttp::m_bIsClient )
// 	{
// 		szIP[0]	=0;
// 		usPort	= 0;
// 	}
// 	else
	{
		if( MHttp::GetHead(pInBuf, nInBufSize, STR_CONNECT, szTmp, sizeof(szTmp)) < 0 )
		{
			return -1;
		}
		if( (pFd = strstr(szTmp,":"))!=NULL && pFd > szTmp)
		{
			nLen = pFd-szTmp;
			memcpy(szIP, szTmp, nLen);
			szIP[nLen] = 0;
			pFd++; nLen++;
			nLen = strlen(szTmp) - nLen;
			memcpy(szPort, pFd, nLen);
			szPort[nLen]=0;
			usPort = atoi(szPort);
		}
	}
	
	if( MHttp::GetHead(pInBuf, nInBufSize, HEAD_Type, szTmp, 30) < 0 )
	{
		return -2;
	}
	ucType = atoi(szTmp);
	if( ucType != MHttp::TP_TRADEUNIT)
	{
		return -3;
	}
	if( MHttp::GetHead(pInBuf, nInBufSize, HEAD_Unit, szTmp, 30) < 0 )
	{
		return -4;
	}
	usUnitSerial = atoi(szTmp);
	if( MHttp::GetHead(pInBuf, nInBufSize, HEAD_LNKNO, szTmp, 30) < 0 )
	{
		return -4;
	}
	ulLinkNo = atoi(szTmp);
	if( MHttp::GetHead(pInBuf, nInBufSize, HEAD_Error, szTmp, 30) < 0 )
	{
		return -7;
	}
	nErrorCode = atoi(szTmp);	
	return 1;
}

/********************************************************************************************************
	Function:		GetUnit
	Description:	获取单元连接信息
	Protocol:
	  Input:			nErrorCode > 0 成功
	  Output:			
*********************************************************************************************************/
int MHttp::GetUnit(char* pInBuf,int nInBufSize, unsigned short &usUnitSerial, unsigned long &ulLinkNo, char* szIP, unsigned short &usPort, int &nErrorCode)
{
	unsigned char  ucType;
	char			szTmp[255];
	int				nLen=0;	
	char			szPort[20];
	char*			pFd;

	pInBuf[nInBufSize]=0;
// 
// 	if( MHttp::m_bIsClient )
// 	{
// 		szIP[0]	=0;
// 		usPort	= 0;
// 	}
// 	else
// 	{
		if( MHttp::GetHead(pInBuf, nInBufSize, STR_CONNECT, szTmp, sizeof(szTmp)) < 0 )
		{
			return -1;
		}
		if( (pFd = strstr(szTmp,":"))!=NULL && pFd > szTmp)
		{
			nLen = pFd-szTmp;
			memcpy(szIP, szTmp, nLen);
			szIP[nLen] = 0;
			pFd++; nLen++;
			nLen = strlen(szTmp) - nLen;
			memcpy(szPort, pFd, nLen);
			szPort[nLen]=0;
			usPort = atoi(szPort);
		}
//	}

	if( MHttp::GetHead(pInBuf, nInBufSize, HEAD_Type, szTmp, 30) < 0 )
	{
		return -2;
	}
	ucType = atoi(szTmp);
	if( ucType != MHttp::TP_UNIT)
	{
		return -3;
	}
	if( MHttp::GetHead(pInBuf, nInBufSize, HEAD_Unit, szTmp, 30) < 0 )
	{
		return -4;
	}
	usUnitSerial = atoi(szTmp);
	if( MHttp::GetHead(pInBuf, nInBufSize, HEAD_LNKNO, szTmp, 30) < 0 )
	{
		return -4;
	}
	ulLinkNo = atoi(szTmp);
	if( MHttp::GetHead(pInBuf, nInBufSize, HEAD_Error, szTmp, 30) < 0 )
	{
		return -7;
	}
	nErrorCode = atoi(szTmp);	
	return 1;
}


/********************************************************************************************************
	Function:		BuildHeart
	Description:	发送心跳包和返回
	Protocol:
	  Input:			
	  Output:		返回实际数据长度	
*********************************************************************************************************/
int MHttp::BuildHeart(char* pOutBuf,int nOutBufLen)
{
	char	szBuf[100]={0};
	pOutBuf[0]=0;

	if( MHttp::m_bIsClient )// 组件请求包
	{
		sprintf(szBuf,STR_GET_CMD,rand());			//GET /?q=随机 HTTP/1.1
		strcat(pOutBuf, szBuf);
		sprintf(szBuf,"%s: %d\r\n", HEAD_Type, MHttp::TP_HEART);
		strcat(pOutBuf, szBuf);
//		strcat(pOutBuf, STR_Accept);			//Accept: **
//		strcat(pOutBuf, STR_AcceptEncoding);	//Accept-Encoding: gzip, deflate\r\n";
//		strcat(pOutBuf, STR_UserAgent);			//User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727)
//		strcat(pOutBuf, STR_Host);				//Host: www.qianlong.com.cn
	}
	else
	{
		strcat(pOutBuf, STR_HTTPOK);			//HTTP/1.1 200 OK
		sprintf(szBuf,"%s: %d\r\n", HEAD_Type, MHttp::TP_HEART);
		strcat(pOutBuf, szBuf);
		//strcat(pOutBuf, STR_Date);				// Date: Thu, 26 Jul 2007 14:00:02 GMT
//		strcat(pOutBuf, STR_Server);	
//		strcat(pOutBuf, STR_Powered);
//		strcat(pOutBuf, STR_Cache);
//		strcat(pOutBuf, STR_ContentType);		
	}
	strcat(pOutBuf, STR_Connection);		//Connection: Keep-Alive
	strcat(pOutBuf, STR_ContentLengthZero);
	strcat(pOutBuf, STR_RET);
	strcat(pOutBuf, STR_DBRET);
	return strlen(pOutBuf);		
}

/********************************************************************************************************
	Function:		BuildData
	Description:	数据发送
	Protocol:
	  Input:			
	  Output:		返回实际数据长度	
*********************************************************************************************************/
int MHttp::BuildData(char* pOutBuf,int nOutBufLen, unsigned short usUnitSerial, unsigned long ulLinkNo, char* pInBuf, int pInSize)
{
	char	szBuf[100]={0};
	int		pOutBufEnd;
	pOutBuf[0]=0;
	
	strcat(pOutBuf, STR_HTTPOK);			//HTTP/1.1 200 OK		
	sprintf(szBuf,"%s: %d\r\n", HEAD_Type, MHttp::TP_DATA);
	strcat(pOutBuf, szBuf);
	sprintf(szBuf,"%s: %d\r\n", HEAD_Unit, usUnitSerial);
	strcat(pOutBuf, szBuf);
	sprintf(szBuf,"%s: %d\r\n", HEAD_LNKNO, ulLinkNo);
	strcat(pOutBuf, szBuf);
//	strcat(pOutBuf, STR_Accept);			//Accept: **
//	strcat(pOutBuf, STR_AcceptEncoding);	//Accept-Encoding: gzip, deflate\r\n";
//	strcat(pOutBuf, STR_UserAgent);			//User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727)
//	strcat(pOutBuf, STR_Host);				//Host: www.qianlong.com.cn
	strcat(pOutBuf, STR_Connection);		//Connection: Keep-Alive
	sprintf(szBuf,STR_ContentLength,pInSize);	//将乾隆数据长度传入Content-Length
	strcat(pOutBuf, szBuf);
	strcat(pOutBuf, STR_RET);				    //双回车开始
	pOutBufEnd = strlen(pOutBuf);
    memcpy (pOutBuf+pOutBufEnd,pInBuf, pInSize);//跟乾隆数据包
	pOutBufEnd += pInSize;						//pOutBufEnd的偏移值为总长度
	memcpy (pOutBuf+pOutBufEnd,STR_DBRET, strlen(STR_DBRET));     //双回车结束
	return pOutBufEnd + strlen(STR_DBRET);
				
}

/********************************************************************************************************
	Function:		GetData
	Description:	数据接收
	Protocol:
	  Input:			
	  Output:		返回实际数据长度	
*********************************************************************************************************/
int MHttp:: GetData(char* pInBuf,int nInBufSize, unsigned short &usUnitSerial, unsigned long &ulLinkNo,int &nDataStart, int &nDataSize)
{
	char			szTmp[30];
	
	int             szLength = 0;
	int				nStart = 0;
	int             nRetSize = 0;
    unsigned char   ucType;
	pInBuf[nInBufSize]=0;

	if( MHttp::GetHead(pInBuf, nInBufSize, HEAD_Type, szTmp, 30) < 0 )
	{
		return -2;
	}

	ucType = atoi(szTmp);
	if( ucType != MHttp::TP_DATA)    //判断数据类型
	{
		return -3;
	}

	if( MHttp::GetHead(pInBuf, nInBufSize, HEAD_Unit, szTmp, 30) < 0 )
	{
		return -4;
	}
	usUnitSerial = atoi(szTmp);

	if( MHttp::GetHead(pInBuf, nInBufSize, HEAD_LNKNO, szTmp, 30) < 0 )
	{
		return -6;
	}
	ulLinkNo = atoi(szTmp);
	
	if( MHttp::GetHead(pInBuf, nInBufSize, STR_ContentLengthStr, szTmp, 30) > 0 )
	{
		szLength = atoi(szTmp);
	}

	MHttp::GetDataSize(pInBuf,nInBufSize,szLength, nStart, nRetSize);
	nDataStart = nStart;
	nDataSize = nRetSize;	
	return 1;	

}

// 3. 双向断开 
/********************************************************************************************************
	Function:		BuildClose
	Description:	双向断开
	Protocol:
	  Input:			
	  Output:		返回实际数据长度	
*********************************************************************************************************/
int MHttp::BuildClose(char* pOutBuf,int nOutBufLen, unsigned short usUnitSerial, unsigned long ulLinkNo)
{
	char	szBuf[100]={0};
	pOutBuf[0]=0;
	
	if( MHttp::m_bIsClient )// 组件请求包
	{
		sprintf(szBuf,STR_GET_CMD,rand());			//GET /?q=随机 HTTP/1.1
		strcat(pOutBuf, szBuf);
//		strcat(pOutBuf, STR_Accept);			//Accept: **
//		strcat(pOutBuf, STR_AcceptEncoding);	//Accept-Encoding: gzip, deflate\r\n";
//		strcat(pOutBuf, STR_UserAgent);			//User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727)
//		strcat(pOutBuf, STR_Host);				//Host: www.qianlong.com.cn
		
	}
	else
	{
		strcat(pOutBuf, STR_HTTPOK);			//HTTP/1.1 200 OK
		//		strcat(pOutBuf, STR_Date);				// Date: Thu, 26 Jul 2007 14:00:02 GMT
//		strcat(pOutBuf, STR_Server);	
//		strcat(pOutBuf, STR_Powered);
//		strcat(pOutBuf, STR_Cache);
//		strcat(pOutBuf, STR_ContentType);	
			
	}
	
	sprintf(szBuf,"%s: %d\r\n", HEAD_Type, MHttp::TP_CLOSE);
	strcat(pOutBuf, szBuf);
	sprintf(szBuf,"%s: %d\r\n", HEAD_Unit, usUnitSerial);
	strcat(pOutBuf, szBuf);
	sprintf(szBuf,"%s: %d\r\n", HEAD_LNKNO, ulLinkNo);
	strcat(pOutBuf, szBuf);
	strcat(pOutBuf, STR_Connection);		//Connection: Keep-Alive
	strcat(pOutBuf, STR_ContentLengthZero);
	strcat(pOutBuf, STR_RET);
	strcat(pOutBuf, STR_DBRET);
	return strlen(pOutBuf);		
}

/********************************************************************************************************
	Function:		GetClose
	Description:	双向断开
	Protocol:
	  Input:			
	  Output:		返回实际数据长度	
*********************************************************************************************************/
int MHttp::GetClose(char* pInBuf,int nInBufSize, unsigned short &usUnitSerial, unsigned long &ulLinkNo)
{
	unsigned char  ucType;
	char			szTmp[30];
	pInBuf[nInBufSize]=0;

	if( MHttp::GetHead(pInBuf, nInBufSize, HEAD_Type, szTmp, 30) < 0 )
	{
		return -2;
	}
	ucType = atoi(szTmp);
	if( ucType != MHttp::TP_CLOSE)
	{
		return -3;
	}
	if( MHttp::GetHead(pInBuf, nInBufSize, HEAD_Unit, szTmp, 30) < 0 )
	{
		return -5;
	}
	usUnitSerial = atoi(szTmp);	
	if( MHttp::GetHead(pInBuf, nInBufSize, HEAD_LNKNO, szTmp, 30) < 0 )
	{
		return -5;
	}
	ulLinkNo = atoi(szTmp);	
	return 1;
}

int MHttp::BuildHelloWorld(char* pOutBuf,int nOutBufLen)
{
	char	szBuf[100]={0};
	pOutBuf[0]=0;
	
	if( MHttp::m_bIsClient )// 组件请求包
	{
		sprintf(szBuf,STR_GET_CMD,rand());			//GET /?q=随机 HTTP/1.1
		strcat(pOutBuf, szBuf);
		strcat(pOutBuf, STR_Accept);			//Accept: **
		strcat(pOutBuf, STR_AcceptEncoding);	//Accept-Encoding: gzip, deflate\r\n";
		strcat(pOutBuf, STR_UserAgent);			//User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727)
		strcat(pOutBuf, STR_Host);				//Host: www.qianlong.com.cn
	}
	else
	{
		strcat(pOutBuf, STR_HTTPOK);			//HTTP/1.1 200 OK
		//		strcat(pOutBuf, STR_Date);				// Date: Thu, 26 Jul 2007 14:00:02 GMT
		strcat(pOutBuf, STR_Server);	
		strcat(pOutBuf, STR_Powered);
		strcat(pOutBuf, STR_Cache);
		strcat(pOutBuf, STR_ContentTypeHtml);		
	}
	strcat(pOutBuf, STR_RET);
	strcat(pOutBuf, "<h1>Hello World!</h1><a href=http://www.qianlong.com.cn>乾隆绿色通道</a>");
	strcat(pOutBuf, STR_DBRET);
	return strlen(pOutBuf);	
}
