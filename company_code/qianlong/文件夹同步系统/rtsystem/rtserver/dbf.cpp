/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		dbf.cpp
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统服务器dbf文件处理模块
	History:		
*********************************************************************************************************/
#include "Global.h"
#include "dbf.h"

RTDBF::RTDBF()
{
	m_poFile = NULL;
	memset(&m_stDbfHeadInfo, 0, sizeof(m_stDbfHeadInfo));
	m_nTimeSize = 0;
	m_nCodeSize = 0;
}

RTDBF::~RTDBF()
{
	Release();
}

int RTDBF::Instance(RTFileData * fileptr)
{
	MLocalSection			local;
	MString					sFieldName;
	MFile					aFile;
	int						ret;

	if(!fileptr)
		return FLAG_SERVER_ERROR;

	Release();

	m_poFile = fileptr;

	if(aFile.OpenRead(m_poFile->m_sFullName) != 1)
		return FLAG_NO_FILE;

	ret = GetInfo(&aFile);
	if(ret < 0)
	{
		Release();
		return ret;
	}
	return ret;
}

void RTDBF::Release()
{
	memset(&m_stDbfHeadInfo, 0, sizeof(m_stDbfHeadInfo));
	m_nTimeSize = 0;
	m_nCodeSize = 0;
}

__int64	RTDBF::Chars2Time(char * chartime)
{
	static char	yyyymmdd[9] = {0};
	static char	hhmmss[7] = {0};
	__int64 time;

	memcpy(yyyymmdd, chartime, 8);
	memcpy(hhmmss, chartime + 8, 6);

	time = __int64(MString::ToULong(yyyymmdd))*__int64(1000000)+__int64(MString::ToULong(hhmmss));

	return time;
}

MString	RTDBF::GetKey( char * rec)
{
	char scode[10] = {0};
	memcpy(scode, rec + m_nCodeOffset, m_nCodeSize);
	MString str = scode;
	return str;
}

int	RTDBF::IsCodeName(MString filed)
{
	if((filed == "STOCK_CODE")||
		(filed == "CODE")||
		(filed == "QZ_CODE"))
		return 1;
	else
		return 0;
}

int	RTDBF::IsTime(MString filed)
{
	if((filed == "UPDATETIME")||
		(filed == "MDATE")||
		(filed == "QZ_CODE"))
		return 1;
	else
		return 0;
}

int	RTDBF::GetInfo(MFile * pFile)
{
	register int			errorcode;
	register int			i;
	MLocalSection			local;
	tagDbfHeadInfo			headinfo;
	tagDbfFieldInfo			fieldInfo;
	MString					sFieldName;
	int						noffset;
	int						ret = 0;

	//读取文件头部数据
	pFile->Seek(0,0);
	if ( pFile->Read((char *)&headinfo,sizeof(tagDbfHeadInfo)) != sizeof(tagDbfHeadInfo) )
	{
		return FLAG_FILE_ERROR;
	}

	if(headinfo.lRecordCount != m_stDbfHeadInfo.lRecordCount)
	{
		ret = 1;//记录数有变
	}

	if(headinfo.sRecordSize != m_stDbfHeadInfo.sRecordSize)
	{
		ret = 2;//这些变化只能用全文件更新进行更新
	}

	memcpy(&m_stDbfHeadInfo, &headinfo, sizeof(tagDbfHeadInfo));

	if(m_stDbfHeadInfo.sRecordSize <= 0)
	{
		return FLAG_FILE_ERROR;
	}
	
	//计算DBF字段数量
	errorcode = (m_stDbfHeadInfo.sDataAddr - sizeof(tagDbfHeadInfo)) / sizeof(tagDbfFieldInfo);
	if ( errorcode <= 0 )
	{
		return FLAG_FILE_ERROR;
	}
	
	sFieldName = "UPDATETIME";
	noffset = 1;//首字节为删除标志
	for(i = 0; i < errorcode; i++)
	{
		if(pFile->Read((char*)&fieldInfo, sizeof(tagDbfFieldInfo)) != sizeof(tagDbfFieldInfo))
		{
			return FLAG_FILE_ERROR;
		}
		
		fieldInfo.lFieldOffset = noffset;
		noffset += fieldInfo.cFieldSize;
		
		if(sFieldName == fieldInfo.strFileName)
		{
			m_nTimeOffset = fieldInfo.lFieldOffset;
			m_nTimeSize = fieldInfo.cFieldSize;
		}

		if(IsCodeName(fieldInfo.strFileName))
		{
			m_nCodeOffset = fieldInfo.lFieldOffset;
			m_nCodeSize = fieldInfo.cFieldSize;
		}
	}

	return ret;
}
 
