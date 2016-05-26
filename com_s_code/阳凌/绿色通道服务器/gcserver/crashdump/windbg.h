
#ifndef __WINDBG_DEF_H__
#define __WINDBG_DEF_H__
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the WINDBG_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// WINDBG_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef  __cplusplus
extern  "C" {
#endif


#ifdef WINDBG_EXPORTS
#define WINDBG_API __declspec(dllexport)
#else
#define WINDBG_API _stdcall
#endif


//��ȡ�汾��
unsigned long WINDBG_API GetDllVersion();

//��������  ����
//���� bShowDlg �������� false ����ʱ���Ƿ���ʾ�Ի���,�����н���ĳ����������Ϊtrue,�����ı������г���������Ϊfalse
//
//����bRestart ��������false �������Ƿ���������.����Щ����ֻ������һ�����̵Ĺ��ܲ�����ʹ��
int WINDBG_API DbgInstance(int bShowDlg , int bRestart);

//�ͷ�,��δʵ�ֹ���
void	WINDBG_API DbgRelease();

#ifdef __cplusplus
}
#endif

#endif //__WINDBG_DEF_H__
