/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		filedata.cpp
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统服务器文件数据处理模块
	History:		
*********************************************************************************************************/
#include "filedata.h"
#include "Global.h"

FileList::FileList()
{
	m_nListCookie = 0;
	m_nFileCookie = 0;
	m_pstData = NULL;
	m_pFirstFile = NULL;
	m_nFileNum = 0;
	m_nAllocNum = 0;
	m_nIsStable = 1;
	m_perent = NULL;
	m_nFlag = -1;
	m_on = 0;
//	m_nFileCount = 0;

//	m_IsDirt = false;
//	m_nRefCounter = 0;
//	m_exclusive = false;
}

FileList::~FileList()
{
	Release();
}

int	FileList::Instance(int nClassID, int nMarketID, RTFile * perent, tagClassInfo * pinfo)
{
	MLocalSection		local;
	int					nInitFileCount;

	Release();

	local.Attch(&m_Section);

	m_nClassID = nClassID;
	m_nMarketID = nMarketID;

	m_stDir = Global_Option.GetClassDir(m_nClassID, m_nMarketID);
	m_SrcDir = Global_Option.GetSrcDir(m_nClassID, m_nMarketID);

	if(m_stDir == "-1")
	{
		m_stDir = "";
		return -1;
	}

	if(m_SrcDir == "-1")
	{
		m_SrcDir = "";
		return -1;
	}

	m_nFlag = 0;
	m_perent = perent;

	if(m_SrcDir == "")
	{
		m_SrcDir = m_perent->m_stMainPath + m_stDir;
	}

	Global_Option.GetClassCfg(nClassID, &m_stCfg);
	m_nNameLenth = m_stCfg.FileName.GetLength();

	if(m_stCfg.FileName.StringPosition("*") >= 0)
	{
		m_nIsStable = 0;
		m_stFindPath = m_SrcDir + m_stCfg.FileName;
	}
	else
	{
		m_nIsStable = 1;
	}

	memcpy(m_buf, m_stCfg.FileName.c_str(), m_nNameLenth);
	m_buf[m_nNameLenth] = 0;

	nInitFileCount = _RefleshFileCount();
	if(!m_pstData)
	{
		m_nAllocNum = nInitFileCount + nInitFileCount / 10 + 1;
		m_pstData = new RTFileData*[m_nAllocNum];
		if(!m_pstData)
		{
			Release();
			return -1;
		}
		memset(m_pstData, 0, sizeof(RTFileData*) * m_nAllocNum);
	}
//	m_nFileCount = nInitFileCount;
	printf("(%d, %d)init, dir = %s, filecount = %d\n", m_nClassID, m_nMarketID, m_SrcDir.c_str(), nInitFileCount);
	return 1;
}

