# Microsoft Developer Studio Project File - Name="gcserver" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=gcserver - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gcserver.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gcserver.mak" CFG="gcserver - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gcserver - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "gcserver - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "gcserver"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gcserver - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /Zd /O2 /I "./lib" /I "./lib/win32" /I "./include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  ws2_32.lib /nologo /subsystem:console /incremental:yes /pdb:"D:\Pro\qianlong\service\improved-gc\gcserver.pdb" /map:"D:\Pro\qianlong\service\improved-gc\gcserver.map" /debug /machine:I386 /out:"D:\Pro\qianlong\service\improved-gc\gcserver.exe"

!ELSEIF  "$(CFG)" == "gcserver - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "./lib" /I "./lib/win32" /I "./include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  ws2_32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "gcserver - Win32 Release"
# Name "gcserver - Win32 Debug"
# Begin Group "lib"

# PROP Default_Filter ""
# Begin Group "win32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lib\win32\event.h
# End Source File
# Begin Source File

SOURCE=.\lib\win32\posix.c
# End Source File
# Begin Source File

SOURCE=.\lib\win32\pthread.h
# End Source File
# Begin Source File

SOURCE=.\lib\win32\rwlock.h
# End Source File
# End Group
# Begin Group "asm-x86"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\lib\asm-x86\atomic.h"
# End Source File
# End Group
# Begin Group "crypto"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lib\crypto\crc16.c
# End Source File
# Begin Source File

SOURCE=.\lib\crypto\crc16.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\lib\features.h
# End Source File
# Begin Source File

SOURCE=.\lib\list.h
# End Source File
# Begin Source File

SOURCE=.\lib\native.h
# End Source File
# Begin Source File

SOURCE=.\lib\rbtree.c
# End Source File
# Begin Source File

SOURCE=.\lib\rbtree.h
# End Source File
# Begin Source File

SOURCE=.\lib\typedef.h
# End Source File
# End Group
# Begin Group "base"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\base\gcs_http.c
# End Source File
# Begin Source File

SOURCE=.\base\gcs_page.c
# End Source File
# Begin Source File

SOURCE=.\base\gcs_parser.c
# End Source File
# Begin Source File

SOURCE=.\base\gcs_sock.c
# End Source File
# Begin Source File

SOURCE=.\base\gcs_wt.c
# End Source File
# End Group
# Begin Group "core"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core\gcs_conn.c
# End Source File
# Begin Source File

SOURCE=.\core\gcs_iocp.c
# End Source File
# Begin Source File

SOURCE=.\core\gcs_link.c
# End Source File
# Begin Source File

SOURCE=.\core\gcs_lsnr.c
# End Source File
# End Group
# Begin Group "init"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\init\gcs_config.c
# End Source File
# Begin Source File

SOURCE=.\init\gcs_ctrl.c
# End Source File
# Begin Source File

SOURCE=.\init\gcs_main.c
# End Source File
# Begin Source File

SOURCE=.\init\gcs_mon.c
# End Source File
# Begin Source File

SOURCE=.\init\gcs_smif.c
# End Source File
# Begin Source File

SOURCE=.\init\gcs_version.c
# End Source File
# End Group
# Begin Group "include"

# PROP Default_Filter ""
# Begin Group "bits"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\include\bits\gcs_cnt64.h
# End Source File
# Begin Source File

SOURCE=.\include\bits\gcs_config.h
# End Source File
# Begin Source File

SOURCE=.\include\bits\gcs_conn.h
# End Source File
# Begin Source File

SOURCE=.\include\bits\gcs_http.h
# End Source File
# Begin Source File

SOURCE=.\include\bits\gcs_iocp.h
# End Source File
# Begin Source File

SOURCE=.\include\bits\gcs_link.h
# End Source File
# Begin Source File

SOURCE=.\include\bits\gcs_lsnr.h
# End Source File
# Begin Source File

SOURCE=.\include\bits\gcs_main.h
# End Source File
# Begin Source File

SOURCE=.\include\bits\gcs_mon.h
# End Source File
# Begin Source File

SOURCE=.\include\bits\gcs_page.h
# End Source File
# Begin Source File

SOURCE=.\include\bits\gcs_parser.h
# End Source File
# Begin Source File

SOURCE=.\include\bits\gcs_smif.h
# End Source File
# Begin Source File

SOURCE=.\include\bits\gcs_wt.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\include\gcs_config.h
# End Source File
# Begin Source File

SOURCE=.\include\gcs_conn.h
# End Source File
# Begin Source File

SOURCE=.\include\gcs_ctrl.h
# End Source File
# Begin Source File

SOURCE=.\include\gcs_errno.h
# End Source File
# Begin Source File

SOURCE=.\include\gcs_http.h
# End Source File
# Begin Source File

SOURCE=.\include\gcs_iocp.h
# End Source File
# Begin Source File

SOURCE=.\include\gcs_link.h
# End Source File
# Begin Source File

SOURCE=.\include\gcs_lsnr.h
# End Source File
# Begin Source File

SOURCE=.\include\gcs_main.h
# End Source File
# Begin Source File

SOURCE=.\include\gcs_mon.h
# End Source File
# Begin Source File

SOURCE=.\include\gcs_page.h
# End Source File
# Begin Source File

SOURCE=.\include\gcs_parser.h
# End Source File
# Begin Source File

SOURCE=.\include\gcs_smif.h
# End Source File
# Begin Source File

SOURCE=.\include\gcs_sock.h
# End Source File
# Begin Source File

SOURCE=.\include\gcs_version.h
# End Source File
# Begin Source File

SOURCE=.\include\gcs_wt.h
# End Source File
# End Group
# End Target
# End Project
