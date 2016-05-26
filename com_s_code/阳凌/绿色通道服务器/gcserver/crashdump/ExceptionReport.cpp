#include "stdafx.h"
#include ".\exceptionreport.h"
#include <atlconv.h>

using namespace ingest;

ingest::ExceptionReport g_exceptionReport;

static FUNCSymCleanup				pSymCleanup;
static FUNCSymInitialize			pSymInitialize;
static FUNCSymGetOptions			pSymGetOptions;
static FUNCSymSetOptions			pSymSetOptions;
static FUNCSymEnumSymbols			pSymEnumSymbols;
static FUNCSymSetContext			pSymSetContext;
static FUNCSymGetLineFromAddr		pSymGetLineFromAddr;
static FUNCSymFromAddr				pSymFromAddr;
static FUNCStackWalk				pStackWalk;
static FUNCSymFunctionTableAccess	pSymFunctionTableAccess;
static FUNCSymGetModuleBase			pSymGetModuleBase;
static FUNCSymGetTypeInfo			pSymGetTypeInfo;
static FUNCMiniDumpWriteDump		pMiniDumpWriteDump;

TCHAR								ExceptionReport::m_szLogFileName[MAX_PATH];
TCHAR								ExceptionReport::m_szDmpFileName[MAX_PATH];
LPTOP_LEVEL_EXCEPTION_FILTER		ExceptionReport::m_previousFilter = NULL;
HANDLE								ExceptionReport::m_hProcess = NULL;
HANDLE								ExceptionReport::m_hReportFile = INVALID_HANDLE_VALUE;
HANDLE								ExceptionReport::m_hDumpFile = INVALID_HANDLE_VALUE;
FUNCProcessCrash					ExceptionReport::m_funcProcessCrash = NULL;

int									ExceptionReport::m_bShowDlg = false;
int									ExceptionReport::m_bRestart = false;


void CALLBACK Func(int bShowDlg , int bRestart, PEXCEPTION_POINTERS pExceptionInfo )
{
	if( bShowDlg )
	{
		int		nLen = 0;
		TCHAR	szMessage[2048];

		memset(szMessage, 0, sizeof(szMessage));

		PEXCEPTION_RECORD pExceptionRecord = pExceptionInfo->ExceptionRecord;

		// First print information about the type of fault
		nLen = sprintf(szMessage + nLen,   TEXT("Exception code: %08X %s\r\n"),
			pExceptionRecord->ExceptionCode,
			ExceptionReport::GetExceptionString(pExceptionRecord->ExceptionCode) );
		
		// Now print information about where the fault occured
		TCHAR szFaultingModule[MAX_PATH];
		DWORD section, offset;
		ExceptionReport::GetLogicalAddress(  pExceptionRecord->ExceptionAddress,
			szFaultingModule,
			sizeof( szFaultingModule ),
			section, offset );
		
		nLen = sprintf(szMessage + nLen, TEXT("Fault address:  %08X %02X:%08X %s\r\n"),
			(DWORD)pExceptionRecord->ExceptionAddress,
			section, offset, szFaultingModule );

		strcat(szMessage,
			"程序异常终止\n"
			"请将进程执行目录下的crashdump目录下最新的*.dmp *.rpt文件以及当前软件版本号一并提供软件支持商,\n"
			"以便及时排查解决问题.谢谢!");
		MessageBox(NULL, szMessage, "上海乾隆高科技有限公司 -  成都开发中心(cdyfzx@qianlong2.net)", MB_OK | MB_ICONSTOP);
	}

	if( bRestart )
	{
		PROCESS_INFORMATION   info;   
		STARTUPINFO startup;   
		TCHAR szPath[128];   
		TCHAR *szCmdLine;
		GetModuleFileName(AfxGetApp()->m_hInstance,   szPath,   sizeof(szPath));   
		szCmdLine   =   GetCommandLine();   
		GetStartupInfo(&startup);   
		BOOL   bSucc   =   CreateProcess(szPath,   szCmdLine,   NULL,   NULL,   
			FALSE,   NORMAL_PRIORITY_CLASS,   NULL,   NULL,   &startup,   &info); 
    }
		
}


