//------------------------------------------------------------------------------------------------------------------------------
#include "MSystemInfo.h"
#include "MFile.h"

#ifndef LINUXCODE
#pragma comment( lib, "netapi32" )
#else
#include <stdlib.h>
#include "MDll.h"
#define __stdcall
#endif
//------------------------------------------------------------------------------------------------------------------------------
char						MSystemInfo::static_strOsInformation[256] = "no initialize";
unsigned char			    MSystemInfo::static_PhyMac[6][6] = {0};
int						    MSystemInfo::static_nCpuCount = 0;
unsigned long				MSystemInfo::static_dCpuFrequency = 0;
unsigned long				MSystemInfo::static_PhyMemSize = 0;
#ifndef LINUXCODE
DWORD ( __stdcall * MSystemInfo::PGetIfTable )( PMIB_IFTABLE, DWORD *, BOOL ) = NULL;
DWORD ( __stdcall * MSystemInfo::PGetTcpStatistics )( PMIB_TCPSTATS ) = NULL;
#endif
MDll						MSystemInfo::static_iphpDll;

/**
 *	����һ��EngineLib�İ汾��
 */
unsigned long GetPublicLibraryVersion()
{
	// add by liuqy 20110124 for ȡ���������ݵĸ��� B001

	//B002
	//20110323 add by guoguo  ��iֵ��һ���ж�,�������i��У��,�ܿ��ܵ���m_sRecordData[i]Խ��
	//20110419 tangj �������ĸ������ݱ��ڵ��� 
	//20110628 lAddtionData �˴��ѱ��޸�.���� ���ݵ�����ֻ�����еı������� tangj  B3
	//modify by liuqy 20110704 for �޸�����DBF���йر�ʱ��û��ȥ�رմ򿪵��ļ�
	//modify by liuqy 20110706 for �޸��˶��м���Ƿ�ΪNULL�Ĵ��󣬺�MFile::Seek��WIN�����޸�,�������seek֧�ִ���4G�ļ� 
	//modify by liuqy 20110711 for �޸�дDBF��¼ʱ������¼����ԭ��¼ʱ��дDBF����ڴ�Խ���������	
	//20110712 by tangj ��Ϊʹ���˸���,���Ի��˴���,��������seek64  
	//add by liuqy 20120808 for ͳ��ÿ����·�������
	return(((unsigned long)100 << 16) + 7);
}

#ifdef WIN32

/************************************************************************/
/* ����һ������FileTime��ֵ�ķ�������CPUʹ���ʼ�������.					*/
/* 20131101 chenxc.														*/
/************************************************************************/
static __int64 FileTimeCmp(const FILETIME fileTime1, const FILETIME fileTime2)
{
	__int64 * pTime1 = (__int64 *)&fileTime1;
	__int64 * pTime2 = (__int64 *)&fileTime2;
	
	return (*pTime1 - *pTime2);
}
#endif


//..............................................................................................................................
MSystemInfo::MSystemInfo(void)
{

}
//..............................................................................................................................
MSystemInfo::~MSystemInfo()
{

}

int MSystemInfo::Instance()
{
	Release();
#ifndef LINUXCODE	
	if( static_iphpDll.LoadDll( "iphlpapi.dll" ) < 0 )
	{
		return -1;
	}

	PGetIfTable = (DWORD ( __stdcall * )( PMIB_IFTABLE, DWORD *, BOOL ) )static_iphpDll.GetDllFunction( "GetIfTable" );
	PGetTcpStatistics = (DWORD ( __stdcall * )( PMIB_TCPSTATS ))static_iphpDll.GetDllFunction( "GetTcpStatistics" );
	if( !PGetTcpStatistics || !PGetIfTable )
	{
		return -2;
	}
#endif
	return 1;
}

void MSystemInfo::Release()
{
	static_iphpDll.CloseDll();
#ifndef LINUXCODE
	PGetIfTable = NULL;
	PGetTcpStatistics = NULL;
#endif
}

char*	MSystemInfo::__BasePath(char *in)
{
	int	len, i;

	if(!in)
		return NULL;

	len = strlen(in);
	for(i = len-1; i >= 0; i--)
	{
		if(in[i] == '\\' || in[i] == '/')
		{
			in[i + 1] = 0;
			break;
		}
	}
	return in;
}

char*	MSystemInfo::__BaseName(char *in)
{
	int	len, i;

	if(!in)
		return NULL;

	len = strlen(in);
	for(i = len - 1; i >= 0; i--)
	{
		if(in[i] == '\\' || in[i] == '/')
		{
			in[i] = 0;
			break;
		}
	}
	return in + i + 1;
}

//..............................................................................................................................
MString MSystemInfo::GetApplicationPath(void * hModule)
{
	char					*lpszPath;
	char					szPath[MAX_PATH];
	MString					strPath;
	int						iRet;

	memset(szPath, 0, sizeof(szPath));
	#ifndef LINUXCODE

		iRet = ::GetModuleFileName((HMODULE)hModule, szPath, MAX_PATH);
		if(iRet <= 0)
		{
			strPath.Empty();
			return strPath;
		}
		else
		{
			lpszPath = __BasePath(szPath);
			strPath.Format("%s", lpszPath);
			return strPath;
		}

	#else

		if(!hModule)
		{
			iRet =  readlink("/proc/self/exe", szPath, MAX_PATH);
			if(iRet <= 0)
			{
				strPath.Empty();
				return strPath;
			}
			else
			{
				lpszPath = __BasePath(szPath);
				strPath.Format("%s", lpszPath);
				return strPath;
			}
		}
		else
		{
			//	LINUX�汾,����ͨ��hModule����ȡ��������ֻ���·���ķ�����
			//	�ο�MDll.cpp����LoadDll���thisָ���MDll����,��ʱ��ֱ��ͨ�����������ȡ���ֻ���·��
			//	hModule == MDll(this)
			class MDll	*pModule;
			pModule = (class MDll *)hModule;
			strncpy(szPath, pModule->GetDllSelfPath(), sizeof(szPath));
			if(strlen(szPath) == 0)
			{
				strPath.Empty();
				return strPath;
			}
			else
			{
				lpszPath = __BasePath(szPath);
				strPath.Format("%s", lpszPath);
				return strPath;
			}
		}
			
	#endif
}

