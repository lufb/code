#include <io.h>
#include "MLocalLock.h"
#include "MLog.h"
#include "error.h"


MWriteLog::MWriteLog()
{
	InitializeCriticalSection(&m_Lock);
	
	m_handle = NULL;
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
	SYSTEMTIME					sys;
	char						logFileName[256] = {'\0'};
	int							rc;
	MLocalSection				locSec;

	InitializeCriticalSection(&m_Lock);
	
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
		"%s%04d%02d%02d_%u.%s", 
		m_cLogPreName, sys.wYear,sys.wMonth,sys.wDay, m_handle, m_cLogSuffix);
	m_iLastDay = sys.wDay;
	
	m_pFile = fopen(logFileName, "ab");
	if(m_pFile == NULL)
		return BUILD_ERROR(_OSerrno(), 0);

	return 0;	
}

/**
 *	initLog		-		初始化一个日志文件
 *
 *	@handle:		[in]		全局句柄
 *	@logName:		[in]		日志文件名
 *	@logSuffix		[in]		日志文件的后缀名
 *
 *	Note:	如果传入的dir是非法的或是不存在的，该函数会返回失败
 *
 *	return
 *		0			成功
 *		<0			失败
 */
int MWriteLog::initLog(HANDLE handle, char *logName, char *logSuffix)
{
	char					szPath[256] = {0};
	char					gcCliLog[256] = {0};	/* gcClient.log的绝对路径 */
	bool					writeDisk = false;		/* 默认不写日志 */
	int						rc;
/*定义需要生成日志文件所创建的文件名(在handle所在的目录下)*/
#define		GCLOG		"gcClient.log"

	m_handle = handle;
	rc = ::GetModuleFileName((HMODULE)handle, szPath, sizeof(szPath)-1);
	if(rc == 0)
		return BUILD_ERROR(_OSerrno(), 0);

	rc = _getDir(szPath, strlen(szPath));
	if(rc != 0)
		return rc;

	rc = _snprintf(gcCliLog, sizeof(gcCliLog)-1-sizeof(GCLOG), szPath);
	if(gcCliLog[strlen(gcCliLog)-1] != '\\'){
		gcCliLog[strlen(gcCliLog)] = '\\';
		++rc;
	}
	_snprintf(gcCliLog+rc, sizeof(GCLOG), "%s", GCLOG);

	if(_access(gcCliLog, 0) != -1)/*存在*/
		writeDisk = true;

	return initLog(szPath, writeDisk, logName, logSuffix);
#undef		GCLOG
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
int MWriteLog::detectLogFile(const SYSTEMTIME &sys)
{
	MLocalSection			locSec;
	
	locSec.Attch(&m_Lock);
	if(sys.wDay != m_iLastDay)
	{
		char				buff[256] = {'\0'};

		destroy();
		m_iLastDay = sys.wDay;
		
		_snprintf(buff, sizeof(buff)-1, "%s%04d%02d%02d_%u.%s", 
			m_cLogPreName, sys.wYear,sys.wMonth,sys.wDay, m_handle, m_cLogSuffix);
		m_pFile = fopen(buff, "ab");
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
#define		MAX_LOG_LEN			10240
#define		LOG_LINE			"\r\n"

	char					buffer[MAX_LOG_LEN] = {0};
	va_list					ap;
	int						rc;
	int						err;
	SYSTEMTIME				sys;
	MLocalSection			locSec;
	
	locSec.Attch(&m_Lock);
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
	case LOG_TYPE_WARN:
		rc = _snprintf(buffer, sizeof(buffer), 
			"[警告] (%04d-%02d-%02d %02d:%02d:%02d-%03d)<%s>\t\t", 
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

	
	if(m_pFile != NULL && rc < MAX_LOG_LEN)
	{
		fwrite(buffer, sizeof(char), rc, m_pFile);
		fflush(m_pFile);
	}
#ifdef _DEBUG
	//printf("%s\n", buffer);		/* fuck 这儿不要打印*/
#endif

#undef MAX_LOG_LEN
#undef LOG_LINE
}

void MWriteLog::writeDist(int writeSize, const char *fmt)
{
#define		LOG_LINE			"\n"
	MLocalSection			locSec;

	locSec.Attch(&m_Lock);
	if(m_pFile != NULL )
	{
		fwrite(fmt, sizeof(char), writeSize, m_pFile);
		//fwrite(LOG_LINE, sizeof(char), sizeof(LOG_LINE), m_pFile);
		fflush(m_pFile);
	}
#undef LOG_LINE
}