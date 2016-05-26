#include "GcClient.h"
#include "MGlobal.h"



BOOL APIENTRY DllMain(HANDLE hModule,
					  DWORD ul_reason_for_call,
					  LPVOID lpReserved)
{
	if(ul_reason_for_call == DLL_PROCESS_ATTACH){
		Global_Log.initLog(hModule, "GcClient", "log");
		Global_Log.writeLog(LOG_TYPE_INFO,
               "用户动态加载了gcClient.dll库, 日志模块启动成功");
	}else if(ul_reason_for_call == DLL_PROCESS_DETACH){
		Global_Log.writeLog(LOG_TYPE_INFO,
			"用户御载动态库");
		Global_Log.writeLog(LOG_TYPE_INFO,
			"日志模块销毁");
		Global_Log.destroy();
	}

	return TRUE;
}

extern "C" _declspec(dllexport)unsigned long  __stdcall GetDllVersion()
{
	return GcCGetDllVersion();
}

extern "C" _declspec(dllexport)int  __stdcall StartWork(unsigned char ucRunModel, unsigned short usPort )
{
	return GcCStartWork(ucRunModel, usPort);
}

extern "C" _declspec(dllexport)int  __stdcall EndWork()
{
	return GcCEndWork();
}

extern "C" _declspec(dllexport)int  __stdcall Config(void * pHwnd)
{
	return GcCConfig(pHwnd);
}

extern "C" _declspec(dllexport)int  __stdcall SetCallBack(void * pFunc)
{
	return GcCSetCallBack(pFunc);
}

extern "C" _declspec(dllexport)int  __stdcall ComCmd(unsigned char cType,void* p1,void* p2)
{
	return GcCComCmd(cType, p1, p2);
}
