/*
 *	writelog.h
 *
 *	Copyright (C) 2013 卢富波 <1164830775@qq.com>
 *
 *
 *	注意:
 *	1	该写日志模块是没怎么考虑效率问题的，即没有相应队列的东西
 *	2	写日志缓冲区最大为10000左右，超过时，写日志会不成功
 *
 *
 *	修改历史:
 *
 *	2013-11-20 - 首次创建
 *
 *                     卢富波 <1164830775@qq.com>
 */
#ifndef _ARGO_LOG_H_
#define _ARGO_LOG_H_

#include <stdio.h>
#include <wtypes.h>
#include "MMutex.h"



/** 写日志类别的枚举*/
enum {
	LOG_TYPE_INFO = 0,					/*	信息*/
	LOG_TYPE_WARN = 1,					/*  警告*/
	LOG_TYPE_ERROR = 2,					/*	错误*/
	LOG_TYPE_DEBUG = 3					/*	调试*/
}; 


class MWriteLog
{
public:
						MWriteLog();
						~MWriteLog();

	int					initLog(char* dir, bool needWriteDisk, char *logName, char *logSuffix);
	int					initLog(HANDLE handle, char *logName, char *logSuffix);
	void				destroy();
	int					detectLogFile(const SYSTEMTIME &sys);
	void				writeLog( unsigned char log_type, const char *fmt, ...);
	void				writeDist(int writeSize, const char *fmt, ... );

private:
	int					_getDir(char *pDir, size_t dirLen);

private:
	char				m_cLogFileDir[256];
	bool				m_bNeedWriteDisk;	
	FILE				*m_pFile;
	int					m_iLastDay;			/** control write a new day log*/
	char				m_cLogPreName[64];	/** log pre name*/
	char				m_cLogSuffix[64];	/** log suffix name*/
	MCriticalSection	m_Lock;
	HANDLE				m_handle;			/* 调用该dll的用户程序handle*/
};




#endif