void ExceptionReport::Instance(int bShowDlg /*= false*/, int bRestart /*= false*/)
{
	g_exceptionReport.SetProcessCrashFunc(Func,bShowDlg,bRestart);

}
ExceptionReport::ExceptionReport(void)
{
	//Install the unhandled exception filter function
	m_previousFilter = SetUnhandledExceptionFilter(ExceptionReport::UnhandledExceptionFilter);

	// Figure out what the report file will be named, and store it away
    GetModuleFileName( 0, m_szLogFileName, MAX_PATH );

    PTSTR pszDot = _tcsrchr( m_szLogFileName, TEXT('\\') );
    if ( pszDot )
    {
		pszDot++;   // Advance past the '\\'

		_tcscpy(m_szDmpFileName, pszDot);
		
		//接上存储崩溃转储信息的目录
		_tcscpy(pszDot, TEXT("crashdump\\"));

		//如果目录不存在,则创建
		if(-1 == GetFileAttributes(m_szLogFileName))
		{
			CreateDirectory(m_szLogFileName, NULL);
		}

		//处理并接上文件名
		pszDot = _tcsrchr(m_szDmpFileName, TEXT('.'));
		if(pszDot)
		{
			*pszDot = 0;
		}
		_tcscat(m_szLogFileName, m_szDmpFileName);
		_tcscat(m_szLogFileName, TEXT("_"));

		_tcscpy( m_szDmpFileName, m_szLogFileName );
    }

	m_hProcess = GetCurrentProcess();
}

ExceptionReport::~ExceptionReport(void)
{
	SetUnhandledExceptionFilter(m_previousFilter);
}

