/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		Log.cpp
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统服务器日志模块
	History:		
*********************************************************************************************************/
#include "Log.h" 
 
Log			Global_Log;
int					Log::m_nUnit	 = 0;
tagSmFun_WriteLog *	Log::m_writelog = NULL;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Log::Log()
{}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Log::~Log()
{}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int	Log::Instance( const tagSrvUnit_PlatInterface * pIn, int nUnitNo )
{
	m_writelog = pIn->WriteLog;
	m_nUnit = nUnitNo;

	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Log::Release()
{
	m_nUnit = 0;
	m_writelog = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Log::WriteError( int nLevel, char* szModule, char* szFormat, ... )
{
	char						tempbuf[2048];
	va_list						stmarker;
	
	va_start( stmarker, szFormat );
	_vsnprintf( tempbuf, sizeof(tempbuf)-1, szFormat, stmarker );
	va_end( stmarker );

	tempbuf[sizeof(tempbuf)-1] = 0;
	
	m_writelog( LOG_TYPE_ERROR, m_nUnit, nLevel, szModule, tempbuf );
	
	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int	Log::WriteWarning( int nLevel, char* szModule, char* szFormat, ... )
{
	char						tempbuf[2048];
	va_list						stmarker;
	
	va_start( stmarker, szFormat );
	_vsnprintf( tempbuf, sizeof(tempbuf)-1, szFormat, stmarker );
	va_end( stmarker );

	tempbuf[sizeof(tempbuf)-1] = 0;
	
	m_writelog( LOG_TYPE_WARN, m_nUnit, nLevel, szModule, tempbuf );
	
	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int	Log::WriteInfo( int nLevel, char* szModule, char* szFormat, ... )
{
	char						tempbuf[2048];
	va_list						stmarker;
	
	va_start( stmarker, szFormat );
	_vsnprintf( tempbuf, sizeof(tempbuf)-1, szFormat, stmarker );
	va_end( stmarker );

	tempbuf[sizeof(tempbuf)-1] = 0;
	
	m_writelog( LOG_TYPE_INFO, m_nUnit, nLevel, szModule, tempbuf );
	
	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int	Log::WriteLog( unsigned char ucType, const char * szFile, int nLine, int nLevel, const char * strmodule, const char *szFormat, ... )
{
	char						tempbuf[2048];
	va_list						stmarker;
	MString						StrModule;


#ifdef _DEBUG
	StrModule = MString(szFile) + "(" + nLine + ")" + "[" + strmodule + "]";
#else
	StrModule = strmodule;
#endif
	
	va_start( stmarker, szFormat );
	_vsnprintf( tempbuf, sizeof(tempbuf)-1, szFormat, stmarker );
	va_end( stmarker );

	tempbuf[sizeof(tempbuf)-1] = 0;
	
	m_writelog( ucType, m_nUnit, nLevel, StrModule.c_str(), tempbuf );

	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int	Log::WriteLog( unsigned char ucType, int nUnit, int nLevel, const char * strmodule, const char *szFormat, ... )
{
	char						tempbuf[2048];
	va_list						stmarker;

	va_start( stmarker, szFormat );
	_vsnprintf( tempbuf, sizeof(tempbuf)-1, szFormat, stmarker );
	va_end( stmarker );

	tempbuf[sizeof(tempbuf)-1] = 0;
	
	m_writelog( ucType, nUnit, nLevel, strmodule, tempbuf );

	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int	Log::GetUnit()
{
	return m_nUnit;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Log::SetUnit( unsigned short uUint )
{
	m_nUnit = uUint;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void __stdcall  WriteLog( unsigned char type, unsigned short wUnitserial, unsigned short wLevel, const char * source, const char * msg )
{
	Global_Log.WriteLog( type, wUnitserial, wLevel, source, msg );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
