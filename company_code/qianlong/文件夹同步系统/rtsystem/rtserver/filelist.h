/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		filedata.h
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统服务器文件数据处理模块
	History:		
*********************************************************************************************************/
#ifndef __FILELIST_H__
#define __FILELIST_H__
#include "../common/commstruct.h"
#include "MEngine.hpp"
#include "Option.h"
#include "FileCache.h"
#include "dbf.h"

class FileList;
class RTFile;

class FileList							
{
public:
	MCriticalSection	m_Section;
	RTFile		*		m_perent;
	RTFileData *		m_pFirstFile;
	RTFileData **		m_pstData;
	tagClassInfo		m_stCfg;
	MString				m_stDir;
	MString				m_SrcDir;
	int					m_nNameLenth;
	int					m_nFileNum;
	int					m_nAllocNum;
	int					m_nClassID;
	int					m_nMarketID;
	int					m_nIsStable;
	int					m_nListCookie;
	int					m_nFileCookie;
	int					m_nFlag;
	int					m_on;
//	int					m_nFileCount;

	MString				m_stFindPath;
	char				m_buf[1024];
	MCounter			m_time;		//for check time

private:
//	bool				m_IsDirt;
//	int					m_nRefCounter;
//	int					m_exclusive;
public:
	FileList();
	~FileList();

	int				Instance(int nClassID, int nMarketID, RTFile * perent, tagClassInfo * pinfo);
	void			Release();
	int				CheckFileUpdateTime();
	int				GetData(void * buf, int & nLen, tagFileResponse *pstResponse, MString stName);
	int				GetInfo(tagInfoResponse *pstResponse, MString stName);
	int				GetUpdataFileList(char * buf, int & nLen, tagListResponse * pstResponse);
	int				GetInitFileList(char * buf, int & nLen, tagListResponse * pstResponse);
	int				GetListInfo(tagListInfoResponse * pstResponse, char *buf, int & nLen);

//	bool			IncrementRefCount(int type);
//	void			DecrementRefCount(int type);
//	void			Invalidself();
//	int				GetRefCount(); 

private:
	int				_RefleshFileCount();
};

#endif // __FILELIST_H__

