# Microsoft Developer Studio Project File - Name="wsclient" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=wsclient - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wsclient.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wsclient.mak" CFG="wsclient - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wsclient - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "wsclient - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wsclient - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP   -DDEBUG_NOT_LOG -nologo -EHsc -MT -O2 -GF -FD   -W3 -DWINDOWS -DMSVC /c
# ADD CPP   -DDEBUG_NOT_LOG -nologo -EHsc -MT -O2 -GF -FD   -W3 -DWINDOWS -DMSVC   -I..\..\src_base\function\. -I..\..\src_base\mongoose\. -I..\..\libs\openssl\1.1.0h\include\. /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib   /libpath:..\..\libs\openssl\1.1.0h\static\windows_x64 crypto.lib ssl.lib Crypt32.lib   -nologo /FIXED:NO     /WARN:2
# ADD LINK32   -nologo /FIXED:NO     /WARN:2 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib   /libpath:..\..\libs\openssl\1.1.0h\static\windows_x64 crypto.lib ssl.lib Crypt32.lib /out:".\.\wsclient.exe"

!ELSEIF  "$(CFG)" == "wsclient - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP   -DDEBUG_NOT_LOG -nologo -EHsc -MTd -Od -Gm -Zi -FD -Zm200   -W3 -DDEBUG -DDEBUG_LOG -DWINDOWS -DMSVC  /c
# ADD CPP   -DDEBUG_NOT_LOG -nologo -EHsc -MTd -Od -Gm -Zi -FD -Zm200   -W3 -DDEBUG -DDEBUG_LOG -DWINDOWS -DMSVC   -I..\..\src_base\function\. -I..\..\src_base\mongoose\. -I..\..\libs\openssl\1.1.0h\include\. /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib   /libpath:..\..\libs\openssl\1.1.0h\static\windows_x64 crypto.lib ssl.lib Crypt32.lib   -nologo /FIXED:NO     /DEBUG /WARN:2
# ADD LINK32   -nologo /FIXED:NO     /DEBUG /WARN:2 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib   /libpath:..\..\libs\openssl\1.1.0h\static\windows_x64 crypto.lib ssl.lib Crypt32.lib /out:".\.\wsclient.exe"

!ENDIF 

# Begin Target

# Name "wsclient - Win32 Release"
# Name "wsclient - Win32 Debug"


# Begin Group "function"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src_base\function\BaseFunction.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\Config.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\platform.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\printinfo.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\profile.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\utility.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\utils.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\version.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\BaseFunction.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\Config.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\ExpEvaluate.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\platform.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\printinfo.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\profile.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\public.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\utility.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\utils.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\version.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\versionValue.h
# End Source File
# End Group

# Begin Group "mongoose"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src_base\mongoose\mongoose.c
# End Source File
# Begin Source File
SOURCE=..\..\src_base\mongoose\mongoose.h
# End Source File
# End Group

# Begin Group "openssl_include"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\libs\openssl\1.1.0h\include\ApiKeyUtil.cpp
# End Source File
# Begin Source File
SOURCE=..\..\libs\openssl\1.1.0h\include\ApiKeyUtil.h
# End Source File
# End Group

# Begin Group "wsclient"
# PROP Default_Filter ""
# Begin Source File
SOURCE=.\websocketClient.cpp
# End Source File
# End Group

# End Target
# End Project