LONG WINAPI ExceptionReport::UnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
	HMODULE hDll=LoadLibrary(TEXT("dbghelp.dll"));
	if(!hDll)
	{
		return EXCEPTION_EXECUTE_HANDLER;
		/*if (m_previousFilter)
			return m_previousFilter(pExceptionInfo);
		else
			return EXCEPTION_CONTINUE_SEARCH;*/
	}

	pSymCleanup				= (FUNCSymCleanup)GetProcAddress(hDll, "SymCleanup");
	pSymInitialize			= (FUNCSymInitialize)GetProcAddress(hDll, "SymInitialize");
	pSymGetOptions			= (FUNCSymGetOptions)GetProcAddress(hDll, "SymGetOptions");
	pSymSetOptions			= (FUNCSymSetOptions)GetProcAddress(hDll, "SymSetOptions");
	pSymEnumSymbols			= (FUNCSymEnumSymbols)GetProcAddress(hDll, "SymEnumSymbols");
	pSymSetContext			= (FUNCSymSetContext)GetProcAddress(hDll, "SymSetContext");
	pSymGetLineFromAddr		= (FUNCSymGetLineFromAddr)GetProcAddress(hDll, "SymGetLineFromAddr");
	pSymFromAddr			= (FUNCSymFromAddr)GetProcAddress(hDll, "SymFromAddr");
	pStackWalk				= (FUNCStackWalk)GetProcAddress(hDll, "StackWalk");
	pSymFunctionTableAccess	= (FUNCSymFunctionTableAccess)GetProcAddress(hDll, "SymFunctionTableAccess");
	pSymGetModuleBase		= (FUNCSymGetModuleBase)GetProcAddress(hDll, "SymGetModuleBase");
	pSymGetTypeInfo			= (FUNCSymGetTypeInfo)GetProcAddress(hDll, "SymGetTypeInfo");
	pMiniDumpWriteDump		= (FUNCMiniDumpWriteDump)GetProcAddress(hDll, "MiniDumpWriteDump");

	if (!pSymCleanup			||
		!pSymInitialize			||
		!pSymGetOptions			||
		!pSymSetOptions			||
		!pSymEnumSymbols		||
		!pSymSetContext			||
		!pSymGetLineFromAddr	||
		!pSymFromAddr			||
		!pStackWalk				||
		!pSymFunctionTableAccess||
		!pSymGetModuleBase		||
		!pSymGetTypeInfo		||
		!pMiniDumpWriteDump)
	{
		FreeLibrary(hDll);
		/*if ( m_previousFilter )
			return m_previousFilter( pExceptionInfo );
		else
			return EXCEPTION_CONTINUE_SEARCH;*/
		return EXCEPTION_EXECUTE_HANDLER;
	}

	//根据发生异常的时间,生成report和dump信息文件名
	SYSTEMTIME st;
	GetLocalTime(&st);

	TCHAR buff[64];
	wsprintf(buff, TEXT("%04d%02d%02d%02d%02d%02d"), st.wYear, st.wMonth, st.wDay, 
		st.wHour, st.wMinute, st.wSecond);

	_tcscat(m_szLogFileName, buff);
	_tcscat(m_szLogFileName, TEXT(".rpt"));

	_tcscat(m_szDmpFileName, buff);
	_tcscat(m_szDmpFileName, TEXT(".dmp"));

	m_hReportFile = CreateFile(m_szLogFileName,
									GENERIC_WRITE,
									FILE_SHARE_READ,
									0,
									CREATE_ALWAYS,
									FILE_FLAG_WRITE_THROUGH,
									0);
		
	m_hDumpFile = CreateFile(m_szDmpFileName,
									GENERIC_WRITE,
									FILE_SHARE_READ,
									0,
									CREATE_ALWAYS,
									FILE_FLAG_WRITE_THROUGH,
									0);

	//记录错误报告和崩溃转储信息
	__try
	{

		if(m_hDumpFile != INVALID_HANDLE_VALUE)
		{
			WriteMiniDump(pExceptionInfo);
		}

		if(m_hReportFile != INVALID_HANDLE_VALUE)
		{
			GenerateExceptionReport(pExceptionInfo);
		}
	}
	__finally
	{
		if(m_hReportFile != INVALID_HANDLE_VALUE)
			CloseHandle(m_hReportFile);
	}

	/*if(IDOK == MessageBox(NULL, TEXT("Ingest Terminal has encountered some troubles!\r\n\r\n")\
		TEXT("It's need to be restarted.\r\n")\
		TEXT("Sorry for the discommodiousness to your work!\r\n\r\n")\
		TEXT("Click OK to restart Ingest Terminal\r\n")\
		TEXT("Click Cancel to exit"), 
		TEXT("Ingest Terminal"), MB_OKCANCEL))
	{*/
	//调用外部处理函数
	if(m_funcProcessCrash)
		m_funcProcessCrash(m_bShowDlg,m_bRestart, pExceptionInfo);
	//}

	FreeLibrary(hDll);
    /*if ( m_previousFilter )
		return m_previousFilter( pExceptionInfo );
	else
        return EXCEPTION_CONTINUE_SEARCH;*/
	return EXCEPTION_EXECUTE_HANDLER;
}

