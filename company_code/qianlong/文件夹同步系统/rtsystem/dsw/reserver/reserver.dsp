# Microsoft Developer Studio Project File - Name="reserver" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=reserver - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "reserver.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "reserver.mak" CFG="reserver - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "reserver - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "reserver - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/实时金融资讯平台/rtsystem/dsw", TPVAAAAA"
# PROP Scc_LocalPath ".."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "reserver - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RESERVER_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zd /O2 /I "..\..\..\..\PublicLibrary\EngineLib\UnitCode" /I "..\..\..\..\PublicLibrary\EngineLib\UnitCode\Security\opensslinclude" /I "..\..\..\..\HqSystem\ServicePlat\SrvUnit\SrvUnitIO" /I "..\..\rtserver\OptDirInc" /I "..\..\common" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RESERVER_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"D:\Pro\qianlong\service\rtserver\rtserver.pdb" /debug /machine:I386 /out:"D:\Pro\qianlong\service\rtserver\rtserver.dll" /libpath:"..\..\..\..\PublicLibrary\LibOutput" /def:../../rtserver/RTServer.def
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "reserver - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RESERVER_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\PublicLibrary\EngineLib\UnitCode" /I "..\..\..\..\PublicLibrary\EngineLib\UnitCode\Security\opensslinclude" /I "..\..\..\..\HqSystem\ServicePlat\SrvUnit\SrvUnitIO" /I "..\..\rtserver\OptDirInc" /I "..\..\common" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RESERVER_EXPORTS" /FR /FD /GZ /c
# SUBTRACT CPP /WX /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"D:\qianlong\service\hqsystem\rtserver.dll" /pdbtype:sept /libpath:"..\..\..\..\PublicLibrary\LibOutput" /def:../../rtserver/RTServer.def
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "reserver - Win32 Release"
# Name "reserver - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "OptDirSrc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\common\chunk.c
# End Source File
# Begin Source File

SOURCE=..\..\rtserver\OptDirSrc\OptDealDirProtocol.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtserver\OptDirSrc\OptDirMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtserver\OptDirSrc\OptFileSystem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\rbtree.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\rtserver\Control.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtserver\dbf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtserver\file.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtserver\FileCache.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtserver\filedata.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtserver\filelist.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtserver\Global.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtserver\Log.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtserver\Option.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtserver\Process.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtserver\Request.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtserver\RTServer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rtserver\Status.cpp
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

SOURCE=..\..\common\OptBaseDef.h
# End Source File
# Begin Source File

SOURCE=..\..\rtserver\OptDirInc\OptDealDirProtocol.h
# End Source File
# Begin Source File

SOURCE=..\..\rtserver\OptDirInc\OptDirMgr.h
# End Source File
# Begin Source File

SOURCE=..\..\common\OptDirTranNetData.h
# End Source File
# Begin Source File

SOURCE=..\..\rtserver\OptDirInc\OptFileSystem.h
# End Source File
# Begin Source File

SOURCE=..\..\common\OptStructDef.h
# End Source File
# Begin Source File

SOURCE=..\..\common\rbtree.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\rtserver\Control.h
# End Source File
# Begin Source File

SOURCE=..\..\rtserver\dbf.h
# End Source File
# Begin Source File

SOURCE=..\..\rtserver\file.h
# End Source File
# Begin Source File

SOURCE=..\..\rtserver\FileCache.h
# End Source File
# Begin Source File

SOURCE=..\..\rtserver\filedata.h
# End Source File
# Begin Source File

SOURCE=..\..\rtserver\filelist.h
# End Source File
# Begin Source File

SOURCE=..\..\rtserver\Global.h
# End Source File
# Begin Source File

SOURCE=..\..\rtserver\include.h
# End Source File
# Begin Source File

SOURCE=..\..\rtserver\Log.h
# End Source File
# Begin Source File

SOURCE=..\..\rtserver\Option.h
# End Source File
# Begin Source File

SOURCE=..\..\rtserver\Process.h
# End Source File
# Begin Source File

SOURCE=..\..\rtserver\Request.h
# End Source File
# Begin Source File

SOURCE=..\..\rtserver\Status.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Doc Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\Doc\RT服务器版本说明书.xls"
# End Source File
# End Group
# End Target
# End Project
