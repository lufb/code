/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		filedata.h
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RTϵͳ���ջ��ļ�����ģ��
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
	unsigned short	m_nSerial;	//�����ס�����ֻ��һ���ο����ڷ�������������Ӧ������Ҫ���в���

	int				m_nFlag;			//-1δ��ʼ����0δ���ڸ��£�1���ڵ��ļ���Ϣ��2���ڸ����ļ���3ֻʣ����ɾ���ļ���
	int				m_nSyncMode;
	int				m_nUpdateMode;
	int				m_nNeedRequest;	
	__int64			m_stTimeOffset;
	int				m_nOffset;
	MString			m_sFileNameTmp;	
	MDateTime		m_stUpdateTimeTmp;
	MDateTime		m_stCreateTimeTmp;
	int				m_nFileSizeTmp;		//-1��ʾ����ʱ�ļ�
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

