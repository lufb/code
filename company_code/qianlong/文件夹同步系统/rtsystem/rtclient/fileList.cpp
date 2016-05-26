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

FileList::FileList()
{
	m_pstData = NULL;
	m_pFirstFile = NULL;
	m_pstDownStatus = NULL;
	m_nStatusNum = 0;
	m_nFileNum = 0;
	m_nAllocNum = 0;
	m_CancelDown = 0;
	m_nListCookie = -1;
	m_nFileCookie = -1;
	m_nInited = 0;
	
	m_nFlag = -1;
	m_nNeedRequest = 0;
	m_nFileCount = 0;
}

FileList::~FileList()
{
	Release();
}

int	FileList::Instance(int nClassID, int nMarketID, RTFile * perent)
{
	MLocalSection		local;
	Release();
	
	local.Attch(&m_Section);
	if(nClassID != -1)
	{
		m_nClassID = nClassID;
		m_nMarketID = nMarketID;
		m_perent = perent;
	}

	//为请求列表信息做准备
	m_nNeedRequest = 1;
	m_nFlag = 0;
	m_AutoUpdate = 1;

	printf("(%d, %d)Instance\n", m_nClassID, m_nMarketID);
	return 1;
}

void FileList::Release()
{
	MLocalSection		local;
	RTFileData * pFile, *pNextFile;
	
	local.Attch(&m_Section);

	printf("(%d, %d)Release\n", m_nClassID, m_nMarketID);

	if(m_pstData)
	{
		delete[] m_pstData;
		m_pstData = NULL;		/*	注意,野指针,ADD GUOGUO 20110125*/
	}
	if(m_pFirstFile)
	{
		pFile = m_pFirstFile;
		while(pFile)
		{	
			pNextFile = pFile->m_pNext;
			delete pFile;
			pFile = pNextFile;
		}
	}
	m_pFirstFile = NULL;
	m_nFileNum = 0;
	m_nAllocNum = 0;
	m_nListCookie = -1;
	m_nFileCookie = -1;
	m_nInited = 0;
	m_nFlag = -1;
	m_AutoUpdate = 0;
	m_nNeedFlag = 0;
	m_nFileCount = 0;
}

int	FileList::OnConnect(int ServerChange)
{
	MLocalSection		local;
	RTFileData		*	pstFile;
	int					i;
	
	local.Attch(&m_Section);

	if(m_nFlag < 0)
		return -1;

	if(ServerChange)
	{
		for(i = 0; i < m_nFileNum; i++)
		{
			pstFile = m_pstData[i];
			if(pstFile->m_nFlag == 2)
			{
				pstFile->CancelUpdateFile();
				pstFile->m_nFlag = 1;
				pstFile->m_nNeedRequest = 1;
				pstFile->m_nFailcount = 0;
				pstFile->m_nPercent = 0;
			}
		}
	}

	//为请求列表信息做准备
	m_nNeedRequest = 1;
	m_nFlag = 0;
	m_nListCookie = -1;
	m_nFileCookie = -1;

	m_nNeedFlag = 0;

	return 1;
}

