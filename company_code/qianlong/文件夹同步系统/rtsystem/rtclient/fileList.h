/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		filedata.h
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统接收机文件数据模块
	History:		
*********************************************************************************************************/
#ifndef __FILELIST_H__
#define __FILELIST_H__
#include "../common/commstruct.h"
#include "MEngine.hpp"
//#include "../common/cfgfile.h"
#include "dbf.h"

class	RTFile;
class	FileList							
{
private:
	MCriticalSection	m_Section;

public:
	RTFile		*		m_perent;
	RTFileData *		m_pFirstFile;
	RTFileData **		m_pstData;
	tagClassInfo		m_stCfg;
	MString				m_stDir;
	int					m_nFileNum;
	int					m_nAllocNum;
	int					m_nClassID;
	int					m_nMarketID;

	int					m_nFlag;			//状态标志：0未初始化，1得到过状态信息，2得到初始化列表，3得到更新列表，-1表示客户端配置文件强制配置为无效,-2表示服务器无配置该列表
	int					m_nInited;
	int					m_nNeedRequest;	
	int					m_nSerial;
	int					m_nSeriale;
	int					m_nFrameNo;	
	unsigned short		m_nRequestNo;	
	MCounter			m_stLastRecvTime;	
	RTFileData *		m_pstDownStatus;
	int					m_nStatusNum;
	int					m_CancelDown;
	int					m_nListCookie;
	int					m_nFileCookie;
	char				m_AutoUpdate;
	unsigned short		m_nFileCount;

						/**
						 *	新增了NeedList 判断,为了减少流量的处理.增加标志.应该可以使用程序其它
						 *	m_nFlag或者m_nInit等来处理的.由于这些标志用的太广泛,很多地方都在改变它
						 *	我一下也无法完全理清楚状态关系,为了安全起见,所以新增加标志来解决流量问题
						 *
						 *	它的基本含义是 
						 *		0 - 本文件列表还未发送请求.
						 *		1 - 本文件列表已经拿到服务器的返回
						 *
						 *				GUOGUO 2010-12-09
						 */
	int					m_nNeedFlag;	

public:
	FileList();
	~FileList();
	int		Instance(int nClassID, int nMarketID, RTFile * perent);
	void	Release();
	int		RecvData(void * buf, int nLen, tagFileResponse *pstRes, tagRecvInfo * pstRecvInfo);
	int		RecvInfo(tagInfoResponse *pstRes, tagRecvInfo * pstRecvInfo);
	int		RecvUpdataFileList(char * buf, int nLen, tagListResponse * pstResponse, tagRecvInfo * pstRecvInfo);
	int		RecvInitFileList(char * buf, int nLen, tagListResponse * pstResponse, tagRecvInfo * pstRecvInfo);
	int		RecvListInfo(tagListInfoResponse * pstRequest, char* buf, int nLen, tagRecvInfo * pstRecvInfo);
	int		CheckSatus();
	int		CheckaFileM(int &out);
	int		CheckaFileA(int &out);
	int		OnConnect(int ServerChange);

public:
	int		ReFleshFile();
	int		GetListInfo(tagListInfoRes * pResponse);
	int		SetUpdate(tagSetUpdateRes * pRes);
	int		ManualDown(tagDownDataRes * pstResponse);
	int		ManualCancelDown(int nClassID, int  nMarketID, int nKindID);
	int		ManualDownInfo(tagDownInfoReq * pstRequest, tagDownInfoRes * pstResponse);
	int		ManualGetError(tagFailCodeReq * pstRequest, tagFailCodeRes * pstResponse, char * buf, int & buflen);
};

#endif // __FILELIST_H__

