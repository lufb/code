# Microsoft Developer Studio Project File - Name="argo_cli" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=argo_cli - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "argo_cli.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "argo_cli.mak" CFG="argo_cli - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "argo_cli - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "argo_cli - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "argo_cli - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zd /O2 /I "./lib" /I "./win_crash_dump/" /I "./compress/" /I "./mask/" /I "./mem_vf" /I "./log" /I "./sock" /I "./base" /I "./global" /I "./core" /I "./pro" /I "./option" /I "./openssl/include" /I "./zlib/include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib ws2_32.lib libeay32.lib zlib.lib /nologo /subsystem:console /incremental:yes /pdb:"bin/argo_cli.pdb" /map:"bin/argo_cli.map" /debug /machine:I386 /out:"bin/argo_cli.exe" /libpath:"openssl/lib" /libpath:"zlib/lib"

!ELSEIF  "$(CFG)" == "argo_cli - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "./lib" /I "./win_crash_dump/" /I "./compress/" /I "./mask/" /I "./mem_vf" /I "./log" /I "./sock" /I "./base" /I "./global" /I "./core" /I "./pro" /I "./option" /I "./openssl/include" /I "./zlib/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib ws2_32.lib netapi32.lib libeay32.lib zlib.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"openssl/lib" /libpath:"zlib/lib"

!ENDIF 

# Begin Target

# Name "argo_cli - Win32 Release"
# Name "argo_cli - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\argo_cli.cpp
# End Source File
# Begin Source File

SOURCE=.\argo_cli.h
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# End Group
# Begin Group "lib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lib\list.h
# End Source File
# End Group
# Begin Group "log"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\log\argo_log.cpp
# End Source File
# Begin Source File

SOURCE=.\log\argo_log.h
# End Source File
# End Group
# Begin Group "sock"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\sock\argo_sock.cpp
# End Source File
# Begin Source File

SOURCE=.\sock\argo_sock.h
# End Source File
# End Group
# Begin Group "base"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\base\argo_base.cpp
# End Source File
# Begin Source File

SOURCE=.\base\argo_base.h
# End Source File
# Begin Source File

SOURCE=.\base\argo_ckcrc16.c
# End Source File
# Begin Source File

SOURCE=.\base\argo_ckcrc16.h
# End Source File
# Begin Source File

SOURCE=.\base\argo_error.h
# End Source File
# Begin Source File

SOURCE=.\base\MCheckCode.cpp
# End Source File
# Begin Source File

SOURCE=.\base\MCheckCode.h
# End Source File
# Begin Source File

SOURCE=.\base\MDateTime.cpp
# End Source File
# Begin Source File

SOURCE=.\base\MDateTime.h
# End Source File
# Begin Source File

SOURCE=.\base\MDll.cpp
# End Source File
# Begin Source File

SOURCE=.\base\MDll.h
# End Source File
# Begin Source File

SOURCE=.\base\MErrorCode.cpp
# End Source File
# Begin Source File

SOURCE=.\base\MErrorCode.h
# End Source File
# Begin Source File

SOURCE=.\base\MFile.cpp
# End Source File
# Begin Source File

SOURCE=.\base\MFile.h
# End Source File
# Begin Source File

SOURCE=.\base\MIniFile.cpp
# End Source File
# Begin Source File

SOURCE=.\base\MIniFile.h
# End Source File
# Begin Source File

SOURCE=.\base\MMutex.cpp
# End Source File
# Begin Source File

SOURCE=.\base\MMutex.h
# End Source File
# Begin Source File

SOURCE=.\base\MSelfRiseArray.cpp
# End Source File
# Begin Source File

SOURCE=.\base\MSelfRiseArray.h
# End Source File
# Begin Source File

SOURCE=.\base\MString.cpp
# End Source File
# Begin Source File

SOURCE=.\base\MString.h
# End Source File
# Begin Source File

SOURCE=.\base\MSystemInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\base\MSystemInfo.h
# End Source File
# Begin Source File

SOURCE=.\base\MThread.cpp
# End Source File
# Begin Source File

SOURCE=.\base\MThread.h
# End Source File
# Begin Source File

SOURCE=.\base\MTypeDefine.cpp
# End Source File
# Begin Source File

SOURCE=.\base\MTypeDefine.h
# End Source File
# End Group
# Begin Group "global"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\global\argo_global.cpp
# End Source File
# Begin Source File

SOURCE=.\global\argo_global.h
# End Source File
# End Group
# Begin Group "core"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core\argo_cli_mgr.cpp
# End Source File
# Begin Source File

SOURCE=.\core\argo_cli_mgr.h
# End Source File
# End Group
# Begin Group "pro"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pro\argo_pro.h
# End Source File
# End Group
# Begin Group "option"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\option\argo_option.cpp
# End Source File
# Begin Source File

SOURCE=.\option\argo_option.h
# End Source File
# End Group
# Begin Group "mask"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\mask\mask_decom.cpp
# End Source File
# Begin Source File

SOURCE=.\mask\mask_decom.h
# End Source File
# End Group
# Begin Group "compress"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\compress\zlib_compress.cpp
# End Source File
# Begin Source File

SOURCE=.\compress\zlib_compress.h
# End Source File
# End Group
# Begin Group "win_crash_dump"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\win_crash_dump\DbgHelp.h
# End Source File
# Begin Source File

SOURCE=.\win_crash_dump\ExceptionReport.cpp
# End Source File
# Begin Source File

SOURCE=.\win_crash_dump\ExceptionReport.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\makefile
# End Source File
# End Target
# End Project
