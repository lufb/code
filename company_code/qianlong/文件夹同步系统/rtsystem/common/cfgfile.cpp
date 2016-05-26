/********************************************************************************************************
	Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		cfgfile.cpp
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统配置文件处理模块
	History:		
*********************************************************************************************************/
#include "commstruct.h"
#include "cfgfile.h"

RTCfgFile::RTCfgFile()
{
	m_nClassCount = 0;
	m_nPosition = 0;
	m_nClassID = 0;
	m_nMarketID = 0;
	m_stClassInfo = NULL;
}

RTCfgFile::~RTCfgFile()
{
	Release();
}

int RTCfgFile::Instance(MString sProgramfile)
{
	int nSectionCount, i, classid;
	MString		stemp, secname;
	MIniFile	oinifile;

	MLocalSection		local;
	local.Attch(&m_Section);

	m_ininame = sProgramfile;

	if(oinifile.Open(m_ininame.c_str())<0)
		return -1;
	
	_ReadCfgSection(&oinifile, "default", &m_stDefCfg, NULL);

	nSectionCount = oinifile.GetSectionCount();
	m_nClassCount = 0;
	for(i = 0; i < nSectionCount; i++)
	{
		stemp = oinifile.GetSectionName(i);
		if(stemp.StringPosition("File") == 0)
		{
			m_nClassCount++;
		}
	}

	m_stClassInfo = new tagClassInfo[m_nClassCount];
	if(!m_stClassInfo)
	{
		Release();
		return -1;
	}

	classid = 0;
	for(i = 0; i < nSectionCount; i++)
	{
		secname = oinifile.GetSectionName(i);
		if(secname.StringPosition("File") == 0)
		{
			_ReadCfgSection(&oinifile, secname, &m_stClassInfo[classid].stConfig, &m_stDefCfg);
			m_stClassInfo[classid].SecName = stemp;
			stemp = oinifile.ReadString(secname, "name", "");
			m_stClassInfo[classid].FileName = stemp;
			stemp = oinifile.ReadString(secname, "info", "");
			m_stClassInfo[classid].info = stemp;
			stemp = oinifile.ReadString(secname, "path", "./");
			m_stClassInfo[classid].DirName = stemp;
			if(stemp.StringPosition("*market") >= 0)
			{
				m_stClassInfo[classid].isSingle = 0;
			}
			else
			{
				m_stClassInfo[classid].isSingle = 1;
			}
			classid++;
		}
	}

	m_nMarketCount = oinifile.ReadInteger("market", "max", 0);
	
	return 1;
}

void RTCfgFile::Release()
{
	MLocalSection		local;
	local.Attch(&m_Section);
	m_nClassCount = 0;
	m_nPosition = 0;
	m_nClassID = 0;
	m_nMarketID = 0;
	if(m_stClassInfo)
		delete [] m_stClassInfo;
	m_stClassInfo = NULL;
}

int RTCfgFile::GetFileClassCount(int ntype)
{
	int			n, i;
	MLocalSection		local;

	local.Attch(&m_Section);

	switch(ntype)
	{
	case 0:
		return m_nClassCount;
	case 1:
		n = 0;
		for(i = 0; i < m_nClassCount; i++)
		{
			if(m_stClassInfo[i].isSingle)
			{
				n++;
			}
		}
		
		return n;
	case 2:
		n = 0;
		for(i = 0; i < m_nClassCount; i++)
		{
			if(!m_stClassInfo[i].isSingle)
			{
				n++;
			}
		}
		
		return n;
	default:
		return -1;
	}
}

