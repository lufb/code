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


RTFileData::RTFileData()
{
	m_nFlag = -1;
	m_nNeedRequest = 0;	
	m_nFileSizeTmp = -1;
	m_poGrup = NULL;
	m_poDbf = NULL;
	m_nPercent = 100;
	m_nPercent = -1;
	m_nFailcount = 0;
}

RTFileData::~RTFileData()
{
	Release();
}

int RTFileData::Instance(MString sname, FileList * grup)
{
	int			doclean = 1;

	Release();

	if(!grup)
		return -1;

	m_sFileName = sname;
	m_sFileNameTmp = sname + ".tmp";
	m_poGrup = grup;
	m_nSerial = -1;
	m_nFlag = 0;
	m_nSyncMode = m_poGrup->m_stCfg.syncmode;

	if(MFile::IsExist(m_poGrup->m_stDir + m_sFileNameTmp))
		MFile::DeleteFile(m_poGrup->m_stDir + m_sFileNameTmp);

	RefleshInfo();

	if(m_poGrup->m_stCfg.updatemode == UPDATEMODE_DBF)
	{
		m_poDbf = new RTDBF;
		if(!m_poDbf)
		{
			Release();
			return -1;
		}
		if(m_poDbf->Instance(this) < 0)
		{
			Release();
			return -1;
		}
	}

	return 1;
}

void RTFileData::Release()
{
	if(m_poGrup)
	{
		if(MFile::IsExist(m_poGrup->m_stDir + m_sFileNameTmp))
			MFile::DeleteFile(m_poGrup->m_stDir + m_sFileNameTmp);
	}

	if(m_poDbf)
		m_poDbf->Release();

	m_nFlag = -1;
	m_poGrup = NULL;
	m_nNeedRequest = 0;	
	m_nFileSizeTmp = -1;
	m_nFailcount = 0;
	m_nPercent = 100;
	m_nSerial = -1;
	m_nSyncMode = 0;
}

int	RTFileData::SetSerial(int serial)
{
	m_nSerial = serial;
	return 1;
}

int RTFileData::CheckUpdate(unsigned long time, int size, int serial)
{
	if(m_nFlag < 0)
		return -1;

	m_nSerial = serial;
	
	if(m_stUpdateTime != time)			//	GUOGUO 20110518
	{
		BeginRecvInfo(1, -1, time);
		return 2;
	}
	
	return 1;
}

int RTFileData::RecvInfo(tagInfoResponse * stResponse)
{
	int		ret;
	MFile	stFile;

	if(m_nFlag != 1)
		return -1;

	if(m_nNeedRequest)
		return -1;

	if(stResponse->nFlag == FLAG_NEED_TOTAL)
	{
		m_nUpdateMode = UPDATEMODE_FILE;
	}
	else if(stResponse->nFlag == FLAG_FILE_ERROR)
	{
		CancelUpdateFile();//出错，取消更新
		m_nPercent = stResponse->nFlag;
		return -1;
	}

	if((m_nSyncMode == 1) && (m_stUpdateTime >= stResponse->stUpdateTime) && (m_nFileSize == stResponse->nFileSzie))
	{
		CancelUpdateFile();
		m_nPercent = 100;
		return 1;
	}

	m_stUpdateTimeTmp = stResponse->stUpdateTime;
	m_stCreateTimeTmp = stResponse->stCreateTime;
	m_nFileSizeTmp = stResponse->nFileSzie;
	m_nSerial = stResponse->nSerial;
	//此处可能根据得到的文件信息修改更新方式//Tan


	ret =  BeginUpdateFile(m_nSyncMode, m_nUpdateMode);

	if(ret < 0)
	{
		CancelUpdateFile();
		m_nPercent = FLAG_CLIENT_ERROR;
		return -1;
	}
	else
	{
		if(m_nUpdateMode == UPDATEMODE_ZONE)//历史数据区间下载
		{
			stFile.OpenWrite(m_poGrup->m_stDir + m_sFileNameTmp);
			if(stFile.Write(stResponse->ex, m_poGrup->m_stCfg.fileheadsize) != m_poGrup->m_stCfg.fileheadsize)
			{
				printf("write file error\n");
				CancelUpdateFile();
				m_nPercent = FLAG_CLIENT_ERROR;
				return -1;
			}
		}
	}

	m_stLastRecvTime.SetCurTickCount();
	return ret;
}

