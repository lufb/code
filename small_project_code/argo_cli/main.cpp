#include "argo_cli.h"


#ifdef	WIN32		// ����crushdumpģ�飬�������ʱ����Ϣ
#include <stdlib.h>
#include <windows.h>
#include <stdio.h>
#include "ExceptionReport.h"
using namespace ingest;
ingest::ExceptionReport		g_exceptionReport;


// �����쳣ʱ�Ļص�����
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
			"�����쳣��ֹ\n"
			"�뽫����ִ��Ŀ¼�µ�crashdumpĿ¼�����µ�*.dmp *.rpt�ļ��Լ���ǰ����汾��һ���ṩ���֧����,\n"
			"�Ա㼰ʱ�Ų�������.лл!");
		MessageBox(NULL, szMessage, "�Ϻ�Ǭ¡�߿Ƽ����޹�˾ -  �ɶ���������(cdyfzx@qianlong2.net)", MB_OK | MB_ICONSTOP);
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