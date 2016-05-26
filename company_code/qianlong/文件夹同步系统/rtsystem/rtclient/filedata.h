/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		filedata.h
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统接收机文件数据模块
	History:		
*********************************************************************************************************/
#ifndef __FILEDATA_H__
#define __FILEDATA_H__
#include "../common/commstruct.h"
#include "MEngine.hpp"
//#include "../common/cfgfile.h"
#include "dbf.h"

class FileList;

class RTFileData
{
public:
	RTDBF		*   m_poDbf;

public:
	FileList  *		m_poGrup;
	MString			m_sFileName;
	MDateTime		m_stUpdateTime;
	int				m_nFileSize;
	unsigned short	m_nSerial;	//这里记住的序号只是一个参考，在服务器上若不对应，还是要进行查找

	int				m_nFlag;			//-1未初始化，0未正在更新，1正在得文件信息，2正在更新文件，3只剩重试删除文件了
	int				m_nSyncMode;
	int				m_nUpdateMode;
	int				m_nNeedRequest;	
	__int64			m_stTimeOffset;
	int				m_nOffset;
	MString			m_sFileNameTmp;	
	MDateTime		m_stUpdateTimeTmp;
	MDateTime		m_stCreateTimeTmp;
	int				m_nFileSizeTmp;		//-1表示无临时文件
	int				m_nFrameNo;	
	unsigned short	m_nRequestNo;	
	int				m_nFailcount;
	RTFileData	*	m_pNext;

	//for manual down history
	int				m_nPercent;
//	int				m_nInterzone;
	tagQlDateTime	m_stStart;
	tagQlDateTime	m_stEnd;

	MCounter		m_stLastRecvTime;	

public:
	RTFileData();
	~RTFileData();
	int Instance(MString sname, FileList * pgrup);
	int	CheckUpdate(unsigned long time, int size, int serial);
	int	SetSerial(int serial);
	void Release();
	int RecvData(tagFileResponse * stResponse, char * , int);
	int RecvInfo(tagInfoResponse * stResponse);
	int	BeginRecvInfo(int nSyncMode, int nUpdateMode, unsigned long time);
	int BeginUpdateFile(int nSyncMode, int nUpdateMode);
	int CompleteUpdateFile();
	int CancelUpdateFile();
	int RefleshInfo();
	int SetFileTime_size(MString name, MDateTime stUpdateTime, MDateTime stCreateTime, int size);
	int	RequestData();
	int RequestInfo();
};

#endif // __FILEDATA_H__