//..............................................................................................................................
MString MSystemInfo::GetApplicationName( void * hModule )
{
	char					*lpszName;
	char					szName[MAX_PATH];
	MString					strName;
	int						iRet;

	memset(szName, 0, sizeof(szName));
	#ifndef LINUXCODE

		iRet = ::GetModuleFileName((HMODULE)hModule, szName, MAX_PATH);
		if(iRet <= 0)
		{
			strName.Empty();
			return strName;
		}
		else
		{
			lpszName = __BaseName(szName);
			strName.Format("%s", lpszName);
			return strName;
		}


	#else

		if(!hModule)
		{
			iRet =  readlink("/proc/self/exe", szName, MAX_PATH);
			if(iRet <= 0)
			{
				strName.Empty();
				return strName;
			}
			else
			{
				lpszName = __BaseName(szName);
				strName.Format("%s", lpszName);
				return strName;
			}
		}
		else
		{
			//	LINUX�汾,����ͨ��hModule����ȡ��������ֻ���·���ķ�����
			//	�ο�MDll.cpp����LoadDll���thisָ���MDll����,��ʱ��ֱ��ͨ�����������ȡ���ֻ���·��
			//	hModule == MDll(this)
			class MDll	*pModule;
			pModule = (class MDll *)hModule;
			strncpy(szName, pModule->GetDllSelfPath(), sizeof(szName));
			if(strlen(szName) == 0)
			{
				strName.Empty();
				return strName;
			}
			else
			{
				lpszName = __BaseName(szName);
				strName.Format("%s", lpszName);
				return strName;
			}
		}

	#endif
}