int RTFileData::RecvData(tagFileResponse * pstResponse, char * buf, int buflen)
{
	MFile stFile;
	unsigned long nsize, writesize;
	int ret, wret;
	int		i;

	if(m_nFlag != 2)
		return -1;

	if(m_nNeedRequest)
		return -1;

	m_nSerial = pstResponse->nSerial;
	
	if(pstResponse->nFlag == FLAG_NEED_TOTAL)
	{
		return BeginUpdateFile(-1, 0);
	}
	else if(pstResponse->nFlag == FLAG_SERVER_RETRY)
	{
		m_nNeedRequest = 2;
		m_stLastRecvTime.SetCurTickCount();
		return 1;
	}
	else if(pstResponse->nFlag == FLAG_CHANGED)
	{//重新开始更新文件。
		m_nFlag = 1;
		m_nNeedRequest = 1;	
		m_nFailcount = 0;
		m_nPercent = 0;
		return 1;
	}
	else if(pstResponse->nFlag < 0)
	{
		CancelUpdateFile();//出错，取消更新
		m_nPercent = pstResponse->nFlag;
		return -1;
	}

	if((pstResponse->nOffset != m_nOffset) && (m_nUpdateMode == UPDATEMODE_FILE))
	{
		return -1;
	}
	if((pstResponse->stOffsetTime != m_stTimeOffset) && (m_nUpdateMode != UPDATEMODE_FILE) && (m_stTimeOffset != -1))
	{
		return -1;
	}

	if(stFile.OpenWrite(m_poGrup->m_stDir + m_sFileNameTmp) != 1)
	{
		MString		strtmp;
		strtmp = m_poGrup->m_stDir + m_sFileNameTmp;
		slib_WriteError( Global_UnitNo, 0, "写数据前,打开文件(%s)失败,错误原因:%s",
			strtmp.c_str(), MErrorCode::GetErrString( MErrorCode::GetSysErr() ).c_str());
		return -1;
	}

	writesize = 0;

	switch(m_nUpdateMode)
	{
	case UPDATEMODE_DBF:
		if(pstResponse->stOffsetTime != -1)
		{
			ret = m_poDbf->RecvData(pstResponse, buf, buflen, &stFile);
			m_nPercent = 50;
			break;
		}

	case UPDATEMODE_ZONE:
	case UPDATEMODE_RECORD:
		if(pstResponse->stOffsetTime != -1)
		{
			ret = 1;
			wret = m_nFileSizeTmp - m_nFileSize;
			if(wret <= 0)
				m_nPercent = 50;
			else
				m_nPercent = (__int64)(pstResponse->nOffset + buflen - m_nFileSize) * 100 / wret;

			if(m_nPercent >= 100)
				m_nPercent = 99;
			if(m_nPercent <= 0)
				m_nPercent = 50;

			if(m_nUpdateMode == UPDATEMODE_ZONE)//历史文件区间下载
			{
				if((buflen >= m_poGrup->m_stCfg.recordsize) && (RTFile::IsNewer(&buf[buflen - m_poGrup->m_stCfg.recordsize + m_poGrup->m_stCfg.rectimepos], &m_stEnd)))
				{
					for(i = 0; i < buflen / m_poGrup->m_stCfg.recordsize; i++)
					{
						if(RTFile::IsNewer(&buf[i * m_poGrup->m_stCfg.recordsize + m_poGrup->m_stCfg.rectimepos], &m_stEnd))
						{
							buflen = i * m_poGrup->m_stCfg.recordsize;
							pstResponse->nFlag |= (1<<2);
						}
					}
				}
				if(buflen)
				{
					nsize = buflen;
					stFile.Seek(0, 2);
					while(writesize < nsize){
						wret = stFile.Write(buf, nsize - writesize);
						if(wret <= 0){
							break;
						}
						writesize += wret;
						buf += wret;
					}
				}
			}
			else
			{
				nsize = buflen;
				stFile.Seek(0, 2);
				while(writesize < nsize){
					wret = stFile.Write(buf, nsize - writesize);
					if(wret <= 0){
						break;
					}
					writesize += wret;
					buf += wret;
				}
			}
			break;
		}

	case UPDATEMODE_FILE:
		ret = 1;
		if(m_nFileSizeTmp <= 0)
			m_nPercent = -1;
		else
			m_nPercent = (__int64)(pstResponse->nOffset + buflen) * 100 / m_nFileSizeTmp;

		if(m_nPercent >= 100)
			m_nPercent = 99;
		
		nsize = buflen;
		stFile.Seek(pstResponse->nOffset, 0);
		while(writesize < nsize){
			wret = stFile.Write(buf, nsize - writesize);
			if(wret <= 0){
				break;
			}
			writesize += wret;
			buf += wret;
		}
		break;

	default:
		break;
	}

	stFile.Close();
	
	if(ret >= 0)
	{
		if(pstResponse->nFlag & (1<<2))//完成
		{
			ret = CompleteUpdateFile();
			if(ret == -1)
			{
				CancelUpdateFile();
				m_nPercent = FLAG_CLIENT_ERROR;
			}
		}
		else
		{
			if(m_nOffset == pstResponse->nNextOffset)
			{
				slib_WriteError( Global_UnitNo, 0, "文件%s长度有异常 (%d %d)",
					m_sFileName.c_str(), m_nOffset, pstResponse->nNextOffset);
				CancelUpdateFile();
				m_nPercent = FLAG_CLIENT_ERROR;
				return	-1;
			}

			m_nOffset = pstResponse->nNextOffset;
			m_stTimeOffset = pstResponse->stNextDateTime;
			if(pstResponse->nFlag & (1<<1))//需要接着请求
			{
				//m_nNeedRequest = 1;
				RequestData();
			}
			else//等待下一帧即可
			{
				m_nFrameNo++;
			}
		}
	}

	m_stLastRecvTime.SetCurTickCount();
	m_nFailcount = 0;
	return 1;
}

