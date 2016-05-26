/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		FileCache.h
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统服务器文件缓存处理模块
	History:		
*********************************************************************************************************/
#ifndef __FILECACHE_H__
#define __FILECACHE_H__
#include "MEngine.hpp"
//#include "../common/cfgfile.h"

class RTFileData;

class RTFileCache
{
public:
	long			m_nHandle;      // Cache句柄
	RTFileData *	m_poFile;
	char		*	m_cBuf;			//暂时用这个
	int				m_nBufSize;

	__int64			m_stOffsetTime;
	int				m_nOffset;
	int				m_nDataSize;	//为字节数，除以记录长度得到记录数
	
public:
	RTFileCache();
	~RTFileCache();

	int		Instance(RTFileData * fileptr);
	void	Release();
	int		LoadBuf();
	int		GetData(tagFileResponse * stResponse, char * buf, int & buflen);
	
};
#endif // __FILECACHE_H__