int	FileList::CheckSatus()
{
	MLocalSection		local;
	
	local.Attch(&m_Section);

	if(m_nFlag < 0)
		return -1;

	switch(m_nFlag)
	{
	case 3:
		
		if(m_stLastRecvTime.GetDuration() > m_stCfg.cupdatecycle)//重新请求更新列表策略：到最后一次收到列表信息后多少时间
		{
			//为再次请求列表信息做准备
			m_nNeedRequest = 1;
			m_nFlag = 4;
			m_stLastRecvTime.SetCurTickCount();
		}
		else
			break;
	case 4:
	case 0:	//ListInfo
		if((m_nNeedRequest)||
			((m_nNeedRequest == 2) && (m_stLastRecvTime.GetDuration() > 2000))||
			(m_stLastRecvTime.GetDuration() > NETWORK_TIME_OUT))
		{
			/**
			 *	控制不必要的请求,减少流量
			 *	本文件列表的m_nNeedFlag 首次=0,在接收的响应后,就会被设置成1.
			 *	发送的条件: 如果m_nNeedFlag == 0 并且m_stCfg.info 不空
			 *		下面的if写法表示的意思是.
			 *	如果本文件列表已经发送,并且也收到了服务器的响应.但是它的info字段不在needlist里面
			 *		那么就不再发送了.
			 */

			if(m_nNeedFlag > 0 && !Global_Option.BinarySearch(m_stCfg.info.c_str()))
				break;

			tagListInfoRequest stRequest;
			stRequest.nClassID = m_nClassID;
			stRequest.nMarketID = m_nMarketID;
#ifdef _DEBUG
			printf("SendRequest: %d, %d\n", stRequest.nClassID, stRequest.nMarketID);
#endif
			m_nFrameNo = 0;
			m_nRequestNo++;
			Global_ClientComm.SendRequest(10, 65, (const char*)&stRequest, sizeof(tagListInfoRequest), m_nRequestNo);
			m_stLastRecvTime.SetCurTickCount();
			m_nNeedRequest = 0;	
		}
		break;

	case 5:
	case 1:	//InitList
		if((m_nNeedRequest)||
			((m_nNeedRequest == 2) && (m_stLastRecvTime.GetDuration() > 2000))||
			(m_stLastRecvTime.GetDuration() > NETWORK_TIME_OUT))
		{
			tagListRequst stRequest;
			stRequest.nClassID = m_nClassID;
			stRequest.nMarketID = m_nMarketID;
			stRequest.nSerial = m_nSerial;
			stRequest.nListCookie = m_nListCookie;

			m_nFrameNo = 0;
			m_nRequestNo++;
			Global_ClientComm.SendRequest(10, 66, (const char*)&stRequest, sizeof(tagListRequst), m_nRequestNo);
			m_nNeedRequest = 0;	
			m_stLastRecvTime.SetCurTickCount();
		}
		break;

	case 2://UpdateList
		if((m_nNeedRequest) ||
			((m_nNeedRequest == 2) && (m_stLastRecvTime.GetDuration() > 2000))||
			(m_stLastRecvTime.GetDuration() > NETWORK_TIME_OUT))
		{
			tagListRequst stRequest;	
			stRequest.nClassID = m_nClassID;
			stRequest.nMarketID = m_nMarketID;
			stRequest.nSerial = m_nSerial;
			stRequest.nListCookie = m_nListCookie;
			m_nFrameNo = 0;
			m_nRequestNo++;
			Global_ClientComm.SendRequest(10, 67, (const char*)&stRequest, sizeof(tagListRequst), m_nRequestNo);
			m_nNeedRequest = 0;	
			m_stLastRecvTime.SetCurTickCount();
		}
		break;

	default:
		break;
	}

	return 1;
}

int FileList::RecvUpdataFileList(char * buf, int nLen, tagListResponse * pstResponse, tagRecvInfo * pstRecvInfo)
{
	unsigned short		i, j;
	int					usesize = 0, filesize;
	MLocalSection		local;
	int					complete = 1;
	unsigned short		serial;
	int					findpos;
	char			*	psname;
	unsigned long		tmptime;

	local.Attch(&m_Section);

	if(m_nFlag != 2)
		return -1;

	if((m_nRequestNo != pstRecvInfo->nRequestNo) ||
		(m_nFrameNo != pstRecvInfo->nFrameNo))
	{
		return -1;
	}

	if(m_nNeedRequest)
		return -1;

	if(pstResponse->nFlag < 0)
	{
		Release();
		return 1;
	}

	if(pstResponse->nFileCount <= 0)
		return 1;

	if((!m_AutoUpdate) || (!m_perent->m_AutoUpdateOn))
	{
		m_nFlag = 3;
		return 1;
	}

	serial = *(unsigned short*)(buf + sizeof(unsigned long));
	if(serial != m_nSerial)
	{
		printf("list serial error1\n");
		return 1;
	}

	//Time | serial | filesize | filename\0
	for(j = 0; j < pstResponse->nFileCount; j++)
	{
		tmptime = *(unsigned long*)(buf + usesize);
		usesize += sizeof(unsigned long);
		serial = *(unsigned short*)(buf + usesize);
		usesize += sizeof(unsigned short);
		filesize = *(int*)(buf + usesize);
		usesize += sizeof(int);
		psname = buf + usesize;
		usesize += strlen(psname) + 1;

		findpos = serial;

		if(findpos > m_nFileNum)
		{
			printf("(%d, %d)recv updatelist error  %d %d %d\n", m_nClassID, m_nMarketID, findpos, m_nFileNum, m_nFlag );
			m_nFlag = 1;
			return 1;
		}

		if((findpos >= m_nFileNum)||
			(m_pstData[findpos]->m_sFileName != psname))
		{
			findpos -= 10;
			if(findpos < 0)
				findpos = 0;

			for(i = findpos; i < m_nFileNum; i++)
			{
				if(m_pstData[i]->m_sFileName == psname)
				{
					m_pstData[i]->CheckUpdate(tmptime, filesize, serial);
				}
			}
		}
		else
		{
			m_pstData[findpos]->CheckUpdate(tmptime, filesize, serial);
		}
	}

	if(pstResponse->nFlag & (1<<2))
	{
		m_nFlag = 3;
	}
	else //未完成,为接着请求做准备
	{
		m_nSerial = pstResponse->nNextSerial;
		if(pstResponse->nFlag & (1<<1))//需要接着请求
		{
			m_nNeedRequest = 1;
		}
		else//等待下一帧即可
		{
			m_nFrameNo++;
		}
	}

	m_stLastRecvTime.SetCurTickCount();
	
	return 1;
}