int RTFileData::BeginRecvInfo(int nSyncMode, int nUpdateMode, unsigned long time)
{
	if(m_nFlag == -1)
		return -1;

	if(nSyncMode == -1)
	{
		nSyncMode = m_poGrup->m_stCfg.syncmode;
	}
	
	if(nUpdateMode == -1)
	{
		nUpdateMode = m_poGrup->m_stCfg.updatemode;
	}

	//调整更新方式
	if((nUpdateMode != UPDATEMODE_FILE) &&
		(nUpdateMode != UPDATEMODE_ZONE) &&
		(nUpdateMode != m_poGrup->m_stCfg.updatemode))
	{
		nUpdateMode = m_poGrup->m_stCfg.updatemode;
	}

	if(m_nFlag >= 1){//正在更新过程中
		if((m_nSyncMode == 0) && (nSyncMode == 1))
		{
			return 1;//手动过程中不允许自动更新
		}

		if(((nSyncMode == -1)||(m_nSyncMode == nSyncMode))&&//更新方式一致
			((nUpdateMode == -1)||(m_nUpdateMode == nUpdateMode))&&//更新方式一致
			((time == 0) || (m_stUpdateTimeTmp == time)))//更新时间一致
		{
			return 1;//不允许重复发相同的更新请求，包括手动
		}
	}

	//新的请求，为获得文件信息重置所有标志
	m_nFlag = 1;
	m_nSyncMode = nSyncMode;
	m_nUpdateMode = nUpdateMode;
	m_nNeedRequest = 1;	
	m_nFailcount = 0;
	m_nPercent = 0;
	return 1;
}

