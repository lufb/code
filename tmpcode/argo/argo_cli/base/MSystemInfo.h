//------------------------------------------------------------------------------------------------------------------------------
//��Ԫ���ƣ�ϵͳ��Ϣ��Ԫ��Ԫ
//��Ԫ������
//�������ڣ�2007.4.23
//������Ա��¬��Զ
//�޸ļ�¼
//�޸�����		�޸���Ա	�޸����ݺ�ԭ��
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
	static int	GetCpuNumber();						//cpu����	
	static unsigned long GetCpuFrequency();			//cpuƽ��Ƶ�� Mhz
	static unsigned long GetPhyMemSize();			//�����ڴ��С
	static void GetMacAddress( void *, int );			
	static MString GetApplicationPath( void * = NULL);		//��ȡ���ڵ�Ŀ¼
	static MString GetApplicationName( void * = NULL );		//��ȡӦ�ó���ģ������
	static MString GetOsInformation();

	static char GetCpuPercent();						//cpuʹ����	
	static char GetMemPercent();						//�ڴ�ʹ����
	static int GetMemUnused();						//�ڴ�ʣ���� M
													//��ǰ���̸�����io��\д��(Kb)
	static int GetSysProcessInfo( int *, unsigned long * , unsigned long * );
													//��ǰtcp������,���緢�͡������ֽ�
	static int GetTcpLinkInfo( int *, unsigned long * , unsigned long * );
};
//------------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------------
