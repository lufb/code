/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		FileCache.cpp
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统服务器文件缓存处理模块
	History:		
*********************************************************************************************************/
#include "Global.h"
#include "FileCache.h"

RTFileCache::RTFileCache()
{
	m_cBuf = NULL;
	m_nBufSize = 0;
	m_poFile = NULL;
	m_nHandle = 0;
	m_nDataSize = 0;
	m_stOffsetTime = 0;
}

RTFileCache::~RTFileCache()
{
	Release();
	if(m_cBuf)
	{
		free(m_cBuf);
	}
}

int RTFileCache::Instance(RTFileData * fileptr)
{
	if(!fileptr)
		return -1;

	Release();

	m_poFile = fileptr;
	return 1;
}

void RTFileCache::Release()
{
	m_poFile = NULL;
	m_nHandle = 0;
	m_nDataSize = 0;
	m_stOffsetTime = 0;
}

int RTFileCache::LoadBuf()
{
	tagFileResponse stResponse;
	int				needsize;
	int				ret;
	int				nrecsize;
	tagGetDataParam aParam;

	if(m_poFile->m_nFileSize == -1)
	{
		m_nBufSize = 0;
		m_nDataSize = 0;
		return 0;
	}

	aParam.Name = m_poFile->m_sFullName;
	aParam.stCreateTime = m_poFile->m_stCreateTime;
	aParam.stUpdateTime = m_poFile->m_stUpdateTime;
	aParam.nFileSize = m_poFile->m_nFileSize;
	aParam.updatemode = m_poFile->m_poGrup->m_stCfg.updatemode;
	aParam.inCall = 1;
	
	switch(m_poFile->m_poGrup->m_stCfg.updatemode)
	{
	case UPDATEMODE_FILE:
		if(m_poFile->m_nFileSize > m_poFile->m_poGrup->m_stCfg.cachesize)
		{
			m_nBufSize = 0;
			return -1;
		}

		if(m_cBuf && (m_nBufSize < m_poFile->m_nFileSize))
		{
			delete [] m_cBuf;
			m_cBuf = NULL;
			m_nBufSize = 0;
		}

		if(!m_cBuf)
		{
			m_cBuf = new char[m_poFile->m_nFileSize];
			if(!m_cBuf)
			{
				m_nBufSize = 0;
				m_nDataSize = 0;
				return -1;
			}
			else
			{
				m_nBufSize = m_poFile->m_nFileSize;
			}
		}

		memset(m_cBuf, 0, m_nBufSize);
		m_nOffset = max(m_poFile->m_nFileSize - m_nBufSize, 0);
		stResponse.nOffset = m_nOffset;
		m_nDataSize = m_nBufSize;
		ret = m_poFile->GetData(&stResponse, m_cBuf, m_nDataSize, &aParam);

		if(aParam.stUpdateTime > m_poFile->m_stUpdateTime)
		{
			m_poFile->m_stUpdateTime = aParam.stUpdateTime;
			m_poFile->m_stCreateTime = aParam.stCreateTime;
			m_poFile->m_nFileSize = aParam.nFileSize;
		}

		if(ret <= 0)
		{
			m_nDataSize = 0;
		}
		break;

	case UPDATEMODE_RECORD:
		needsize = (m_poFile->m_nFileSize - m_poFile->m_poGrup->m_stCfg.fileheadsize) / m_poFile->m_poGrup->m_stCfg.recordsize;
		if(needsize > m_poFile->m_poGrup->m_stCfg.cachesize)
		{
			needsize = m_poFile->m_poGrup->m_stCfg.cachesize;
		}
		needsize *= m_poFile->m_poGrup->m_stCfg.recordsize;

		if(m_cBuf && (m_nBufSize < needsize))
		{
			delete [] m_cBuf;
			m_cBuf = NULL;
			m_nBufSize = 0;
		}

		if(!m_cBuf)
		{
			m_cBuf = new char[needsize];
			if(!m_cBuf)
			{
				m_nBufSize = 0;
				m_nDataSize = 0;
				return -1;
			}
			else
			{
				m_nBufSize = needsize;
			}
		}

		memset(m_cBuf, 0, m_nBufSize);
		stResponse.stOffsetTime = 0;
		m_nDataSize = m_nBufSize;
		aParam.fileheadsize = m_poFile->m_poGrup->m_stCfg.fileheadsize;
		aParam.recordsize = m_poFile->m_poGrup->m_stCfg.recordsize;
		ret = m_poFile->GetData(&stResponse, m_cBuf, m_nDataSize, &aParam);

		if(aParam.stUpdateTime > m_poFile->m_stUpdateTime)
		{
			m_poFile->m_stUpdateTime = aParam.stUpdateTime;
			m_poFile->m_stCreateTime = aParam.stCreateTime;
			m_poFile->m_nFileSize = aParam.nFileSize;
		}
		if(ret <= 0)
		{
			m_nDataSize = 0;
		}
		else
		{
			m_stOffsetTime = stResponse.stOffsetTime;
		}
		break;

	case UPDATEMODE_DBF:
		nrecsize = m_poFile->m_poDbf->m_stDbfHeadInfo.sRecordSize + 2;//加上两字节文件内序号
		needsize = m_poFile->m_nFileSize / nrecsize;
		if(needsize > m_poFile->m_poGrup->m_stCfg.cachesize)
		{
			needsize = m_poFile->m_poGrup->m_stCfg.cachesize;
		}
		needsize *= nrecsize;
		
		if(m_cBuf && (m_nBufSize < needsize))
		{
			delete [] m_cBuf;
			m_cBuf = NULL;
			m_nBufSize = 0;
		}
		
		if(!m_cBuf)
		{
			m_cBuf = new char[needsize];
			if(!m_cBuf)
			{
				m_nBufSize = 0;
				m_nDataSize = 0;
				return -1;
			}
			else
			{
				m_nBufSize = needsize;
			}
		}
		
		memset(m_cBuf, 0, m_nBufSize);
		stResponse.nOffset = -1;
		m_nDataSize = m_nBufSize;
		aParam.Dbf = *m_poFile->m_poDbf;
		ret = m_poFile->GetData(&stResponse, m_cBuf, m_nDataSize, &aParam);
		if(aParam.stUpdateTime > m_poFile->m_stUpdateTime)
		{
			m_poFile->m_stUpdateTime = aParam.stUpdateTime;
			m_poFile->m_stCreateTime = aParam.stCreateTime;
			m_poFile->m_nFileSize = aParam.nFileSize;
		}
		if(ret < 0)
		{
			m_nDataSize = 0;
		}
		else
		{
			m_stOffsetTime = stResponse.stOffsetTime;
			m_nOffset = stResponse.nOffset;
		}
		break;

	default:
		break;
	}

	return ret;
}