int FileList::RecvInitFileList(char * buf, int nLen, tagListResponse * pstResponse, tagRecvInfo * pstRecvInfo)
{
	unsigned short		i, j;
	int					usesize = 0, itmp;
	MLocalSection		local;
	int					complete = 1;
	unsigned short		serial;
	int					findpos;
	char			*	psname;
	RTFileData		*	pFile;
	RTFileData		**	ptmp;
	
	local.Attch(&m_Section);
	
	if((m_nFlag != 1) && (m_nFlag != 5))
		return -1;

	if(m_nNeedRequest)
		return -1;
	
	if((m_nRequestNo != pstRecvInfo->nRequestNo) ||
		(m_nFrameNo != pstRecvInfo->nFrameNo))
	{
		return -1;
	}

	if(pstResponse->nFlag == FLAG_CHANGING)
	{
		m_nNeedRequest = 2;
		m_stLastRecvTime.SetCurTickCount();
		return 1;
	}

	if(pstResponse->nFlag == FLAG_CHANGED)
	{//重新请求
		m_nSerial = 0;
		m_nNeedRequest = 1;
		m_nListCookie = pstResponse->nListCookie;
		return 1;
	}
	
	if(pstResponse->nFlag < 0)
	{
		Release();
		return 1;
	}

	if(pstResponse->nFileCount <= 0)
		return 1;

	serial = *(unsigned short*)buf;
	if(serial != m_nSerial)
	{
		printf("list serial error\n");
		return 1;
	}

#if 0
	if(m_nFileCount == 0)
	{
		m_nFileCount = pstResponse->nFileCount;
	}
	else
	{
		if( !(pstResponse->nFlag & (1<<2) ))
		{
			m_nFileCount += pstResponse->nFileCount;
		}
		else
		{
			if(pstResponse->nFileCount > m_nFileCount)
			{
				Release();
				Instance(m_nClassID, m_nMarketID, m_perent);
				return 1;
			}
		}
	}
#endif

	//serial | filename\0
	for(j = 0; j < pstResponse->nFileCount; j++)
	{
		findpos = *(unsigned short*)(buf + usesize);
		usesize += sizeof(unsigned short);
		psname = buf + usesize;
		usesize += strlen(psname) + 1;

		if(findpos > m_nFileNum)
		{
			printf("(%d, %d)recv initlist error\n", m_nClassID, m_nMarketID);
			return 1;
		}

		if((findpos == m_nFileNum)||
			(m_pstData[findpos]->m_sFileName != psname))
		{//现考虑findnext不会重复
			for(i = findpos; i < m_nFileNum; i++)
			{
				if(m_pstData[i]->m_sFileName == psname)
					break;
			}

			if(i == m_nFileNum)//新文件，插入
			{
				if((m_nAllocNum > m_nFileNum) && (m_pstData[m_nFileNum]))
				{
					pFile = m_pstData[m_nFileNum];
				}
				else
				{
					pFile = new RTFileData;
					pFile->m_pNext = m_pFirstFile;
					m_pFirstFile = pFile;
				}

				if(!pFile)
				{
					return -1;
				}

				pFile->Instance(psname, this);

				if(m_nAllocNum > m_nFileNum)
				{
					if(m_nFileNum - findpos)
						memmove(&m_pstData[findpos + 1], &m_pstData[findpos], (m_nFileNum - findpos) * sizeof(RTFileData *));
				}
				else
				{
					ptmp = m_pstData;
					itmp = m_nAllocNum + m_nAllocNum / 10 + 1;
					m_pstData = new RTFileData*[itmp];
					if(!m_pstData)
					{
						return -1;
					}
					m_nAllocNum = itmp;
					if(findpos)
						memcpy(m_pstData, ptmp, findpos * sizeof(RTFileData*));
					if(m_nFileNum - findpos)
						memcpy(&m_pstData[findpos + 1], &ptmp[findpos], (m_nFileNum - findpos) * sizeof(RTFileData *));

					if(ptmp)
						delete[] ptmp;

					memset(&m_pstData[m_nFileNum + 1], 0, (m_nAllocNum - m_nFileNum - 1) * sizeof(RTFileData*));
				}
				m_pstData[findpos] = pFile;
				m_pstData[findpos]->SetSerial(findpos);
				m_nFileNum++;
			}
			else
			{	//位置错乱，交换
				pFile = m_pstData[findpos];
				m_pstData[findpos] = m_pstData[i];
				m_pstData[findpos]->SetSerial(findpos);
				m_pstData[i] = pFile;
			}
		}
		else
		{
			m_pstData[findpos]->SetSerial(findpos);
		}
	}

	if(pstResponse->nFlag & (1<<2))
	{
		if(m_nInited == 1)
			printf("(%d, %d)list build complete: %s FileNum = %d\n", m_nClassID, m_nMarketID, m_stDir.c_str(), m_nFileNum);

		if((m_stCfg.syncmode == 1)&&
			(m_AutoUpdate))
		{
			m_nFlag	= 2;
			m_nSerial = 0;
			m_nNeedRequest = 1;
		}
		else
		{
			m_nFlag	= 3;
		}

		findpos++;
		itmp = findpos;
		for(; findpos < m_nFileNum; findpos++)
		{
			m_pstData[findpos]->Release();
		}
		m_nFileNum = itmp;

		m_nInited = 2;
	}
	else //未完成,为接着请求做准备
	{
		m_nSerial = pstResponse->nNextSerial;
		if(pstResponse->nFlag & (1<<1))//需要接着请求
		{
			m_nNeedRequest = 1;
		}
		else//等待下一帧即可
		{
			m_nFrameNo++;
		}
	}

	m_stLastRecvTime.SetCurTickCount();

	return 1;
}

