/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		filedata.cpp
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统接收机文件数据模块
	History:		
*********************************************************************************************************/
#include "filedata.h"
#include "Global.h"

RTFile::RTFile()
{
	m_RecordData = NULL;
	m_nMarketCount = 0;
	m_nClassCount = 0;
	m_nCommSerial = -1;
	m_nUpFileCount = 0;
	m_nFlag = -1;
	m_NewDown = 0;
	m_AutoUpdateOn = 1;
}

RTFile::~RTFile()
{
	Release();
}

int RTFile::Instance()
{
	MLocalSection		local;
	int			ret;
	MFile		afile;
 
	Release();

	local.Attch(&m_Section);

	m_nFlag = 0;
	m_nNeedRequest = 1;

	m_nRecBufPercent = 0;

	m_nLClassID = 0;
	m_nLMarketID = -1;

	if(MFile::IsExist("./LONKIND.CFG") == 1)
	{
		Global_LongKind.UpdateData("./LONKIND.CFG");
	}

	if( ( ret = m_stProcessListThread.StartThread( "列表处理线程", ProcessListThread,this ) ) != 1 )
	{
		slib_WriteError( Global_UnitNo, 0, "[数据模块]:列表处理线程初始化失败" );
		Release();	
		return -1;
	}
	
	ResetParamFM();
	if( ( ret = m_stProcessFileThreadM.StartThread( "手动文件处理线程", ProcessFileThreadM,this ) ) != 1 )
	{
		slib_WriteError( Global_UnitNo, 0, "[数据模块]:文件处理线程初始化失败" );
		Release();	
		return -1;
	}

	ResetParamFA();
	if( ( ret = m_stProcessFileThreadA.StartThread( "自动文件处理线程", ProcessFileThreadA,this ) ) != 1 )
	{
		slib_WriteError( Global_UnitNo, 0, "[数据模块]:文件处理线程初始化失败" );
		Release();	
		return -1;
	}
	return 1;
}


void RTFile::Release()
{
	MLocalSection		local;
	local.Attch(&m_Section);

	int		i;

	m_stProcessListThread.StopThread();
	m_stProcessFileThreadM.StopThread();
	m_stProcessFileThreadA.StopThread();

	if(m_RecordData)
	{
		for(i = 0; i < m_nClassCount; i++)
		{
			if(m_RecordData[i])
				delete []m_RecordData[i];
		}
		delete [] m_RecordData;
		m_RecordData = NULL;
	}

	m_nMarketCount = 0;
	m_nClassCount = 0;
	m_nCommSerial = -1;
	m_nUpFileCount = 0;
	m_nFlag = -1;
	m_NewDown = 0;
	m_AutoUpdateOn = 1;
}

int RTFile::RecvCfg(char * buf, int nLen, tagCfgResponse * pstResponse)
{
	MString			stemp;
	int				i, j;

	tagListFlag	*	pFlag = (tagListFlag*)buf;

	MLocalSection	local;

	local.Attch(&m_Section);

	if(m_nNeedRequest)
		return -1;

	if((m_nFlag == 1) || (m_nFlag < 0))
		return 1;

	if(nLen != sizeof(tagListFlag) * pstResponse->nClassCount * (pstResponse->nMarketCount + 1))
		return -1;

	if(m_nFlag == 2)//连接断开后收到的配置信息，需要比较与原来的服务器是否兼容
	{
		if((m_nClassCount != pstResponse->nClassCount)||
			(m_nMarketCount != pstResponse->nMarketCount))
		{
			printf("RTFile ReInstance\n");
			printf("class count:(%d, %d), market count:(%d, %d)\n", m_nClassCount, pstResponse->nClassCount, m_nMarketCount, pstResponse->nMarketCount);
			local.UnAttch();
			Instance();
			return 1;
		}
	}

	m_nClassCount = pstResponse->nClassCount;
	m_nMarketCount = pstResponse->nMarketCount;

	if(m_nFlag == 0)
	{
		if(!m_RecordData)
		{
			m_RecordData = new FileList*[m_nClassCount];
			if(!m_RecordData)
			{
				return -1;
			}
			memset(m_RecordData, 0, sizeof(FileList*) * m_nClassCount);
		}
			
		for(i = 0; i < m_nClassCount; i++)
		{
			if(!m_RecordData[i])
				m_RecordData[i] = new FileList[m_nMarketCount + 1];
			if(!m_RecordData[i])
			{
				return -1;
			}
		}
	}

	/**
	 *	如果是重新连接的情况,把所有的都释放一下,其实在这里处理并不是最完美的
	 *	但是由于原来代码的逻辑上的问题,综合考究在这里,相对最简单,也最安全
	 *										GUOGUO 20110125
	 */
	if(m_nFlag == 2)
	{
		for(i = 0; i < m_nClassCount; i++)
		{
			for(j = 0; j < m_nMarketCount + 1; j++)
			{
				m_RecordData[i][j].Release();
			}
		}
	}


	for(i = 0; i < m_nClassCount; i++)
	{
		for(j = 0; j < m_nMarketCount + 1; j++)
		{
			if((pFlag[i * (m_nMarketCount + 1) + j].cListFlag >=0) &&
				(m_RecordData[i][j].m_nFlag < 0))
			{
				if(j == m_nMarketCount)
					m_RecordData[i][j].Instance(i, -1, this);
				else
					m_RecordData[i][j].Instance(i, j, this);
			}

			if((pFlag[i * (m_nMarketCount + 1) + j].cListFlag < 0) &&
				(m_RecordData[i][j].m_nFlag >= 0))
			{
				m_RecordData[i][j].Release();
			}
		}
	}

	m_nFlag = 1;

	return 1;
}

