/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		filedata.h
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统接收机文件数据模块
	History:		
*********************************************************************************************************/
#ifndef __RTFILE_H__
#define __RTFILE_H__
#include "../common/commstruct.h"
#include "MEngine.hpp"
//#include "../common/cfgfile.h"
#include "dbf.h"

class FileList;

class RTFile
{
private:
	MThread				m_stProcessListThread;
	MThread				m_stProcessFileThreadM;
	MThread				m_stProcessFileThreadA;
	MCriticalSection	m_Section;

public://for CheckList()//单线程
	int					m_nLClassID;
	int					m_nLMarketID;

public://for CheckFileM()//单线程
	int					m_nFClassID;
	int					m_nFMarketID;
	int					m_nFSerial;
	int					m_nFClassID0;
	int					m_nFMarketID0;
	int					m_nFSerial0;

	unsigned short		m_nSendReqNumM2;
	unsigned short		m_nSendReqNumM1;

	char				m_NewDown;

public://for CheckFileA()//单线程
	int					m_nFAClassID;
	int					m_nFAMarketID;
	int					m_nFASerial;
	int					m_nFAClassID0;
	int					m_nFAMarketID0;
	int					m_nFASerial0;
	
	unsigned short		m_nSendReqNumA2;
	unsigned short		m_nSendReqNumA1;

public:
	FileList **		m_RecordData;
	unsigned short	m_nClassCount;
	unsigned short	m_nMarketCount;
	unsigned long	m_nUpFileCount;
	int				m_nCookie;
	char			m_AutoUpdateOn;

	int				m_nCommSerial;
	int				m_nFlag;
	MCounter		m_Counter;
	int				m_nNeedRequest;
	unsigned short	m_nMyRequestNo;


	int				m_nRecBufPercent;

public:
	RTFile();
	~RTFile();
	int Instance();
	void Release();
	int RecvCfg(char * buf, int nLen, tagCfgResponse * pstResponse);
	int RecvData(void * buf, int nLen, tagFileResponse *pstResponse, tagRecvInfo * pstRecvInfo);
	int RecvInfo(tagInfoResponse *pstResponse, tagRecvInfo * pstRecvInfo);
	int RecvUpdataFileList(char * buf, int nLen, tagListResponse * pstResponse, tagRecvInfo * pstRecvInfo);
	int RecvInitFileList(char * buf, int nLen, tagListResponse * pstResponse, tagRecvInfo * pstRecvInfo);
	int RecvListInfo(tagListInfoResponse * pstRequest, char* buf, int nLen, tagRecvInfo * pstRecvInfo);
	void UpdateRecBufPercent(int percent);
	int	OnConnect(int serial);

public:
	int	GetListCount(tagListCountRes * pRes);
	int	GetListInfo(tagListInfoRes * pRes);
	int	SetUpdate(tagSetUpdateRes * pRes);
	int	IsUpdateOn();
	unsigned long GetUpFileCount();
	int	ManualDown(tagDownDataRes * pstResponse);
	int ManualCancelDown(int nClassID, int  nMarketID, int nKindID);
	int	ManualDownInfo(tagDownInfoReq * pstRequest, tagDownInfoRes * pstResponse);
	int	ManualGetError(tagFailCodeReq * pstRequest, tagFailCodeRes * pstResponse, char * buf, int & buflen);

public:
	static int IsNewer(void * time1, void * time2);

private:
	FileList * _GetListPtr(int nClassID, int nMarketID);

private:
	int	CheckList();
	int	CheckFileM();
	int	CheckFileA();
	void ResetParamFM();
	void ResetParamFA();

protected:
	static void * __stdcall ProcessListThread(void * In);
	static void * __stdcall ProcessFileThreadM(void * In);
	static void * __stdcall ProcessFileThreadA(void * In);
};

#endif // __RTFILE_H__