int RTFileData::BeginUpdateFile(int nSyncMode, int nUpdateMode)
{
	char	filebuf = 0;
	int		i;
	MFile	stFile;
	tagQlDateTime tmptime;
	int		foffset;

	if(m_nFlag < 0)
		return -2;

	printf("%s: begin>>>>>>\n", m_sFileName.c_str());

	if(nSyncMode != -1)
		m_nSyncMode = nSyncMode;

	if(nUpdateMode != -1)
		m_nUpdateMode = nUpdateMode;

	if(MFile::IsExist(m_poGrup->m_stDir + m_sFileNameTmp))
		MFile::DeleteFile(m_poGrup->m_stDir + m_sFileNameTmp);

	MFile::CreateDirectoryTree(m_poGrup->m_stDir);

	m_nFlag = 2;
	m_nNeedRequest = 1;	

	switch(m_nUpdateMode)
	{
	case UPDATEMODE_ZONE:

		if(stFile.Create(m_poGrup->m_stDir + m_sFileNameTmp) != 1)
		{
			printf("can not create more file\n");
			return -1;
		}

		m_nOffset = -1;

		m_stTimeOffset = 0;
		memcpy(&m_stTimeOffset, &m_stStart, 4);

		break;

	case UPDATEMODE_RECORD:
		if(MFile::CopyFile(m_poGrup->m_stDir + m_sFileName, m_poGrup->m_stDir + m_sFileNameTmp, 0) != 1)
		{
			if(stFile.Create(m_poGrup->m_stDir + m_sFileNameTmp) != 1)
			{
				printf("can not create more file\n");
				return -1;
			}
			m_nFileSize = 0;
		}

		if(m_nFileSize < m_poGrup->m_stCfg.recordsize + m_poGrup->m_stCfg.fileheadsize)
		{
			m_nUpdateMode = UPDATEMODE_FILE;
			m_nOffset = 0;
			m_stTimeOffset = -1;
		}
		else
		{
			m_nOffset = -1;

			stFile.OpenRead(m_poGrup->m_stDir + m_sFileName);
			m_nFileSize = stFile.GetFileLength();
			foffset = m_nFileSize - ((m_nFileSize - m_poGrup->m_stCfg.fileheadsize) % m_poGrup->m_stCfg.recordsize) - m_poGrup->m_stCfg.recordsize + m_poGrup->m_stCfg.rectimepos;
			stFile.Seek(foffset, 0);
			if(stFile.Read((char*)&tmptime, 4) != 4)
			{
				CancelUpdateFile();
				m_nPercent = FLAG_CLIENT_ERROR;
				return -1;
			}

			if(m_sFileName.StringPosition(".d01") >=0)
			{//日线
				tmptime.Hour = 24;
				tmptime.Minute = 60;
			}
			
			if(m_sFileName.StringPosition(".mon") >=0)
			{//月线
				tmptime.Day = 31;
				tmptime.Hour = 24;
				tmptime.Minute = 60;
			}
			
			m_stTimeOffset = 0;
			memcpy(&m_stTimeOffset, &tmptime, 4);
		}

		break;

	case UPDATEMODE_DBF:
		if(MFile::CopyFile(m_poGrup->m_stDir + m_sFileName, m_poGrup->m_stDir + m_sFileNameTmp, 0) != 1)
		{
			if(stFile.Create(m_poGrup->m_stDir + m_sFileNameTmp) != 1)
			{
				printf("can not create more file\n");
				return -1;
			}
			m_nOffset = 0;
			m_stTimeOffset = -1;
			m_nUpdateMode = UPDATEMODE_FILE;
		}
		else
		{
			stFile.OpenWrite(m_poGrup->m_stDir + m_sFileNameTmp);
			stFile.SetFileLength(m_nFileSizeTmp);
			if(m_nFileSizeTmp > m_nFileSize)
			{
				stFile.Seek(m_nFileSize, 0);
				for(i = 0; i < m_nFileSizeTmp - m_nFileSize; i++)
				{
					stFile.Write(&filebuf, 1);
				}
			}
			m_stTimeOffset = m_poDbf->m_nLastRecTime;
			m_nOffset = 0;//可能出现重复，保证取完更新的记录。
		}

		break;

	case UPDATEMODE_FILE:
		if(stFile.Create(m_poGrup->m_stDir + m_sFileNameTmp) != 1)
		{
			printf("can not create more file\n");
			return -1;
		}
		
		m_nOffset = 0;
		m_stTimeOffset = -1;
		
		break;

	default:
		break;
	}
	
	return 1;
}

