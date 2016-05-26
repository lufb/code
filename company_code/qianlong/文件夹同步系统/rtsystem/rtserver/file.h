/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		filedata.h
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统服务器文件数据处理模块
	History:		
*********************************************************************************************************/
#ifndef __RTFILE_H__
#define __RTFILE_H__
#include "../common/commstruct.h"
#include "MEngine.hpp"
#include "Option.h"
#include "FileCache.h"
#include "dbf.h"
#include "filelist.h"
#include "filedata.h"

class FileList;

class RTFile
{
public:
//	MThread				m_stUpdateCacheThread;
	MThread		*		m_stProcessFileThread;

private:
	MCriticalSection	m_Section;
	//for CheckFile()
	unsigned short		m_nUpClassID;
	int					m_nUpMarketID;

	//for CheckCache()
	FileList *			m_pList1;
	unsigned short		m_nSerial1;
	int					m_nProcessNum1;

public:
	FileList **		m_RecordData;
	unsigned short	m_nClassCount;
	unsigned short	m_nMarketCount;
	int				m_nCkeckThreadCount;
	MString			m_stMainPath;

public:
	RTFile();
	~RTFile();
	int Instance();
	void Release();
	int	GetCfg(char	*buf, int &nLen, tagCfgResponse * pstResponse);
	int GetData(void * buf, int & nLen, tagFileResponse *pstResponse, MString stName);
	int GetInfo(tagInfoResponse *pstResponse, MString stName);
	int GetUpdataFileList(char * buf, int & nLen, tagListResponse * pstResponse);
	int GetInitFileList(char * buf, int & nLen, tagListResponse * pstResponse);
	int GetListInfo(tagListInfoResponse * pstResponse, char *buf, int & nLen);

public:
	static int IsNewer(void * time1, void * time2);
	
private:
	FileList * _GetListPtr(int nClassID, int nMarketID);
	int		GetError(int ret, int flag);
	
private:
	int	CheckFile();
//	int	CheckCache(MCounter * pCounter);
	
protected:
//	static void * __stdcall UpdateCacheThread(void * In);
	static void * __stdcall ProcessFileThread(void * In);
};

#endif // __RTFILE_H__

