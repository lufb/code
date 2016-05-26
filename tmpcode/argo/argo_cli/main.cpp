#include "argo_cli.h"


#ifdef	WIN32		// 增加crushdump模块，保存崩溃时的信息
#include <stdlib.h>
#include <windows.h>
#include <stdio.h>
#include "ExceptionReport.h"
using namespace ingest;
ingest::ExceptionReport		g_exceptionReport;


// 发生异常时的回调函数
void CALLBACK ExceptionFunc( int bShowDlg , int bRestart, PEXCEPTION_POINTERS pExceptionInfo )
{
	if( bShowDlg )
	{
		int		nLen = 0;
		TCHAR	szMessage[2048];
		
		memset(szMessage, 0, sizeof(szMessage));
		
		PEXCEPTION_RECORD pExceptionRecord = pExceptionInfo->ExceptionRecord;
		
		// First print information about the type of fault
		nLen = sprintf(szMessage + nLen,   TEXT("Exception code: %08X %s\r\n"),
			pExceptionRecord->ExceptionCode,
			ExceptionReport::GetExceptionString(pExceptionRecord->ExceptionCode) );
		
		// Now print information about where the fault occured
		TCHAR szFaultingModule[MAX_PATH];
		DWORD section, offset;
		ExceptionReport::GetLogicalAddress(  pExceptionRecord->ExceptionAddress,
			szFaultingModule,
			sizeof( szFaultingModule ),
			section, offset );
		
		nLen = sprintf(szMessage + nLen, TEXT("Fault address:  %08X %02X:%08X %s\r\n"),
			(DWORD)pExceptionRecord->ExceptionAddress,
			section, offset, szFaultingModule );
		
		strcat(szMessage,
			"程序异常终止\n"
			"请将进程执行目录下的crashdump目录下最新的*.dmp *.rpt文件以及当前软件版本号一并提供软件支持商,\n"
			"以便及时排查解决问题.谢谢!");
		MessageBox(NULL, szMessage, "上海乾隆高科技有限公司 -  成都开发中心(cdyfzx@qianlong2.net)", MB_OK | MB_ICONSTOP);
	}
}

#endif		// WINDOWSCODE

int
main(int argc, char *argv[])
{
#ifdef WIN32
	g_exceptionReport.SetProcessCrashFunc( &ExceptionFunc, false, false );
#endif 
	return argo_cli();
}