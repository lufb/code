
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


//获取版本号
unsigned long WINDBG_API GetDllVersion();

//启动加载  函数
//参数 bShowDlg 建议设置 false 崩溃时候是否显示对话框,建议有界面的程序可以设置为true,其他的背景运行程序建议设置为false
//
//参数bRestart 建议设置false 崩溃后是否重新启动.对有些程序只能运行一个进程的功能不建议使用
int WINDBG_API DbgInstance(int bShowDlg , int bRestart);

//释放,暂未实现功能
void	WINDBG_API DbgRelease();

#ifdef __cplusplus
}
#endif

#endif //__WINDBG_DEF_H__
