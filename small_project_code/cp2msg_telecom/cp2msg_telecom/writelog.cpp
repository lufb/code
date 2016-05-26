#include "writelog.h"
#include "LocalLock.h"


MWriteLog::MWriteLog()
{
	InitializeCriticalSection(&m_Lock);

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
	DeleteCriticalSection(&m_Lock);
}
/**
 *	initLog		-		初始化一个日志文件
 *
 *	@logDes:		[in]		logFile的描述信息
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
	SYSTEMTIME					sys;
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
	
	if(logFileName[strlen(logFileName)-1] != '\\')
	{
		logFileName[strlen(logFileName)] = '\\';	
		++rc;
	}
	
	_snprintf(m_cLogFileDir, sizeof(m_cLogFileDir)-1, logFileName);
	
	
	GetLocalTime(&sys);
	_snprintf(logFileName+rc, sizeof(logFileName)-rc-1, 
		"%s%04d%02d%02d.%s", 
		m_cLogPreName, sys.wYear,sys.wMonth,sys.wDay, m_cLogSuffix);
	m_iLastDay = sys.wDay;
	
	m_pFile = fopen(logFileName, "a");
	if(m_pFile == NULL)
		return -2;

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
		fclose(m_pFile);
		m_pFile = NULL;
	}
}

/**
 *	detectLogFile	-		探测是否要生成一个新日志文件,如果是则生成新日志文件
 *	@sys		[in]		传入的SYSTEMTIME结构体
 *
 *	return
 *		==0					成功
 *		!=0					失败
 */
int MWriteLog::detectLogFile(const SYSTEMTIME &sys)
{
	MLocalSection			locSec;
	
	locSec.Attch(&m_Lock);
	if(sys.wDay != m_iLastDay)
	{
		char				buff[256] = {'\0'};

		destroy();
		m_iLastDay = sys.wDay;
		
		_snprintf(buff, sizeof(buff)-1, "%s%04d%02d%02d.%s", 
			m_cLogPreName, sys.wYear,sys.wMonth,sys.wDay, m_cLogSuffix);
		m_pFile = fopen(buff, "a");
		if(m_pFile == NULL)
			return -1;
	}

	return 0;
}



/**
 *	writeLog	-		写日志文件
 *
 *	@log_type	[in]	写日志的等级(信息/出错)
 *	@fmt		[in]	需要写的日志信息
 *
 *	return
 *		无
 */
void MWriteLog::writeLog(unsigned char log_type, const char *fmt, ...)
{
#define		MAX_LOG_LEN			10240
#define		LOG_LINE			"\n"

	char					buffer[MAX_LOG_LEN];
	va_list					ap;
	int						rc;
	int						err;
	SYSTEMTIME				sys;
	MLocalSection			locSec;
	
	if(!m_bNeedWriteDisk)
		return;

	GetLocalTime(&sys);

	if(detectLogFile(sys) != 0)
		return;

	switch (log_type)
	{
	case LOG_TYPE_INFO:
		rc = _snprintf(buffer, sizeof(buffer), 
			"[信息] (%04d-%02d-%02d %02d:%02d:%02d-%03d)<%s>\t\t", 
			sys.wYear,sys.wMonth,sys.wDay,sys.wHour,
			sys.wMinute,sys.wSecond,sys.wMilliseconds,
			m_cLogPreName);
		break;
	case LOG_TYPE_ERROR:
		rc = _snprintf(buffer, sizeof(buffer), 
			"[错误] (%04d-%02d-%02d %02d:%02d:%02d-%03d)<%s>\t\t", 
			sys.wYear,sys.wMonth,sys.wDay,sys.wHour,
			sys.wMinute,sys.wSecond,sys.wMilliseconds,
			m_cLogPreName);
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

	locSec.Attch(&m_Lock);
	if(m_pFile != NULL && rc < MAX_LOG_LEN)
		fwrite(buffer, sizeof(char), rc, m_pFile);

#ifdef _DEBUG
	printf("%s", buffer);
#endif

	fflush(m_pFile);

#undef MAX_LOG_LEN
#undef MAX_LOG_LEN
}