int RTFile::RecvData(void * buf, int nLen, tagFileResponse *pstResponse, tagRecvInfo * pstRecvInfo)
{
	MLocalSection	local;
	FileList		*pstList;
	
	local.Attch(&m_Section);
	pstList = _GetListPtr(pstResponse->nClassID, pstResponse->nMarketID);
	local.UnAttch();

	if(!pstList)
		return -1;
	
	return pstList->RecvData(buf, nLen, pstResponse, pstRecvInfo);
}

int RTFile::RecvInfo(tagInfoResponse *pstResponse, tagRecvInfo * pstRecvInfo)
{
	MLocalSection	local;
	FileList		*pstList;
	
	local.Attch(&m_Section);
	pstList = _GetListPtr(pstResponse->nClassID, pstResponse->nMarketID);
	local.UnAttch();
	
	if(!pstList)
		return -1;
	
	return pstList->RecvInfo(pstResponse, pstRecvInfo);
}

int RTFile::RecvUpdataFileList(char * buf, int nLen, tagListResponse * pstResponse, tagRecvInfo * pstRecvInfo)
{
	MLocalSection		local;
	FileList		*	pstList;
	
	local.Attch(&m_Section);
	pstList = _GetListPtr(pstResponse->nClassID, pstResponse->nMarketID);
	local.UnAttch();
	
	if(!pstList)
		return -1;
	
	return pstList->RecvUpdataFileList(buf, nLen, pstResponse, pstRecvInfo);
}

int RTFile::RecvInitFileList(char * buf, int nLen, tagListResponse * pstResponse, tagRecvInfo * pstRecvInfo)
{
	MLocalSection		local;
	FileList		*	pstList;
	
	local.Attch(&m_Section);
	pstList = _GetListPtr(pstResponse->nClassID, pstResponse->nMarketID);
	local.UnAttch();
	
	if(!pstList)
		return -1;
	
	return pstList->RecvInitFileList(buf, nLen, pstResponse, pstRecvInfo);
}

int RTFile::RecvListInfo(tagListInfoResponse * pInfo, char* buf, int nLen, tagRecvInfo * pstRecvInfo)
{
	MLocalSection		local;
	FileList		*	pstList;
	
	local.Attch(&m_Section);
	pstList = _GetListPtr(pInfo->nClassID, pInfo->nMarketID);
	local.UnAttch();
	
	if(!pstList)
		return -1;
	
	return pstList->RecvListInfo(pInfo, buf, nLen, pstRecvInfo);
}

int	RTFile::OnConnect(int serial)
{
	MLocalSection		local;
	int					i, j;
	FileList		*	pstList;

	local.Attch(&m_Section);

	if(m_nFlag < 0)
		return -1;

	if(m_nFlag != 0)
	{
		m_nFlag = 2;
	}

	m_nNeedRequest = 1;

	if(m_nCommSerial != -1)
	{
		for(i = 0; i < m_nClassCount; i++)
		{
			for(j = -1; j < m_nMarketCount; j++)
			{
				pstList = _GetListPtr(i, j);
				if(pstList)
				{
					pstList->OnConnect(m_nCommSerial != serial);
				}
			}
		}
	}

	m_nCommSerial = serial;
	return 1;
}

