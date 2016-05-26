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

RTFileData::RTFileData()
{
	m_nFlag = -1;
	m_poCache = NULL;
	m_poDbf = NULL;
	m_nFileSize = -1;
	m_on = 0;
}

RTFileData::~RTFileData()
{
	Release();
	if(m_poCache)
	{
		delete m_poCache;
	}
	if(m_poDbf)
	{
		delete m_poDbf;
	}
}

int RTFileData::Instance(tagMFindFileInfo *info, FileList * grup)
{
	Release();

	if((!grup) || (!info))
		return -1;

	m_poGrup = grup;
	m_sFileName = info->szName;
	m_sFullName = m_poGrup->m_SrcDir + m_sFileName;
	m_stCreateTime = info->mCreateTime;
	m_stUpdateTime = info->mUpdateTime;
	m_nFileSize = info->lSize;

	if(m_stUpdateTime == 0)
	{
		printf("file %s not found\n", m_sFullName.c_str());
	}

	m_nFlag = 1;

	if(m_poGrup->m_stCfg.updatemode == UPDATEMODE_DBF)//dbf
	{
		if(!m_poDbf)
			m_poDbf = new RTDBF;
		if(!m_poDbf)
		{
			Release();
			return -1;
		}
		if(m_poDbf->Instance(this) == FLAG_SERVER_ERROR)
		{
			Release();
			return -1;
		}
	}

	if(m_poGrup->m_stCfg.cachesize > 0)
	{
		if(!m_poCache)
			m_poCache = new RTFileCache;
		if(!m_poCache)
		{
			Release();
			return -1;
		}

		if(m_poCache->Instance(this) < 0)
		{
			Release();
			return -1;
		}
	}

	return 1;
}

void RTFileData::Release()
{
	if(m_poDbf)
		m_poDbf->Release();

	if(m_poCache)
		m_poCache->Release();

	m_nFileSize = -1;
	m_nFlag = -1;
	m_on = 0;
}

int RTFileData::GetInfo(tagInfoResponse * stResponse)
{
	MFile			stFile;

	if((m_poGrup->m_stCfg.fileheadsize > 0) &&
		(m_poGrup->m_stCfg.fileheadsize <= MAX_FILE_INFO_SIZE))
	{
		stFile.OpenRead(m_sFullName);
		stFile.Seek(0, 0);
		if(stFile.Read(stResponse->ex, m_poGrup->m_stCfg.fileheadsize) != m_poGrup->m_stCfg.fileheadsize)
		{
			return FLAG_FILE_ERROR;
		}
	}

	stResponse->stUpdateTime = m_stUpdateTime.DateTimeToTimet();
	stResponse->stCreateTime = m_stCreateTime.DateTimeToTimet();
	stResponse->nFileSzie = m_nFileSize;

	stResponse->nFlag = FLAG_LAST_PACKET;
	return stResponse->nFlag;
}

int RTFileData::GetDataFromCache(tagFileResponse * stResponse, char * buf, int &buflen)
{
	if((stResponse->nFileSzie != m_nFileSize) ||
		(m_stUpdateTime != stResponse->stUpdateTime))
	{
		return FLAG_CHANGED;
	}

	stResponse->nFlag = 0;

	if(m_nFlag == 1)
	{
		if(m_poCache)
		{
			if(m_poCache->LoadBuf() > 0)
			{
				m_nFlag = 0;
			}
		}
	}
	
	if(m_nFlag == 0)
	{
		if(m_poCache && (m_poCache->GetData(stResponse, buf, buflen) >= 0))
			return stResponse->nFlag;
	}

	return FLAG_SERVER_ERROR;
}

