/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		filedata.h
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统服务器文件数据处理模块
	History:		
*********************************************************************************************************/
#ifndef __FILEDATA_H__
#define __FILEDATA_H__
#include "../common/commstruct.h"
#include "MEngine.hpp"
#include "Option.h"
#include "FileCache.h"
#include "dbf.h"

typedef struct 
{
	RTDBF		Dbf;
	int			updatemode;
	int			inCall;
	MString		Name;
	int			nFileSize;
	MDateTime	stUpdateTime;
	MDateTime	stCreateTime;
	int			recordsize;
	int			fileheadsize;
} tagGetDataParam;

class FileList;
class RTFile;

class RTFileData
{
public:
	RTFileCache	*	m_poCache;
	RTDBF		*   m_poDbf;
public:
	FileList  *		m_poGrup;
	int				m_nFlag;		//-1未初始化，0文件未更新，1文件已更新
	int				m_on;
	MString			m_sFileName;
	MString			m_sFullName;
	MDateTime		m_stUpdateTime;
	MDateTime		m_stCreateTime;
	int				m_nFileSize;
	RTFileData	*	m_pNext;
public:
	RTFileData();
	~RTFileData();
	int Instance(tagMFindFileInfo *info, FileList * pgrup);
	void Release();
	static	int GetData(tagFileResponse * stResponse, char * , int &size, tagGetDataParam * pParam);
	int GetDataFromCache(tagFileResponse * stResponse, char * , int &size);
	int GetInfo(tagInfoResponse * stResponse);
	int RefleshInfo();
	int CheckUpdate(tagMFindFileInfo *info);
//	int CheckCacheUpdate();
};

#endif // __FILEDATA_H__

