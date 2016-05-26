/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		filedata.cpp
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统服务器文件数据处理模块
	History:		
*********************************************************************************************************/
#include "file.h"
#include "Global.h"

RTFile::RTFile()
{
	m_RecordData = NULL;
	m_nMarketCount = 0;
	m_nClassCount = 0;
	m_nCkeckThreadCount = 0;
}

RTFile::~RTFile()
{
	Release();
}

int RTFile::Instance()
{
	MString			name;
	MLocalSection	local;
	int				i, j;
	tagClassInfo	info;
	int				ret;

	FileList * ptmp = NULL;
 
	Release();

	local.Attch(&m_Section);
	
	m_nMarketCount = Global_Option.GetMarketCount();
	m_nClassCount = Global_Option.GetFileClassCount();
	m_stMainPath = Global_Option.GetWorkDir();

	if((m_nClassCount < 0) || (m_nMarketCount < 0))
		return -1;

	if(!m_RecordData)
	{
		m_RecordData = new FileList*[m_nClassCount];
		if(!m_RecordData)
		{
			Release();
			return -1;
		}
		memset(m_RecordData, 0, sizeof(FileList*) * m_nClassCount);
	}

	for(i = 0; i < m_nClassCount; i++)
	{
		m_RecordData[i] = new FileList[m_nMarketCount + 1];
		if(!m_RecordData[i])
		{
			Release();
			return -1;
		}
		Global_Option.GetClassCfg(i, &info);
		if(info.DirName.StringPosition("*market") >= 0)
		{
			for(j = 0; j < m_nMarketCount; j++)
			{
				m_RecordData[i][j].Instance(i, j, this, &info);
			}
		}
		else if(info.market < 0)
		{
			m_RecordData[i][m_nMarketCount].Instance(i, -1, this, &info);
		}
		else if(info.market < m_nMarketCount)
		{
			m_RecordData[i][info.market].Instance(i, info.market, this, &info);
		}
	}

	m_nUpClassID = 0;
	m_nUpMarketID = -1;
	m_nCkeckThreadCount = Global_Option.GetCheckThreadCount();
	m_stProcessFileThread = new MThread[m_nCkeckThreadCount];
	if(!m_stProcessFileThread)
	{
		Release();	
		return -1;
	}
	for(i = 0; i < m_nCkeckThreadCount; i++)
	{
		if( ( ret = m_stProcessFileThread[i].StartThread( "文件处理线程", ProcessFileThread, (void*)i ) ) != 1 )
		{
			Release();	
			return -1;
		}
	}
/*
	ResetParam1();
	if( ( ret = m_stUpdateCacheThread.StartThread( "缓存更新线程", UpdateCacheThread, this ) ) != 1 )
	{
		Release();	
		return -1;
	}
	*/
	return 1;
}

void RTFile::Release()
{
	MLocalSection		local;
	local.Attch(&m_Section);

	int		i;

	for(i = 0; i < m_nCkeckThreadCount; i++)
	{
		m_stProcessFileThread[i].StopThread();
	}
//	m_stUpdateCacheThread.StopThread();

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
}

int	RTFile::GetCfg(char	*buf, int &nLen, tagCfgResponse * pstResponse)
{
	int					i, j;
	tagListFlag		*	pFlag = (tagListFlag*)buf;
	

	if(nLen < sizeof(tagListFlag) * m_nClassCount * (m_nMarketCount + 1))
	{
		pstResponse->nClassCount = 0;
		pstResponse->nMarketCount = 0;
		nLen = 0;
	}
	else
	{
		pstResponse->nClassCount = m_nClassCount;
		pstResponse->nMarketCount = m_nMarketCount;
		nLen = sizeof(tagListFlag) * m_nClassCount * (m_nMarketCount + 1);
		for(i = 0; i < m_nClassCount; i++)
		{
			for(j = 0; j < m_nMarketCount + 1; j++)
			{
				pFlag[i * (m_nMarketCount + 1) + j].cListFlag = m_RecordData[i][j].m_nFlag;
			}
		}
	}

	return 1;
}

int RTFile::GetData(void * buf, int & nLen, tagFileResponse *pstResponse, MString stName)
{
	FileList		*pstList;
	int					ret;

	pstList = _GetListPtr(pstResponse->nClassID, pstResponse->nMarketID);

	if(!pstList)
	{
		pstResponse->nFlag = FLAG_NO_LIST;
		nLen = 0;
		return 1;
	}

	ret = pstList->GetData(buf, nLen, pstResponse, stName);
	if(ret < 0)
	{
		nLen = 0;
	}
	pstResponse->nFlag = ret;

//	pstList->DecrementRefCount(0);

	return 1;
}

