/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		Option.cpp
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统服务器配置模块
	History:		
*********************************************************************************************************/
#include "Option.h"
#include "Log.h"

/********************************************************************************************************
	Function:		BaseOption
	Description:	缺省构造函数
	Input:			
	Output:
*********************************************************************************************************/
Option::Option()
{
	m_IniPath.Empty();
	m_dataIOpt = NULL;
	m_nClassCount = 0;
	m_stClassInfo = NULL;
	m_checkthreadcount = 1;
}
/********************************************************************************************************
	Function:		~Option
	Description:	析构函数
	Input:			
	Output:
*********************************************************************************************************/
Option::~Option()
{
	Release();
}
/********************************************************************************************************
	Function:		_LoadSrvConfig
	Description:	转载服务器配置文件的内联函数
	Input:			
	Output:
	int				--结果
	-1			驱动队列大小配置为0
	-2			服务器所配驱动数量为0
	-3			配置文件不存在
*********************************************************************************************************/
int Option::_LoadSrvConfig( const void * DrvHandle,const char * marketDir )
{
	MString			strItem, strItemName, strcfg;
	MIniFile		oIniFile;
	char			DllPath[128] = {0};
	char			dllName[128] = {0};
	MLocalSection	llock;

	int				nSectionCount, i, classid, index;
	MString			stemp, secname;

	strItem = MSystemInfo::GetApplicationName( (void *)DrvHandle );
	for( i = strItem.GetLength() -1; i > 0 ; --i )
	{
		if( strItem[i] == '.' )
		{
			strItem.SetAt( i, 0 );
			break;
		}
	} 
	//m_IniPath = MSystemInfo::GetApplicationPath() + strItem + ".ini" ;
	// add by tangj 20100105 需要支持子目录
	m_IniPath = MSystemInfo::GetApplicationPath((void *)DrvHandle ) + strItem + ".ini" ;

	if(oIniFile.Open(m_IniPath) >= 0)				// 配置文件存在，读取配置文件
	{
		char buf[16];
		int marketid;

		llock.Attch( &m_Section );

		WorkDir = oIniFile.ReadString("Server", "mainpath", "../../");
		if((WorkDir[WorkDir.GetLength()-1] != '/') && (WorkDir[WorkDir.GetLength()-1] != '\\'))
		{
			WorkDir += "/";
		}
		
		if(strcmp(WorkDir.c_str(), "../../") == 0)
		{
			TraceLog(LOG_WARN_NORMAL,"RT系统服务器", "您没配置mainpath,或配置的mainpath为../../，请确认配置的mainpath是否正确");
		}
		
		m_checkthreadcount = oIniFile.ReadInteger("Server", "checkthreadcount", 10);

		m_nMarketCount = oIniFile.ReadInteger("market", "max", 0);

		m_MarketDir = new MString[m_nMarketCount + 1];

		for(marketid = 0; marketid < m_nMarketCount; marketid++)
		{
			memset(buf, 0, 16);
			
			sprintf(buf, "m%d", marketid);
			
			m_MarketDir[marketid] = oIniFile.ReadString("market", buf, "");
		}
		
		nSectionCount = oIniFile.GetSectionCount();
		m_nClassCount = 0;
		m_nDirCfgCount = 0;//LUFUBO
		for(i = 0; i < nSectionCount; i++)
		{
			stemp = oIniFile.GetSectionName(i);
			if(stemp.StringPosition("File") == 0)
			{
				m_nClassCount++;
			}
			if(stemp.StringPosition("Directory") == 0)//LUFUBO
			{
				m_nDirCfgCount++;
			}
		}
		
		m_stClassInfo = new tagClassInfo[m_nClassCount];
		if(!m_stClassInfo)
		{
			Release();
			return -1;
		}
		m_stDirCfgInfo = new tagDirTranCfg[m_nDirCfgCount];//LUFUBO
		if(!m_stDirCfgInfo)
		{
			Release();
			return -1;
		}

		classid = 0;
		index = 0;//LUFUBO
		for(i = 0; i < nSectionCount; i++)
		{
			secname = oIniFile.GetSectionName(i);
			if(secname.StringPosition("File") == 0)
			{
				_ReadCfgSection(&oIniFile, secname, &m_stClassInfo[classid]);
				classid++;
			}
			if(secname.StringPosition("Directory") == 0)//LUFUBO
			{
				_ReadCfgSection_Dir(&oIniFile, secname, &m_stDirCfgInfo[index]);
				index++;
			}
		}
		llock.UnAttch();
	}
	else		// 没有配置文件，开发时使用默认配置，使用时报错
	{
		TraceLog(LOG_ERROR_NORMAL,"配置", "提取服务器配置失败[服务器配置文件不存在，请查证]");
		return(-3);
	}
	
	return( 1 );
}