int RTCfgFile::_ReadCfgSection(MIniFile *pinifile, MString section, tagInCfg * pcfg, tagInCfg * pdefcfg)
{
	if(!pdefcfg){
		pcfg->syncmode = pinifile->ReadInteger(section, "syncmode", 0);
		pcfg->updatemode = pinifile->ReadInteger(section, "updatemode", 0);
		pcfg->cupdatecycle = pinifile->ReadInteger(section, "cupdatecycle", 5000);
		pcfg->cupdatetime = pinifile->ReadInteger(section, "cupdatetime", 0);
		pcfg->supdatecycle = pinifile->ReadInteger(section, "supdatecycle", 0);
		pcfg->supdatetime = pinifile->ReadInteger(section, "supdatetime", 0);
		pcfg->fileheadsize = pinifile->ReadInteger(section, "fileheadsize", 0);
		pcfg->recordsize = pinifile->ReadInteger(section, "recordsize", 0);
		pcfg->rectimepos = pinifile->ReadInteger(section, "rectimepos", 0);
		pcfg->cachesize = pinifile->ReadInteger(section, "cachesize", 0);
		
	}
	else
	{
		pcfg->syncmode = pinifile->ReadInteger(section, "syncmode", pdefcfg->syncmode);
		pcfg->updatemode = pinifile->ReadInteger(section, "updatemode", pdefcfg->updatemode);
		pcfg->cupdatecycle = pinifile->ReadInteger(section, "cupdatecycle", pdefcfg->cupdatecycle);
		pcfg->cupdatetime = pinifile->ReadInteger(section, "cupdatetime", pdefcfg->cupdatetime);
		pcfg->supdatecycle = pinifile->ReadInteger(section, "supdatecycle", pdefcfg->supdatecycle);
		pcfg->supdatetime = pinifile->ReadInteger(section, "supdatetime", pdefcfg->supdatetime);
		pcfg->fileheadsize = pinifile->ReadInteger(section, "fileheadsize", pdefcfg->fileheadsize);
		pcfg->recordsize = pinifile->ReadInteger(section, "recordsize", pdefcfg->recordsize);
		pcfg->rectimepos = pinifile->ReadInteger(section, "rectimepos", pdefcfg->rectimepos);
		pcfg->cachesize = pinifile->ReadInteger(section, "cachesize", pdefcfg->cachesize);
	}

	return 1;
}

int	RTCfgFile::GetClassFils(char * buf, int nsize, int classid, int marketid)
{	
	MLocalSection		local;
	local.Attch(&m_Section);

	if((classid >= m_nClassCount)||(marketid >= m_nMarketCount))
		return -1;

	memcpy(buf, m_stClassInfo[classid].FileName.c_str(), m_stClassInfo[classid].FileName.GetLength());
	buf[m_stClassInfo[classid].FileName.GetLength()] = 0;

	if(m_stClassInfo[classid].FileName.GetLength() > 12)
		return 2;
	else
		return 1;
}

MString	RTCfgFile::GetClassDir(int classid, int marketid)
{
	MIniFile	oinifile;
	MString strdir, strmaket;
	MString str1, str2;
	char	buf[16] = {0};
	
	MLocalSection		local;
	local.Attch(&m_Section);

	if((classid >= m_nClassCount)||(marketid >= m_nMarketCount))
		return -1;
	
	if(oinifile.Open(m_ininame.c_str())<0)
		return -1;

	sprintf(buf, "m%d", marketid);
	strdir = m_stClassInfo[classid].DirName;
	strmaket = oinifile.ReadString("market", buf, "");
	if(strdir.GetMatchString("*market", &str1, &str2) == 1){
		strdir = str1 + strmaket + str2;
	}

	return strdir;
}

int	RTCfgFile::GetMarketCount()
{
	MLocalSection		local;
	local.Attch(&m_Section);

	return m_nMarketCount;
}

int	RTCfgFile::GetClassCfg(int classid, tagInCfg * cfg)
{	
	MLocalSection		local;
	local.Attch(&m_Section);

	if(classid >= m_nClassCount)
		return -1;

	memcpy(cfg, &m_stClassInfo[classid].stConfig, sizeof(tagInCfg));
	
	return 1;
}

int	RTCfgFile::GetDataPos(int classid, int &type, int &nSerial)
{
	int			n, i;
	MLocalSection		local;
	local.Attch(&m_Section);

	if(classid >= m_nClassCount)
		return -1;

	n = 0;
	if(m_stClassInfo[classid].isSingle)
	{
		type = 1;
		for(i = 0; i < classid; i++)
		{
			if(m_stClassInfo[i].isSingle)
			{
				n++;
			}
		}
	}
	else
	{
		type = 2;
		for(i = 0; i < classid; i++)
		{
			if(!m_stClassInfo[i].isSingle)
			{
				n++;
			}
		}
	}
	nSerial = n;
	return 1;
}