int	RTFileData::CompleteUpdateFile()
{
	if(m_nSyncMode == 1)
	{
		if((!m_poGrup->m_AutoUpdate) || (!m_poGrup->m_perent->m_AutoUpdateOn))
		{
			CancelUpdateFile();
			m_nFlag = 1;
			m_nNeedRequest = 1;
			m_nFailcount = 0;
			m_nPercent = 0;
			return 1;
		}
	}

	if((m_nUpdateMode == UPDATEMODE_ZONE) &&
		(m_nFlag == 2))
	{
		MFile	aFile, aFileNew, stFile;
		int		i, j, k;
		tagQlDateTime timei, timej, curtime;
		int		nFileHeadSize, nTimePos, nRecSize;
		char	HisRec[256];
		int		done;

		nFileHeadSize = m_poGrup->m_stCfg.fileheadsize;
		nTimePos = m_poGrup->m_stCfg.rectimepos;
		nRecSize = m_poGrup->m_stCfg.recordsize;
		
		if(MFile::IsExist(m_poGrup->m_stDir + m_sFileNameTmp + "1"))
		{
			if(aFile.OpenWrite(m_poGrup->m_stDir + m_sFileNameTmp + "1") < 0)
			{
				m_nPercent = FLAG_CLIENT_ERROR;
				return -1;
			}
			aFile.SetFileLength(0);
		}
		else
		{
			if(aFile.Create(m_poGrup->m_stDir + m_sFileNameTmp + "1") < 0)
			{
				printf("create file error\n");
				m_nPercent = FLAG_CLIENT_ERROR;
				return -1;
			}
		}
		
		if(aFileNew.OpenRead(m_poGrup->m_stDir + m_sFileNameTmp) < 0)
		{
			m_nPercent = FLAG_CLIENT_ERROR;
			return -1;
		}

		aFileNew.Seek(0, 0);
		aFile.Seek(0, 0);
		
		if(aFileNew.Read(HisRec, nFileHeadSize) != nFileHeadSize)
		{
			m_nPercent = FLAG_CLIENT_ERROR;
			return -1;
		}

		if(aFile.Write(HisRec, nFileHeadSize) != nFileHeadSize)
		{
			printf("write file error\n");
			m_nPercent = FLAG_CLIENT_ERROR;
			return -1;
		}
			
		i = 0;
		j = 0;
		k = 0;

		stFile.OpenRead(m_poGrup->m_stDir + m_sFileName);
		memset(&curtime, 0, sizeof(tagQlDateTime));

		done = 0;
		while(1)
		{
			aFile.Seek(nFileHeadSize + k * nRecSize, 0);

			stFile.Seek(nFileHeadSize + i * nRecSize + nTimePos, 0);
			if(stFile.Read((char*)&timei, 4) != 4)
			{
				char	c[2048];
				int		nread;
				aFileNew.Seek(nFileHeadSize + j * nRecSize, 0);
				while(1)
				{
					nread = aFileNew.Read(c, 2048);
					if(nread == 0)
					{
						done = 1;
						break;
					}
					if(aFile.Write(c, nread) != nread)
					{
						printf("write file error\n");
						m_nPercent = FLAG_CLIENT_ERROR;
						return -1;
					}
				}
			}

			if(done)
				break;

			aFileNew.Seek(nFileHeadSize + j * nRecSize + nTimePos, 0);
			if(aFileNew.Read((char*)&timej, 4) != 4)
			{
				char	c[2048];
				int		nread;
				stFile.Seek(nFileHeadSize + i * nRecSize, 0);
				while(1)
				{
					nread = stFile.Read(c, 2048);
					if(nread == 0)
					{
						done = 1;
						break;
					}

					if(aFile.Write(c, nread) != nread)
					{
						printf("write file error\n");
						m_nPercent = FLAG_CLIENT_ERROR;
						return -1;
					}
				}
			}

			if(done)
				break;

			if(m_sFileName.StringPosition(".d01") >=0)
			{//日线
				*(unsigned long *)&timej &= 0xfffff800;
				*(unsigned long *)&timei &= 0xfffff800;
			}

			if(m_sFileName.StringPosition(".mon") >=0)
			{//月线
				*(unsigned long *)&timej &= 0xffff0000;
				*(unsigned long *)&timei &= 0xffff0000;
			}

			if(*(unsigned long*)&timej == *(unsigned long*)&timei)
			{
				aFileNew.Seek(nFileHeadSize + j * nRecSize, 0);
				aFileNew.Read(HisRec, nRecSize);
				if(*(unsigned long *)&timej > *(unsigned long *)&curtime)
					aFile.Write(HisRec, nRecSize);
				curtime = timej;
				i++;
				j++;
				k++;
			}
			else if(*(unsigned long*)&timej < *(unsigned long*)&timei)
			{
				aFileNew.Seek(nFileHeadSize + j * nRecSize, 0);
				aFileNew.Read(HisRec, nRecSize);
				if(*(unsigned long *)&timej > *(unsigned long *)&curtime)
					aFile.Write(HisRec, nRecSize);
				curtime = timej;
				j++;
				k++;
			}
			else
			{
				stFile.Seek(nFileHeadSize + i * nRecSize, 0);
				stFile.Read(HisRec, nRecSize);
				if(*(unsigned long *)&timei > *(unsigned long *)&curtime)
					aFile.Write(HisRec, nRecSize);
				curtime = timei;
				i++;
				k++;
			}
		}

		m_nFileSizeTmp = -1;

		aFileNew.Close();
		aFile.Close();
		stFile.Close();
		MFile::DeleteFile(m_poGrup->m_stDir + m_sFileNameTmp);
		if(MFile::RenameFileName(m_poGrup->m_stDir + m_sFileNameTmp + "1", m_poGrup->m_stDir + m_sFileNameTmp) < 0)
		{
			printf("rename error1\n");
			m_nPercent = FLAG_CLIENT_ERROR;
			return -1;
		}

	}

	if(m_nFlag == 2)
	{
		if((m_nUpdateMode == UPDATEMODE_RECORD)||
			(m_nUpdateMode == UPDATEMODE_ZONE))
		{
			m_nFileSizeTmp = -1;
		}

		if(SetFileTime_size(m_poGrup->m_stDir + m_sFileNameTmp, m_stUpdateTimeTmp, m_stCreateTimeTmp, m_nFileSizeTmp) < 0)
		{
			m_nPercent = FLAG_CLIENT_ERROR;
			return -1;
		}
	}

	if(MFile::IsExist(m_poGrup->m_stDir + m_sFileNameTmp) != 1)
	{
		CancelUpdateFile();
		m_nPercent = FLAG_CLIENT_ERROR;

		if(m_nFlag == 2)
		{
			m_nFlag = 1;
			m_nNeedRequest = 1;
			m_nFailcount = 0;
			m_nPercent = 0;
		}
		
		return 1;
	}

	m_nFlag = 3;

	if(MFile::IsExist(m_poGrup->m_stDir + m_sFileName) != 1)
	{
		if(MFile::RenameFileName(m_poGrup->m_stDir + m_sFileNameTmp, m_poGrup->m_stDir + m_sFileName) < 0)
		{
			printf("rename error\n");
			m_nPercent = FLAG_CLIENT_ERROR;
			return -1;
		}
		goto end;
	}

	if(MFile::DeleteFile(m_poGrup->m_stDir + m_sFileName) == 1)
	{	
		if(MFile::RenameFileName(m_poGrup->m_stDir + m_sFileNameTmp, m_poGrup->m_stDir + m_sFileName) < 0)
		{
			printf("rename error\n");
			m_nPercent = FLAG_CLIENT_ERROR;
			return -1;
		}
		goto end;
	}

	printf("%s delete error %d\n", m_sFileName.c_str(), m_nFailcount);

	if(m_nFailcount >= 3)
	{
		printf("%s use replace\n", m_sFileName.c_str());
		if(MFile::CopyFile(m_poGrup->m_stDir + m_sFileNameTmp, m_poGrup->m_stDir + m_sFileName, 0) != 1)
		{
			printf("replace file error\n");
			CancelUpdateFile();
			m_nPercent = FLAG_CLIENT_ERROR;
			return -2;
		}
		MFile::DeleteFile(m_poGrup->m_stDir + m_sFileNameTmp);
	}
	else
	{
		return -2;
	}

end:

	RefleshInfo();

	m_nFileSizeTmp = -1;

	if(m_poGrup->m_stCfg.updatemode == UPDATEMODE_DBF)
	{
		if(m_poDbf)
		{
			if(m_nUpdateMode == UPDATEMODE_FILE)
			{
				this->m_poDbf->Instance(this);//获得全文件后，重新初始化。
			}
		}
	}

	slib_WriteInfo( Global_UnitNo,0,"文件%s更新成功", m_sFileName.c_str());
	printf("%s: complete<<<<<<\n", m_sFileName.c_str());
	if(m_poGrup->m_stCfg.syncmode != 0)
		Global_FileData.m_nUpFileCount++;

	m_nFlag = 0;
	m_nPercent = 100;

	m_nSyncMode = m_poGrup->m_stCfg.syncmode;
	m_nUpdateMode = m_poGrup->m_stCfg.updatemode;

	if(m_sFileName == "LONKIND.CFG")
	{
		Global_LongKind.UpdateData(m_poGrup->m_stDir + m_sFileName, 1);
		MFile::CopyFile(m_poGrup->m_stDir + m_sFileName, m_sFileName, 0);
	}

	return 1;
}

