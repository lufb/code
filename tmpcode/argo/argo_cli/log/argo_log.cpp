//#include <io.h>
#include "argo_log.h"
#include "argo_error.h"
#ifdef LINUXCODE
#include <stdarg.h>
#define _snprintf snprintf
#define _vsnprintf vsnprintf
#endif


MWriteLog::MWriteLog()
{
	memset(m_cLogFileDir, '\0', sizeof(m_cLogFileDir));
	memset(m_cLogPreName, '\0', sizeof(m_cLogPreName));
	memset(m_cLogSuffix, '\0', sizeof(m_cLogSuffix));
	m_iLastDay = 0xFFFFFFFF;
	m_pFile = NULL;
	m_bNeedWriteDisk = true;
}

MWriteLog::~MWriteLog()
{
	destroy();
}
/**
 *	initLog		-		初始化一个日志文件
 *
 *	@handle						加载该日志的句柄
 *	@dir:			[in]		写日志文件的目录(非文件)
 *	@needWriteDisk:	[in]		是否需要写盘
 *	@logName:		[in]		日志文件名
 *	@logSuffix		[in]		日志文件的后缀名
 *
 *	Note:	如果传入的dir是非法的或是不存在的，该函数会返回失败
 *
 *	return
 *		0			成功
 *		<0			失败
 */
int MWriteLog::initLog(char* dir, bool writeDisk, char *logName, char *logSuffix)
{
	MDateTime					ti(time(NULL));
	char						logFileName[256] = {'\0'};
	int							rc;
	MLocalSection				locSec;

	
	if(dir == NULL || logName == NULL || logSuffix == NULL)
		return -1;

	locSec.Attch(&m_Lock);
	m_bNeedWriteDisk = writeDisk;
	if(m_bNeedWriteDisk == false)
		return 0;

	_snprintf(m_cLogPreName, sizeof(m_cLogPreName)-1, logName);
	_snprintf(m_cLogSuffix, sizeof(m_cLogSuffix)-1, logSuffix);

	rc = _snprintf(logFileName, sizeof(logFileName)-2, dir);
	
	if(logFileName[strlen(logFileName)-1] != '/')
	{
		logFileName[strlen(logFileName)] = '/';	
		++rc;
	}
	
	_snprintf(m_cLogFileDir, sizeof(m_cLogFileDir)-1, logFileName);
	
	_snprintf(logFileName+rc, sizeof(logFileName)-rc-1, 
		"%s%04d%02d%02d.%s", 
		m_cLogPreName, ti.GetYear(), ti.GetMonth(), ti.GetDay(),m_cLogSuffix);
	m_iLastDay = ti.GetDay();
	
	m_pFile = fopen(logFileName, "a");
	if(m_pFile == NULL)
		return BUILD_ERROR(_OSerrno(), 0);

	return 0;	
}




/**
 *	destroyLog		-		销毁一个日志文件
 *
 *
 *	return
 *		无
 */
void MWriteLog::destroy()
{	
	MLocalSection			locSec;
	
	locSec.Attch(&m_Lock);
	if(m_pFile)
	{
		fflush(m_pFile);
		fclose(m_pFile);
		m_pFile = NULL;
	}
	locSec.UnAttch();
}

/**
 *	detectLogFile	-		探测是否要生成一个新日志文件,如果是则生成新日志文件
 *	@sys		[in]		传入的SYSTEMTIME结构体
 *
 *	return
 *		==0					成功
 *		!=0					失败
 */

int MWriteLog::detectLogFile()
{
	MLocalSection			locSec;
	MDateTime				ti(time(NULL));

	locSec.Attch(&m_Lock);
	if(ti.GetDay() != m_iLastDay)
	{
		char				buff[256] = {'\0'};

		destroy();
		m_iLastDay = ti.GetDay();
		
		_snprintf(buff, sizeof(buff)-1, 
			"%s%04d%02d%02d.%s", 
		m_cLogPreName, ti.GetYear(), ti.GetMonth(), ti.GetDay(),m_cLogSuffix);
		m_pFile = fopen(buff, "a");
		if(m_pFile == NULL)
			return -1;
	}

	return 0;
}