int FileList::RecvListInfo(tagListInfoResponse * pInfo, char* buf, int nLen, tagRecvInfo * pstRecvInfo)
{
	MLocalSection		local;
	
	local.Attch(&m_Section);

	if((m_nFlag != 0) && (m_nFlag != 4))
		return -1;

	if(m_nNeedRequest)
		return -1;

	if((m_nRequestNo != pstRecvInfo->nRequestNo) ||
		(m_nFrameNo != pstRecvInfo->nFrameNo))
	{
		return -1;
	}
	
	if(pInfo->nFlag < 0)
	{
		Release();
		return 1;
	}

	if(m_nFlag == 4)//检查是否有更新
	{
		if(pInfo->nListCookie != m_nListCookie)
		{//为再次获得初始化列表准备
			m_nFlag	= 5;
			m_nSerial = 0;
			m_nNeedRequest = 1;	
			m_nListCookie = pInfo->nListCookie;
		}
		else
		{//自动更新，为取得更新列表准备
			if((pInfo->nFileCookie != m_nFileCookie) &&
				(m_stCfg.syncmode != 0))
			{
				m_nFlag	= 2;
				m_nSerial = 0;
				m_nNeedRequest = 1;
				m_nFileCookie = pInfo->nFileCookie;
			}
			else
			{
				m_nFlag = 3;//本次检查未发现改变
				m_stLastRecvTime.SetCurTickCount();
			}
		}
		return 1;
	}

	//m_nFlag == 0

	if(m_nInited > 0)
	{	//重新连接服务器
		if((m_stCfg.syncmode != pInfo->stCfg.syncmode)||
			(m_stCfg.updatemode != pInfo->stCfg.updatemode)||
			(m_stCfg.info != pInfo->stCfg.info)||
			(m_stDir != Global_Option.GetMainPath() + buf))
		{
			printf("syncmode:(%d, %d), updatemode:(%d, %d)\n", m_stCfg.syncmode, pInfo->stCfg.syncmode, m_stCfg.updatemode, pInfo->stCfg.updatemode);
			printf("info:(%s, %s)\ndir:(%s, %s)\n", m_stCfg.info.c_str(), pInfo->stCfg.info, m_stDir.c_str(), buf);
			Instance(-1, -1, 0);
			return 1;
		}
	}

	m_stDir = Global_Option.GetMainPath() + buf;
	m_stCfg.syncmode = pInfo->stCfg.syncmode;
	m_stCfg.updatemode = pInfo->stCfg.updatemode;
	m_stCfg.fileheadsize = pInfo->stCfg.fileheadsize;
	m_stCfg.recordsize = pInfo->stCfg.recordsize;
	m_stCfg.rectimepos = pInfo->stCfg.rectimepos;
	m_stCfg.market = pInfo->stCfg.market;
	m_stCfg.cupdatecycle = pInfo->stCfg.cupdatecycle;
	m_nMarketID = pInfo->nMarketID;
	m_nListCookie = pInfo->nListCookie;
	m_nFileCookie = pInfo->nFileCookie;
	m_stCfg.info = pInfo->stCfg.info;
	m_nInited = 1;	//已取得过状态信息

	/**
	 *	只是自动更新的模式,接收这个needlist的限制
	 */
	if(m_stCfg.syncmode == 1)
	{
		m_nNeedFlag = 1;
		
		if(!Global_Option.BinarySearch(m_stCfg.info.c_str()))
		{
			return 1;
		}
	}

	if(!m_pstData)
	{
		m_nAllocNum = pInfo->nFileCount + pInfo->nFileCount / 10 + 1;
		m_pstData = new RTFileData*[m_nAllocNum];
		if(!m_pstData)
		{
			Release();
			return -1;
		}
		memset(m_pstData, 0, m_nAllocNum * sizeof(RTFileData*));
	}

	//为获得初始化列表作准备
	m_nFlag	= 1;
	m_nSerial = 0;
	m_nNeedRequest = 1;	
	m_stLastRecvTime.SetCurTickCount();

	return 1;
}

