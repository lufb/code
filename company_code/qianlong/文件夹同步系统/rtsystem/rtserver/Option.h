/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		Option.h
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统服务器配置模块
	History:		
*********************************************************************************************************/
#ifndef __OPTION_H__
#define __OPTION_H__
#include "MEngine.hpp"
#include "../common/commstruct.h"

#define	 MAX_CLASS  64
#define	 MAX_MARKET	64

class RTFile;
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Option
{
protected:
	MCriticalSection			m_Section;
	MString						m_IniPath;					//  配置文件路径
	RTFile					*	m_dataIOpt;					//	数据层接口指针
	bool						m_bIsCalc;					// 计算标志
	unsigned long 				Verstion;
	MString						WorkDir;

	MString					*	m_MarketDir;
	int							m_checkthreadcount;

	tagClassInfo *				m_stClassInfo;
	int							m_nClassCount;
	int							m_nMarketCount;
	////////////////////////////////////////////////LUFUBO
	int							m_nDirCfgCount;
	tagDirTranCfg*				m_stDirCfgInfo;
	////////////////////////////////////////////////LUFUBO

public:
	Option();
	virtual ~Option();
public:
	virtual int					Instance( const void * DrvHandle, unsigned long Verstion, const char * marketDir,RTFile * dataPt );
	virtual void				Release();
public:
	// 取服务器版本
	unsigned long				GetSrvVersion() const;
	MString						GetSrvVersionStr() const;
	//取工作目录
	MString						GetWorkDir() const;
	//MString						GetSrcPath() const;
	int							GetCheckThreadCount();

	int							GetFileClassCount();
	int							GetDirCfgCount(); //
	tagDirTranCfg*				GetDirCfgContent(unsigned int index);//
	int							GetMarketCount();
	MString						GetClassDir(int classid, int marketid = 0);
	MString						GetSrcDir(int classid, int marketid = 0);
	int							GetClassCfg(int nserial, tagClassInfo * cfg);
	
	
	
private:
	int _ReadCfgSection(MIniFile *pinifile, MString sec, tagClassInfo * pcfg);
	int _ReadCfgSection_Dir(MIniFile *pinifile, MString sec, tagDirTranCfg * pcfg);//LUFUBO

protected:
	// 装入服务器配置文件
	int							_LoadSrvConfig( const void * DrvHandle, const char * marketDir );
	// 调整K线根数
	int							_AdjustKLine();
};

#endif//__OPTION_H__
