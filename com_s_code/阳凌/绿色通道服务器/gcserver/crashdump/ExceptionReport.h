#pragma once

/*
����������. ����б�������,������Զ��ӹܴ�������

�÷�: ����һ��ȫ�ֵ������,����: ingest::ExceptionReport g_exceptionReport;

Ĭ�������,������������˳�,û���κ���ʾ,Ӧ�ó�������ڱ���������õ�֪ͨ,������:

1.����һ��ȫ�ֻ�̬����: void CALLBACK FUNCProcessCrash();
2.���÷���ingest::ExceptionReport::SetProcessCrashFunc()�����������,����������ú����ᱻ����
3.��FUNCProcessCrash()�б�д�Լ��Ĵ�������, ������Ե���һ����ʾ�Ի���(�ͻ���ʹ��),
  �����Զ�������������(�����ʹ��),���ߵ���һ�����ԶԻ����
4.����������,������Զ���¼���������ĳ���,���������ַ,������ģ��,CPU״̬�͵��ö�ջ��,
  ��Щ��Ϣ��¼��Ӧ�ó���Ŀ¼�µ�crashdumpĿ¼����
*/
namespace ingest
{

enum BasicType  // Stolen from CVCONST.H in the DIA 2.0 SDK
{
    btNoType = 0,
    btVoid = 1,
    btChar = 2,
    btWChar = 3,
    btInt = 6,
    btUInt = 7,
    btFloat = 8,
    btBCD = 9,
    btBool = 10,
    btLong = 13,
    btULong = 14,
    btCurrency = 25,
    btDate = 26,
    btVariant = 27,
    btComplex = 28,
    btBit = 29,
    btBSTR = 30,
    btHresult = 31
};
#include "DbgHelp.h"

typedef BOOL (_stdcall *FUNCSymFromAddr)(
	IN HANDLE hProcess,
	IN DWORD64 Address,
	OUT PDWORD64 Displacement,
	IN OUT PSYMBOL_INFO Symbol
);
typedef DWORD (_stdcall *FUNCSymGetOptions)();
typedef DWORD (_stdcall *FUNCSymSetOptions)(
	IN DWORD SymOptions
);
typedef BOOL (_stdcall *FUNCSymCleanup)(
	IN HANDLE hProcess
);
typedef BOOL (_stdcall *FUNCSymInitialize)(
	IN HANDLE hProcess,
	IN PSTR UserSearchPath,
	IN BOOL fInvadeProcess
);
typedef BOOL (_stdcall *FUNCSymEnumSymbols)(
	IN HANDLE hProcess,
	IN ULONG64 BaseOfDll,
	IN PCSTR Mask,
	IN PSYM_ENUMERATESYMBOLS_CALLBACK EnumSymbolsCallback,
	IN PVOID UserContext
);
typedef ULONG (_stdcall *FUNCSymSetContext)(
	HANDLE hProcess,
	PIMAGEHLP_STACK_FRAME StackFrame,
	PIMAGEHLP_CONTEXT Context
);
typedef BOOL (_stdcall *FUNCSymGetLineFromAddr)(
	IN HANDLE hProcess,
	IN DWORD dwAddr,
	OUT PDWORD pdwDisplacement,
	OUT PIMAGEHLP_LINE Line
);
typedef BOOL (_stdcall *FUNCStackWalk)(
	DWORD							  MachineType,
    HANDLE                            hProcess,
    HANDLE                            hThread,
	LPSTACKFRAME                      StackFrame,
	PVOID                             ContextRecord,
	PREAD_PROCESS_MEMORY_ROUTINE      ReadMemoryRoutine,
	PFUNCTION_TABLE_ACCESS_ROUTINE    FunctionTableAccessRoutine,
	PGET_MODULE_BASE_ROUTINE          GetModuleBaseRoutine,
	PTRANSLATE_ADDRESS_ROUTINE        TranslateAddress
);
typedef PVOID (_stdcall *FUNCSymFunctionTableAccess)(
	HANDLE  hProcess,
	DWORD   AddrBase
);
typedef DWORD (_stdcall *FUNCSymGetModuleBase)(
	IN HANDLE     hProcess,
	IN DWORD      dwAddr
);
typedef BOOL (_stdcall *FUNCSymGetTypeInfo)(
	IN HANDLE          hProcess,
	IN DWORD64         ModBase,
	IN ULONG           TypeId,
	IN IMAGEHLP_SYMBOL_TYPE_INFO GetType,
	OUT PVOID           pInfo
);
typedef BOOL (_stdcall *FUNCMiniDumpWriteDump)(
	HANDLE hProcess,
	DWORD ProcessId,
	HANDLE hFile,
	MINIDUMP_TYPE DumpType,
	PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
	PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
	PMINIDUMP_CALLBACK_INFORMATION CallbackParam
);

typedef void (CALLBACK *FUNCProcessCrash)(int bShowDlg , int bRestart, PEXCEPTION_POINTERS pExceptionInfo);

class ExceptionReport
{
public:
	ExceptionReport(void);
	~ExceptionReport(void);

	static void SetProcessCrashFunc(FUNCProcessCrash func,int bShowDlg , int bRestart ) 
	{ 
		m_funcProcessCrash = func; 
		m_bShowDlg = bShowDlg;
		m_bRestart = bRestart;
	}

public:
	static LONG WINAPI UnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo);

	// where report info is extracted and generated 
    static void GenerateExceptionReport(PEXCEPTION_POINTERS pExceptionInfo);

	static LPTSTR GetExceptionString(DWORD dwCode);

	static BOOL GetLogicalAddress(
		PVOID addr,
		PTSTR szModule,
		DWORD len,
		DWORD& section,
		DWORD& offset
	);

	static void WriteStackDetails(PCONTEXT pContext, bool bWriteVariables);

	static BOOL CALLBACK EnumerateSymbolsCallback(PSYMBOL_INFO,ULONG, PVOID);

	static bool FormatSymbolValue(PSYMBOL_INFO, STACKFRAME *, TCHAR * pszBuffer, unsigned cbBuffer);

	static TCHAR * DumpTypeIndex(TCHAR *, DWORD64, DWORD, unsigned, DWORD, bool &);

	static BasicType GetBasicType(DWORD typeIndex, DWORD64 modBase);

	static TCHAR * FormatOutputValue(TCHAR * pszCurrBuffer, BasicType basicType, DWORD64 length, PVOID pAddress);

	static bool WriteMiniDump(PEXCEPTION_POINTERS pExceptionInfo);

	static int __cdecl _tprintf(const TCHAR * format, ...);

	static void Instance(int bShowDlg = false, int bRestart = false);

protected:
	static TCHAR						m_szLogFileName[MAX_PATH];
	static TCHAR						m_szDmpFileName[MAX_PATH];
	static LPTOP_LEVEL_EXCEPTION_FILTER m_previousFilter;
	static HANDLE						m_hProcess;
	static HANDLE						m_hReportFile;
	static HANDLE						m_hDumpFile;
	static FUNCProcessCrash				m_funcProcessCrash;

	static int							m_bShowDlg,m_bRestart;
};



}

extern ingest::ExceptionReport g_exceptionReport;