int	FileList::CheckaFileM(int &Serial)
{
	MLocalSection		local;
	RTFileData		*	pstFile;
	int					i;
	
	i = Serial;
	Serial = -1;

	local.Attch(&m_Section);

	if(m_nFlag < 0)
		return -1;

	for(; i < m_nFileNum; i++)
	{
		pstFile = m_pstData[i];

		if(pstFile->m_nSyncMode != 0)
		{
			continue;
		}

		switch(pstFile->m_nFlag)
		{
		case 0:
			break;

		case 1:
			if((pstFile->m_nNeedRequest)||
				((m_nNeedRequest == 2) && (m_stLastRecvTime.GetDuration() > 2000)))
			{
				pstFile->RequestInfo();
			}

			if(pstFile->m_stLastRecvTime.GetDuration() > NETWORK_TIME_OUT)
			{
				pstFile->m_nFailcount++;
				pstFile->m_stLastRecvTime.SetCurTickCount();
				if(pstFile->m_nFailcount > MAX_FAIL_COUNT)
				{
					printf("net work error1\n");
					pstFile->CancelUpdateFile();
					pstFile->m_nPercent = -1;
//					return -20110518;
				}
				else
					pstFile->RequestInfo();
			}

			if(m_perent->m_nSendReqNumM1 + m_perent->m_nSendReqNumM2 == 0)
			{	
				if(Serial == -1)
					Serial = i;
			}

			m_perent->m_nSendReqNumM1++;
			if(m_perent->m_nSendReqNumM1 + m_perent->m_nSendReqNumM2 >= MAX_REQUEST_FILE * 3)
			{
				return 2;
			}
			break;

		case 2:	
			if(m_perent->m_nSendReqNumM2 < MAX_REQUEST_FILE)
			{
				if((pstFile->m_nNeedRequest)||
					((m_nNeedRequest == 2) && (m_stLastRecvTime.GetDuration() > 2000)))
				{
					pstFile->RequestData();
				}

				if(pstFile->m_stLastRecvTime.GetDuration() > NETWORK_TIME_OUT)
				{
					pstFile->m_nFailcount++;
					pstFile->m_stLastRecvTime.SetCurTickCount();
					if(pstFile->m_nFailcount > MAX_FAIL_COUNT)
					{
						printf("net work error2\n");
						pstFile->CancelUpdateFile();
						pstFile->m_nPercent = -1;
//						return -20110518;
					}
					else
						pstFile->RequestData();
				}
			}

			if(m_perent->m_nSendReqNumM1 + m_perent->m_nSendReqNumM2 == 0)
			{	
				if(Serial == -1)
					Serial = i;
			}

			m_perent->m_nSendReqNumM2++;
			if(m_perent->m_nSendReqNumM1 + m_perent->m_nSendReqNumM2 >= MAX_REQUEST_FILE * 3)
			{
				return 2;
			}
			break;

		case 3:
			if(m_perent->m_nSendReqNumM1 + m_perent->m_nSendReqNumM2 == 0)
			{	
				if(Serial == -1)
					Serial = i;
			}
			
			if(pstFile->m_stLastRecvTime.GetDuration() > 2000)//每两秒重新试验删除一次，删除几次不成功就替换
			{
				pstFile->m_nFailcount++;
				pstFile->m_stLastRecvTime.SetCurTickCount();
				pstFile->CompleteUpdateFile();
			}
			
			break;

		default:
			break;
		}

		if(i %5000 == 4999)
		{
			MThread::Sleep(1);
		}
	}

	return 1;
}


