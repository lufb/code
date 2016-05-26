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
 *	initLog		-		��ʼ��һ����־�ļ�
 *
 *	@handle						���ظ���־�ľ��
 *	@dir:			[in]		д��־�ļ���Ŀ¼(���ļ�)
 *	@needWriteDisk:	[in]		�Ƿ���Ҫд��
 *	@logName:		[in]		��־�ļ���
 *	@logSuffix		[in]		��־�ļ��ĺ�׺��
 *
 *	Note:	��������dir�ǷǷ��Ļ��ǲ����ڵģ��ú����᷵��ʧ��
 *
 *	return
 *		0			�ɹ�
 *		<0			ʧ��
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
 *	initLog		-		��ʼ��һ����־�ļ�
 *
 *	@handle:		[in]		ȫ�־��
 *	@logName:		[in]		��־�ļ���
 *	@logSuffix		[in]		��־�ļ��ĺ�׺��
 *
 *	Note:	��������dir�ǷǷ��Ļ��ǲ����ڵģ��ú����᷵��ʧ��
 *
 *	return
 *		0			�ɹ�
 *		<0			ʧ��
 */
int MWriteLog::initLog(HANDLE handle, char *logName, char *logSuffix)
{
	char					szPath[256] = {0};
	char					gcCliLog[256] = {0};	/* gcClient.log�ľ���·�� */
	bool					writeDisk = false;		/* Ĭ�ϲ�д��־ */
	int						rc;
/*������Ҫ������־�ļ����������ļ���(��handle���ڵ�Ŀ¼��)*/
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

	if(_access(gcCliLog, 0) != -1)/*����*/
		writeDisk = true;

	return initLog(szPath, writeDisk, logName, logSuffix);
#undef		GCLOG
}


/**
 *	destroyLog		-		����һ����־�ļ�
 *
 *
 *	return
 *		��
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
 *	detectLogFile	-		̽���Ƿ�Ҫ����һ������־�ļ�,���������������־�ļ�
 *	@sys		[in]		�����SYSTEMTIME�ṹ��
 *
 *	return
 *		==0					�ɹ�
 *		!=0					ʧ��
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
 *	_getDir	-			��һ���ļ����ľ���·���еõ����ļ���Ŀ¼
 *
 *	@pDir		[in/out]	�ļ����ľ���·��
 *	@fmt		[in]		�ļ����ĳ���(������������)
 *
 *	return
 *		==0				�ɹ�
 *		!=0				����
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
 *	writeLog	-		д��־�ļ�
 *
 *	@log_type	[in]	д��־�ĵȼ�(��Ϣ/����/����)
 *	@fmt		[in]	��Ҫд����־��Ϣ
 *
 *	return
 *		��
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
			"[��Ϣ] (%04d-%02d-%02d %02d:%02d:%02d-%03d)<%s>\t\t", 
			sys.wYear,sys.wMonth,sys.wDay,sys.wHour,
			sys.wMinute,sys.wSecond,sys.wMilliseconds,
			m_cLogPreName);
		break;
	case LOG_TYPE_WARN:
		rc = _snprintf(buffer, sizeof(buffer), 
			"[����] (%04d-%02d-%02d %02d:%02d:%02d-%03d)<%s>\t\t", 
			sys.wYear,sys.wMonth,sys.wDay,sys.wHour,
			sys.wMinute,sys.wSecond,sys.wMilliseconds,
			m_cLogPreName);
		break;
	case LOG_TYPE_ERROR:
		rc = _snprintf(buffer, sizeof(buffer), 
			"[����] (%04d-%02d-%02d %02d:%02d:%02d-%03d)<%s>\t\t", 
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
		"д��־�ļ�ʱ����ĳ��ȹ���д��־����������");
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
	//printf("%s\n", buffer);		/* fuck �����Ҫ��ӡ*/
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