int RTFileData::GetData(tagFileResponse * stResponse, char * buf, int & buflen, tagGetDataParam * pParam)
{
	unsigned long		nsize, ret, readsize, rret;
	int					i;
	int					recordsize;
	char			*	buf1 = buf;

	unsigned long		stDateTime;
	int					roffset;
	MFile				stFile;
	MDateTime			stTmpTime;

	stResponse->nFlag = 0;

	//从文件中获得数据
	if(stFile.OpenRead(pParam->Name) != 1)
	{
		return FLAG_NO_FILE;
	}

	if(stFile.LockFile(0, stFile.GetFileLength()) != 1)
	{
		return FLAG_SERVER_RETRY;
	}

	pParam->stCreateTime = stFile.GetCreateDateTime();
	pParam->nFileSize = stFile.GetFileLength();
	stTmpTime = stFile.GetUpdateDateTime();

	if(stTmpTime > pParam->stUpdateTime)
	{
		pParam->stUpdateTime = stTmpTime;
		if(!pParam->inCall)
		{
			stFile.UnLockFile(0, stFile.GetFileLength());
			return FLAG_CHANGED;
		}
	}
	pParam->stUpdateTime = stTmpTime;

	switch(pParam->updatemode)
	{
	case UPDATEMODE_DBF:
		if(stResponse->stOffsetTime != -1)
		{
			ret = pParam->Dbf.GetData(stResponse, buf, buflen, pParam->inCall, &stFile);

			if(ret < 0)
			{
				return ret;
			}
			else
				break;
		}

	case UPDATEMODE_RECORD:
		if(stResponse->stOffsetTime != -1)
		{
			recordsize = pParam->recordsize;

			if(pParam->inCall)
			{
				roffset = pParam->fileheadsize;
				i = (pParam->nFileSize - pParam->fileheadsize) / recordsize;
				if(i > (buflen / recordsize))
					roffset = pParam->fileheadsize + (i - (buflen / recordsize)) * recordsize;

				stFile.Seek(roffset, 0);
				
				if(stFile.Read((char*)&stDateTime, 4) != 4)
				{
					return FLAG_FILE_ERROR;
				}
				memset(&stResponse->stOffsetTime, 0, sizeof(stResponse->stOffsetTime));
				memcpy(&stResponse->stOffsetTime, &stDateTime, 4);
			}
			else
			{
				roffset = stResponse->nOffset;
				
				if(roffset != -1)
				{
					stFile.Seek(roffset, 0);
					if(stFile.Read((char*)&stDateTime, 4) != 4)
					{
						roffset = -1;
					}
					if(RTFile::IsNewer(&stDateTime, &stResponse->stOffsetTime))
					{
						if(roffset >= pParam->fileheadsize + recordsize)
						{
							stFile.Seek(roffset - recordsize, 0);
							if(stFile.Read((char*)&stDateTime, 4) != 4)
							{
								roffset = -1;
							}
							if(*(unsigned long *)&stDateTime != *(unsigned long *)&stResponse->stOffsetTime)
							{
								roffset = -1;
							}
						}
						else
						{
							roffset = -1;
						}
					}
					else
					{
						roffset = -1;
					}
				}
				
				if(roffset == -1)//无效的偏移量，需要计算偏移, 采用2分法查找。
				{//返回大于指定日期的
					int offset1, offset2;
					offset1 = 0;
					//begin lufubo add 20131008 for只包含文件头时更新失败
					if(stFile.Read((char*)&stDateTime, 4) != 4)
					{
						return FLAG_FILE_ERROR;
					}
					//end lufubo add 20131008 for只包含文件头时更新失败
					offset2 = ((pParam->nFileSize - pParam->fileheadsize) / recordsize) - 1;

					stFile.Seek(pParam->fileheadsize, 0);
					if(stFile.Read((char*)&stDateTime, 4) != 4)
					{
						return FLAG_FILE_ERROR;
					}

					if(RTFile::IsNewer(&stDateTime, &stResponse->stOffsetTime))
					{
						roffset = pParam->fileheadsize;
					}
					else
					{
						stFile.Seek(pParam->nFileSize - ((pParam->nFileSize - pParam->fileheadsize) % recordsize) - recordsize, 0);
						if(stFile.Read((char*)&stDateTime, 4) != 4)
						{
							return FLAG_FILE_ERROR;
						}
						
						if(!RTFile::IsNewer(&stDateTime, &stResponse->stOffsetTime))
						{
							roffset = pParam->nFileSize - ((pParam->nFileSize - pParam->fileheadsize) % recordsize);
						}
						else
						{
							while(1)
							{
								roffset = (offset1 + offset2)/2 * recordsize + pParam->fileheadsize;
								stFile.Seek(roffset, 0);
								if(stFile.Read((char*)&stDateTime, 4) != 4)
								{
									return FLAG_FILE_ERROR;
								}
								if(RTFile::IsNewer(&stDateTime, &stResponse->stOffsetTime))
								{
									roffset -= recordsize;
									stFile.Seek(roffset, 0);
									if(stFile.Read((char*)&stDateTime, 4) != 4)
									{
										return FLAG_FILE_ERROR;
									}
									if(!RTFile::IsNewer(&stDateTime, &stResponse->stOffsetTime))
									{
										roffset += recordsize;
										break;
									}
									else
									{
										offset2 = (offset1 + offset2)/2;
									}
								}
								else if(RTFile::IsNewer(&stResponse->stOffsetTime, &stDateTime))
								{
									roffset += recordsize;
									stFile.Seek(roffset, 0);
									//返回大于指定日期的
									if(stFile.Read((char*)&stDateTime, 4) != 4)
									{
										return FLAG_FILE_ERROR;
									}
									if(RTFile::IsNewer(&stDateTime, &stResponse->stOffsetTime))
									{
										break;
									}
									else if(RTFile::IsNewer(&stResponse->stOffsetTime, &stDateTime))
									{
										offset1 = (offset1 + offset2)/2;
									}
									else
									{
										roffset += recordsize;
										break;
									}
								}
								else
								{
									roffset += recordsize;
									break;
								}
								//if(offset1 == (offset1 + recordsize) / 2)
								 if(offset1 >= offset2)
								{
									return FLAG_FILE_ERROR;
								}
							}
						}
					}
					stResponse->nOffset = roffset;
				}
			}

			nsize = (buflen / recordsize) * recordsize;
			
			if(pParam->nFileSize - roffset <= nsize)
			{
				nsize = pParam->nFileSize - roffset;
				stResponse->nFlag |= FLAG_LAST_PACKET;
			}

			stFile.Seek(roffset, 0);
			readsize = 0;
			while(readsize < nsize){
				rret = stFile.Read(buf1, nsize - readsize);
				if(rret <= 0){
					stResponse->nFlag |= FLAG_LAST_PACKET;
					break;
				}
				readsize += rret;
				buf1 += rret;
			}
			buflen = nsize;
			stResponse->nNextOffset = stResponse->nOffset + nsize;
			memset(&stResponse->stNextDateTime, 0, sizeof(stResponse->stNextDateTime));
			memcpy(&stResponse->stNextDateTime, &buf[nsize - recordsize], 4);
	
			break;

		}

	case UPDATEMODE_FILE:
		nsize = buflen;
		if(pParam->nFileSize - stResponse->nOffset <= nsize)
		{
			nsize = pParam->nFileSize - stResponse->nOffset;
			stResponse->nFlag |= FLAG_LAST_PACKET;
		}
		stFile.Seek(stResponse->nOffset, 0);
		readsize = 0;
		while(readsize < nsize){
			rret = stFile.Read(buf1, nsize - readsize);
			if(rret <= 0){
				stResponse->nFlag |= FLAG_LAST_PACKET;
				break;
			}
			readsize += rret;
			buf1 += rret;
		}
		buflen = nsize;
		stResponse->nNextOffset = stResponse->nOffset + readsize;
		stResponse->stNextDateTime = stResponse->stOffsetTime;

		break;

	default:
		return FLAG_SERVER_ERROR;
	}

	stFile.UnLockFile(0, stFile.GetFileLength());

	return stResponse->nFlag;
}