int	FileList::CheckaFileA(int &Serial)
{
	MLocalSection		local;
	RTFileData		*	pstFile;
	int					i;

	i = Serial;
	Serial = -1;
	
	local.Attch(&m_Section);
	
	if(m_nFlag < 0)
		return -1;

	if(!m_AutoUpdate)
	{
		return -1;
	}
	
	for(; i < m_nFileNum; i++)
	{
		pstFile = m_pstData[i];

		if(pstFile->m_nSyncMode != 1)
		{
			continue;
		}
		
		switch(pstFile->m_nFlag)
		{
		case 0:
			break;
			
		case 1:
			if((pstFile->m_nNeedRequest)||
				((m_nNeedRequest == 2) && (m_stLastRecvTime.GetDuration() > 2000)))
			{
				pstFile->RequestInfo();
			}
			
			if(pstFile->m_stLastRecvTime.GetDuration() > NETWORK_TIME_OUT)
			{
				pstFile->m_nFailcount++;
				pstFile->m_stLastRecvTime.SetCurTickCount();
				if(pstFile->m_nFailcount > MAX_FAIL_COUNT)
				{
					printf("net work error1\n");
					pstFile->CancelUpdateFile();
					pstFile->m_nPercent = -1;
//					return -20110518;
				}
				else
					pstFile->RequestInfo();
			}

			if(m_perent->m_nSendReqNumA1 + m_perent->m_nSendReqNumA2 == 0)
			{	
				if(Serial == -1)
					Serial = i;
			}
			
			m_perent->m_nSendReqNumA1++;
			if(m_perent->m_nSendReqNumA1 + m_perent->m_nSendReqNumA2 >= MAX_REQUEST_FILE * 3)
			{
				return 2;
			}
			break;
			
		case 2:	
			if(m_perent->m_nSendReqNumA2 < MAX_REQUEST_FILE)
			{
				if((pstFile->m_nNeedRequest)||
					((m_nNeedRequest == 2) && (m_stLastRecvTime.GetDuration() > 2000)))
				{
					pstFile->RequestData();
				}
				
				if(pstFile->m_stLastRecvTime.GetDuration() > NETWORK_TIME_OUT)
				{
					pstFile->m_nFailcount++;
					pstFile->m_stLastRecvTime.SetCurTickCount();
					if(pstFile->m_nFailcount > MAX_FAIL_COUNT)
					{
						printf("net work error2\n");
						pstFile->CancelUpdateFile();
						pstFile->m_nPercent = -1;
//						return -20110518;
					}
					else
						pstFile->RequestData();
				}
			}

			if(m_perent->m_nSendReqNumA1 + m_perent->m_nSendReqNumA2 == 0)
			{	
				if(Serial == -1)
					Serial = i;
			}
			
			m_perent->m_nSendReqNumA2++;
			if(m_perent->m_nSendReqNumA1 + m_perent->m_nSendReqNumA2 >= MAX_REQUEST_FILE * 3)
			{
				return 2;
			}
			break;

			
		case 3:
			if(m_perent->m_nSendReqNumA1 + m_perent->m_nSendReqNumA2 == 0)
			{	
				if(Serial == -1)
					Serial = i;
			}

			if(pstFile->m_stLastRecvTime.GetDuration() > 2000)//每两秒重新试验删除一次，删除几次不成功就替换
			{
				pstFile->m_nFailcount++;
				pstFile->m_stLastRecvTime.SetCurTickCount();
				pstFile->CompleteUpdateFile();
			}

			break;
			
		default:
			break;
		}

		if(i %5000 == 4999)
		{
			MThread::Sleep(1);
		}
	}
	
	return 1;
}

int FileList::RecvInfo(tagInfoResponse *pstResponse, tagRecvInfo * pstRecvInfo)
{
	MLocalSection	local;
	RTFileData		*pFile;
	int				i;
	
	local.Attch(&m_Section);
	if(!m_pstData)		//	GUOGUO 20110509
		return -1;
	
	if(m_nFlag < 0)
		return -1;
	
	if((pstResponse->nSerial >= m_nFileNum)||
		(m_pstData[pstResponse->nSerial]->m_sFileName != pstResponse->sFileName))
	{
		i = pstResponse->nSerial - 10;
		if(i < 0)
			i = 0;

		for(; i < m_nFileNum; i++)
		{
			if(m_pstData[i]->m_sFileName == pstResponse->sFileName)
			{
				break;
			}
		}
		if(i == m_nFileNum)//没找到
		{
			return -1;
		}
		else
		{
			pFile = m_pstData[i];
		}
	}
	else
	{
		pFile = m_pstData[pstResponse->nSerial];
	}
	if(!pFile)		//	GUOGUO 20110509
		return -1;
	
	if((pFile->m_nFlag != 1) ||
		(pFile->m_nRequestNo != pstRecvInfo->nRequestNo) ||
		(pFile->m_nFrameNo != pstRecvInfo->nFrameNo))
	{
		return -1;
	}

	if(pFile->m_nSyncMode == 1)
	{
		if((!m_AutoUpdate) || (!m_perent->m_AutoUpdateOn))
		{
			pFile->m_nNeedRequest = 1;
			pFile->m_nFailcount = 0;
			pFile->m_nPercent = 0;
			return 1;
		}
	}

	return pFile->RecvInfo(pstResponse);
}