/********************************************************************************************************
Function:		Instance
Description:	模块初始化，装入服务器配置
Input:			
Output:			
*********************************************************************************************************/
int Option::Instance( const void * DrvHandle, unsigned long Verstion, const char * marketDir, RTFile * dataPt )
{
	MLocalSection					llock;
	
	llock.Attch( &m_Section );
	if( dataPt == NULL )
	{
		return( -1 );
	}
	
	int	nRes;

	m_dataIOpt = dataPt;
	
	// 装载服务配置
	if((nRes = _LoadSrvConfig( DrvHandle, marketDir )) < 0)
	{
		return nRes;
	}

	return(1);
	
}

/********************************************************************************************************
	Function:		Release
	Description:	回收处理
	Input:			
	Output:			
*********************************************************************************************************/
void Option::Release()
{
	MLocalSection					llock;
	
	llock.Attch( &m_Section );
	m_IniPath.Empty();
	m_dataIOpt = NULL;
	m_bIsCalc = false;

	m_nClassCount = 0;

	if(m_stClassInfo)
		delete [] m_stClassInfo;
	m_stClassInfo = NULL;
}

unsigned long Option::GetSrvVersion() const
{
	MLocalSection					llock;
	
	llock.Attch( &m_Section );

	return Verstion;
}

MString	Option::GetSrvVersionStr() const
{
	MLocalSection					llock;
	
	llock.Attch( &m_Section );
	MString				verstion;

	verstion.Format( "V%.2f B%03d", (Verstion >> 16)/100.f, Verstion & 0xFFFF );

	return verstion;
}

/********************************************************************************************************
	Function:		GetWorkPath
	Description:	取工作路径
	Input:			
	Output:			
	MString			--路径
*********************************************************************************************************/
MString Option::GetWorkDir() const
{
	MLocalSection					llock;

	llock.Attch( &m_Section );

	return WorkDir;
}

int	Option::GetCheckThreadCount()
{
	MLocalSection					llock;
	
	llock.Attch( &m_Section );

	return m_checkthreadcount;
}

int Option::GetFileClassCount()
{
	MLocalSection		local;

	local.Attch(&m_Section);

	return m_nClassCount;
}

int Option::GetDirCfgCount()
{
	MLocalSection		local;
	
	local.Attch(&m_Section);
	
	return m_nDirCfgCount;
}

//下标从0开始
tagDirTranCfg* Option::GetDirCfgContent(unsigned int index)
{
	if(index >= m_nDirCfgCount)
		return NULL;
	return  &(m_stDirCfgInfo[index]);
}

int Option::_ReadCfgSection(MIniFile *pinifile, MString section, tagClassInfo * pcfg)
{
	MString tmp;
	pcfg->syncmode = pinifile->ReadInteger(section, "syncmode", 0);
	pcfg->updatemode = pinifile->ReadInteger(section, "updatemode", 0);
	pcfg->cupdatecycle = pinifile->ReadInteger(section, "cupdatecycle", 5000);
	pcfg->supdatecycle = pinifile->ReadInteger(section, "supdatecycle", 10000);
	pcfg->fileheadsize = pinifile->ReadInteger(section, "fileheadsize", 0);
	pcfg->recordsize = pinifile->ReadInteger(section, "recordsize", 20);
	pcfg->rectimepos = pinifile->ReadInteger(section, "rectimepos", 0);
	pcfg->cachesize = pinifile->ReadInteger(section, "cachesize", 0);
	pcfg->market = pinifile->ReadInteger(section, "market", -1);
	pcfg->info = pinifile->ReadString(section, "info", "");
	tmp = pinifile->ReadString(section, "path", "./");
	if((tmp[tmp.GetLength()-1] != '/') && (tmp[tmp.GetLength()-1] != '\\'))
	{
		tmp += "/";
	}
	// add by tangj 20100105 需要过滤掉 ./
	if( tmp.GetLength() >= 2 && tmp[0] == '.' && ( tmp[1]=='/' || tmp[1]=='\\') )
	{
		tmp = tmp.Right(2);
	}
	
	pcfg->DirName = tmp;
	pcfg->FileName = pinifile->ReadString(section, "name", "");
	tmp = pinifile->ReadString(section, "srcpath", "");
	if((tmp[tmp.GetLength()-1] != '/') && (tmp[tmp.GetLength()-1] != '\\') && tmp != "")
	{
		tmp += "/";
	}
	pcfg->SrcDir = tmp;
	pcfg->SecName = section;

	return 1;
}

