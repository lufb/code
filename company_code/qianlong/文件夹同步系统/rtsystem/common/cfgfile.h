/********************************************************************************************************
	Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		cfgfile.h
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统配置文件处理模块
	History:		
*********************************************************************************************************/
#ifndef __CFGFILE_H__
#define __CFGFILE_H__
#include "MEngine.hpp"


typedef struct								
{
	int			syncmode;
	int			updatemode;
	int			cupdatecycle;
	int			cupdatetime;
	int			supdatecycle;
	int			supdatetime;
	int			fileheadsize;
	int			recordsize;
	int			rectimepos;
	int			cachesize;
} tagInCfg;

typedef struct  
{
	MString SecName;
	MString DirName;
	MString FileName;
	MString info;
	char	isSingle;
	tagInCfg stConfig;
} tagClassInfo;

class RTCfgFile
{
private:
	MString			m_ininame;
	tagInCfg		m_stDefCfg;
	tagClassInfo *	m_stClassInfo;
	int				m_nClassCount;
	int				m_nMarketCount;
	MCriticalSection	m_Section;

	//for get file
	int				m_nPosition;
	int				m_nClassID;
	int				m_nMarketID;
	MFindFile		m_stFindFile;
	char			m_buf[1024];

public:
	RTCfgFile();
	~RTCfgFile();
	int		Instance(MString sProgramfile);
	void	Release();
	int		GetFileClassCount(int ntype);
	int		GetClassFils(char * buf, int nsize, int classid, int marketid = 0);
	int		GetMarketCount();
	MString		GetClassDir(int classid, int marketid = 0);
	int		GetClassCfg(int nserial, tagInCfg * cfg);
	int		GetDataPos(int classid, int &type, int &nSerial);

	int		GetClassFileCount(unsigned short nClassID, unsigned short nMarketID);
	MString	GetFirstFileName(unsigned short nClassID, unsigned short nMarketID);
	MString GetNextFileName();
	MString GetClassInfoString(unsigned short nClassID);
	void	CloseGetFile();
	int		GetStable(unsigned short nClassID);

private:
	int _ReadCfgSection(MIniFile *pinifile, MString sec, tagInCfg * pcfg, tagInCfg * pdefcfg);
};

#endif // __SZL1HQSRV_DATAIO_H__

