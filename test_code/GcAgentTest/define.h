/**
*	define.h
*
*	Copyright (C) 2012 yjj  <ayjj_8109@qq.com>
*
*	ϵͳͷ�ļ������ú�һЩ�궨��
*
*	�޸���ʷ:
*
*	2013-01-07 - �״δ���
*
*                     yjj	<ayjj_8109@qq.com>
*/

#ifndef _DEFINE_H_
#define _DEFINE_H_

#pragma warning (disable: 4786)
#include <windows.h>
#include <process.h>
#include <winsock.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <Tlhelp32.h>

#ifndef LOAD_WS2_32LIB_FLAG
#pragma comment(lib,"ws2_32.lib")
#endif

//#include "errorcode.h"

#define		SEL_R	0x1
#define		SEL_W	0x2
#define		SEL_E	0x4

#ifndef		IN
#define		IN
#endif

#ifndef		OUT
#define		OUT
#endif

#ifndef		IN_OUT
#define		IN_OUT
#endif

#endif // _DEFINE_H_