int	RTDBF::GetData(tagFileResponse * stResponse, char * buf, int &buflen, int inCall, MFile * pFile)
{//调用此函数时pFile在打开并锁定状态
	int		ret;
	int		i, j, n;
	char	TempBuf[20];
	__int64	tmptime, starttime;
	int		pos;
	char *	dst;

	int		count = 0;
	int		nCacheCount;
	int		offset;
	int		nrecsize;
	tagSortUnit * WorkBuf;

	ret = GetInfo(pFile);

	if(ret < 0)
		return ret;
	
	if((ret > 0) && (!inCall))
	{
		return FLAG_CHANGED;
	}

	nrecsize = m_stDbfHeadInfo.sRecordSize + 2;//前两字节为序号
	nCacheCount = buflen / nrecsize;
	stResponse->nFlag = 0;

	if(inCall)//内部请求，用以更新缓存
	{
		starttime = m_poFile->m_poCache->m_stOffsetTime;
	}
	else
	{
		starttime = stResponse->stOffsetTime;
	}

	WorkBuf = new tagSortUnit[m_stDbfHeadInfo.lRecordCount];
	if(!WorkBuf)
		return FLAG_SERVER_ERROR;
	
	n = 0;
	ret = 1;
	for(i = 0; i < m_stDbfHeadInfo.lRecordCount; i++)
	{
		pFile->Seek(m_stDbfHeadInfo.sDataAddr + i * m_stDbfHeadInfo.sRecordSize + m_nTimeOffset, 0);
		if(pFile->Read(TempBuf, m_nTimeSize) != m_nTimeSize)
		{
			ret = FLAG_FILE_ERROR;
			goto exit;
		}
		
		tmptime = Chars2Time(TempBuf);
		
		if(tmptime >= starttime)
		{
			WorkBuf[n].serial = i + 1;
			WorkBuf[n].time = tmptime;
			n++;
		}
	}

	if(inCall)//内部请求，用以更新缓存
	{
		for(i = 0; i < n; i++)
		{
			tmptime = 0;
			for(j = 0; j < n; j++)
			{
				if(WorkBuf[j].serial > 0)
				{
					if(WorkBuf[j].time > tmptime)
					{
						tmptime = WorkBuf[j].time;
						pos = j;
					}
				}
			}
			if(tmptime == 0)
				break;
			for(j = n - 1; j >= pos; j--)
			{
				if(WorkBuf[j].serial > 0)
				{
					if(WorkBuf[j].time == tmptime)
					{
						if(count < nCacheCount)
						{
							count++;
							dst = buf + (nCacheCount - count) * nrecsize;
							pFile->Seek(m_stDbfHeadInfo.sDataAddr + (WorkBuf[j].serial - 1) * m_stDbfHeadInfo.sRecordSize, 0);
							if(pFile->Read(dst + 2, m_stDbfHeadInfo.sRecordSize) != m_stDbfHeadInfo.sRecordSize)
							{
								ret = FLAG_FILE_ERROR;
								goto exit;
							}
							stResponse->stOffsetTime = Chars2Time(dst + 2 + m_nTimeOffset);
							stResponse->nOffset = WorkBuf[j].serial - 1;
							*(unsigned short*)dst = stResponse->nOffset;
							
							WorkBuf[j].serial = -WorkBuf[j].serial;
						}
						else
						{
							goto end;
						}
					}
				}
			}
		}
end:
		if(count < nCacheCount)
		{
			pos = (nCacheCount - count) * nrecsize;
			memmove(buf, buf + pos, buflen - pos);
		}
		buflen = count * nrecsize;
	}
	else//外部请求直接获取文件内容
	{
		if(n <= nCacheCount)
		{
			stResponse->nFlag |= (1<<2);
		}
		for(i = 0; i < n; i++)
		{
			if((stResponse->stOffsetTime == WorkBuf[i].time)&&
				(stResponse->nOffset > WorkBuf[i].serial))
			{
				WorkBuf[i].serial = -WorkBuf[i].serial;
			}
		}
		offset = stResponse->nOffset;
		for(i = 0; i < n; i++)
		{
			tmptime = 5000000000000000;
			for(j = 0; j < n; j++)
			{
				if(WorkBuf[j].serial > 0)
				{
					if(WorkBuf[j].time < tmptime)
					{
						tmptime = WorkBuf[j].time;
						pos = j;
					}
				}
			}
			if(tmptime == 5000000000000000)
				break;
			
			for(j = pos; j < n; j++)
			{
				if(WorkBuf[j].serial > 0)
				{
					if((WorkBuf[j].time == tmptime)&&
						(offset < WorkBuf[j].serial))
					{
						if(count < nCacheCount)
						{
							dst = buf + count * nrecsize;
							count++;
							pFile->Seek(m_stDbfHeadInfo.sDataAddr + (WorkBuf[j].serial - 1) * m_stDbfHeadInfo.sRecordSize, 0);
							if(pFile->Read(dst + 2, m_stDbfHeadInfo.sRecordSize) != m_stDbfHeadInfo.sRecordSize)
							{
								ret =  FLAG_FILE_ERROR;
								goto exit;
							}
							stResponse->stNextDateTime = Chars2Time(dst + 2 + m_nTimeOffset);
							stResponse->nNextOffset = WorkBuf[j].serial;
							*(unsigned short*)dst = WorkBuf[j].serial - 1;

							WorkBuf[j].serial = -WorkBuf[j].serial;
						}
						else
						{
							goto end1;
						}
					}
				}
			}
			offset = 0;
		}
end1:
		buflen = count * nrecsize;
	}

	ret = count;

exit:
	delete [] WorkBuf;
	return ret;
}
//-------------------------------------------------------------------------------