int RTFileData::CancelUpdateFile()
{
	if(MFile::IsExist(m_poGrup->m_stDir + m_sFileNameTmp))
		MFile::DeleteFile(m_poGrup->m_stDir + m_sFileNameTmp);

	if(MFile::IsExist(m_poGrup->m_stDir + m_sFileNameTmp + "1"))
		MFile::DeleteFile(m_poGrup->m_stDir + m_sFileNameTmp + "1");

	slib_WriteError( Global_UnitNo, 0, "文件%s更新失败", m_sFileName.c_str() );
	printf("%s: canceld************\n", m_sFileName.c_str());

	m_nFileSizeTmp = -1;
	m_nFlag = 0;
	if((m_nSyncMode == 0) && (m_poGrup->m_stCfg.syncmode == 1))
	{
		BeginRecvInfo(1, -1, 0);
	}
	else
	{
		m_nSyncMode = m_poGrup->m_stCfg.syncmode;
		m_nUpdateMode = m_poGrup->m_stCfg.updatemode;
	}
	return 1;
}

int RTFileData::RefleshInfo()
{
	MFindFile			aFind;
	tagMFindFileInfo	info;

	if(aFind.FindFirst(m_poGrup->m_stDir + m_sFileName, &info) != 1)
	{
		m_stUpdateTime = 0;
		m_nFileSize = -1;
		aFind.CloseFind();
		return -1;
	}

	if(m_sFileName == "LONKIND.CFG")
	{
		Global_LongKind.UpdateData(m_poGrup->m_stDir + m_sFileName, 1);
	}

	m_stUpdateTime = info.mUpdateTime;
	m_nFileSize = info.lSize;
	
	aFind.CloseFind();
	return 1;
}

