/*
 *	writelog.h
 *
 *	Copyright (C) 2013 ¬���� <1164830775@qq.com>
 *
 *
 *	ע��:
 *	1	��д��־ģ����û��ô����Ч������ģ���û����Ӧ���еĶ���
 *	2	д��־���������Ϊ10000���ң�����ʱ��д��־�᲻�ɹ�
 *
 *
 *	�޸���ʷ:
 *
 *	2013-11-20 - �״δ���
 *
 *                     ¬���� <1164830775@qq.com>
 */
#ifndef _WRITE_LOG_H_
#define _WRITE_LOG_H_

#include <stdio.h>
#include <wtypes.h>



/** д��־����ö��*/
enum {
	LOG_TYPE_INFO = 0,					/*	��Ϣ*/
	LOG_TYPE_ERROR,						/*	����*/
}; 


class MWriteLog
{
public:
						MWriteLog();
						~MWriteLog();

	int					initLog(char* dir, bool needWriteDisk, char *logName, char *logSuffix);
	void				destroy();
	int					detectLogFile(const SYSTEMTIME &sys);
	void				writeLog( unsigned char log_type, const char *fmt, ...);

private:
	char				m_cLogFileDir[256];
	bool				m_bNeedWriteDisk;	
	FILE				*m_pFile;
	int					m_iLastDay;			/** control write a new day log*/
	char				m_cLogPreName[64];	/** log pre name*/
	char				m_cLogSuffix[64];	/** log suffix name*/
	CRITICAL_SECTION	m_Lock;				/** lock this*/
};




#endif