unsigned long RTFile::GetUpFileCount()
{
	MLocalSection		local;
	
	local.Attch(&m_Section);

	return m_nUpFileCount;
}

int	RTFile::IsUpdateOn()
{
	MLocalSection		local;
	
	local.Attch(&m_Section);
	
	return m_AutoUpdateOn;
}

FileList * RTFile::_GetListPtr(int nClassID, int nMarketID)
{
	if((nClassID >= m_nClassCount) ||
		(nClassID < 0)||
		(nMarketID >=m_nMarketCount))
	{
		return NULL;
	}

	if(nMarketID < 0)
		return &m_RecordData[nClassID][m_nMarketCount];
	else
		return &m_RecordData[nClassID][nMarketID];
}

int	RTFile::CheckList()
{
	MLocalSection		local;
	FileList		*	pstList = NULL;
	
	local.Attch(&m_Section);

	if(m_nFlag < 0)
		return -1;
	
	if((m_nFlag == 0) || (m_nFlag == 2))
	{
		if((m_nNeedRequest == 1)||
			((m_nNeedRequest == 2) && (m_Counter.GetDuration() > 2000))||
			(m_Counter.GetDuration() > NETWORK_TIME_OUT))
		{
			tagInfoRequst stRequest;
			
			Global_ClientComm.SendRequest(10, 70, (const char*)&stRequest, sizeof(tagInfoRequst), 0);
			m_Counter.SetCurTickCount();
			m_nNeedRequest = 0;	
		}
		return 1;
	}
	
	//m_nFlag == 1, 检查列表状态
	if(m_nLClassID >= m_nClassCount)
	{
		m_nLClassID = 0;
		m_nLMarketID = -1;
	}
	
	if(m_nLMarketID >= m_nMarketCount)
	{
		m_nLClassID++;
		m_nLMarketID = -1;
	}
	
	pstList = _GetListPtr(m_nLClassID, m_nLMarketID);
	
	m_nLMarketID++;
	local.UnAttch();
	
	if(pstList)
		pstList->CheckSatus();

	return 1;
}

int	RTFile::CheckFileM()
{
	FileList		*	pstList = NULL;
	int					ret;
	int					Serial;
	MLocalSection		local;

//单线程运行

	if(m_nClassCount <= 0)
		return 500;

	if(m_NewDown)
	{
		m_NewDown = 0;
		ResetParamFM();
	}

	if(m_nFMarketID >= m_nMarketCount)
	{
		m_nFClassID++;
		m_nFMarketID = -1;
		m_nFSerial = 0;
	}

	if(m_nFClassID >= m_nClassCount)
	{
		m_nFMarketID = m_nFMarketID0;
		m_nFClassID = m_nFClassID0;
		m_nFSerial = m_nFSerial0;
		m_nSendReqNumM2 = 0;
		m_nSendReqNumM1 = 0;
		return 100;
	}
	
	local.Attch(&m_Section);
	pstList = _GetListPtr(m_nFClassID, m_nFMarketID);
	local.UnAttch();

	if(!pstList)
	{
		m_nFMarketID++;
		m_nFSerial = 0;
		return 100;
	}
	
	Serial = m_nFSerial;
	ret = pstList->CheckaFileM(Serial);

	if(Serial >= 0)
	{
		m_nFMarketID0 = m_nFMarketID;
		m_nFClassID0 = m_nFClassID;
		m_nFSerial0 = Serial;
	}

	if(ret == 2)
	{
		m_nFMarketID = m_nFMarketID0;
		m_nFClassID = m_nFClassID0;
		m_nFSerial = m_nFSerial0;
		m_nSendReqNumM2 = 0;
		m_nSendReqNumM1 = 0;
		return 100;
	}
	else if(ret == -20110518)
	{
		slib_WriteError( Global_UnitNo, 0, "[手动处理线程]:发现有超时的文件");
		Global_ClientComm.CloseLink(false);
		return 10;			//GUOGUO 20110518
	}
	else
	{
		m_nFMarketID++;
		m_nFSerial = 0;
		return 50;
	}
}