int RTFile::GetInfo(tagInfoResponse *pstResponse, MString stName)
{
	FileList		*pstList;
	int					ret;

	pstList = _GetListPtr(pstResponse->nClassID, pstResponse->nMarketID);

	if(!pstList)
	{
		pstResponse->nFlag = FLAG_NO_LIST;
		return 1;
	}

	ret = pstList->GetInfo(pstResponse, stName);
	pstResponse->nFlag = ret;

//	pstList->DecrementRefCount(0);

	return 1;
}

int RTFile::GetUpdataFileList(char * buf, int & nLen, tagListResponse * pstResponse)
{
	FileList		*	pstList;
	int					ret;

	pstList = _GetListPtr(pstResponse->nClassID, pstResponse->nMarketID);
	
	if(!pstList)
	{
		pstResponse->nFlag = FLAG_NO_LIST;
		return 1;
	}

	ret = pstList->GetUpdataFileList(buf, nLen, pstResponse);
	if(ret < 0)
	{
		nLen = 0;
	}
	pstResponse->nFlag = ret;

//	pstList->DecrementRefCount(0);
	return 1;
}

int RTFile::GetInitFileList(char * buf, int & nLen, tagListResponse * pstResponse)
{
	FileList		*	pstList;
	int					ret;

	pstList = _GetListPtr(pstResponse->nClassID, pstResponse->nMarketID);

	if(!pstList)
	{
		pstResponse->nFlag = FLAG_NO_LIST;
		return 1;
	}

	ret = pstList->GetInitFileList(buf, nLen, pstResponse);
	if(ret < 0)
	{
		nLen = 0;
	}
	pstResponse->nFlag = ret;

//	pstList->DecrementRefCount(0);
	return 1;
}

int RTFile::GetListInfo(tagListInfoResponse * pstResponse, char *buf, int & nLen)
{
	FileList		*	pstList;
	int					ret;
	
	pstList = _GetListPtr(pstResponse->nClassID, pstResponse->nMarketID);

	if(!pstList)
	{
		pstResponse->nFlag = FLAG_NO_LIST;
		return 1;
	}

	ret = pstList->GetListInfo(pstResponse, buf, nLen);
	if(ret < 0)
	{
		nLen = 0;
	}
	pstResponse->nFlag = ret;

//	pstList->DecrementRefCount(0);

	return 1;
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


int RTFile::IsNewer(void * time1, void * time2)
{
	if(*(unsigned long*)time1 > *(unsigned long*)time2)
		return 1;
	else
		return 0;
}


int	RTFile::CheckFile()
{
	MLocalSection		local;
	FileList		*	pstList = NULL;
	
	local.Attch(&m_Section);
	
	if(m_nUpClassID >= m_nClassCount)
	{
		m_nUpClassID = 0;
		m_nUpMarketID = -1;
	}

	if(m_nUpMarketID >= m_nMarketCount)
	{
		m_nUpClassID++;
		m_nUpMarketID = -1;
	}
	
	pstList = _GetListPtr(m_nUpClassID, m_nUpMarketID);
	
	m_nUpMarketID++;
	local.UnAttch();

	if(pstList) {
		pstList->CheckFileUpdateTime();
//		pstList->DecrementRefCount(1);
	}

	return 1;
}

/*
int	RTFile::CheckCache(MCounter * pCounter)
{
	MLocalSection		local;
	FileList		*	pstList = NULL;
	
	return 1;
}

void * __stdcall RTFile::UpdateCacheThread(void * In)
{
	RTFile *	classptr;
	MCounter	counter;
	int			ret;

	classptr = (RTFile *)In;

	counter.SetCurTickCount();
	while ( classptr->m_stUpdateCacheThread.GetThreadStopFlag( ) == false )
	{
		try
		{
			ret = 0;
			while(ret >= 0)
			{
				ret = classptr->CheckCache(&counter);
				if(ret == 2)
					MThread::Sleep(100);
			}
		//	MThread::Sleep(Global_Option.GetCheckFileCycle());
		}
		catch( ... )
		{
			TraceLog( LOG_ERROR_NORMAL, MODULENAME, "缓存更新线程发生未知异常");
		}
	}
	return 0;
}
*/
void * __stdcall RTFile::ProcessFileThread(void * In)
{
	int	no;
	
	no = (int)In;
	
	while (Global_DataIO.m_stProcessFileThread[no].GetThreadStopFlag( ) == false )
	{
		Global_DataIO.CheckFile();
		try
		{
			//Global_DataIO.CheckFile();
			MThread::Sleep(50);
		}
		catch( ... )
		{
			TraceLog( LOG_ERROR_NORMAL, MODULENAME, "文件检查线程发生未知异常");
		}
	}
	return 0;
}

//-------------------------------------------------------------------------------