//..............................................................................................................................
void MSystemInfo::GetBaseStaticInfo()
{

#ifndef LINUXCODE
	SYSTEM_INFO			systeminfo;
	MEMORYSTATUS		memstatus;
	OSVERSIONINFO		osversion;
	unsigned long		ulPrequency[2];
	unsigned long		ulCurSave[2];
	
	//cpu����
	GetSystemInfo( &systeminfo );
	static_nCpuCount = systeminfo.dwNumberOfProcessors;
	
	//cpuƵ��
	ulCurSave[0] = GetPriorityClass( GetCurrentProcess() );
	ulCurSave[1] = GetThreadPriority( GetCurrentThread() );

	SetPriorityClass( GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	__asm
	{
		__emit 0x0f
		__emit 0x31
		mov [ulPrequency], eax
		mov [ulPrequency+4], edx
	}
	Sleep( 512 );
	__asm
	{
		__emit 0x0f
		__emit 0x31
		sub eax, [ulPrequency]
		sbb edx, [ulPrequency+4]
		mov [ulPrequency], eax
		mov [ulPrequency+4], edx
	}

	SetPriorityClass( GetCurrentProcess(), ulCurSave[0] );
	SetThreadPriority( GetCurrentThread(), ulCurSave[1] );

	static_dCpuFrequency = __int64((ulPrequency[0]+(((__int64)ulPrequency[1])<<32)))>>19;

	/**
	 *	ADD BY GUOGUO 20110127
	 *		���еĻ�����2GB-4GB�������ڴ�,��Ҫ����һ�������������������GlobalMemoryStatus ��ȷ�������ڴ��С
	 *		����ʼ��,GlobalMemoryStatus���ֻ֧��С��4GB���ڴ�,
	 *		Ϊ�˸��õ�֧�ֳ���4GB�ڴ����Ϣ��ȡ.��ȡGlobalMemoryStatusEx �������ﵽĿ��.
	 *		VC6.0 ��Ҫ��װ�Ƚ��µ�SDK������,����GlobalMemoryStatusEx�⺯��.
	 *		���Լ���ԱȽ���,���ѵ�ȥ����SDK��������.MSDN����˵�������������
	 *		kernel32.lib��,�Ҳ�����kernel32.dll����Ӧ��Ҳ���������.
	 *		׼��ֱ��LoadDll kernel32.dll�������������,��һ����Ҫ�ر�ע��
	 *		֧�ֵĲ���ϵͳ��
	 *		Windows NT/2000: Requires Windows 2000 or later.
	 *		Windows 95/98: Unsupported.
	 *		Windows2000 Pro�汾,�Ҳ��Ǻ�ȷ��֧����.����û��ϵ,���ȡ�������������ַ,�Ͳ�ȡ��ͳ��GlobalMemoryStatus����
	 *		���ֵ�Ĵ���,����Ӱ��SM������.
	 *
	 *
	 */

	/**	����Ľṹ�Ǵ�MSDN COPY������,û�а�װSDK��û����ṹ��,Ϊ����ʹ��.�Ҿ��Լ�����һ��.
	 *	Ϊ�˱������а�װ��SDK�Ļ�������ṹ���ظ�����,�һ�������,��Ա��Щ����һ����.
	 */
	typedef struct _GUOGUO {
	  DWORD dwLength; 
	  DWORD dwMemoryLoad; 
	  DWORDLONG ullTotalPhys; 
	  DWORDLONG ullAvailPhys; 
	  DWORDLONG ullTotalPageFile; 
	  DWORDLONG ullAvailPageFile; 
	  DWORDLONG ullTotalVirtual; 
	  DWORDLONG ullAvailVirtual; 
	  DWORDLONG ullAvailExtendedVirtual;
	} GUOGUO, *LPGUOGUO; 

	do {
		HINSTANCE	hInst;
		GUOGUO		mem;
		BOOL (CALLBACK *pFuncGlobalMemoryStatusEx)(LPGUOGUO);

		/**	��ʵ���Կ���ʹ��������� GetModuleHandle,��Ϊkernel32.dll�ǲ����ܱ�FreeLib��*/
		/**	����ʹ��GetModuleHandle,����ǱȽϰ�ȫ��,���DLL�����ڱ�unmap���*/
		/**	��LoadLibҲ���������ܵ�����,��μ���,����LoadLibֻ���������ü��� MSDN˵��*/
		hInst = LoadLibrary("kernel32.dll");
		if(hInst)
		{
			pFuncGlobalMemoryStatusEx = (BOOL (CALLBACK*)(LPGUOGUO))GetProcAddress(hInst, "GlobalMemoryStatusEx");
			if(pFuncGlobalMemoryStatusEx)
			{
				mem.dwLength = sizeof(mem);
				pFuncGlobalMemoryStatusEx(&mem);
				static_PhyMemSize = mem.ullTotalPhys >> 20;
			}
			else
			{
				memstatus.dwLength = sizeof( memstatus );
				GlobalMemoryStatus( &memstatus );
				static_PhyMemSize = memstatus.dwTotalPhys>>20;
			}
			FreeLibrary(hInst);
		}
		else
		{
			memstatus.dwLength = sizeof( memstatus );
			GlobalMemoryStatus( &memstatus );
			static_PhyMemSize = memstatus.dwTotalPhys>>20;
		}
	}while(0);


	//����ϵͳ��Ϣ
	osversion.dwOSVersionInfoSize = sizeof( osversion );
	if( GetVersionEx( &osversion ) == 0 )
	{
		my_snprintf( static_strOsInformation, 256, "��ȡ����ϵͳ��Ϣ��������[ERR��%d]", GetLastError() );
	}
	else if( osversion.dwPlatformId == VER_PLATFORM_WIN32_NT )
	{
		my_snprintf( static_strOsInformation, 256, "Windows 2000/NT %d.%d B%d %s", osversion.dwMajorVersion, osversion.dwMinorVersion, osversion.dwBuildNumber ,osversion.szCSDVersion );
	}
	else if (osversion.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		my_snprintf( static_strOsInformation, 256,"Windows 95/98 %d.%d B%d %s", osversion.dwMajorVersion, osversion.dwMinorVersion, osversion.dwBuildNumber, osversion.szCSDVersion );
	}
	else
	{
		my_snprintf( static_strOsInformation, 256, "Win3.1 %d.%d B%d %s", osversion.dwMajorVersion, osversion.dwMinorVersion, osversion.dwBuildNumber, osversion.szCSDVersion );
	}		

#else
	MFile							fileptr;
	MString							tempstring;
	register int					i;
	char							tempbuf[8192];
	char						*	tempptr;
	char							changebuf[50];
	register int					k;
	register int					errorcode;
	MString							readstring;
	int								famliy = 0,mode = 0,steping = 0;

//	struct ifreq					ifr;

	//��ȡ�ڴ�����
	if ( fileptr.OpenRead("/proc/meminfo") < 0 )
	{
		static_PhyMemSize = 0;	
	}
	else
	{
		fileptr.Read(tempbuf,8192);
		if ( (tempptr = strstr(tempbuf,"Mem")) == NULL )
		{
			static_PhyMemSize = 0;	
		}
		else
		{
			while ( (* tempptr) != ':' && tempptr < (tempbuf + 8192) )
			{
				tempptr ++;	
			}
				
			tempptr ++;
			
			while ( (* tempptr) == ' ' && tempptr < (tempbuf + 8192) )
			{
				tempptr ++;	
			}
				
			k = 0;
			while ( (* tempptr) != ' ' && k < 49 )
			{
				changebuf[k] = * tempptr;
				tempptr ++;
				k ++;
			}
			k = 0;
				
			static_PhyMemSize = strtol(changebuf,NULL,10) / 1024;//	��MBΪ��λ
		}
		fileptr.Close();	
	}

	//��ȡ����ϵͳ��Ϣ
	if ( fileptr.OpenRead("/proc/version") < 0 )
	{
		my_snprintf( static_strOsInformation,256,"��ȡ����ϵͳ��Ϣ��������[/proc/version������]");
	}
	else if ( (errorcode = fileptr.Read( static_strOsInformation,256)) < 0 )
	{
		fileptr.Close();
		my_snprintf( static_strOsInformation,256,"��ȡ����ϵͳ��Ϣ��������[��ȡ���ݴ���]");
	}
	else
	{
		for( i=0;i<errorcode;i++ )
		{
			if ( static_strOsInformation[i] == '(' || static_strOsInformation[i] == ')' || static_strOsInformation[i] == '#' )
			{
				static_strOsInformation[i] = 0;
				break;
			}
		}
			
		fileptr.Close();
	}

		//��ȡCPU��Ϣ		
	errorcode = 0;		
	if ( fileptr.OpenRead("/proc/cpuinfo") < 0 )
	{
		//�޷���ȡCPU��Ϣ
		static_nCpuCount = 0;
	}
	else
	{
		while ( fileptr.ReadString(&readstring) > 0 )
		{
			if ( readstring.StringPosition("processor") >= 0 )
			{
				errorcode ++;
				if ( errorcode >= 32 )
				{
					break;
				}
					
				famliy = 0;
				mode = 0;
				steping = 0;
			}
			else if ( readstring.StringPosition("cpu MHz") >= 0 )
			{
//				if ( errorcode >= 0 && errorcode < insize )
				if ( errorcode >= 0 && errorcode < 32)
				{
					tempstring = readstring.GetMatchBackString(":");
					tempstring.TrimLeft();
					tempstring.TrimRight();
					static_dCpuFrequency += tempstring.ToULong();
				}
			}
		}
		static_nCpuCount = errorcode;
		static_dCpuFrequency /= static_nCpuCount;
		fileptr.Close();
	}	

	//������ַ
/*	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock >= 0)
	{
		strncpy(ifr.ifr_name, ETH_NAME, IFNAMSIZ);
		ifr.ifr_name[IFNAMSIZ - 1] = 0;

		if (ioctl(sock, SIOCGIFHWADDR, &ifr) >=0 )
		{
			memcpy(&sa, &ifr.ifr_addr, sizeof(sa));
			memcpy(static_PhyMac[0], sa.sa_data, sizeof(mac));
		}
	}*/
#endif

}

//------------------------------------------------------------------------------------------------------------------------------
int	MSystemInfo::GetCpuNumber()
{
	if( static_nCpuCount == 0 )
	{
		GetBaseStaticInfo();
	}
	
	return static_nCpuCount;
}

//------------------------------------------------------------------------------------------------------------------------------
unsigned long MSystemInfo::GetCpuFrequency()
{
	if( static_dCpuFrequency == 0 )
	{
		GetBaseStaticInfo();
	}
	return static_dCpuFrequency;
}

//------------------------------------------------------------------------------------------------------------------------------
unsigned long MSystemInfo::GetPhyMemSize()
{
	if( static_PhyMemSize == 0 )
	{
		GetBaseStaticInfo();
	}
	return static_PhyMemSize;
}

//------------------------------------------------------------------------------------------------------------------------------
MString MSystemInfo::GetOsInformation()
{
	MString osinfo = static_strOsInformation;

	if( osinfo == "no initialize" )
	{
		GetBaseStaticInfo();
	}
	return static_strOsInformation;
}

//------------------------------------------------------------------------------------------------------------------------------
void MSystemInfo::GetMacAddress( void * pOut, int n )
{
#ifndef	LINUXCODE	//	GUOGUO 20090723
	int		nIndex = min( n, 6 );
	bool	bNeedGet = true;

	for( register int i = 0; i < 6; ++i )
	{
		if( static_PhyMac[0][i] != 0 )
		{
			bNeedGet = false;
			break;
		}
	}

	if( bNeedGet )
	{
		NCB					ncb; 
		UCHAR				uRetCode;
		LANA_ENUM			lana_enum;

#pragma pack( 1 )
		typedef struct _ASTAT_
		{
			ADAPTER_STATUS adapt;
    		NAME_BUFFER NameBuff [30];
		}ASTAT, * PASTAT;
#pragma pack()

		ASTAT				Adapter;
		char				tempmac[6] = {0};

		memset( &ncb, 0, sizeof(NCB) );
		ncb.ncb_command = NCBENUM;
		ncb.ncb_buffer = (unsigned char *) &lana_enum;
		ncb.ncb_length = sizeof(LANA_ENUM);
		uRetCode = Netbios( &ncb );
		if( uRetCode == NRC_GOODRET )
		{
			int				mCount = 0;
			for( int lana=0; lana<lana_enum.length; lana++ )
			{
				ncb.ncb_command = NCBRESET;
				ncb.ncb_lana_num = lana_enum.lana[lana];
				uRetCode = Netbios( &ncb );
				if( uRetCode != NRC_GOODRET )
					continue;

				memset( &ncb, 0, sizeof(ncb) );
				ncb.ncb_command = NCBASTAT;
				ncb.ncb_lana_num = lana_enum.lana[lana]; 
				strcpy( (char* )ncb.ncb_callname, "*" );
				ncb.ncb_buffer = (unsigned char *) &Adapter;
				ncb.ncb_length = sizeof(Adapter);
				uRetCode = Netbios( &ncb );
				if( uRetCode != NRC_GOODRET )
					continue;

				memset( tempmac, 0, 6 );
				if( memcmp( tempmac, Adapter.adapt.adapter_address, 6 ) == 0 )
				{
					continue;
				}

				memset( tempmac, 0xFF, 6 );
				if( memcmp( tempmac, Adapter.adapt.adapter_address, 6 ) == 0 )
				{
					continue;
				}
				if( mCount >= 6 )
				{
						break;
				}
				memcpy( static_PhyMac[mCount++], Adapter.adapt.adapter_address, 6 );
			}
		}
	}

	--nIndex;
	memcpy( pOut, static_PhyMac[nIndex], 6 );
#endif
}

//------------------------------------------------------------------------------------------------------------------------------
char MSystemInfo::GetCpuPercent()
{
#ifndef LINUXCODE

	// ��̬���������Դ洢�ϴλ�ȡ������.
	static FILETIME pre_userTime;
	static FILETIME pre_kernelTime;
	static FILETIME pre_idleTime;
	
	// ��̬����GetSysytemTimes����.
	long ( __stdcall *GetSystemTimes )( LPFILETIME, LPFILETIME, LPFILETIME );
	GetSystemTimes = (long(__stdcall*)(LPFILETIME,LPFILETIME,LPFILETIME))::GetProcAddress( ::GetModuleHandle( "kernel32.dll" ),"GetSystemTimes" );
	if ( GetSystemTimes == NULL )
	{
		return(0);
	}
	
	// ��ȡ��ǰ����.
	FILETIME cur_userTime;
	FILETIME cur_kernelTime;
	FILETIME cur_idleTime;
	GetSystemTimes(&cur_idleTime, &cur_kernelTime, &cur_userTime);
	
	// �����ֵ.
	__int64 iIdleTime = FileTimeCmp(cur_idleTime, pre_idleTime);
	__int64 iKernelTime = FileTimeCmp(cur_kernelTime, pre_kernelTime);
	__int64 iUserTime = FileTimeCmp(cur_userTime, pre_userTime);
	
	// ����ǰֵ��¼����̬����.
	pre_idleTime = cur_idleTime;
	pre_kernelTime = cur_kernelTime;
	pre_userTime = cur_userTime;
	
	// ���������.
	__int64 iSysTime = iKernelTime + iUserTime;
	if(0 == iSysTime)
	{
		return (0);
	}
	return (char)( (1 - (double)iIdleTime / iSysTime) * 100 + 0.5 );

#else
	MFile					fileptr;
	char					tempbuf[8192];
	char				*	tempptr;
	char				*	nextptr;
	static	long long		total_old = 0;
	long long				total_new = 0;
	static	long long		idle_old = 0;
	long long				idle_new = 0;
	long long				total = 0, idle = 0;
	long long				errorcode;
	double					k;

	if ( fileptr.OpenRead("/proc/stat") < 0 )
	{
		return(0);	
	}
	else
	{
		fileptr.Read(tempbuf,8192);
		fileptr.Close();	

		if ( (tempptr = strstr(tempbuf,"cpu")) == NULL )
		{
			return(0);	
		}
		else
		{
			tempptr += 3;

			errorcode = strtoll(tempptr,&nextptr,10);
			total += errorcode;
			tempptr = nextptr;

			errorcode = strtoll(tempptr,&nextptr,10);
			total += errorcode;
			tempptr = nextptr;

			errorcode = strtoll(tempptr,&nextptr,10);
			total += errorcode;
			tempptr = nextptr;

			errorcode = strtoll(tempptr,&nextptr,10);
			total += errorcode; idle += errorcode;
			tempptr = nextptr;

			errorcode = strtoll(tempptr,&nextptr,10);
			total += errorcode;
			tempptr = nextptr;

			errorcode = strtoll(tempptr,&nextptr,10);
			total += errorcode;
			tempptr = nextptr;

			errorcode = strtoll(tempptr,&nextptr,10);
			total += errorcode;
			tempptr = nextptr;

			if(total_old == 0)
			{
				//	�״ε���
				total_old = total; idle_old = idle; return 0;
			}
			else
			{
				//	��������
				total_new = total; idle_new = idle;
				//	Ȼ�����
				k = ((double)((total_new - total_old) - (idle_new - idle_old)) / (double)(total_new - total_old)) * 100;
				total_old = total_new; idle_old = idle_new;
				return (char)k;
			}
		}
	}
#endif
}

//------------------------------------------------------------------------------------------------------------------------------
char MSystemInfo::GetMemPercent()
{
	
#ifndef LINUXCODE
	int				nErrorCode = 0;
	MEMORYSTATUS	memstatus;
		
	memstatus.dwLength = sizeof( memstatus );

	GlobalMemoryStatus( &memstatus );
	
	nErrorCode = memstatus.dwMemoryLoad;

	return nErrorCode;
#else
	MFile				fileptr;
	char				tempbuf[8192];
	char			*	tempptr;
	//char			*	nextptr;
	double				user,total;
		
	if ( fileptr.OpenRead("/proc/meminfo") < 0 )
	{
		return(0);	
	}
	else
	{
		fileptr.Read(tempbuf,8192);
		fileptr.Close();
			
		if ( (tempptr = strstr(tempbuf,"MemTotal:")) == NULL )
		{
			return(0);
		}
		else
		{
			tempptr += 9;
			total = strtod(tempptr, NULL);
		}
		if ( (tempptr = strstr(tempbuf,"MemFree:")) == NULL )
		{
			return(0);
		}
		else
		{
			tempptr += 8;
			user = strtod(tempptr, NULL);
		}
		return((unsigned char)((total - user) * 100 / total));
	}
#endif
}

//------------------------------------------------------------------------------------------------------------------------------
int MSystemInfo::GetMemUnused()
{
#ifndef LINUXCODE

	int dwRet;
	/**
	 *	����ط�,���ڴ���2GB���ڴ�Ҳ��������.�����ƺ�����û��CODE������.���Ǵ��뻹��׼�����˴������2GB������
	 */
	MEMORYSTATUS	memstatus;
	/**
	 *	ADD BY GUOGUO 20110127
	 *		���еĻ�����2GB-4GB�������ڴ�,��Ҫ����һ�������������������GlobalMemoryStatus ��ȷ�������ڴ��С
	 *		����ʼ��,GlobalMemoryStatus���ֻ֧��С��4GB���ڴ�,
	 *		Ϊ�˸��õ�֧�ֳ���4GB�ڴ����Ϣ��ȡ.��ȡGlobalMemoryStatusEx �������ﵽĿ��.
	 *		VC6.0 ��Ҫ��װ�Ƚ��µ�SDK������,����GlobalMemoryStatusEx�⺯��.
	 *		���Լ���ԱȽ���,���ѵ�ȥ����SDK��������.MSDN����˵�������������
	 *		kernel32.lib��,�Ҳ�����kernel32.dll����Ӧ��Ҳ���������.
	 *		׼��ֱ��LoadDll kernel32.dll�������������,��һ����Ҫ�ر�ע��
	 *		֧�ֵĲ���ϵͳ��
	 *		Windows NT/2000: Requires Windows 2000 or later.
	 *		Windows 95/98: Unsupported.
	 *		Windows2000 Pro�汾,�Ҳ��Ǻ�ȷ��֧����.����û��ϵ,���ȡ�������������ַ,�Ͳ�ȡ��ͳ��GlobalMemoryStatus����
	 *		���ֵ�Ĵ���,����Ӱ��SM������.
	 *
	 *
	 */

	/**	����Ľṹ�Ǵ�MSDN COPY������,û�а�װSDK��û����ṹ��,Ϊ����ʹ��.�Ҿ��Լ�����һ��.
	 *	Ϊ�˱������а�װ��SDK�Ļ�������ṹ���ظ�����,�һ�������,��Ա��Щ����һ����.
	 */
	typedef struct _GUOGUO {
	  DWORD dwLength; 
	  DWORD dwMemoryLoad; 
	  DWORDLONG ullTotalPhys; 
	  DWORDLONG ullAvailPhys; 
	  DWORDLONG ullTotalPageFile; 
	  DWORDLONG ullAvailPageFile; 
	  DWORDLONG ullTotalVirtual; 
	  DWORDLONG ullAvailVirtual; 
	  DWORDLONG ullAvailExtendedVirtual;
	} GUOGUO, *LPGUOGUO; 

	do {
		HINSTANCE	hInst;
		GUOGUO		mem;
		BOOL (CALLBACK *pFuncGlobalMemoryStatusEx)(LPGUOGUO);

		/**	��ʵ���Կ���ʹ��������� GetModuleHandle,��Ϊkernel32.dll�ǲ����ܱ�FreeLib��*/
		/**	����ʹ��GetModuleHandle,����ǱȽϰ�ȫ��,���DLL�����ڱ�unmap���*/
		/**	��LoadLibҲ���������ܵ�����,��μ���,����LoadLibֻ���������ü��� MSDN˵��*/
		hInst = LoadLibrary("kernel32.dll");
		if(hInst)
		{
			pFuncGlobalMemoryStatusEx = (BOOL (CALLBACK*)(LPGUOGUO))GetProcAddress(hInst, "GlobalMemoryStatusEx");
			if(pFuncGlobalMemoryStatusEx)
			{
				mem.dwLength = sizeof(mem);
				pFuncGlobalMemoryStatusEx(&mem);
				dwRet = mem.ullAvailPhys >> 20;
			}
			else
			{		
				memstatus.dwLength = sizeof( memstatus );
				GlobalMemoryStatus( &memstatus );
				dwRet = memstatus.dwAvailPhys>>20;
			}
			FreeLibrary(hInst);
		}
		else
		{
			memstatus.dwLength = sizeof( memstatus );
			GlobalMemoryStatus( &memstatus );
			dwRet = memstatus.dwAvailPhys>>20;
		}
	}while(0);
	
	return dwRet;
#else
	MFile				fileptr;
	char				tempbuf[8192];
	char			*	tempptr;
	char			*	nextptr;
	double				user,total;
		
	if ( fileptr.OpenRead("/proc/meminfo") < 0 )
	{
		return(0);	
	}
	else
	{
		fileptr.Read(tempbuf,8192);
		fileptr.Close();
			
		if ( (tempptr = strstr(tempbuf,"Mem:")) == NULL )
		{
			return(0);
		}
		else
		{
			tempptr += 4;
			total = strtod(tempptr,&nextptr) + 1;
			tempptr = nextptr;
			user = strtod(tempptr,&nextptr);		
			return (long)(total - user)>>20;
		}
	}

#endif
}

//------------------------------------------------------------------------------------------------------------------------------
int MSystemInfo::GetSysProcessInfo( int * pProCount, unsigned long * PIoRead, unsigned long * pIoWrite )
{
#ifndef LINUXCODE
	HANDLE			hProcess;
	HANDLE			hToken;
	HANDLE			procSnap;
	bool			bEnable = true;
	BOOL			bRet;
	MDll			dll;
	PROCESSENTRY32	procEntry;

	typedef struct _IO_COUNTERS 
	{ 
		ULONGLONG ReadOperationCount; 
		ULONGLONG WriteOperationCount; 
		ULONGLONG OtherOperationCount;
		ULONGLONG ReadTransferCount; 
		ULONGLONG WriteTransferCount; 
		ULONGLONG OtherTransferCount;
	} IO_COUNTERS, *PIO_COUNTERS;

	IO_COUNTERS		IoCounters;
	
	BOOL ( __stdcall * PGetProcessIoCounters )( HANDLE, PIO_COUNTERS ) = ( BOOL ( __stdcall * )( HANDLE, PIO_COUNTERS ))GetProcAddress( GetModuleHandle( "kernel32" ), "GetProcessIoCounters" );

	if( pProCount )
	{
		*pProCount = 0;
	}
	if( PIoRead )
	{
		* PIoRead = 0;
	}
	if( pIoWrite )
	{
		* pIoWrite = 0;
	}
	if( OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken ) )
	{
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount=1;
	    if( !LookupPrivilegeValue( NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid ) )
		{
			bEnable = false;
		}
		tp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
		if( !AdjustTokenPrivileges( hToken, FALSE, &tp, sizeof(tp), NULL, NULL ) )
		{
			bEnable = false;
		}
		CloseHandle( hToken );
	}
	if( !bEnable )
	{
		return -1;
	}

	procSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    if( procSnap == INVALID_HANDLE_VALUE )
    {
        return -2;
    }
    //

	if( pProCount )
	{
		memset( &procEntry, 0, sizeof( procEntry ) );

		procEntry.dwSize = sizeof( PROCESSENTRY32 );
		bRet = Process32First( procSnap, &procEntry );
		while( bRet )
		{
			++(*pProCount);
			//
			hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,false, procEntry.th32ProcessID );
			if ( hProcess )
			{
				memset( &IoCounters, 0, sizeof( IoCounters ) );
				if( PGetProcessIoCounters )
				{
					PGetProcessIoCounters( hProcess, &IoCounters );
				}
				if( PIoRead )
				{
					//*PIoRead += IoCounters.ReadOperationCount;
					*PIoRead += (IoCounters.ReadTransferCount / 1024);
				}
				if( pIoWrite )
				{
					//*pIoWrite += IoCounters.WriteOperationCount;
					*pIoWrite += (IoCounters.WriteTransferCount / 1024);
				}
				CloseHandle( hProcess );
			}
			bRet = Process32Next( procSnap, &procEntry );
		}
		CloseHandle( procSnap );
	}
