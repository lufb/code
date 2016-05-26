/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		dbf.cpp
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统接收机dbf文件处理模块
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
	m_nLastRecTime = 0;
}
 
RTDBF::~RTDBF()
{
	Release();
}

int RTDBF::Instance(RTFileData * fileptr, MFile * pFile)
{
	register int			i;
	MLocalSection			local;
	MString					sFieldName;
	char					TempBuf[20];
	__int64					tmptime;
	MFile					aFile;
	int						ret;

	if(!fileptr)
		return -1;

	Release();

	m_poFile = fileptr;

	if(!pFile)
	{
		if(aFile.OpenRead(m_poFile->m_poGrup->m_stDir + m_poFile->m_sFileName) != 1)
			return 0;
		pFile = &aFile;
	}

	ret = GetInfo(pFile);
	if(ret < 0)
	{
		Release();
		return 0;
	}

	for(i = 0; i < m_stDbfHeadInfo.lRecordCount; i++)
	{
		pFile->Seek(m_stDbfHeadInfo.sDataAddr + i * m_stDbfHeadInfo.sRecordSize + m_nTimeOffset, 0);
		if(pFile->Read(TempBuf, m_nTimeSize) != m_nTimeSize)
		{
			Release();
			return 0;
		}
		
		tmptime = Chars2Time(TempBuf);
		
		if(tmptime > m_nLastRecTime)
		{
			m_nLastRecTime = tmptime;
		}
	}

	return ret + 1;
}

void RTDBF::Release()
{
	memset(&m_stDbfHeadInfo, 0, sizeof(m_stDbfHeadInfo));
	m_nTimeSize = 0;
	m_nCodeSize = 0;
	m_nLastRecTime = 0;
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
		return -5;
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
		return -5;
	}
	
	//计算DBF字段数量
	errorcode = (m_stDbfHeadInfo.sDataAddr - sizeof(tagDbfHeadInfo)) / sizeof(tagDbfFieldInfo);
	if ( errorcode <= 0 )
	{
		return -5;
	}
	
	sFieldName = "UPDATETIME";
	noffset = 1;//首字节为删除标志
	for(i = 0; i < errorcode; i++)
	{
		if(pFile->Read((char*)&fieldInfo, sizeof(tagDbfFieldInfo)) != sizeof(tagDbfFieldInfo))
		{
			return -5;
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

int	RTDBF::RecvData(tagFileResponse * stResponse, char * buf, int buflen, MFile * fileptr)
{//调用此函数时临时文件在打开写状态
	int		ret;
	int		i, j;
	char	TempBuf[20] = {0};
	MString	str;
	__int64	lasttime = 0;

	int		count = 0;

	int		offset;
	int		nrecsize = m_stDbfHeadInfo.sRecordSize + 2;//前两字节为记录序号

	if(m_stDbfHeadInfo.sRecordSize <= 0)
	{
		fileptr->Close();
		m_poFile->BeginUpdateFile(-1, 0);
		return -1;
	}

	if(buflen%nrecsize != 0)//这些情况下应该全文件更新
	{
		fileptr->Close();
		m_poFile->BeginUpdateFile(-1, 0);
		return -1;
	}

	for(i = 0; i < buflen / m_stDbfHeadInfo.sRecordSize; i++)
	{
		j = *(unsigned short*)(buf + i * nrecsize);

		offset = m_stDbfHeadInfo.sDataAddr + j * m_stDbfHeadInfo.sRecordSize + m_nCodeOffset;
		if(offset >= m_poFile->m_nFileSizeTmp)
			return -1;

		fileptr->Seek(offset, 0);
		ret = fileptr->Read(TempBuf, m_nCodeSize);
		if(ret == m_nCodeSize)
		{
			str = TempBuf;
			if((str == GetKey(buf + i * nrecsize + 2))||
				(str == ""))
			{
				offset = m_stDbfHeadInfo.sDataAddr + j * m_stDbfHeadInfo.sRecordSize;
				fileptr->Seek(offset, 0);
				fileptr->Write(buf + i * nrecsize + 2, m_stDbfHeadInfo.sRecordSize);
				lasttime = Chars2Time(buf + i * nrecsize + 2 + m_nTimeOffset);
			}
			else
			{
				fileptr->Close();
				m_poFile->BeginUpdateFile(-1, 0);
				return -1;
			}
		}
	}

	if((stResponse->nFlag & (1<<2)) && (lasttime != 0))
	{//更新文件最新记录时间
		m_nLastRecTime = lasttime; 
	}

	return count;
}
//-------------------------------------------------------------------------------