void ExceptionReport::GenerateExceptionReport(PEXCEPTION_POINTERS pExceptionInfo)
{
	// Start out with a banner
    _tprintf(TEXT("//=====================================================\r\n"));
	/*_tprintf(_T("System details:\r\n"));
	TCHAR buffer[200];
	if (DisplaySystemVersion(buffer))
		_tprintf(_T("%s\r\n"), buffer);
	CProcessorInfo pi;
	CMemoryInfo mi;
	_tprintf(_T("%s\r\n"), (LPCTSTR)pi.GetProcessorName());
	_tprintf(_T("%s\r\n\r\n"), (LPCTSTR)mi.GetMemoryInfo());*/

    PEXCEPTION_RECORD pExceptionRecord = pExceptionInfo->ExceptionRecord;

    // First print information about the type of fault
    _tprintf(   TEXT("Exception code: %08X %s\r\n"),
                pExceptionRecord->ExceptionCode,
                GetExceptionString(pExceptionRecord->ExceptionCode) );

    // Now print information about where the fault occured
    TCHAR szFaultingModule[MAX_PATH];
    DWORD section, offset;
    GetLogicalAddress(  pExceptionRecord->ExceptionAddress,
                        szFaultingModule,
                        sizeof( szFaultingModule ),
                        section, offset );

    _tprintf( TEXT("Fault address:  %08X %02X:%08X %s\r\n"),
                (DWORD)pExceptionRecord->ExceptionAddress,
                section, offset, szFaultingModule );

    PCONTEXT pCtx = pExceptionInfo->ContextRecord;

    // Show the registers
    #ifdef _M_IX86  // X86 Only!
    _tprintf( TEXT("\r\nRegisters:\r\n") );

    _tprintf(TEXT("EAX:%08X\r\nEBX:%08X\r\nECX:%08X\r\nEDX:%08X\r\nESI:%08X\r\nEDI:%08X\r\n")
            ,pCtx->Eax, pCtx->Ebx, pCtx->Ecx, pCtx->Edx,
            pCtx->Esi, pCtx->Edi );

    _tprintf( TEXT("CS:EIP:%04X:%08X\r\n"), pCtx->SegCs, pCtx->Eip );
    _tprintf( TEXT("SS:ESP:%04X:%08X  EBP:%08X\r\n"),
                pCtx->SegSs, pCtx->Esp, pCtx->Ebp );
    _tprintf( TEXT("DS:%04X  ES:%04X  FS:%04X  GS:%04X\r\n"),
                pCtx->SegDs, pCtx->SegEs, pCtx->SegFs, pCtx->SegGs );
    _tprintf( TEXT("Flags:%08X\r\n"), pCtx->EFlags );

    #endif

	// Set up the symbol engine.
	DWORD dwOpts = pSymGetOptions() ;

	// Turn on line loading and deferred loading.
	pSymSetOptions( dwOpts                |
                        SYMOPT_DEFERRED_LOADS |
                        SYMOPT_LOAD_LINES      ) ;
	
    // Initialize DbgHelp
    if ( !pSymInitialize( GetCurrentProcess(), 0, TRUE ) )
        return;

    CONTEXT trashableContext = *pCtx;

    WriteStackDetails( &trashableContext, false );

    //#ifdef _M_IX86  // X86 Only!

    //_tprintf( _T("========================\r\n") );
    //_tprintf( _T("Local Variables And Parameters\r\n") );

    //trashableContext = *pCtx;
    //WriteStackDetails( &trashableContext, true );

    ///*_tprintf( _T("========================\r\n") );
    //_tprintf( _T("Global Variables\r\n") );

    //SymEnumSymbols( GetCurrentProcess(),
    //                (DWORD64)GetModuleHandle(szFaultingModule),
    //                0, EnumerateSymbolsCallback, 0 );
    //*/
    //#endif      // X86 Only!

    pSymCleanup( GetCurrentProcess() );

    _tprintf( TEXT("\r\n") );
}

LPTSTR ExceptionReport::GetExceptionString(DWORD dwCode)
{
	#define EXCEPTION( x ) case EXCEPTION_##x: return TEXT(#x);

	switch (dwCode)
	{
		EXCEPTION( ACCESS_VIOLATION )
		EXCEPTION( DATATYPE_MISALIGNMENT )
		EXCEPTION( BREAKPOINT )
		EXCEPTION( SINGLE_STEP )
		EXCEPTION( ARRAY_BOUNDS_EXCEEDED )
		EXCEPTION( FLT_DENORMAL_OPERAND )
		EXCEPTION( FLT_DIVIDE_BY_ZERO )
		EXCEPTION( FLT_INEXACT_RESULT )
		EXCEPTION( FLT_INVALID_OPERATION )
		EXCEPTION( FLT_OVERFLOW )
		EXCEPTION( FLT_STACK_CHECK )
		EXCEPTION( FLT_UNDERFLOW )
		EXCEPTION( INT_DIVIDE_BY_ZERO )
		EXCEPTION( INT_OVERFLOW )
		EXCEPTION( PRIV_INSTRUCTION )
		EXCEPTION( IN_PAGE_ERROR )
		EXCEPTION( ILLEGAL_INSTRUCTION )
		EXCEPTION( NONCONTINUABLE_EXCEPTION )
		EXCEPTION( STACK_OVERFLOW )
		EXCEPTION( INVALID_DISPOSITION )
		EXCEPTION( GUARD_PAGE )
		EXCEPTION( INVALID_HANDLE )
	}

	// If not one of the "known" exceptions, try to get the string
    // from NTDLL.DLL's message table.

    static TCHAR szBuffer[512] = { 0 };

    FormatMessage( FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_HMODULE,
                   GetModuleHandle( TEXT("NTDLL.DLL") ),
                   dwCode, 0, szBuffer, sizeof( szBuffer ), 0 );

    return szBuffer;
}