int		RTCfgFile::GetClassFileCount(unsigned short nClassID, unsigned short nMarketID)
{
	MLocalSection		local;
	MString				name;
	
	local.Attch(&m_Section);

	int count = 0;
	if(GetFirstFileName(nClassID, nMarketID) != "")
	{
		count++;
	}
	while(GetNextFileName() != "")
	{
		count++;
	}
		
	return count;
}

MString	RTCfgFile::GetFirstFileName(unsigned short nClassID, unsigned short nMarketID)
{
	MString				path;
	tagMFindFileInfo	stOut;
	MLocalSection		local;
	MString				name;
	int					l, i;
	char				namebuf[256];
	
	local.Attch(&m_Section);
	m_nClassID = nClassID;
	m_nMarketID = nMarketID;
	
	if (m_stClassInfo[nClassID].FileName.StringPosition("*") >=0)
	{
		m_stFindFile.CloseFind();
		path = GetClassDir(nClassID, nMarketID) + m_stClassInfo[nClassID].FileName;
		if(m_stFindFile.FindFirst(path, &stOut) != 1)
			return "";

		if(stOut.lAttr == MFILE_ATTR_NORMALFILE)
		{
			name = stOut.szName;
		}
		else
		{
			name = GetNextFileName();
		}
	}
	else
	{
		memcpy(m_buf, m_stClassInfo[nClassID].FileName.c_str(), m_stClassInfo[nClassID].FileName.GetLength());
		m_buf[m_stClassInfo[nClassID].FileName.GetLength()] = 0;

		l = 0;
		while(m_buf[l] == ' ')
			l++;
		
		for(i = 0; l < 1024*1024; l++, i++)
		{
			if((m_buf[l] == ' ')||(m_buf[l] == 0))
			{
				namebuf[i] = 0;
				break;
			}
			
			namebuf[i] = m_buf[l];
		}

		if(namebuf[0] == 0)
			return "";

		m_nPosition = l; 
		name = namebuf;
	}

	return name;
}
 
MString RTCfgFile::GetNextFileName()
{
	tagMFindFileInfo	stOut;
	MString				name;
	MLocalSection		local;
	int					l, i;
	char				namebuf[256];
	
	local.Attch(&m_Section);

	if (m_stClassInfo[m_nClassID].FileName.StringPosition("*") >=0)
	{
		while(1)
		{
			if(m_stFindFile.FindNext(&stOut) != 1)
				return "";

			if(stOut.lAttr == MFILE_ATTR_NORMALFILE)
			{
				name = stOut.szName;
				break;
			}
		}
	}
	else
	{
		l = m_nPosition;
		while(m_buf[l] == ' ')
			l++;
		
		for(i = 0; l < 1024*1024; l++, i++)
		{
			if((m_buf[l] == ' ')||(m_buf[l] == 0))
			{
				namebuf[i] = 0;
				break;
			}
			
			namebuf[i] = m_buf[l];
		}

		if(namebuf[0] == 0)
			return "";

		m_nPosition = l; 
		name = namebuf;
	}

	return name;
}

void	RTCfgFile::CloseGetFile()
{
	MLocalSection		local;
	
	local.Attch(&m_Section);

	if (m_stClassInfo[m_nClassID].FileName.StringPosition("*") >=0)
	{
		m_stFindFile.CloseFind();
	}
}

int		RTCfgFile::GetStable(unsigned short nClassID)
{
	MLocalSection		local;
	
	local.Attch(&m_Section);

	if (m_stClassInfo[nClassID].FileName.StringPosition("*") >=0)
	{
		return 0;
	}
	return 1;
}

MString RTCfgFile::GetClassInfoString(unsigned short nClassID)
{
	MLocalSection		local;
	
	local.Attch(&m_Section);
	return m_stClassInfo[nClassID].info;
}
//-------------------------------------------------------------------------------