MCounter gcount;
int	recdata = 0;
int	usedata = 0;
int FileList::RecvData(void * buf, int nLen, tagFileResponse *pstResponse, tagRecvInfo * pstRecvInfo)
{
	MLocalSection	local;
	RTFileData		*pFile;
	int				i;
	
	local.Attch(&m_Section);
	
	if(gcount.GetDuration() > 1000)
	{
		gcount.SetCurTickCount();
		printf("rec:%d, use:%d\n", recdata, usedata);
	}
	if(!m_pstData)		//	GUOGUO 20110509
		return -1;
	
	recdata += nLen;
	
	if(m_nFlag < 0)
		return -1;
	
	if((pstResponse->nSerial >= m_nFileNum)||
		(m_pstData[pstResponse->nSerial]->m_sFileName != pstResponse->sFileName))
	{
		i = pstResponse->nSerial - 10;
		if(i < 0)
			i = 0;
		for(; i < m_nFileNum; i++)
		{
			if(m_pstData[i]->m_sFileName == pstResponse->sFileName)
			{
				break;
			}
		}
		if(i == m_nFileNum)//没找到
		{
			return -1;
		}
		else
		{
			pFile = m_pstData[i];
		}
	}
	else
	{
		pFile = m_pstData[pstResponse->nSerial];
	}

	if(!pFile)			//	GUOGUO 20110509
		return -1;
	
	if((pFile->m_nFlag != 2) ||
		(pFile->m_nRequestNo != pstRecvInfo->nRequestNo) ||
		(pFile->m_nFrameNo != pstRecvInfo->nFrameNo))
	{
		return -1;
	}
	
	usedata += nLen;

	if(pFile->m_nSyncMode == 1)
	{
		if((!m_AutoUpdate) || (!m_perent->m_AutoUpdateOn))
		{
			pFile->CancelUpdateFile();
			pFile->m_nFlag = 1;
			pFile->m_nNeedRequest = 1;
			pFile->m_nFailcount = 0;
			pFile->m_nPercent = 0;
			return 1;
		}
	}
	
	return pFile->RecvData(pstResponse, (char*)buf, nLen);
}


int	FileList::ManualDownInfo(tagDownInfoReq* pstRequest, tagDownInfoRes * pstResponse)
{
	MLocalSection		local;
	int					i, n = -1;
	int					percent;
	
	local.Attch(&m_Section);

	if(m_nFlag < 0)
		return FLAG_NO_LIST;


	if(pstRequest->szCode[0] == 0)
	{
		pstResponse->nTotal = 0;
		pstResponse->nSuccCount = 0;
		pstResponse->nFailure = 0;

		for(i = 0; i < m_nFileNum; i++)
		{
			if(Global_LongKind.IsKind(m_pstData[i]->m_sFileName, pstRequest->cMarketID, pstRequest->cKindID) != 1)
				continue;

			pstResponse->nTotal++;
			if(m_pstData[i]->m_nPercent == 100)
			{
				pstResponse->nSuccCount++;
			}
			else if(m_pstData[i]->m_nPercent < 0)
			{
				pstResponse->nFailure++;
			}
			else
			{
				n = m_pstData[i]->m_nPercent;
			}
		}
	}
	else
	{
		pstResponse->nTotal = 0;
		pstResponse->nSuccCount = 0;
		pstResponse->nFailure = 0;
		for(i = 0; i < m_nFileNum; i++)
		{
			if(m_pstData[i]->m_sFileName.StringPosition(pstRequest->szCode) >=0)
			{
				if(Global_LongKind.IsKind(m_pstData[i]->m_sFileName, pstRequest->cMarketID, pstRequest->cKindID) != 1)
					continue;

				pstResponse->nTotal++;
				if(m_pstData[i]->m_nPercent == 100)
				{
					pstResponse->nSuccCount++;
				}
				else if(m_pstData[i]->m_nPercent < 0)
				{
					pstResponse->nFailure++;
				}
				else
				{
					n = m_pstData[i]->m_nPercent;
				}
			}
		}
	}

	if(pstResponse->nTotal == 0)
		return FLAG_NO_FILE;

	percent = pstResponse->nSuccCount + pstResponse->nFailure;
	percent = percent * 100 / pstResponse->nTotal;

	if((pstResponse->nTotal == 1) &&
		(n >= 0))
	{
		percent = n;
	}
	
	if(percent >= 100)
		percent = 99;
	
	if(pstResponse->nSuccCount + pstResponse->nFailure == pstResponse->nTotal)
		percent = 100;
	
	return percent;
}


int	FileList::ManualGetError(tagFailCodeReq * pstRequest, tagFailCodeRes * pstResponse, char * buf, int & buflen)
{
	/*
	FileList		*	pstList;
	MLocalSection		local;
	int					i, j, k;
	int					count;
	int					size;
	
	local.Attch(&m_Section);

	for(j = 0; j < 5; j++)
	{
		if(pstRequest->nFileMask & (1<<j))
		{	
			memset(buf, 0, buflen);
			size = 0;
			count = 0;

			for(k = pstRequest->nListSerial; k < m_nClassCount; k++)
			{
				pstList = _GetListPtr(k, pstRequest->cMarketID);
				if(!pstList)
					continue;

				if(cinfo[j] != pstList->m_stCfg.info)
				{
					continue;
				}
				
				if(pstList->nFlag < 2)
					return -3;

				pstResponse->nListCookie = pstList->nCookie;
				
				for(i = pstRequest->usSerial; i < pstList->nFileNum; i++)
				{
					if(pstList->pstData[i].m_nPercent < 0)
					{
						if(size + pstList->pstData[i].m_sFileName.GetLength() >= buflen)
						{
							buflen = size;
							pstResponse->cCount = count;
							pstResponse->nListSerial = k;
							return i;
						}

						memcpy(buf + size, pstList->pstData[i].m_sFileName.c_str(), pstList->pstData[i].m_sFileName.GetLength());

						size += pstList->pstData[i].m_sFileName.GetLength() + 1;
						count++;
					}
				}
				pstRequest->usSerial = 0;
			}

			buflen = size;
			pstResponse->cCount = count;
			return -2;
		}
	}
*/
	return -4;
}