BOOL ExceptionReport::GetLogicalAddress(PVOID addr, PTSTR szModule, DWORD len, DWORD& section, DWORD& offset)
{
	MEMORY_BASIC_INFORMATION mbi;

    if (!VirtualQuery( addr, &mbi, sizeof(mbi) ))
        return FALSE;

    DWORD hMod = (DWORD)mbi.AllocationBase;

    if ( !GetModuleFileName( (HMODULE)hMod, szModule, len ) )
        return FALSE;

    // Point to the DOS header in memory
    PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)hMod;

    // From the DOS header, find the NT (PE) header
    PIMAGE_NT_HEADERS pNtHdr = (PIMAGE_NT_HEADERS)(hMod + pDosHdr->e_lfanew);

    PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION( pNtHdr );

    DWORD rva = (DWORD)addr - hMod; // RVA is offset from module load address

    // Iterate through the section table, looking for the one that encompasses
    // the linear address.
    for (   unsigned i = 0;
            i < pNtHdr->FileHeader.NumberOfSections;
            i++, pSection++ )
    {
        DWORD sectionStart = pSection->VirtualAddress;
        DWORD sectionEnd = sectionStart
                    + max(pSection->SizeOfRawData, pSection->Misc.VirtualSize);

        // Is the address in this section???
        if ( (rva >= sectionStart) && (rva <= sectionEnd) )
        {
            // Yes, address is in the section.  Calculate section and offset,
            // and store in the "section" & "offset" params, which were
            // passed by reference.
            section = i+1;
            offset = rva - sectionStart;
            return TRUE;
        }
    }

    return FALSE;   // Should never get here!
}

void ExceptionReport::WriteStackDetails(PCONTEXT pContext, bool bWriteVariables)
{
	USES_CONVERSION;
    _tprintf( TEXT("\r\nCall stack:\r\n") );

    _tprintf( TEXT("Address   Frame     Function            SourceFile\r\n") );

    DWORD dwMachineType = 0;
    // Could use SymSetOptions here to add the SYMOPT_DEFERRED_LOADS flag

    STACKFRAME sf;
    memset( &sf, 0, sizeof(sf) );

    #ifdef _M_IX86
    // Initialize the STACKFRAME structure for the first call.  This is only
    // necessary for Intel CPUs, and isn't mentioned in the documentation.
    sf.AddrPC.Offset       = pContext->Eip;
    sf.AddrPC.Mode         = AddrModeFlat;
    sf.AddrStack.Offset    = pContext->Esp;
    sf.AddrStack.Mode      = AddrModeFlat;
    sf.AddrFrame.Offset    = pContext->Ebp;
    sf.AddrFrame.Mode      = AddrModeFlat;

    dwMachineType = IMAGE_FILE_MACHINE_I386;
    #endif

    while ( 1 )
    {
        // Get the next stack frame
        if ( ! pStackWalk(  dwMachineType,
                            m_hProcess,
                            GetCurrentThread(),
                            &sf,
                            pContext,
                            0,
                            pSymFunctionTableAccess,
                            pSymGetModuleBase,
                            0 ) )
            break;

        if ( 0 == sf.AddrFrame.Offset ) // Basic sanity check to make sure
            break;                      // the frame is OK.  Bail if not.

        _tprintf( TEXT("%08X  %08X  "), sf.AddrPC.Offset, sf.AddrFrame.Offset );

        // Get the name of the function for this stack frame entry
        BYTE symbolBuffer[ sizeof(SYMBOL_INFO) + 1024 ];
        PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)symbolBuffer;
        pSymbol->SizeOfStruct = sizeof(symbolBuffer);
        pSymbol->MaxNameLen = 1024;
                        
        DWORD64 symDisplacement = 0;    // Displacement of the input address,
                                        // relative to the start of the symbol

        if ( pSymFromAddr(m_hProcess,sf.AddrPC.Offset,&symDisplacement,pSymbol))
        {
            _tprintf( TEXT("%hs+%I64X"), pSymbol->Name, symDisplacement );
            
        }
        else    // No symbol found.  Print out the logical address instead.
        {
            TCHAR szModule[MAX_PATH] = TEXT("");
            DWORD section = 0, offset = 0;

            GetLogicalAddress(  (PVOID)sf.AddrPC.Offset,
                                szModule, sizeof(szModule), section, offset );

            _tprintf( TEXT("%04X:%08X %s"), section, offset, szModule );
        }

        // Get the source line for this stack frame entry
        IMAGEHLP_LINE lineInfo = { sizeof(IMAGEHLP_LINE) };
        DWORD dwLineDisplacement;
        if ( pSymGetLineFromAddr( m_hProcess, sf.AddrPC.Offset,
                                &dwLineDisplacement, &lineInfo ) )
        {
            _tprintf(TEXT("  %s line %u"), A2T(lineInfo.FileName), lineInfo.LineNumber); 
        }

        _tprintf( TEXT("\r\n") );

        // Write out the variables, if desired
        if ( bWriteVariables )
        {
            // Use SymSetContext to get just the locals/params for this frame
            IMAGEHLP_STACK_FRAME imagehlpStackFrame;
            imagehlpStackFrame.InstructionOffset = sf.AddrPC.Offset;
            pSymSetContext( m_hProcess, &imagehlpStackFrame, 0 );

            // Enumerate the locals/parameters
            pSymEnumSymbols( m_hProcess, 0, 0, EnumerateSymbolsCallback, &sf );

            _tprintf( TEXT("\r\n") );
        }
    }
}

