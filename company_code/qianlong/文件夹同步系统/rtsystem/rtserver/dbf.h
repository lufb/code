/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		dbf.h
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RTϵͳ������dbf�ļ�����ģ��
	History:		
*********************************************************************************************************/
#ifndef __DBF_H__
#define __DBF_H__
#include "MEngine.hpp"

class RTFileData;
//dbf �Ȳ����Ǽ�¼��仯�����
typedef struct
{
	int		serial;
	__int64 time;
} tagSortUnit;
class RTDBF
{
public:
	tagDbfHeadInfo			m_stDbfHeadInfo;
	int						m_nTimeOffset;
	int						m_nTimeSize;
	int						m_nCodeOffset;
	int						m_nCodeSize;
	RTFileData			*	m_poFile;
public:
	RTDBF();
	~RTDBF();
	int		Instance(RTFileData * fileptr);
	void	Release();

	int		GetData(tagFileResponse * stResponse, char * buf, int &buflen, int inCall, MFile * pFile);
	int		GetInfo(MFile * pFile);

	static	__int64	Chars2Time(char * chartime);
	MString	GetKey( char * rec);
	int		IsCodeName(MString filed);
	int		IsTime(MString filed);

private:

};

#endif // __DBF_H__

