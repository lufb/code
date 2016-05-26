//------------------------------------------------------------------------------------------------------------------------------
//单元名称：系统信息单元单元
//单元描述：
//创建日期：2007.4.23
//创建人员：卢明远
//修改纪录
//修改日期		修改人员	修改内容和原因
//
//
//
//
//
//------------------------------------------------------------------------------------------------------------------------------
#ifndef __MEngineLib_MSystemInfoH__
#define __MEngineLib_MSystemInfoH__
//------------------------------------------------------------------------------------------------------------------------------
#include "MString.h"
#include "MDll.h"

#ifndef LINUXCODE

#include <tlhelp32.h>
#include <Iprtrmib.h>
#include <nb30.h>

#else
	
#endif

#define DEFAULTBUFSIZE	81920

//------------------------------------------------------------------------------------------------------------------------------
class MSystemInfo
{
protected:
	static char						static_strApplicationPath[256];
	static char						static_strApplicationName[256];
	static char						static_strOsInformation[256];
	static unsigned char			static_PhyMac[6][6];
	static int						static_nCpuCount;
	static unsigned long			static_dCpuFrequency;
	static unsigned long			static_PhyMemSize;	
#ifndef LINUXCODE
	static DWORD ( __stdcall * PGetIfTable )( PMIB_IFTABLE, DWORD *, BOOL );
	static DWORD ( __stdcall * PGetTcpStatistics )( PMIB_TCPSTATS );
#endif
	static MDll						static_iphpDll;
protected:
	static void GetBaseStaticInfo();
	__inline	static char *__BasePath(char *);
	__inline	static char *__BaseName(char *);
public:
	MSystemInfo(void);
	virtual ~MSystemInfo();
	static int Instance();
	static void Release();
public:
	static int	GetCpuNumber();						//cpu个数	
	static unsigned long GetCpuFrequency();			//cpu平均频率 Mhz
	static unsigned long GetPhyMemSize();			//物理内存大小
	static void GetMacAddress( void *, int );			
	static MString GetApplicationPath( void * = NULL);		//获取所在的目录
	static MString GetApplicationName( void * = NULL );		//获取应用程序模块名称
	static MString GetOsInformation();

	static char GetCpuPercent();						//cpu使用率	
	static char GetMemPercent();						//内存使用率
	static int GetMemUnused();						//内存剩余量 M
													//当前进程个数，io读\写量(Kb)
	static int GetSysProcessInfo( int *, unsigned long * , unsigned long * );
													//当前tcp连接数,网络发送、接收字节
	static int GetTcpLinkInfo( int *, unsigned long * , unsigned long * );
};
//------------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------------