int RTFileData::SetFileTime_size(MString name, MDateTime stUpdateTime, MDateTime stCreateTime, int size)
{	
	MFile		afile;

	if(afile.OpenWrite(name) != 1)
		return -1;

	if(size >= 0)
	{
		if(afile.SetFileLength(size) < 0)
			return -1;
	}

	if(stUpdateTime != 0)
	{
		if(afile.SetUpdateDateTime(stUpdateTime) < 0)
			return -1;
	}

	if(stCreateTime != 0)
	{
		if(afile.SetCreateDateTime(stCreateTime) < 0)
			return -1;
	}

	return 1;
}

int	RTFileData::RequestData()
{
	char			reqbuf[1024];
	tagFileRequst * pstRequest = (tagFileRequst*)reqbuf;
	int				length = m_sFileName.GetLength();
	pstRequest->nClassID = m_poGrup->m_nClassID;
	pstRequest->nMarketID = m_poGrup->m_nMarketID;
	pstRequest->nSerial = m_nSerial;
	pstRequest->stUpdateTime = m_stUpdateTimeTmp.DateTimeToTimet();
	pstRequest->nFileSzie = m_nFileSizeTmp;
	pstRequest->stOffsetTime = m_stTimeOffset;
	pstRequest->nOffset = m_nOffset;
	memcpy(reqbuf + sizeof(tagFileRequst), m_sFileName.c_str(), length);
	reqbuf[sizeof(tagFileRequst) + length] = 0;

	m_nRequestNo++;
	Global_ClientComm.SendRequest(10, 68, (const char*)reqbuf, sizeof(tagFileRequst) + length + 1, m_nRequestNo);
	m_nFrameNo = 0;
	m_nNeedRequest = 0;	
	m_stLastRecvTime.SetCurTickCount();
	return 1;
}

int	RTFileData::RequestInfo()
{
	char			reqbuf[1024];
	tagInfoRequst * pstRequest = (tagInfoRequst*)reqbuf;
	int				length = m_sFileName.GetLength();
	pstRequest->nClassID = m_poGrup->m_nClassID;
	pstRequest->nMarketID = m_poGrup->m_nMarketID;
	pstRequest->nSerial = m_nSerial;
	if((m_stUpdateTime == 0) && (m_nUpdateMode == UPDATEMODE_ZONE))
	{
		pstRequest->nNeedHead = 1;
	}
	else
	{
		pstRequest->nNeedHead = 0;
	}
	memcpy(reqbuf + sizeof(tagInfoRequst), m_sFileName.c_str(), length);
	reqbuf[sizeof(tagInfoRequst) + length] = 0;
	
	m_nRequestNo++;
	Global_ClientComm.SendRequest(10, 69, (const char*)reqbuf, sizeof(tagInfoRequst) + length + 1, m_nRequestNo);
	m_nFrameNo = 0;
	m_nNeedRequest = 0;	
	m_stLastRecvTime.SetCurTickCount();
	return 1;
}

//-------------------------------------------------------------------------------