void FileList::Release()
{
	MLocalSection		local;
	RTFileData * pFile, *pNextFile;
	
	local.Attch(&m_Section);

	if(m_pstData)
	{
		delete[] m_pstData;
		m_pstData = NULL;	/*	注意,野指针*/
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
	m_nFlag = -1;
	m_on = 0;
//	m_nFileCount = 0;

//	m_IsDirt = false;
//	m_nRefCounter = 0;
	//m_exclusive = false;
}


int FileList::GetData(void * buf, int & nLen, tagFileResponse *pstResponse, MString stName)
{
	MLocalSection		local;
	int					i, pos;
	int					ret;

	if(nLen <= 0)
		return FLAG_SERVER_ERROR;

	local.Attch(&m_Section);

	if(m_nFlag < 0)
	{
		return FLAG_NO_LIST;
	}

	if((pstResponse->nSerial >= m_nFileNum) ||
		(stName != m_pstData[pstResponse->nSerial]->m_sFileName))
	{
		pos = pstResponse->nSerial - 10;
		if(pos < 0)
			pos = 0;
		for(i = pos; i < m_nFileNum; i++)
		{
			if(stName == m_pstData[i]->m_sFileName)
			{
				pos = -1;
				break;
			}
		}
		if(pos >= 0)
		{
			for(i = 0; i < pos; i++)
			{
				if(stName == m_pstData[i]->m_sFileName)
				{
					pos = -1;
					break;
				}
			}
		}
		if(pos >= 0)
		{
			pstResponse->nFlag = FLAG_NO_FILE;
			return 1;
		}
	}
	else
	{
		i = pstResponse->nSerial;
	}

	pstResponse->nSerial = i;//改变序号
		
	memcpy(pstResponse->sFileName, stName.c_str(), stName.GetLength());
	pstResponse->sFileName[stName.GetLength()] = 0;

	ret = m_pstData[i]->GetDataFromCache(pstResponse, (char*)buf, nLen);

	if(ret >= 0)
	{
		return pstResponse->nFlag;
	}

	if(ret == FLAG_CHANGED)
	{
		return FLAG_CHANGED;
	}

	tagGetDataParam aParam;
	aParam.updatemode = m_stCfg.updatemode;
	aParam.Name = m_pstData[i]->m_sFullName;
	aParam.inCall = 0;
	aParam.stCreateTime = m_pstData[i]->m_stCreateTime;
	aParam.stUpdateTime = m_pstData[i]->m_stUpdateTime;
	aParam.nFileSize = m_pstData[i]->m_nFileSize;

	if((m_pstData[i]->m_poDbf) && (pstResponse->stOffsetTime != -1))
		aParam.Dbf = *(m_pstData[i]->m_poDbf);

	aParam.fileheadsize = m_stCfg.fileheadsize;
	aParam.recordsize = m_stCfg.recordsize;
	
	local.UnAttch();

	ret = RTFileData::GetData(pstResponse, (char*)buf, nLen, &aParam);

	return ret;
}

int FileList::GetInfo(tagInfoResponse *pstResponse, MString stName)
{
	MLocalSection		local;
	int					i, pos;

	local.Attch(&m_Section);

	if(m_nFlag < 0)
	{
		return FLAG_NO_LIST;
	}

	if((pstResponse->nSerial >= m_nFileNum) ||
		(stName != m_pstData[pstResponse->nSerial]->m_sFileName))
	{
		pos = pstResponse->nSerial - 10;
		if(pos < 0)
			pos = 0;
		for(i = pos; i < m_nFileNum; i++)
		{
			if(stName == m_pstData[i]->m_sFileName)
			{
				pos = -1;
				break;
			}
		}
		if(pos >= 0)
		{
			for(i = 0; i < pos; i++)
			{
				if(stName == m_pstData[i]->m_sFileName)
				{
					pos = -1;
					break;
				}
			}
		}
		if(pos >= 0)
		{
			pstResponse->nFlag = FLAG_NO_FILE;
			return 1;
		}
	}
	else
	{
		i = pstResponse->nSerial;
	}

	pstResponse->nSerial = i;//改变序号

	memcpy(pstResponse->sFileName, stName.c_str(), stName.GetLength());
	pstResponse->sFileName[stName.GetLength()] = 0;

	return m_pstData[i]->GetInfo(pstResponse);
}

int FileList::GetUpdataFileList(char * buf, int & nLen, tagListResponse * pstResponse)
{
	unsigned short		i;
	int					usesize = 0, ntemp, n = 0;
	MLocalSection		local;
	int					complete = 1;
	int					filesize;

	if(nLen <= 0)
		return FLAG_SERVER_ERROR;

	memset(buf, 0, nLen);

	local.Attch(&m_Section);

	if(m_nFlag < 0)
	{
		return FLAG_NO_LIST;
	}

	pstResponse->nFlag = 0;
	i = pstResponse->nNextSerial;
	if(i == 0)//开始包
	{
		pstResponse->nFlag |= 1;
	}
	//unsigned long | serial | filesize | filename\0
	for( ; i < m_nFileNum; i++)
	{
		ntemp = m_pstData[i]->m_sFileName.GetLength();
		if((nLen - usesize) < (sizeof(unsigned long) + sizeof(i) + sizeof(filesize) + ntemp + 1))
		{
			complete = 0;
			break;
		}
		filesize = m_pstData[i]->m_nFileSize;
		*(unsigned long *)(buf + usesize) = m_pstData[i]->m_stUpdateTime.DateTimeToTimet();
		usesize += sizeof(unsigned long);
		memcpy(buf + usesize, &i, sizeof(i));
		usesize += sizeof(i);
		memcpy(buf + usesize, &filesize, sizeof(filesize));
		usesize += sizeof(filesize);
		memcpy(buf + usesize, m_pstData[i]->m_sFileName.c_str(), ntemp);
		usesize += ntemp + 1;
		n++;
	}
	pstResponse->nFileCount = n;
	
	if(complete)
		pstResponse->nFlag |= (1<<2);
	
	pstResponse->nNextSerial = i;
	nLen = usesize;
	return pstResponse->nFlag;
}

int FileList::GetInitFileList(char * buf, int & nLen, tagListResponse * pstResponse)
{
	unsigned short		i;
	int					usesize = 0, ntemp, n = 0;
	MLocalSection		local;
	int					complete = 1;

	if(nLen <= 0)
		return FLAG_SERVER_ERROR;

	memset(buf, 0, nLen);

	local.Attch(&m_Section);

	if(m_nFlag < 0)
	{
		return FLAG_NO_LIST;
	}
	
	if((m_on) || (m_nFlag == 0))
	{
		return FLAG_CHANGING;
	}

	if(pstResponse->nListCookie != m_nListCookie)
	{
		pstResponse->nListCookie = m_nListCookie;
		return FLAG_CHANGED;
	}

	pstResponse->nFlag = 0;
	i = pstResponse->nNextSerial;

	if(i == 0)//开始包
	{
		pstResponse->nFlag |= 1;
	}

	//serial | filename\0
	for( ; i < m_nFileNum; i++)
	{
		ntemp = m_pstData[i]->m_sFileName.GetLength();
		if((nLen - usesize) < (sizeof(i) + ntemp + 1))
		{
			complete = 0;
			break;
		}

		memcpy(buf + usesize, &i, sizeof(i));
		usesize += sizeof(i);
		memcpy(buf + usesize, m_pstData[i]->m_sFileName.c_str(), ntemp);
		usesize += ntemp + 1;
		n++;
	}
	pstResponse->nFileCount = n;

	if(complete)
		pstResponse->nFlag |= (1<<2);

	pstResponse->nNextSerial = i;
	nLen = usesize;
	return pstResponse->nFlag;
}

int FileList::GetListInfo(tagListInfoResponse * pInfo, char *buf, int & nLen)
{
	MLocalSection		local;

	if(nLen <= 0)
		return FLAG_SERVER_ERROR;
	
	local.Attch(&m_Section);

	if(m_nFlag < 0)
	{
		return FLAG_NO_LIST;
	}

	pInfo->stCfg.syncmode = m_stCfg.syncmode;
	pInfo->stCfg.updatemode = m_stCfg.updatemode;
	pInfo->stCfg.fileheadsize = m_stCfg.fileheadsize;
	pInfo->stCfg.recordsize = m_stCfg.recordsize;
	pInfo->stCfg.rectimepos = m_stCfg.rectimepos;
	pInfo->stCfg.market = m_stCfg.market;
	pInfo->stCfg.cupdatecycle = m_stCfg.cupdatecycle;
	pInfo->nListCookie = m_nListCookie;
	pInfo->nFileCookie = m_nFileCookie;
	pInfo->nMarketID = m_nMarketID;
	pInfo->nFileCount = m_nAllocNum;
	if(nLen < m_stDir.GetLength())
		return FLAG_SERVER_ERROR;

	memcpy(buf, m_stDir.c_str(), m_stDir.GetLength());
	memcpy(pInfo->stCfg.info, m_stCfg.info.c_str(), 16);		//Tan
	buf[m_stDir.GetLength()] = 0;
	nLen = m_stDir.GetLength() + 1;

	return FLAG_LAST_PACKET;
}


int	FileList::_RefleshFileCount()
{
	MFindFile			aFind;
	tagMFindFileInfo	stOut;
	int					count = 0;
	int					pos = 0;

	aFind.CloseFind();
			
	while(1)
	{
		if (m_nIsStable == 0)
		{
			if(pos == 0)
			{
				if(aFind.FindFirst(m_stFindPath, &stOut) != 1)
					return count;
			}
			else
			{
				if(aFind.FindNext(&stOut) != 1)
					return count;
			}

			pos++;
			if(stOut.lAttr == MFILE_ATTR_NORMALFILE)
			{
				count++;
			}
		}
		else
		{
			while(m_buf[pos] == ' ')
			{
				pos++;
			}

			if(m_buf[pos] == 0)
				return count;

			count++;
			
			while(m_buf[pos] != ' ')
			{
				if(m_buf[pos] == 0)
					return count;

				pos++;
			}
		}
	}
	
	return count;
}

int	FileList::CheckFileUpdateTime()
{
	MLocalSection		local;
	MFindFile			aFind;
	tagMFindFileInfo	stOut;
	int					pos = 0;
	int					findpos = 0;
	int					i;
	MString				path1;
	RTFileData		**	ptmp;
	RTFileData      *	pFile;
	int					itmp;
	char				name[256];
//	int					_count = 0;

	int					filechange = 0;
	int					listchange = 0;
	
	local.Attch(&m_Section);

	if(m_nFlag < 0)
		return -1;

	if(m_time.GetDuration() < m_stCfg.supdatecycle)
	{
		return 1;
	}
	if(m_on == 1)
		return 1;

	m_on = 1;
	local.UnAttch();

//单线程
	
	while(1)
	{
		if (m_nIsStable == 0)
		{
			if(pos == 0)
			{
				aFind.CloseFind();
				if(aFind.FindFirst(m_stFindPath, &stOut) != 1)
					break;
			}
			else
			{
				if(aFind.FindNext(&stOut) != 1)
					break;
			}
			
			pos++;
			if(stOut.lAttr != MFILE_ATTR_NORMALFILE)
			{
				continue;
			}
		}
		else
		{
			while(m_buf[pos] == ' ')
			{
				pos++;
			}
			
			if(m_buf[pos] == 0)
				break;
			
			i = 0;
			while(m_buf[pos] != ' ')
			{
				if(m_buf[pos] == 0)
					break;
				name[i] = m_buf[pos];
				pos++;
				i++;
			}

			name[i] = 0;
			path1 = m_SrcDir + name;
			aFind.CloseFind();
			if(aFind.FindFirst(path1, &stOut) != 1)
			{
				memcpy(stOut.szName, name, i);
				stOut.szName[i] = 0;
				stOut.mCreateTime = 0;
				stOut.mUpdateTime = 0;
			}
		}

		local.Attch(&m_Section);

		if((findpos >= m_nFileNum)||
			(m_pstData[findpos]->m_sFileName != stOut.szName))
		{//现考虑findnext不会重复
			for(i = findpos; i < m_nFileNum; i++)
			{
				if(m_pstData[i]->m_sFileName == stOut.szName)
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
				
				pFile->Instance(&stOut, this);

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
				m_nFileNum++;
				filechange = 1;
				listchange = 1;
				
			}
			else
			{	//位置错乱，交换
				listchange = 1;
				pFile = m_pstData[findpos];
				m_pstData[findpos] = m_pstData[i];
				m_pstData[i] = pFile;
				if(m_pstData[findpos]->CheckUpdate(&stOut) == 2)
				{
					filechange = 1;
				}
			}
		}
		else
		{
			if(m_pstData[findpos]->CheckUpdate(&stOut) == 2)
			{
				filechange = 1;
			}
		}

		findpos++;
		local.UnAttch();

		if(findpos%100 == 0)
			MThread::Sleep(1);
	}

	local.Attch(&m_Section);

	if(findpos < m_nFileNum)
	{
		listchange = 1;
		itmp = findpos;
		for(; findpos < m_nFileNum; findpos++)
		{
			m_pstData[findpos]->Release();
		}
		m_nFileNum = itmp;
	}

	m_on = 0;
	m_time.SetCurTickCount();
	if(m_nFlag == 0)
	{
		printf("(%d, %d)first check complete\n", m_nClassID, m_nMarketID);
	}

	m_nFlag = 1;
	m_nListCookie += listchange;
	m_nFileCookie += filechange;
	if(m_nListCookie < 0)
		m_nListCookie = 0;
	if(m_nFileCookie < 0)
		m_nFileCookie = 0;
	local.UnAttch();

	return 1;
}

#if 0
//	type 0 表示 读引用,1 表示写引用
//	如果是读引用,受到m_IsDirt 控制,如果是脏的那么就不允许再用了
//	如果是写引用,受到m_exclusive控制,如果是已经互斥,说明已经有人在用了,自己放弃使用它
bool FileList::IncrementRefCount(int type)
{
	MLocalSection		local;

	local.Attch(&m_Section);

	if(type)
	{
		if(m_exclusive)
			return false;
		else
		{
			m_exclusive = true;
			return true;
		}
	}
	else
	{
		if(m_IsDirt || m_exclusive) {
			/*	wait,如果返回NULL,会遭成CLIENT不再请求了,说实话整套程序逻辑很乱,稍微一点错误的改动都可能导致新问题产生
				一定要仔细修改任何的标志,目前也只有这样处理了*/
			while(m_IsDirt || m_exclusive)
			{
				local.UnAttch();
				MThread::Sleep(100);
				local.Attch(&m_Section);
			}
		}
		
		m_nRefCounter++;
		
		return true;
	}
}

void FileList::DecrementRefCount(int type)
{
	MLocalSection		local;

	local.Attch(&m_Section);

	if(type)
	{
		m_exclusive = false;
	}
	else
	{
		m_nRefCounter--;
	}
}

void FileList::Invalidself()
{
	MLocalSection		local;

	local.Attch(&m_Section);

	m_IsDirt = true;

	local.UnAttch();

	//	DEBUG,办法
	//MThread::Sleep(30000);
}

int FileList::GetRefCount()
{
	MLocalSection		local;

	local.Attch(&m_Section);

	return m_nRefCounter;
}
#endif
//-------------------------------------------------------------------------------