#else
	FILE *			fpipe = NULL;
	int				nProCount = 0;
	char			tempbuf[8192];
	char			*saveptr = NULL;

	if( pProCount )
	{
		*pProCount = 0;
	}
	if( PIoRead )
	{
		* PIoRead = 0;
	}
	if( pIoWrite )
	{
		* pIoWrite = 0;
	}

	//��ý�������
	nProCount = 0;
	fpipe = popen( "ps -e", "r" );
	if( fpipe != NULL )
	{	
		while( fgets( tempbuf, 256, fpipe ) )
		{	
			++nProCount;
		}
		if( nProCount )
		{
			--nProCount;
		}
		pclose( fpipe );
	}
	if( pProCount )
	{
		*pProCount = nProCount;
	}

	/*
	 *	����ͨ��stat����,����ȡ�Լ����ڵĵ��豸��st_dev��
	 *	Ϊ�˱����˵���,����ֻ�ǵ�һ�λ�ȡһ�¾�OK��
	 *	����ҲĬ����,��ͳ��Ӧ�ó������ڵķ������̵�IO���,
	 *	���Ҫ��һ��˵,�������������IO,�������ں˵��ļ�ϵͳ�������Ķ�д���
	 */
	static int IsFirst = true;
	struct stat buf;
	static int	ma = 0;
	static int	mi = 0;
	if(IsFirst)
	{
		if(stat(GetApplicationPath().c_str(), &buf) != 0)
		{
			return 1;
		}
		else
		{
			/*
			 *	�������豸�ź����豸��
			 */
			ma = major(buf.st_dev);
			mi = minor(buf.st_dev);
			IsFirst = false;
		}
	}

	/*
	 *	 ����������豸�ź����豸����/proc/diskstatus�ļ�������
	 *		LINUX��/proc�ļ�ϵͳ��ʽ�仯�ܿ�.��һ��Ҫע��
	 *	��1�� : �������豸��(major)
		��2�� : ���̴��豸��(minor)
		��3�� : ���̵��豸��(name)
		��4�� : ����������(rio)
		��5�� : �ϲ��Ķ���������(rmerge)
		��6�� : ����������(rsect)
		��7�� :   �����ݻ��ѵ�ʱ�䣬��λ��ms.(��__make_request�� end_that_request_last)(ruse)
		��8�� :   д��������(wio)
		��9�� :   �ϲ���д��������(wmerge)
		��10�� : д��������(wsect)
		��11�� : д���ݻ��ѵ�ʱ�䣬��λ��ms. (��__make_request�� end_that_request_last)(wuse)
		��12�� : �������ڽ��е�I/O��(running),����I/O������������
		��13�� : ϵͳ����������I/O�ϵ�ʱ�䣬��ȥ�ظ��ȴ�ʱ��(aveq)
		��14�� : ϵͳ��I/O�ϻ��ѵ�ʱ��(use)��
	 */
	/**
	 *	strtok �����̰߳�ȫ,����POSIX �����strtok_r
	 */
	FILE	*fp;
	char	*p;
	fp = fopen("/proc/diskstats", "r");
	if(fp)
	{
		memset(tempbuf, 0, sizeof(tempbuf));
		while(fgets(tempbuf, 1024, fp))
		{
			p = strtok_r(tempbuf, " \t\r\n", &saveptr);			//	1
			if(p)
			{
				//	���豸��
				if(ma != atol(p))
				{
					memset(tempbuf, 0, sizeof(tempbuf));
					continue;
				}
				else
					p = strtok_r(NULL, " \t\r\n", &saveptr);	//	2
			}
			if(p)
			{
				//	���豸��
				if(mi != atol(p))
				{
					memset(tempbuf, 0, sizeof(tempbuf));
					continue;
				}
				else
					p = strtok_r(NULL, " \t\r\n", &saveptr);	//	3
			}
			p = strtok_r(NULL, " \t\r\n", &saveptr);	//	4
			p = strtok_r(NULL, " \t\r\n", &saveptr);	//	5
			p = strtok_r(NULL, " \t\r\n", &saveptr);	//	6
			if(p)
			{
				*PIoRead = (strtoul(p, NULL, 10)) / 2;	//	Ĭ��һ��������512�ֽ�
			}
			p = strtok_r(NULL, " \t\r\n", &saveptr);
			p = strtok_r(NULL, " \t\r\n", &saveptr);
			p = strtok_r(NULL, " \t\r\n", &saveptr);
			p = strtok_r(NULL, " \t\r\n", &saveptr);
			if(p)
			{
				*pIoWrite = strtoul(p, NULL, 10) / 2;	//	Ĭ��һ��������512�ֽ�
			}
			break;
		}
		fclose(fp);
	}

