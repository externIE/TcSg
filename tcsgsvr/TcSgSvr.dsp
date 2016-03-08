# Microsoft Developer Studio Project File - Name="TcSgSvr" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=TcSgSvr - Win32 DebugS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TcSgSvr.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TcSgSvr.mak" CFG="TcSgSvr - Win32 DebugS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TcSgSvr - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "TcSgSvr - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "TcSgSvr - Win32 DebugS" (based on "Win32 (x86) Console Application")
!MESSAGE "TcSgSvr - Win32 ReleaseS" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Library/XYGAME1.0/xygame", PPEBAAAA"
# PROP Scc_LocalPath "..\xygame"
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TcSgSvr - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /Zi /Od /D "UWL_TRACE" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 tcgament2d.lib xygame2d.lib xygament2d.lib tcgskd2.lib tcgsknt2d.lib uwld.lib uwlntd.lib uwltrace.lib tcnsd.lib xyapid.lib jsoncppd.lib TcyCommond.lib /nologo /subsystem:console /map /debug /machine:I386 /out:"Debug/TcSgSvrcd.exe" /pdbtype:sept
# SUBTRACT LINK32 /incremental:no

!ELSEIF  "$(CFG)" == "TcSgSvr - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 uwl.lib uwlnt.lib /nologo /subsystem:console /machine:I386 /out:"Release/TcSgSvrc.exe"

!ELSEIF  "$(CFG)" == "TcSgSvr - Win32 DebugS"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugS"
# PROP BASE Intermediate_Dir "DebugS"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugS"
# PROP Intermediate_Dir "DebugS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "UWL_TRACE" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "UWL_TRACE" /D "UWL_SERVICE" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 uwld.lib uwlntd.lib uwltrace.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 tcgament2d.lib xygame2d.lib xygament2d.lib tcgskd2.lib tcgsknt2d.lib uwld.lib uwlntd.lib uwltrace.lib tcnsd.lib xyapid.lib /nologo /subsystem:console /debug /machine:I386 /out:"DebugS/TcSgSvrd.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "TcSgSvr - Win32 ReleaseS"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseS"
# PROP BASE Intermediate_Dir "ReleaseS"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseS"
# PROP Intermediate_Dir "ReleaseS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /D "UWL_SERVICE" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 uwl.lib uwlnt.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 tcgament2.lib xygame2.lib xygament2.lib tcgsk2.lib tcgsknt2.lib uwl.lib uwlnt.lib uwltrace.lib tcns.lib xyapi.lib jsoncpp.lib TcyCommon.lib /nologo /subsystem:console /map /debug /machine:I386 /pdbtype:sept
# Begin Special Build Tool
OutDir=.\ReleaseS
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(OutDir)\*.exe .\..\..\bin
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "TcSgSvr - Win32 Debug"
# Name "TcSgSvr - Win32 Release"
# Name "TcSgSvr - Win32 DebugS"
# Name "TcSgSvr - Win32 ReleaseS"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DataRecord.cpp
# End Source File
# Begin Source File

SOURCE=.\Markup.cpp
# End Source File
# Begin Source File

SOURCE=.\MySockClit.cpp
# End Source File
# Begin Source File

SOURCE=.\Server.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TcSgSvr.cpp
# End Source File
# Begin Source File

SOURCE=.\TcSgSvr.rc
# End Source File
# Begin Source File

SOURCE=..\common\TcSg\TcSgTbl.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\COMMON\TcSg\CommonDef.h
# End Source File
# Begin Source File

SOURCE=..\COMMON\TcSg\CommonReq.h
# End Source File
# Begin Source File

SOURCE=.\DataRecord.h
# End Source File
# Begin Source File

SOURCE=.\Markup.h
# End Source File
# Begin Source File

SOURCE=.\MySockClit.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Server.h
# End Source File
# Begin Source File

SOURCE=.\Service.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\Common\TcSg\TcSgDef.h
# End Source File
# Begin Source File

SOURCE=..\Common\TcSg\TcSgReq.h
# End Source File
# Begin Source File

SOURCE=.\TcSgSvr.h
# End Source File
# Begin Source File

SOURCE=..\Common\TcSg\TcSgTbl.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "SOAP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SOAP\ItemAwardServiceSoap.cpp
# End Source File
# Begin Source File

SOURCE=.\SOAP\soap12.h
# End Source File
# Begin Source File

SOURCE=.\SOAP\soapC.cpp
# End Source File
# Begin Source File

SOURCE=.\SOAP\soapClient.cpp
# End Source File
# Begin Source File

SOURCE=.\SOAP\soapH.h
# End Source File
# Begin Source File

SOURCE=.\SOAP\soapStub.h
# End Source File
# Begin Source File

SOURCE=.\SOAP\stdsoap2.cpp
# End Source File
# Begin Source File

SOURCE=.\SOAP\stdsoap2.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