BOOL CALLBACK ExceptionReport::EnumerateSymbolsCallback(PSYMBOL_INFO pSymInfo, 
														ULONG SymbolSize, PVOID UserContext)
{
	USES_CONVERSION;
    TCHAR szBuffer[4096];

    __try
    {
        if ( FormatSymbolValue( pSymInfo, (STACKFRAME*)UserContext,
                                szBuffer, sizeof(szBuffer) ) )  
            _tprintf( TEXT("\t%s\r\n"), szBuffer );
    }
    __except( 1 )
    {
        _tprintf( TEXT("punting on symbol %s\r\n"), A2T(pSymInfo->Name) );
    }

    return TRUE;
}

bool ExceptionReport::FormatSymbolValue(
            PSYMBOL_INFO pSym,
            STACKFRAME * sf,
            TCHAR * pszBuffer,
            unsigned cbBuffer )
{
	USES_CONVERSION;
    TCHAR * pszCurrBuffer = pszBuffer;

    // Indicate if the variable is a local or parameter
    if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_PARAMETER )
        pszCurrBuffer += _stprintf( pszCurrBuffer, TEXT("Parameter ") );
    else if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_LOCAL )
        pszCurrBuffer += _stprintf( pszCurrBuffer, TEXT("Local ") );

    // If it's a function, don't do anything.
    if ( pSym->Tag == 5 )   // SymTagFunction from CVCONST.H from the DIA SDK
        return false;

    // Emit the variable name
    pszCurrBuffer += _stprintf( pszCurrBuffer, TEXT("\'%s\'"), A2T(pSym->Name) );

    DWORD pVariable = 0;    // Will point to the variable's data in memory

    if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_REGRELATIVE )
    {
        if ( pSym->Register == 8 )   // EBP is the value 8 (in DBGHELP 5.1)
        {                               //  This may change!!!
            pVariable = sf->AddrFrame.Offset;
            pVariable += (DWORD)pSym->Address;
        }
        else
          return false;
    }
    else if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_REGISTER )
    {
        return false;   // Don't try to report register variable
    }
    else
    {
        pVariable = (DWORD)pSym->Address;   // It must be a global variable
    }

    // Determine if the variable is a user defined type (UDT).  IF so, bHandled
    // will return true.
    bool bHandled;
    pszCurrBuffer = DumpTypeIndex(pszCurrBuffer, pSym->ModBase, pSym->TypeIndex,
                                    0, pVariable, bHandled );

    if ( !bHandled )
    {
	    // The symbol wasn't a UDT, so do basic, stupid formatting of the
        // variable.  Based on the size, we're assuming it's a char, WORD, or
        // DWORD.
        BasicType basicType = GetBasicType( pSym->TypeIndex, pSym->ModBase );
        
        pszCurrBuffer = FormatOutputValue(pszCurrBuffer, basicType, pSym->Size,
                                            (PVOID)pVariable ); 
    }


    return true;
}

