/**
 *	gcClient.h
 *	����: ����һЩ��������
 *
 *		CopyRight @¬���� <1164830775@qq.com>
 *
 *	�޸���ʷ
 *		2013-11-21	¬����	����
 *			¬���� <1164830775@qq.com>
 */
#ifndef _GC_CLIENT_H_
#define _GC_CLIENT_H_

extern "C" _declspec(dllexport)unsigned long	__stdcall GetDllVersion();
extern "C" _declspec(dllexport)int				__stdcall StartWork(unsigned char ucRunModel, unsigned short usPort );
extern "C" _declspec(dllexport)int				__stdcall EndWork();
extern "C" _declspec(dllexport)int				__stdcall Config(void * pHwnd);
extern "C" _declspec(dllexport)int				__stdcall SetCallBack(void * pFunc);
extern "C" _declspec(dllexport)int				__stdcall ComCmd(unsigned char cType,void* p1,void* p2);

#endif