int	FileList::ManualDown(tagDownDataRes * pstResponse)
{
	MLocalSection		local;
	int					i, num;
	
	local.Attch(&m_Section);
	
	if(m_nFlag < 0)
		return FLAG_NO_LIST;

	printf("start: %d-%d-%d\n", pstResponse->stStartTime.Year, pstResponse->stStartTime.Month, pstResponse->stStartTime.Day);
	printf("end: %d-%d-%d\n", pstResponse->stEndTime.Year, pstResponse->stEndTime.Month, pstResponse->stEndTime.Day);

	if(pstResponse->updatemode == UPDATEMODE_ZONE)
	{
		if(m_stCfg.updatemode != UPDATEMODE_RECORD)
		{
			pstResponse->updatemode = m_stCfg.updatemode;
		}
		else if(m_stCfg.info == "mon")
		{
			pstResponse->stStartTime.Hour = 0;
			pstResponse->stStartTime.Minute = 0;
			pstResponse->stStartTime.Day = 0;
			pstResponse->stEndTime.Hour = 24;
			pstResponse->stEndTime.Minute = 60;
			pstResponse->stEndTime.Day = 31;
		}
		else
		{
			pstResponse->stStartTime.Hour = 0;
			pstResponse->stStartTime.Minute = 0;
			pstResponse->stEndTime.Hour = 24;
			pstResponse->stEndTime.Minute = 60;
		}
		(*(unsigned long*)&pstResponse->stStartTime)--;
	}

	num = 0;
	for(i = 0; i < m_nFileNum; i++)
	{
		if((pstResponse->szCode[0] == 0)||
			(m_pstData[i]->m_sFileName.StringPosition(pstResponse->szCode) >=0))
		{
			if(Global_LongKind.IsKind(m_pstData[i]->m_sFileName, pstResponse->cMarketID, pstResponse->cKindID) != 1)
			{
				continue;
			}

			if(pstResponse->updatemode == UPDATEMODE_ZONE)
			{
				m_pstData[i]->m_stStart = pstResponse->stStartTime;
				m_pstData[i]->m_stEnd = pstResponse->stEndTime;
			}
			m_pstData[i]->BeginRecvInfo(0, pstResponse->updatemode, 0);
			num++;
		}
	}

	if(num == 0)
		return FLAG_NO_FILE;
	else
		return num;
}

int	FileList::ManualCancelDown(int nClassID, int  nMarketID, int nKindID)
{
	MLocalSection		local;
	int					i;

	local.Attch(&m_Section);

	if(m_nFlag < 0)
		return FLAG_NO_LIST;

	for(i = 0; i < m_nFileNum; i++)
	{
		if((m_pstData[i]->m_nFlag > 0) && (m_pstData[i]->m_nSyncMode == 0))
		{
			if(Global_LongKind.IsKind(m_pstData[i]->m_sFileName, nMarketID, nKindID) != 1)
				continue;

			m_pstData[i]->CancelUpdateFile();
			m_pstData[i]->m_nPercent = 100;
		}
	}

	return 1;
}

int	FileList::GetListInfo(tagListInfoRes * pRes)
{
	MLocalSection	local;
	int				len;

	local.Attch(&m_Section);

	if(m_nFlag < 0)
		return FLAG_NO_LIST;

	pRes->market = m_stCfg.market;

	len = m_stCfg.info.GetLength();
	if(len > 15)
		len = 15;
	memcpy(pRes->name, m_stCfg.info.c_str(), len);

	len = m_stDir.GetLength();
	if(len > 63)
		len = 63;
	memcpy(pRes->dir, m_stDir.c_str(), len);

	len = m_stCfg.info.GetLength();
	if(len > 15)
		len = 15;
	memcpy(pRes->info, m_stCfg.info.c_str(), len);

	pRes->syncmode = m_stCfg.syncmode;
	pRes->updatemode = m_stCfg.updatemode;
	pRes->timecycle = m_stCfg.cupdatecycle;
	pRes->autoupdate = m_AutoUpdate;

	return 1;
}

int	FileList::SetUpdate(tagSetUpdateRes * pRes)
{
	MLocalSection	local;

	local.Attch(&m_Section);

	if(m_nFlag < 0)
		return FLAG_NO_LIST;

	m_AutoUpdate = pRes->nFlag;

	if(pRes->nFlag == 1)//启动
	{
		m_nFileCookie = -1;
	}

	return 1;
}

int	FileList::ReFleshFile()
{
	MLocalSection	local;
	
	local.Attch(&m_Section);

	m_nFileCookie = -1;
	return 1;
}

//-------------------------------------------------------------------------------