int	RTFile::CheckFileA()
{
	FileList		*	pstList = NULL;
	int					ret;
	int					Serial;
	MLocalSection		local;

	//单线程运行
	if(m_nClassCount <= 0)
		return 500;

	if(!m_AutoUpdateOn)
	{
		return 500;
	}

	if(m_nFAMarketID >= m_nMarketCount)
	{
		m_nFAClassID++;
		m_nFAMarketID = -1;
		m_nFASerial = 0;
	}

	if(m_nFAClassID >= m_nClassCount)
	{
		if(m_nSendReqNumA1 + m_nSendReqNumA2 > 0)
		{
			m_nFAMarketID = m_nFAMarketID0;
			m_nFAClassID = m_nFAClassID0;
			m_nFASerial = m_nFASerial0;
			m_nSendReqNumA2 = 0;
			m_nSendReqNumA1 = 0;
		}
		else
		{
			ResetParamFA();
		}
		return 100;
	}

	local.Attch(&m_Section);	
	pstList = _GetListPtr(m_nFAClassID, m_nFAMarketID);
	local.UnAttch();

	if(!pstList)
	{
		m_nFAMarketID++;
		m_nFASerial = 0;
		return 100;
	}
	
	Serial = m_nFASerial;
	ret = pstList->CheckaFileA(Serial);

	if(Serial >= 0)
	{
		m_nFAMarketID0 = m_nFAMarketID;
		m_nFAClassID0 = m_nFAClassID;
		m_nFASerial0 = Serial;
	}
	
	if(ret == 2)
	{
		m_nFAMarketID = m_nFAMarketID0;
		m_nFAClassID = m_nFAClassID0;
		m_nFASerial = m_nFASerial0;
		m_nSendReqNumA2 = 0;
		m_nSendReqNumA1 = 0;
		return 100;
	}
	else if(ret == -20110518)
	{
		slib_WriteError( Global_UnitNo, 0, "[自动处理线程]:发现有超时的文件");
		Global_ClientComm.CloseLink(false);
		return 10;			//GUOGUO 20110518
	}
	else
	{
		m_nFAMarketID++;
		m_nFASerial = 0;
		return 50;
	}
}

void RTFile::ResetParamFM()
{
	m_nFClassID0 = 0;
	m_nFMarketID0 = -1;
	m_nFSerial0 = 0;
	m_nFClassID = 0;
	m_nFMarketID = -1;
	m_nFSerial = 0;
}


void RTFile::ResetParamFA()
{
	m_nFAClassID0 = 0;
	m_nFAMarketID0 = -1;
	m_nFASerial0 = 0;
	m_nFAClassID = 0;
	m_nFAMarketID = -1;
	m_nFASerial = 0;
}


void RTFile::UpdateRecBufPercent(int percent)
{
	MLocalSection	local;
	
	local.Attch(&m_Section);
	
	m_nRecBufPercent = percent;
}


int RTFile::IsNewer(void * time1, void * time2)
{
	if(*(unsigned long*)time1 > *(unsigned long*)time2)
		return 1;
	else
		return 0;
}

void * __stdcall RTFile::ProcessListThread(void * In)
{
	RTFile *	classptr;
	int			ret;

	classptr = (RTFile *)In;

	while ( classptr->m_stProcessListThread.GetThreadStopFlag( ) == false )
	{
		if(Global_ClientComm.GetLinkFlg() != 1)
		{
			ret = 500;
		}
		else
		{
			ret = classptr->CheckList();
		}
		try
		{
			MThread::Sleep(50);
		}
		catch( ... )
		{
			slib_WriteError( Global_UnitNo, 0, "[数据模块]:列表处理线程发生未知异常" );
		}
	}
	return 0;
}

void * __stdcall RTFile::ProcessFileThreadM(void * In)
{
	RTFile *	classptr;
	int			ret;
	
	classptr = (RTFile *)In;
	
	while ( classptr->m_stProcessFileThreadM.GetThreadStopFlag( ) == false )
	{
		if(Global_ClientComm.GetLinkFlg() != 1)
		{
			ret = 500;
		}
		else
		{
			ret = classptr->CheckFileM();
		}

		try
		{


			MThread::Sleep(ret);	
		}
		catch( ... )
		{
			slib_WriteError( Global_UnitNo, 0, "[数据模块]:文件处理线程发生未知异常" );
		}
	}
	return 0;
}

