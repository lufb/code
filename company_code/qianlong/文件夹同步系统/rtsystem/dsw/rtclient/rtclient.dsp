# Microsoft Developer Studio Project File - Name="rtclient" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=rtclient - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "rtclient.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "rtclient.mak" CFG="rtclient - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "rtclient - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "rtclient - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/实时金融资讯平台/rtsystem/dsw", TPVAAAAA"
# PROP Scc_LocalPath ".."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "rtclient - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GR /GX /Zd /O2 /I "..\..\..\gcsystem\common" /I "..\..\..\..\PublicLibrary\EngineLib\UnitCode" /I "..\..\..\..\PublicLibrary\EngineLib\UnitCode\Security\opensslinclude" /I "..\..\rtclient\OptDirInc" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib SERVICEINTERACTIVELIBR.lib /nologo /subsystem:console /incremental:yes /pdb:"D:\Pro\qianlong\service\rtclient\rtclient.pdb" /debug /machine:I386 /out:"D:\Pro\qianlong\service\rtclient\rtclient.exe" /libpath:"..\..\..\..\PublicLibrary\LibOutput"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "rtclient - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "..\..\..\gcsystem\common" /I "..\..\..\..\PublicLibrary\EngineLib\UnitCode" /I "..\..\..\..\PublicLibrary\EngineLib\UnitCode\Security\opensslinclude" /I "..\..\rtclient\OptDirInc" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib SERVICEINTERACTIVELIBD.lib /nologo /subsystem:console /debug /machine:I386 /out:"D:\qianlong\service\hqsystem2\rtclient.exe" /pdbtype:sept /libpath:"..\..\..\..\PublicLibrary\LibOutput"

!ENDIF 

# Begin Target

# Name "rtclient - Win32 Release"
# Name "rtclient - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "OptDirSrc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\common\chunk.c
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\OptDirSrc\Communicate.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\OptDirSrc\DealwithProtocol.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\OptDirSrc\OptDirMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\OptDirSrc\OptFileSystem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\OptDirSrc\OptIniFileContent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\rbtree.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\rtclient\ClientComm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\Control.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\dbf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\file.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\filedata.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\fileList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\Global.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\longkind.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\Option.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\Process.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\Proxy.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\Queue.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\RTClient.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\Status.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "OptDirInc"

# PROP Default_Filter ""
# Begin Group "bits"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\common\bits\chunk.h
# End Source File
# Begin Source File

SOURCE=..\..\common\bits\features.h
# End Source File
# Begin Source File

SOURCE=..\..\common\bits\list.h
# End Source File
# Begin Source File

SOURCE=..\..\common\bits\local.h
# End Source File
# Begin Source File

SOURCE=..\..\common\bits\lock.h
# End Source File
# Begin Source File

SOURCE=..\..\common\bits\typedef.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\common\chunk.h
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\OptDirInc\Communicate.h
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\OptDirInc\DealwithProtocol.h
# End Source File
# Begin Source File

SOURCE=..\..\common\OptBaseDef.h
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\OptDirInc\OptDirMgr.h
# End Source File
# Begin Source File

SOURCE=..\..\common\OptDirTranNetData.h
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\OptDirInc\OptFileSystem.h
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\OptDirInc\OptIniFileContent.h
# End Source File
# Begin Source File

SOURCE=..\..\common\OptStructDef.h
# End Source File
# Begin Source File

SOURCE=..\..\common\rbtree.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\rtclient\ClientComm.h
# End Source File
# Begin Source File

SOURCE=..\..\common\commstruct.h
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\Control.h
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\dbf.h
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\file.h
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\filedata.h
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\fileList.h
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\Global.h
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\include.h
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\longkind.h
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\Option.h
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\Process.h
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\Proxy.h
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\Queue.h
# End Source File
# Begin Source File

SOURCE=..\..\rtclient\Status.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Doc Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\Doc\RT接收机版本说明书.xls"
# End Source File
# End Group
# End Target
# End Project