#endif
	return 1;
}

//------------------------------------------------------------------------------------------------------------------------------
int MSystemInfo::GetTcpLinkInfo( int * pTcpCout, unsigned long * pSendNum, unsigned long * pRecvNum )
{
#ifndef LINUXCODE
	DWORD			dwSize = 0;
	char *			pBuf = NULL;
	MIB_TCPSTATS	tcpstats;
	int				nMac = 0;
	PMIB_IFTABLE	pTable = NULL;
	unsigned long   ulInBytes = 0;
	unsigned long   ulOutBytes = 0;
	char			defaultbuf[DEFAULTBUFSIZE];


//	DWORD ( __stdcall * PGetIfTable )( PMIB_IFTABLE, DWORD *, BOOL ) = NULL;
//	DWORD ( __stdcall * PGetTcpStatistics )( PMIB_TCPSTATS ) = NULL;
	
	if( pTcpCout )
	{
		*pTcpCout = 0;
	}
	if( pSendNum )
	{
		*pSendNum = 0;
	}
	if( pRecvNum )
	{
		*pRecvNum = 0;
	}
	
	try
	{
/*		if( dll.LoadDll( "iphlpapi.dll" ) < 0 )
		{
			return -1;
		}

		PGetIfTable = (DWORD ( __stdcall * )( PMIB_IFTABLE, DWORD *, BOOL ) )dll.GetDllFunction( "GetIfTable" );
		PGetTcpStatistics = (DWORD ( __stdcall * )( PMIB_TCPSTATS ))dll.GetDllFunction( "GetTcpStatistics" );
		if( !PGetTcpStatistics )
		{
			return -2;
		}
*/
		if( PGetTcpStatistics == NULL || PGetIfTable == NULL )
		{
			Instance();
		}
		
		if( PGetTcpStatistics == NULL || PGetIfTable == NULL )
		{
			return -2;
		}

		PGetIfTable( NULL, &dwSize, false );
		if( dwSize > DEFAULTBUFSIZE )
		{
			pBuf = new char[dwSize];
			if( pBuf == NULL )
			{
				return -3;
			}
		}
		else
		{
			pBuf = defaultbuf;
		}
		pTable = reinterpret_cast< PMIB_IFTABLE >(pBuf);
		PGetIfTable( pTable, &dwSize, false );
		if( pTcpCout )
		{
			*pTcpCout = pTable->dwNumEntries;
		}

		for( int i = 0; i < pTable->dwNumEntries; ++i )
		{
			if ( pTable->table[i].dwType != MIB_IF_TYPE_LOOPBACK )
			{
				ulInBytes += pTable->table[i].dwInOctets;
				ulOutBytes += pTable->table[i].dwOutOctets;
//				if( nMac < 6 )
//				{
//					memcpy( static_PhyMac[nMac], pTable->table[i].bPhysAddr, 6 );
//				}
			}
		}
		if( pBuf != defaultbuf )
		{
			delete []pBuf;
			pBuf = NULL;
		}
		
		memset( &tcpstats, 0, sizeof( tcpstats ) );
		PGetTcpStatistics( &tcpstats );

		if( pTcpCout )
		{
			*pTcpCout = tcpstats.dwNumConns;
		}
		if( pSendNum )
		{
			*pSendNum = ulOutBytes;
		}

		if( pRecvNum )
		{
			*pRecvNum = ulInBytes;
		}

	}
	catch( ... )
	{
		Release();
		return -1;
	}
#else
	MFile					fileptr;
	MString					tempstring;
	unsigned long			recvamount = 0;
	unsigned long			sendamount = 0;
	int						tcpcount = 0;
	char					tempbuf[4096];
	char					*tempptr, *nextptr;

	if( pTcpCout )
	{
		*pTcpCout = 0;
	}
	if( pSendNum )
	{
		*pSendNum = 0;
	}
	if( pRecvNum )
	{
		*pRecvNum = 0;
	}

	if ( fileptr.OpenRead("/proc/net/dev") < 0 )
	{
		return(0);	
	}
	else
	{
		fileptr.Read(tempbuf,8192);
		fileptr.Close();
			
		if ( (tempptr = strstr(tempbuf,"eth0:")) == NULL )
		{
			return(0);
		}
		else
		{
			tempptr += 5;

			/*
			 *	64λ��LINUX����ϵͳ,/proc/net/dev  �ļ�ϵͳ
			 *	������ʾ���ֽ�,������0-0xFFFFFFFF ����ѭ��ʹ����
			 *	������ʾ����4GB���ֽ�,Ϊ�˼���32λ�����������ǰ���32λ�ķ�ʽ�ع���ʽ����
			 *				--GUOGUO 2010-11-19
			 */
#ifndef LINUXCODE64
			recvamount = strtoul(tempptr,&nextptr,10);
#else
			unsigned long long	xr = 0;
			xr = strtoull(tempptr,&nextptr,10);
			recvamount = (unsigned long)(xr % 0xFFFFFFFF);
#endif
			tempptr = nextptr;

			strtoul(tempptr,&nextptr,10); tempptr = nextptr;
			strtoul(tempptr,&nextptr,10); tempptr = nextptr;
			strtoul(tempptr,&nextptr,10); tempptr = nextptr;
			strtoul(tempptr,&nextptr,10); tempptr = nextptr;
			strtoul(tempptr,&nextptr,10); tempptr = nextptr;
			strtoul(tempptr,&nextptr,10); tempptr = nextptr;
			strtoul(tempptr,&nextptr,10); tempptr = nextptr;
#ifndef LINUXCODE64
			sendamount = strtoul(tempptr,&nextptr,10);
#else
			unsigned long long	xs = 0;
			xs = strtoull(tempptr,&nextptr,10);
			sendamount = (unsigned long)(xs % 0xFFFFFFFF);
#endif
			tempptr = nextptr;
		}
	}


	//���tcp��������
#if 0
	tcpcount = 0;
	fpipe = popen( "netstat -n", "r" );
	if( fpipe != NULL )
	{	
		while( fgets( tempbuf, 256, fpipe ) )
		{	
			if( !bcmp( tempbuf, "tcp", 3) )
			{
				++tcpcount;
			}
		}
		pclose( fpipe );
	}
#endif
	tcpcount = 0;
	//	��ȡ�ļ���С
	{
		FILE	*fp;
		char	szBuf[1024 + 1];
		fp = fopen("/proc/net/tcp", "rb");
		if(fp != NULL)
		{
			while(fgets(szBuf, 1024, fp))
			{
				tcpcount++;
			}
			fclose(fp);
		}
	}
	
	if( pTcpCout )
	{
		*pTcpCout = tcpcount;
	}
	if( pSendNum )
	{
		*pSendNum = sendamount;
	}

	if( pRecvNum )
	{
		*pRecvNum = recvamount;
	}

#endif

	return 1;
}
