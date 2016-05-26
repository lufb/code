/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		FileCache.h
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RTϵͳ�������ļ����洦��ģ��
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
	long			m_nHandle;      // Cache���
	RTFileData *	m_poFile;
	char		*	m_cBuf;			//��ʱ�����
	int				m_nBufSize;

	__int64			m_stOffsetTime;
	int				m_nOffset;
	int				m_nDataSize;	//Ϊ�ֽ��������Լ�¼���ȵõ���¼��
	
public:
	RTFileCache();
	~RTFileCache();

	int		Instance(RTFileData * fileptr);
	void	Release();
	int		LoadBuf();
	int		GetData(tagFileResponse * stResponse, char * buf, int & buflen);
	
};
#endif // __FILECACHE_H__