TCHAR * ExceptionReport::DumpTypeIndex(
        TCHAR * pszCurrBuffer,
        DWORD64 modBase,
        DWORD dwTypeIndex,
        unsigned nestingLevel,
        DWORD offset,
        bool & bHandled )
{
	USES_CONVERSION;
    bHandled = false;

    // Get the name of the symbol.  This will either be a Type name (if a UDT),
    // or the structure member name.
    WCHAR * pwszTypeName;
    if ( pSymGetTypeInfo( m_hProcess, modBase, dwTypeIndex, TI_GET_SYMNAME,
                        &pwszTypeName ) )
    {
        pszCurrBuffer += _stprintf( pszCurrBuffer, TEXT(" %ls"), W2T(pwszTypeName) );
        LocalFree( pwszTypeName );
    }

    // Determine how many children this type has.
    DWORD dwChildrenCount = 0;
    pSymGetTypeInfo( m_hProcess, modBase, dwTypeIndex, TI_GET_CHILDRENCOUNT,
                    &dwChildrenCount );

    if ( !dwChildrenCount )     // If no children, we're done
	{
        return pszCurrBuffer;
	}
    // Prepare to get an array of "TypeIds", representing each of the children.
    // SymGetTypeInfo(TI_FINDCHILDREN) expects more memory than just a
    // TI_FINDCHILDREN_PARAMS struct has.  Use derivation to accomplish this.
    struct FINDCHILDREN : TI_FINDCHILDREN_PARAMS
    {
        ULONG   MoreChildIds[1024];
        FINDCHILDREN(){Count = sizeof(MoreChildIds) / sizeof(MoreChildIds[0]);}
    } children;

    children.Count = dwChildrenCount;
    children.Start= 0;

    // Get the array of TypeIds, one for each child type
    if ( !pSymGetTypeInfo( m_hProcess, modBase, dwTypeIndex, TI_FINDCHILDREN,
                            &children ) )
    {
        return pszCurrBuffer;
    }

    // Append a line feed
    pszCurrBuffer += _stprintf( pszCurrBuffer, TEXT("\r\n") );

    // Iterate through each of the children
    for ( unsigned i = 0; i < dwChildrenCount; i++ )
    {
        // Add appropriate indentation level (since this routine is recursive)
        for ( unsigned j = 0; j <= nestingLevel+1; j++ )
            pszCurrBuffer += _stprintf( pszCurrBuffer, TEXT("\t") );

        // Recurse for each of the child types
        bool bHandled2;
        pszCurrBuffer = DumpTypeIndex( pszCurrBuffer, modBase,
                                        children.ChildId[i], nestingLevel+1,
                                        offset, bHandled2 );

        // If the child wasn't a UDT, format it appropriately
        if ( !bHandled2 )
        {
            // Get the offset of the child member, relative to its parent
            DWORD dwMemberOffset;
            pSymGetTypeInfo( m_hProcess, modBase, children.ChildId[i],
                            TI_GET_OFFSET, &dwMemberOffset );

            // Get the real "TypeId" of the child.  We need this for the
            // SymGetTypeInfo( TI_GET_TYPEID ) call below.
            DWORD typeId;
            pSymGetTypeInfo( m_hProcess, modBase, children.ChildId[i],
                            TI_GET_TYPEID, &typeId );

            // Get the size of the child member
            ULONG64 length;
            pSymGetTypeInfo(m_hProcess, modBase, typeId, TI_GET_LENGTH,&length);

            // Calculate the address of the member
            DWORD dwFinalOffset = offset + dwMemberOffset;

            BasicType basicType = GetBasicType(children.ChildId[i], modBase );

            pszCurrBuffer = FormatOutputValue( pszCurrBuffer, basicType,
                                                length, (PVOID)dwFinalOffset ); 

            pszCurrBuffer += _stprintf( pszCurrBuffer, TEXT("\r\n") );
        }
    }

    bHandled = true;
    return pszCurrBuffer;
}

