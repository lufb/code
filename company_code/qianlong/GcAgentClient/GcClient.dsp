# Microsoft Developer Studio Project File - Name="GcClient" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=GcClient - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GcClient.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GcClient.mak" CFG="GcClient - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GcClient - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "GcClient - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "GcClient"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GcClient - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GCCLIENT_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zd /O2 /I "./lib" /I "./base" /I "./core" /I "./interface" /I "./agent" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GCCLIENT_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib ws2_32.lib netapi32.lib /nologo /dll /incremental:yes /map /debug /machine:I386 /out:"D:\pro\qianlong\service\improved-gc\GcClient.dll"

!ELSEIF  "$(CFG)" == "GcClient - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GCCLIENT_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "./lib" /I "./base" /I "./core" /I "./interface" /I "./agent" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GCCLIENT_EXPORTS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib ws2_32.lib netapi32.lib /nologo /dll /debug /machine:I386 /out:"E:\MyDataManager\code\平时写的测试代码\GcAgentTest\Debug\GcClient.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "GcClient - Win32 Release"
# Name "GcClient - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "lib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lib\list.h
# End Source File
# Begin Source File

SOURCE=.\lib\ReadMe.txt
# End Source File
# End Group
# Begin Group "base"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\base\error.h
# End Source File
# Begin Source File

SOURCE=.\base\MAgeComm.cpp
# End Source File
# Begin Source File

SOURCE=.\base\MAgeComm.h
# End Source File
# Begin Source File

SOURCE=.\base\MBaseConfig.h
# End Source File
# Begin Source File

SOURCE=.\base\MBaseFunc.cpp
# End Source File
# Begin Source File

SOURCE=.\base\MBaseFunc.h
# End Source File
# Begin Source File

SOURCE=.\base\MCrc32.cpp
# End Source File
# Begin Source File

SOURCE=.\base\MCrc32.h
# End Source File
# Begin Source File

SOURCE=.\base\MHttpParase.cpp
# End Source File
# Begin Source File

SOURCE=.\base\MHttpParase.h
# End Source File
# Begin Source File

SOURCE=.\base\MList.cpp
# End Source File
# Begin Source File

SOURCE=.\base\MList.h
# End Source File
# Begin Source File

SOURCE=.\base\MLocalLock.cpp
# End Source File
# Begin Source File

SOURCE=.\base\MLocalLock.h
# End Source File
# Begin Source File

SOURCE=.\base\MLog.cpp
# End Source File
# Begin Source File

SOURCE=.\base\MLog.h
# End Source File
# Begin Source File

SOURCE=.\base\MNetRand.cpp
# End Source File
# Begin Source File

SOURCE=.\base\MNetRand.h
# End Source File
# Begin Source File

SOURCE=.\base\MOption.cpp
# End Source File
# Begin Source File

SOURCE=.\base\MOption.h
# End Source File
# Begin Source File

SOURCE=.\base\MSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\base\MSocket.h
# End Source File
# Begin Source File

SOURCE=.\base\ReadMe.txt
# End Source File
# End Group
# Begin Group "core"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core\MAgent.cpp
# End Source File
# Begin Source File

SOURCE=.\core\MAgent.h
# End Source File
# Begin Source File

SOURCE=.\core\MConnect.cpp
# End Source File
# Begin Source File

SOURCE=.\core\MConnect.h
# End Source File
# Begin Source File

SOURCE=.\core\MCoreMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\core\MCoreMgr.h
# End Source File
# Begin Source File

SOURCE=.\core\MGlobal.cpp
# End Source File
# Begin Source File

SOURCE=.\core\MGlobal.h
# End Source File
# Begin Source File

SOURCE=.\core\MHand.cpp
# End Source File
# Begin Source File

SOURCE=.\core\MHand.h
# End Source File
# Begin Source File

SOURCE=.\core\MListen.cpp
# End Source File
# Begin Source File

SOURCE=.\core\MListen.h
# End Source File
# Begin Source File

SOURCE=.\core\MRcvOpen.cpp
# End Source File
# Begin Source File

SOURCE=.\core\MRcvOpen.h
# End Source File
# Begin Source File

SOURCE=.\core\MTrans.cpp
# End Source File
# Begin Source File

SOURCE=.\core\MTrans.h
# End Source File
# Begin Source File

SOURCE=.\core\ReadMe.txt
# End Source File
# End Group
# Begin Group "interface"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\interface\gcCliInterface.h
# End Source File
# Begin Source File

SOURCE=.\interface\ReadMe.txt
# End Source File
# End Group
# Begin Source File

SOURCE=.\GcClient.cpp
# End Source File
# Begin Source File

SOURCE=.\gcClient.def
# End Source File
# Begin Source File

SOURCE=.\GcClient.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
