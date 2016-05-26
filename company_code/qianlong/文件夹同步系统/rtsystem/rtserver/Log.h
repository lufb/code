/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		Log.h
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RTϵͳ��������־ģ��
	History:		
*********************************************************************************************************/
#ifndef __LOG_H__
#define __LOG_H__

#include "MSrvUnitInterface.hpp"

#define LOG_INFO_NORMAL		LOG_TYPE_INFO, __FILE__, __LINE__, LOG_LEVEL_CUSTOM_NORMAL			// ��ͨ��Ϣ��־
#define LOG_INFO_ADMIN		LOG_TYPE_INFO, __FILE__, __LINE__, LOG_LEVEL_CUSTOM_ADMIN			// ����Ա��Ϣ��־
#define LOG_INFO_SUPPORT	LOG_TYPE_INFO, __FILE__, __LINE__, LOG_LEVEL_SUPPORT				// ά��Ա��Ϣ��־
#define LOG_INFO_TEST		LOG_TYPE_INFO, __FILE__, __LINE__, LOG_LEVEL_TEST					// ����Ա��Ϣ��־
#define LOG_INFO_PROGRAMS	LOG_TYPE_INFO, __FILE__, __LINE__, LOG_LEVEL_PROGRAMS				// ������Ա��Ϣ��־
#define LOG_INFO_DEBUG		LOG_TYPE_INFO, __FILE__, __LINE__, LOG_LEVEL_DEBUG					// ������Ա��Ϣ��־

#define LOG_WARN_NORMAL		LOG_TYPE_WARN, __FILE__, __LINE__, LOG_LEVEL_CUSTOM_NORMAL			// ��ͨ������־
#define LOG_WARN_ADMIN		LOG_TYPE_WARN, __FILE__, __LINE__, LOG_LEVEL_CUSTOM_ADMIN			// ����Ա������־
#define LOG_WARN_SUPPORT	LOG_TYPE_WARN, __FILE__, __LINE__, LOG_LEVEL_SUPPORT				// ά��Ա������־
#define LOG_WARN_TEST		LOG_TYPE_WARN, __FILE__, __LINE__, LOG_LEVEL_TEST					// ����Ա������־
#define LOG_WARN_PROGRAMS	LOG_TYPE_WARN, __FILE__, __LINE__, LOG_LEVEL_PROGRAMS				// ������Ա������־
#define LOG_WARN_DEBUG		LOG_TYPE_WARN, __FILE__, __LINE__, LOG_LEVEL_DEBUG					// ������Ա������־

#define LOG_ERROR_NORMAL	LOG_TYPE_ERROR, __FILE__, __LINE__, LOG_LEVEL_CUSTOM_NORMAL			// ��ͨ������־
#define LOG_ERROR_ADMIN		LOG_TYPE_ERROR, __FILE__, __LINE__, LOG_LEVEL_CUSTOM_ADMIN			// ����Ա������־
#define LOG_ERROR_SUPPORT	LOG_TYPE_ERROR, __FILE__, __LINE__, LOG_LEVEL_SUPPORT				// ά��Ա������־
#define LOG_ERROR_TEST		LOG_TYPE_ERROR, __FILE__, __LINE__, LOG_LEVEL_TEST					// ����Ա������־
#define LOG_ERROR_PROGRAMS	LOG_TYPE_ERROR, __FILE__, __LINE__, LOG_LEVEL_PROGRAMS				// ������Ա������־
#define LOG_ERROR_DEBUG		LOG_TYPE_ERROR, __FILE__, __LINE__, LOG_LEVEL_DEBUG					// ������Ա������־

// ��־����
enum 
{
	LOG_TYPE_INFO		= 1,		// ��Ϣ
	LOG_TYPE_WARN		= 2,		// ����
	LOG_TYPE_ERROR		= 3			// ����
};

class Log
{
public:
	Log();
	virtual ~Log();
public:
	static int		 Instance( const tagSrvUnit_PlatInterface *, int nUnitNo );
	static void		 Release();
	static int		 WriteError( int nLevel, char* szModule, char* szFormat, ... );
	static int		 WriteWarning( int nLevel, char* szModule, char* szFormat, ... );
	static int		 WriteInfo( int nLevel, char* szModule, char* szFormat, ... );
	static int		 WriteLog( unsigned char ucType, const char *szFile, int nLine, int nLevel, const char * strmodule, const char *szFormat, ... );
	static int		 WriteLog( unsigned char ucType, int nUnit, int nLevel, const char * strmodule, const char *szFormat, ... );
	static int		 GetUnit();
	static void 	 SetUnit( unsigned short uUint ); 
protected:
	static int					m_nUnit;
	static tagSmFun_WriteLog *	m_writelog;
};

extern void __stdcall  WriteLog( unsigned char type, unsigned short wUnitserial, unsigned short wLevel, const char *, const char * );

extern  Log		 Global_Log;

#define TraceLog		 Global_Log.WriteLog


#endif//__LOG_H__

