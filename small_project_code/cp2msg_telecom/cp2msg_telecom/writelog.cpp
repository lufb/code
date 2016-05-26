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
 *	initLog		-		��ʼ��һ����־�ļ�
 *
 *	@logDes:		[in]		logFile��������Ϣ
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
		fclose(m_pFile);
		m_pFile = NULL;
	}
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
		
		_snprintf(buff, sizeof(buff)-1, "%s%04d%02d%02d.%s", 
			m_cLogPreName, sys.wYear,sys.wMonth,sys.wDay, m_cLogSuffix);
		m_pFile = fopen(buff, "a");
		if(m_pFile == NULL)
			return -1;
	}

	return 0;
}



/**
 *	writeLog	-		д��־�ļ�
 *
 *	@log_type	[in]	д��־�ĵȼ�(��Ϣ/����)
 *	@fmt		[in]	��Ҫд����־��Ϣ
 *
 *	return
 *		��
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
			"[��Ϣ] (%04d-%02d-%02d %02d:%02d:%02d-%03d)<%s>\t\t", 
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