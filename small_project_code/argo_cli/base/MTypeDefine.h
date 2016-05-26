//------------------------------------------------------------------------------------------------------------------------------
//��Ԫ���ƣ����Ͷ��嵥Ԫ
//��Ԫ��������ҪΪ�˼���Windows��Linux����ϵͳ������������������幫���ĸ��캯����
//�������ڣ�2007.3.14
//������Ա��¬��Զ
//�޸ļ�¼
//�޸�����		�޸���Ա	�޸����ݺ�ԭ��
//
//
//
//
//
//------------------------------------------------------------------------------------------------------------------------------
#ifndef __MEngine_MTypeDefineH__
#define __MEngine_MTypeDefineH__
//------------------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <exception>
//------------------------------------------------------------------------------------------------------------------------------
#ifndef LINUXCODE
//..............................................................................................................................
//Windows����ϵͳ
#define _WIN32_WINNT 0x0400
#include <windows.h>
//..............................................................................................................................
#else
//..............................................................................................................................
//Linux����ϵͳ
//..............................................................................................................................
#endif
//------------------------------------------------------------------------------------------------------------------------------
//������������
//..............................................................................................................................
//�����ַ�����Ϊ��������׼strncpy�����ڲ���iCountʱ������0������
extern char * my_strncpy(char * strDest,const char * strSource,size_t iCount);
//..............................................................................................................................
//�����ַ�����Ϊ��������׼strncat�����ڲ���iCountʱ������0������
extern char * my_strncat(char * strDest,const char * strSource,size_t iCount);
//..............................................................................................................................
//��ӡ�ַ�����Ϊ��������׼snprintf�����ڲ���iCountʱ������0������
extern void my_snprintf(char * lpszBuffer,size_t lCount,const char * lpszFormat,...);
//------------------------------------------------------------------------------------------------------------------------------
#define my_assert(str)		{ if ( !(str) ) { assert(0);	return(ERR_PUBLIC_SLOPOVER);	} }
//------------------------------------------------------------------------------------------------------------------------------
#ifndef LINUXCODE
//..............................................................................................................................
//Windows����ϵͳ
#include <windows.h>
#include <io.h>
#include <process.h>

#define MSG_NOSIGNAL		0
//..............................................................................................................................
#else
//..............................................................................................................................
//Linux����ϵͳ
#include "WinAPI.h"
#endif
//------------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------------