/**
 *	_getDir	-			从一个文件名的绝对路径中得到该文件的目录
 *
 *	@pDir		[in/out]	文件名的绝对路径
 *	@fmt		[in]		文件名的长度(不包括结束符)
 *
 *	return
 *		==0				成功
 *		!=0				错误
 */
int MWriteLog::_getDir(char *pDir, size_t dirLen)
{
	if(dirLen < 1)
		return -1;

	for(size_t i = dirLen-1; i > 0; --i)
	{
		if(pDir[i] == '\\' || pDir[i] == '/')
		{
			//pDir[i] = '\n';
			pDir[i] = 0;
			return 0;
		}
	}

	return -2;
}
/**
 *	writeLog	-		写日志文件
 *
 *	@log_type	[in]	写日志的等级(信息/警告/出错)
 *	@fmt		[in]	需要写的日志信息
 *
 *	return
 *		无
 */
void MWriteLog::writeLog(unsigned char log_type, const char *fmt, ...)
{
//	在非DEBUG版本中，不打日志
#ifndef _DEBUG
	if(log_type == LOG_TYPE_DEBUG)
		return;
#endif

#define		MAX_LOG_LEN			10240
#define		LOG_LINE			"\r\n"

	char					buffer[MAX_LOG_LEN] = {0};
	va_list					ap;
	int						rc;
	int						err;
	MLocalSection			locSec;
	MDateTime				ti(time(NULL));
	
	locSec.Attch(&m_Lock);
	if(!m_bNeedWriteDisk)
		return;

	if(detectLogFile() != 0)
		return;

	switch (log_type)
	{
	case LOG_TYPE_INFO:
		rc = _snprintf(buffer, sizeof(buffer), 
			"[信息] %s <%s>\t\t", ti.DateTimeToString().c_str(), m_cLogPreName);
		break;
	case LOG_TYPE_WARN:
		rc = _snprintf(buffer, sizeof(buffer), 
			"[警告] %s <%s>\t\t", ti.DateTimeToString().c_str(), m_cLogPreName);
		break;
	case LOG_TYPE_ERROR:
		rc = _snprintf(buffer, sizeof(buffer), 
			"[错误] %s <%s>\t\t", ti.DateTimeToString().c_str(), m_cLogPreName);
		break;
	case LOG_TYPE_DEBUG:
		rc = _snprintf(buffer, sizeof(buffer), 
			"[调试] %s <%s>\t\t", ti.DateTimeToString().c_str(), m_cLogPreName);
		break;
	default:
		rc = -1;
		break;
	}
	if(rc < 0)
		return;

	va_start(ap, fmt);
	err = _vsnprintf(buffer+rc, MAX_LOG_LEN-rc-2, fmt, ap);
	if(err < 0)
		rc += _snprintf(buffer+rc, MAX_LOG_LEN-rc-2, 
		"写日志文件时传入的长度过大，写日志缓冲区不够");
	else
		rc += err;

	rc += _snprintf(buffer+rc, sizeof(LOG_LINE), LOG_LINE);
	va_end(ap);

	
	if(m_pFile != NULL && rc < MAX_LOG_LEN)
	{
		fwrite(buffer, sizeof(char), rc, m_pFile);
		fflush(m_pFile);
	}
#ifdef _DEBUG
	printf("%s\n", buffer);
#endif

#undef MAX_LOG_LEN
#undef LOG_LINE
}

void MWriteLog::writeDist(int writeSize, const char *fmt, ... )
{
#define		LOG_LINE			"\n"
	char					buffer[20480] = {0};
	va_list					ap;
	int						rc;
	MLocalSection			locSec;
	int						err;
	
	rc = _snprintf(buffer, sizeof(buffer), "\n刚刚收到的数据:");

	va_start(ap, fmt);
	err = _vsnprintf(buffer+rc, sizeof(buffer), fmt, ap);
	err = _snprintf(buffer+rc+writeSize, sizeof(LOG_LINE), LOG_LINE);

	locSec.Attch(&m_Lock);
	if(m_pFile != NULL )
	{
		fwrite(buffer, sizeof(char), rc+writeSize, m_pFile);
		fflush(m_pFile);
	}
#undef LOG_LINE
}