int RTFileCache::GetData(tagFileResponse * stResponse, char * buf, int & buflen)
{
	int			offset, size;
	__int64		time;
	int			i;
	int			nrecsize;
	unsigned	long	nexttime;

	if((!m_cBuf) ||
		(!m_nDataSize) ||
		(!m_nBufSize))
	{
		return -1;
	}

	switch(m_poFile->m_poGrup->m_stCfg.updatemode)
	{
	case UPDATEMODE_FILE:
		offset = stResponse->nOffset - m_nOffset;

		if(offset < 0)
			return -1;

		size = min(buflen, m_nDataSize - offset);

		if(size <= 0)
		{
			stResponse->nFlag |= (1<<2);
			buflen = 0;
			stResponse->nNextOffset = stResponse->nOffset;
			break;
		}
		memcpy(buf, m_cBuf + offset, size);
		
		if(m_nDataSize - offset <= buflen)
		{
			stResponse->nFlag |= (1<<2);
		}

		buflen = size;
		stResponse->stNextDateTime = stResponse->stOffsetTime;
		stResponse->nNextOffset = stResponse->nOffset + size;

		break;

	case UPDATEMODE_RECORD:
		if(stResponse->stOffsetTime == -1)//执行全文件请求只从文件获得
			return -1;

		if(RTFile::IsNewer(&m_stOffsetTime, &stResponse->stOffsetTime))
			return -1;

		nrecsize = m_poFile->m_poGrup->m_stCfg.recordsize;

		offset = stResponse->nOffset;
		if(offset != -1)
		{
			if(offset > m_nDataSize)
			{
				offset = -1;
			}
			else
			{
				if(RTFile::IsNewer(m_cBuf + offset, &stResponse->stOffsetTime))
				{
					if(offset >= nrecsize)
					{
						if(*(unsigned long *)(m_cBuf + offset - nrecsize) != *(unsigned long *)&stResponse->stOffsetTime)
						{
							offset = -1;
						}
					}
					else
					{
						offset = -1;
					}
				}
				else
				{
					offset = -1;
				}
			}
		}
		if(offset == -1)//无效的偏移量，需要计算偏移, 采用2分法查找。
		{//返回大于指定日期的
			int offset1, offset2;
			offset1 = 0;
			offset2 = (m_nDataSize - (m_nDataSize % nrecsize)) / nrecsize - 1;
			
			if(!RTFile::IsNewer(m_cBuf + offset2 * nrecsize, &stResponse->stOffsetTime))
			{
				offset = -1;
			}
			else
			{
				while(1)
				{
					offset = (offset1 + offset2)/2 * nrecsize;
					if(RTFile::IsNewer(m_cBuf + offset, &stResponse->stOffsetTime))
					{
						offset -= nrecsize;
						if(!RTFile::IsNewer(m_cBuf + offset, &stResponse->stOffsetTime))
						{
							offset += nrecsize;
							break;
						}
						else
						{
							offset2 = (offset1 + offset2)/2;
						}
					}
					else if(RTFile::IsNewer(&stResponse->stOffsetTime, m_cBuf + offset))
					{
						offset += nrecsize;
						if(RTFile::IsNewer(m_cBuf + offset, &stResponse->stOffsetTime))
						{
							break;
						}
						else if(RTFile::IsNewer(&stResponse->stOffsetTime, m_cBuf + offset))
						{
							offset1 = (offset1 + offset2)/2;
						}
						else
						{
							offset += nrecsize;
							break;
						}
					}
					else
					{
						offset += nrecsize;
						break;
					}
					if(offset1 == (offset1 + nrecsize) / 2)
					{
						return FLAG_FILE_ERROR;
					}
				}
			}
			stResponse->nOffset = offset;
		}
		
		if(offset < 0)
		{
			stResponse->nFlag |= (1<<2);
			buflen = 0;
			stResponse->stNextDateTime = stResponse->stOffsetTime;
			stResponse->nNextOffset = stResponse->nOffset;
			break;
		}

		buflen = buflen / nrecsize * nrecsize;

		if(m_nDataSize - offset <= buflen)
		{
			stResponse->nFlag |= (1<<2);
			buflen = m_nDataSize - offset;
		}
		
		memcpy(buf, m_cBuf + offset, buflen);
		memcpy(&nexttime, buf + buflen - nrecsize, 4);
		memset(&stResponse->stNextDateTime, 0, sizeof(stResponse->stNextDateTime));
		memcpy(&stResponse->stNextDateTime, &nexttime, 4);
		stResponse->nNextOffset = stResponse->nOffset + buflen;

		break;

	case UPDATEMODE_DBF:
		if(stResponse->stOffsetTime == -1)//执行全文件请求只从文件获得
			return -1;

		nrecsize = m_poFile->m_poDbf->m_stDbfHeadInfo.sRecordSize + 2;
		if(m_stOffsetTime > stResponse->stOffsetTime)
			return -1;

		if((m_stOffsetTime == stResponse->stOffsetTime)&&
			(*(unsigned short*)m_cBuf > stResponse->nOffset))
			return -1;

		offset = -1;
		for(i = 0; i < m_nDataSize / nrecsize; i++)
		{
			time = RTDBF::Chars2Time(m_cBuf + nrecsize * i + 2 + m_poFile->m_poDbf->m_nTimeOffset);
			if((time == stResponse->stOffsetTime)&&
				(*(unsigned short*)(m_cBuf + nrecsize * i) >= stResponse->nOffset))
			{
				offset = i;
				break;
			}

			if(time > stResponse->stOffsetTime)
			{
				offset = i;
				break;
			}
		}

		if(offset < 0)
		{
			stResponse->nFlag |= (1<<2);
			buflen = 0;
			stResponse->stNextDateTime = stResponse->stOffsetTime;
			stResponse->nNextOffset = stResponse->nOffset;
			break;
		}

		offset *= nrecsize;

		if(m_nDataSize - offset <= buflen)
		{
			stResponse->nFlag |= (1<<2);
			buflen = m_nDataSize - offset;
		}

		memcpy(buf, m_cBuf + offset, buflen);

		time = RTDBF::Chars2Time(buf + buflen - nrecsize + 2 + m_poFile->m_poDbf->m_nTimeOffset);
		stResponse->stNextDateTime = time;
		stResponse->nNextOffset = *(unsigned short*)(buf + buflen - nrecsize) + 1;

		break;

	default:
		break;
	}

	return 1;
}


//-------------------------------------------------------------------------------