//LUFUBO
int Option::_ReadCfgSection_Dir(MIniFile *pinifile, MString section, tagDirTranCfg * pcfg)
{

	MString tmp;
	pcfg->sectionName = section;//
	pcfg->info = pinifile->ReadString(section, "info", "");
	pcfg->name = pinifile->ReadString(section, "name", "*.*");
	pcfg->supdatecycle = pinifile->ReadInteger(section, "supdatecycle", 0);
	pcfg->cupdatecycle = pinifile->ReadInteger(section, "cupdatecycle", 0);
	pcfg->syncmode = pinifile->ReadInteger(section, "syncmode", 0);
	pcfg->updatemode = pinifile->ReadInteger(section, "updatemode", 0);
	pcfg->usefilecrc32 = pinifile->ReadInteger(section, "usefilecrc32", 0);
	
	tmp = pinifile->ReadString(section, "path", "./");
	if((tmp[tmp.GetLength()-1] != '/') && (tmp[tmp.GetLength()-1] != '\\'))
	{
		tmp += "/";
	}
	// add by tangj 20100105 需要过滤掉 ./
	if( tmp.GetLength() >= 2 && tmp[0] == '.' && ( tmp[1]=='/' || tmp[1]=='\\') )
	{
		tmp = tmp.Right(2);
	}
	
	if(strcmp(tmp.c_str(), "./") == 0)
	{
		TraceLog(LOG_WARN_NORMAL,"RT系统服务器", "您没配置path，或者配置为当前目录了，请确认是否采用默认值");
	}

	pcfg->path = tmp;

	tmp = pinifile->ReadString(section, "srcpath", "./");
	if((tmp[tmp.GetLength()-1] != '/') && (tmp[tmp.GetLength()-1] != '\\'))
	{
		tmp += "/";
	}
	// add by tangj 20100105 需要过滤掉 ./
	if( tmp.GetLength() >= 2 && tmp[0] == '.' && ( tmp[1]=='/' || tmp[1]=='\\') )
	{
		tmp = tmp.Right(2);
	}

	pcfg->srcpath = tmp;
	pcfg->sectionName = section;
	
	return 1;
}

MString	Option::GetClassDir(int classid, int marketid)
{
	MIniFile	oinifile;
	MString strdir, strmaket;
	MString str1, str2;
	
	MLocalSection		local;
	local.Attch(&m_Section);

	if((classid >= m_nClassCount)||(marketid >= m_nMarketCount))
		return -1;

	strdir = m_stClassInfo[classid].DirName;
	if(strdir.GetMatchString("*market", &str1, &str2) == 1){
		if((marketid < 0) || (m_MarketDir[marketid] == ""))
			return -1;
		strdir = str1 + m_MarketDir[marketid] + str2;
	}

	return strdir;
}

MString	Option::GetSrcDir(int classid, int marketid)
{
	MIniFile	oinifile;
	MString strdir, strmaket;
	MString str1, str2;
	
	MLocalSection		local;
	local.Attch(&m_Section);
	
	if((classid >= m_nClassCount)||(marketid >= m_nMarketCount))
		return -1;
	
	strdir = m_stClassInfo[classid].SrcDir;
	if(strdir.GetMatchString("*market", &str1, &str2) == 1){
		if((marketid < 0) || (m_MarketDir[marketid] == ""))
			return -1;
		strdir = str1 + m_MarketDir[marketid] + str2;
	}
	
	return strdir;
}

int	Option::GetMarketCount()
{
	MLocalSection		local;
	local.Attch(&m_Section);

	return m_nMarketCount;
}

int	Option::GetClassCfg(int classid, tagClassInfo * cfg)
{	
	MLocalSection		local;
	local.Attch(&m_Section);

	if(classid >= m_nClassCount)
		return -1;

	cfg->syncmode = m_stClassInfo[classid].syncmode;
	cfg->updatemode = m_stClassInfo[classid].updatemode;
	cfg->cupdatecycle = m_stClassInfo[classid].cupdatecycle;
	cfg->cupdatetime = m_stClassInfo[classid].cupdatetime;
	cfg->supdatecycle = m_stClassInfo[classid].supdatecycle;
	cfg->supdatetime = m_stClassInfo[classid].supdatetime;
	cfg->fileheadsize = m_stClassInfo[classid].fileheadsize;
	cfg->recordsize = m_stClassInfo[classid].recordsize;
	cfg->rectimepos = m_stClassInfo[classid].rectimepos;
	cfg->cachesize = m_stClassInfo[classid].cachesize;
	cfg->market = m_stClassInfo[classid].market;
	cfg->info = m_stClassInfo[classid].info;
	cfg->SecName = m_stClassInfo[classid].SecName;
	cfg->DirName = m_stClassInfo[classid].DirName;
	cfg->FileName = m_stClassInfo[classid].FileName;
	cfg->SrcDir = m_stClassInfo[classid].SrcDir;
	
	return 1;
}