BasicType ExceptionReport::GetBasicType( DWORD typeIndex, DWORD64 modBase )
{
    BasicType basicType;
    if ( pSymGetTypeInfo( m_hProcess, modBase, typeIndex,
                        TI_GET_BASETYPE, &basicType ) )
    {
        return basicType;
    }

    // Get the real "TypeId" of the child.  We need this for the
    // SymGetTypeInfo( TI_GET_TYPEID ) call below.
    DWORD typeId;
    if (pSymGetTypeInfo(m_hProcess,modBase, typeIndex, TI_GET_TYPEID, &typeId))
    {
        if ( pSymGetTypeInfo( m_hProcess, modBase, typeId, TI_GET_BASETYPE,
                            &basicType ) )
        {
            return basicType;
        }
    }

    return btNoType;
}

TCHAR * ExceptionReport::FormatOutputValue(   TCHAR * pszCurrBuffer,
                                                    BasicType basicType,
                                                    DWORD64 length,
                                                    PVOID pAddress )
{
    // Format appropriately (assuming it's a 1, 2, or 4 bytes (!!!)
    if ( length == 1 )
        pszCurrBuffer += _stprintf( pszCurrBuffer, TEXT(" = %X"), *(PBYTE)pAddress );
    else if ( length == 2 )
        pszCurrBuffer += _stprintf( pszCurrBuffer, TEXT(" = %X"), *(PWORD)pAddress );
    else if ( length == 4 )
    {
        if ( basicType == btFloat )
        {
            pszCurrBuffer += _stprintf(pszCurrBuffer, TEXT(" = %f"), *(PFLOAT)pAddress);
        }
        else if ( basicType == btChar )
        {
            if ( !IsBadStringPtr( *(LPTSTR*)pAddress, 32) )
            {
                pszCurrBuffer += _stprintf( pszCurrBuffer, TEXT(" = \"%.31s\""),
                                            *(PDWORD)pAddress );
            }
            else
                pszCurrBuffer += _stprintf( pszCurrBuffer, TEXT(" = %X"),
                                            *(PDWORD)pAddress );
        }
        else
		{
            pszCurrBuffer += _stprintf(pszCurrBuffer, TEXT(" = %X"), *(PDWORD)pAddress);
		}
    }
    else if ( length == 8 )
    {
        if ( basicType == btFloat )
        {
            pszCurrBuffer += _stprintf( pszCurrBuffer, TEXT(" = %lf"),
                                        *(double *)pAddress );
        }
        else
            pszCurrBuffer += _stprintf( pszCurrBuffer, TEXT(" = %I64X"),
                                        *(DWORD64*)pAddress );
    }

    return pszCurrBuffer;
}

bool ExceptionReport::WriteMiniDump(PEXCEPTION_POINTERS pExceptionInfo)
{
	//
	// Write the minidump to the file
	//
	MINIDUMP_EXCEPTION_INFORMATION eInfo;
	eInfo.ThreadId = GetCurrentThreadId();
	eInfo.ExceptionPointers = pExceptionInfo;
	eInfo.ClientPointers = FALSE;

	MINIDUMP_CALLBACK_INFORMATION cbMiniDump;
	cbMiniDump.CallbackRoutine = 0;
	cbMiniDump.CallbackParam = 0;


	pMiniDumpWriteDump(
		GetCurrentProcess(),
		GetCurrentProcessId(),
		m_hDumpFile,
		MiniDumpNormal,
		pExceptionInfo ? &eInfo : NULL,
		NULL,
		&cbMiniDump);

	// Close file
	CloseHandle(m_hDumpFile);

	return true;
}

int __cdecl ExceptionReport::_tprintf(const TCHAR * format, ...)
{
    TCHAR szBuff[1024];
    int retValue;
    DWORD cbWritten;
    va_list argptr;
          
    va_start( argptr, format );
    retValue = _vstprintf( szBuff, format, argptr );
    va_end( argptr );

    WriteFile(m_hReportFile, szBuff, retValue * sizeof(TCHAR), &cbWritten, 0 );

    return retValue;
}