void * __stdcall RTFile::ProcessFileThreadA(void * In)
{
	RTFile *	classptr;
	int			ret;
	
	classptr = (RTFile *)In;
	
	while ( classptr->m_stProcessFileThreadA.GetThreadStopFlag( ) == false )
	{
		if(Global_ClientComm.GetLinkFlg() != 1)
		{
			ret = 500;
		}
		else
		{
			ret = classptr->CheckFileA();
		}
		
		try
		{
			
			
			MThread::Sleep(ret);	
		}
		catch( ... )
		{
			slib_WriteError( Global_UnitNo, 0, "[数据模块]:文件处理线程发生未知异常" );
		}
	}
	return 0;
}

int	RTFile::ManualDownInfo(tagDownInfoReq * pstRequest, tagDownInfoRes * pstResponse)
{
	MLocalSection		local;
	FileList		*	pstList;
	
	local.Attch(&m_Section);
	pstList = _GetListPtr(pstRequest->cClassID, pstRequest->cMarketID);
	
	if(!pstList)
		return FLAG_NO_LIST;

	return pstList->ManualDownInfo(pstRequest, pstResponse);
}


int	RTFile::ManualGetError(tagFailCodeReq * pstRequest, tagFailCodeRes * pstResponse, char * buf, int & buflen)
{
	MLocalSection		local;
	FileList		*	pstList;
	
	local.Attch(&m_Section);
	pstList = _GetListPtr(pstRequest->cClassID, pstRequest->cMarketID);
	
	if(!pstList)
		return FLAG_NO_LIST;

	return pstList->ManualGetError(pstRequest, pstResponse, buf, buflen);

}

int	RTFile::ManualDown(tagDownDataRes * pstResponse)
{
	MLocalSection		local;
	FileList		*	pstList;
	int					ret;
	
	local.Attch(&m_Section);
	pstList = _GetListPtr(pstResponse->cClassID, pstResponse->cMarketID);
	
	if(!pstList)
		return FLAG_NO_LIST;

	ret = pstList->ManualDown(pstResponse);
	if(ret > 0)
	{
		m_NewDown = 1;
	}
	return ret;
}

int	RTFile::ManualCancelDown(int mClassID, int  nMarketID, int nKindID)
{
	MLocalSection		local;
	FileList		*	pstList;
	
	local.Attch(&m_Section);
	pstList = _GetListPtr(mClassID, nMarketID);
	
	if(!pstList)
		return FLAG_NO_LIST;

	return pstList->ManualCancelDown(mClassID, nMarketID, nKindID);

}

int	RTFile::GetListCount(tagListCountRes * pRes)
{
	MLocalSection		local;

	local.Attch(&m_Section);
	pRes->cClassCount = m_nClassCount;
	pRes->cMarketCount = m_nMarketCount;

	return 1;
}

int	RTFile::GetListInfo(tagListInfoRes * pRes)
{
	MLocalSection		local;
	FileList		*	pstList;

	local.Attch(&m_Section);
	pstList = _GetListPtr(pRes->cClassID, pRes->cMarketID);
	pRes->nFlag = m_nCookie;

	if(!pstList)
		return FLAG_NO_LIST;

	return pstList->GetListInfo(pRes);
}

int	RTFile::SetUpdate(tagSetUpdateRes * pRes)
{
	MLocalSection		local;
	FileList		*	pstList;
	int					i, j;
	
	local.Attch(&m_Section);
	if(pRes->cClassID < 0)
	{
		m_AutoUpdateOn = pRes->nFlag;
		if(m_AutoUpdateOn)
		{
			for(i = 0; i < m_nClassCount; i++)
			{
				for(j = -1; j < m_nMarketCount; j++)
				{
					pstList = _GetListPtr(i, j);
					if(pstList)
					{
						pstList->ReFleshFile();
					}
				}
			}
		}
		return 1;
	}

	pstList = _GetListPtr(pRes->cClassID, pRes->cMarketID);
	
	if(!pstList)
		return FLAG_NO_LIST;
	
	return pstList->SetUpdate(pRes);
}

//-------------------------------------------------------------------------------