int RTFileData::RefleshInfo()
{
	MFindFile			aFind;
	tagMFindFileInfo	info;

	if(aFind.FindFirst(m_sFullName, &info) != 1)
	{
		aFind.CloseFind();
		return FLAG_NO_FILE;
	}

	if((info.mUpdateTime > m_stUpdateTime)||
		(info.lSize != m_nFileSize))
	{
		m_nFlag = 1;
		m_stUpdateTime = info.mUpdateTime;
		m_stCreateTime = info.mCreateTime;
		m_nFileSize = info.lSize;
		printf("\n(%d, %d)%s update\n", m_poGrup->m_nClassID, m_poGrup->m_nMarketID, m_sFileName.c_str());
		aFind.CloseFind();
		return FLAG_CHANGED;
	}
	
	aFind.CloseFind();
	return 1;
}

int RTFileData::CheckUpdate(tagMFindFileInfo *info)
{
	if(info->mUpdateTime > m_stUpdateTime)
	{
		m_nFlag = 1;
		m_stUpdateTime = info->mUpdateTime;
		m_stCreateTime = info->mCreateTime;
		m_nFileSize = info->lSize;
		printf("\n(%d, %d)%s update\n", m_poGrup->m_nClassID, m_poGrup->m_nMarketID, m_sFileName.c_str());
		return 2;
	}

	return 1;
}
/*
int RTFileData::CheckCacheUpdate()
{
	if((m_poCache)&&
		(m_nFlag == 1))
	{
		while(m_poCache->LoadBuf() == -4);
		m_nFlag = 0;
	}
	return 1;
}
*/

