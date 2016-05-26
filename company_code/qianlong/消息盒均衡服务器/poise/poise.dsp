# Microsoft Developer Studio Project File - Name="poise" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=poise - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "poise.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "poise.mak" CFG="poise - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "poise - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "poise - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "poise"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "poise - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SAMPLE_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zd /O2 /I "../../include/lib" /I "../../include" /I "../include" /I "./include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SAMPLE_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /debug /machine:I386

!ELSEIF  "$(CFG)" == "poise - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SAMPLE_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../include/lib" /I "../../include" /I "../include" /I "./include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SAMPLE_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "poise - Win32 Release"
# Name "poise - Win32 Debug"
# Begin Group "spif"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\if.h
# End Source File
# End Group
# Begin Group "include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\include\cleanup.h
# End Source File
# Begin Source File

SOURCE=.\include\cmd.h
# End Source File
# Begin Source File

SOURCE=.\include\config.h
# End Source File
# Begin Source File

SOURCE=.\include\link.h
# End Source File
# Begin Source File

SOURCE=.\include\poise.h
# End Source File
# Begin Source File

SOURCE=.\include\protocol.h
# End Source File
# Begin Source File

SOURCE=.\include\request.h
# End Source File
# Begin Source File

SOURCE=.\include\structure.h
# End Source File
# Begin Source File

SOURCE=.\include\thread.h
# End Source File
# Begin Source File

SOURCE=.\include\version.h
# End Source File
# End Group
# Begin Group "src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\cleanup.c
# End Source File
# Begin Source File

SOURCE=.\src\cmd.c
# End Source File
# Begin Source File

SOURCE=.\src\config.c
# End Source File
# Begin Source File

SOURCE=.\src\consult.c
# End Source File
# Begin Source File

SOURCE=.\src\global.c
# End Source File
# Begin Source File

SOURCE=.\src\link.c
# End Source File
# Begin Source File

SOURCE=.\src\poise.c
# End Source File
# Begin Source File

SOURCE=.\src\print.c
# End Source File
# Begin Source File

SOURCE=.\src\protocol.c
# End Source File
# Begin Source File

SOURCE=.\src\request.c
# End Source File
# Begin Source File

SOURCE=.\src\startup.c
# End Source File
# Begin Source File

SOURCE=.\src\thread.c
# End Source File
# Begin Source File

SOURCE=.\src\version.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\poise.def
# End Source File
# End Target
# End Project
