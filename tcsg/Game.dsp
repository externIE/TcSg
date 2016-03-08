# Microsoft Developer Studio Project File - Name="Game" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Game - Win32 DebugOffline
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Game.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Game.mak" CFG="Game - Win32 DebugOffline"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Game - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Game - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Game - Win32 DebugOffline" (based on "Win32 (x86) Application")
!MESSAGE "Game - Win32 ReleaseOffline" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Project/GameChannel/1.00/SRC/YXDP", IHTAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Game - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_OPEN_ADVERT_" /D "_AFXDLL" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 uwl.lib uwltrace.lib xygame2.lib xygui.lib xyFrame2.lib tcSkin.lib xyapi.lib xyJpeg.lib ShinyNova.lib ShinyNovaPublic.lib cximagecrt.lib tcgsk2.lib xySKBase.lib qrcode.lib TcyCommon.lib /nologo /subsystem:windows /machine:I386 /out:"Release/tcsg.exe"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy                   ".\release\tcsg.exe"                "D:\Program Files (x86)\gamechannel\tcsg\tcsg.exe"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Game - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "UWL_TRACE" /Fr /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 uwld.lib uwltrace.lib xygame2d.lib xyguid.lib xyFrame2d.lib xyapid.lib xyJpegd.lib cximagecrt.lib ShinyNovad.lib ShinyNovaPublicd.lib tcSkin.lib tcgskd2.lib xySKBased.lib qrcoded.lib TcyCommond.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Game - Win32 DebugOffline"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Game___Win32_DebugOffline"
# PROP BASE Intermediate_Dir "Game___Win32_DebugOffline"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Game___Win32_DebugOffline"
# PROP Intermediate_Dir "Game___Win32_DebugOffline"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "UWL_TRACE" /D "_OFFLINE_" /Fr /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "UWL_TRACE" /D "_OFFLINE_" /Fr /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 uwld.lib uwltrace.lib xygame3d.lib xyguid.lib xyFrame3d.lib xyapid.lib xyJpegd.lib cximagecrt.lib ShinyNovad.lib tcSkin.lib vldmtdll.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT BASE LINK32 /map /nodefaultlib
# ADD LINK32 uwld.lib uwltrace.lib xygame2d.lib xyguid.lib xyFrame2d.lib tcgskd2.lib xySKBased.lib xyapid.lib xyJpegd.lib cximagecrt.lib ShinyNovad.lib ShinyNovaPublicd.lib qrcoded.lib TcyCommond.lib uwld.lib uwltrace.lib xygame2d.lib xyguid.lib xyFrame2d.lib xyapid.lib xyJpegd.lib cximagecrt.lib ShinyNovad.lib ShinyNovaPublicd.lib tcSkin.lib tcgskd2.lib xySKBased.lib qrcoded.lib TcyCommond.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /profile /incremental:no /map /nodefaultlib

!ELSEIF  "$(CFG)" == "Game - Win32 ReleaseOffline"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Game___Win32_ReleaseOffline"
# PROP BASE Intermediate_Dir "Game___Win32_ReleaseOffline"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseOffline"
# PROP Intermediate_Dir "ReleaseOffline"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_OPEN_ADVERT_" /D "_AFXDLL" /Fr /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_OPEN_ADVERT_" /D "_AFXDLL" /D "_OFFLINE_" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 uwl.lib uwltrace.lib xygame2.lib xygui.lib xyFrame2.lib tcSkin.lib xyapi.lib xyJpeg.lib ShinyNova.lib ShinyNovaPublic.lib cximagecrt.lib tcgsk2.lib xySKBase.lib /nologo /subsystem:windows /machine:I386 /out:"Release/tcsg.exe"
# SUBTRACT BASE LINK32 /incremental:yes /debug
# ADD LINK32 uwl.lib uwltrace.lib xygame2.lib xygui.lib xyFrame2.lib tcSkin.lib xyapi.lib xyJpeg.lib ShinyNova.lib ShinyNovaPublic.lib cximagecrt.lib tcgsk2.lib xySKBase.lib /nologo /subsystem:windows /machine:I386 /out:"ReleaseOffline/tcsg.exe"
# SUBTRACT LINK32 /incremental:yes /debug

!ENDIF 

# Begin Target

# Name "Game - Win32 Release"
# Name "Game - Win32 Debug"
# Name "Game - Win32 DebugOffline"
# Name "Game - Win32 ReleaseOffline"
# Begin Group "Ô´"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AutoSetDepositDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CardMaster.cpp
# End Source File
# Begin Source File

SOURCE=.\Game.cpp
# End Source File
# Begin Source File

SOURCE=.\Game.rc
# End Source File
# Begin Source File

SOURCE=.\GameChargeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GameInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\GameNotifyEx.cpp
# End Source File
# Begin Source File

SOURCE=.\GameRequest.cpp
# End Source File
# Begin Source File

SOURCE=.\GameSet.cpp
# End Source File
# Begin Source File

SOURCE=.\HtmlDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\MyCard.cpp
# End Source File
# Begin Source File

SOURCE=.\MyFileSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\MyFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\MyGame.cpp
# End Source File
# Begin Source File

SOURCE=.\MyPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\MySubSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\QrcodeCharge.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\common\TcSg\TcSgTbl.cpp
# End Source File
# Begin Source File

SOURCE=.\TgControls.cpp
# End Source File
# Begin Source File

SOURCE=.\WelfareDialog.cpp
# End Source File
# End Group
# Begin Group "Í·"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AutoSetDepositDlg.h
# End Source File
# Begin Source File

SOURCE=.\CardMaster.h
# End Source File
# Begin Source File

SOURCE=..\common\tcsg\CommonDef.h
# End Source File
# Begin Source File

SOURCE=..\common\tcsg\CommonReq.h
# End Source File
# Begin Source File

SOURCE=.\define.h
# End Source File
# Begin Source File

SOURCE=.\Game.h
# End Source File
# Begin Source File

SOURCE=.\GameChargeDlg.h
# End Source File
# Begin Source File

SOURCE=.\GameInfo.h
# End Source File
# Begin Source File

SOURCE=.\GameNotifyEx.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Lib2.0\xyHWBase2.0\GamePlayer.h
# End Source File
# Begin Source File

SOURCE=.\GameRequest.h
# End Source File
# Begin Source File

SOURCE=.\GameSet.h
# End Source File
# Begin Source File

SOURCE=.\HtmlDlg.h
# End Source File
# Begin Source File

SOURCE=.\MainFrame.h
# End Source File
# Begin Source File

SOURCE=.\MyCard.h
# End Source File
# Begin Source File

SOURCE=.\MyFileSystem.h
# End Source File
# Begin Source File

SOURCE=.\MyFrame.h
# End Source File
# Begin Source File

SOURCE=.\MyGame.h
# End Source File
# Begin Source File

SOURCE=.\MyPlayer.h
# End Source File
# Begin Source File

SOURCE=.\MySubSystem.h
# End Source File
# Begin Source File

SOURCE=.\QrcodeCharge.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TgControls.h
# End Source File
# Begin Source File

SOURCE=.\WelfareDialog.h
# End Source File
# End Group
# Begin Group "×Ê"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Arrow1.ico
# End Source File
# Begin Source File

SOURCE=.\res\Arrow2.ico
# End Source File
# Begin Source File

SOURCE=.\res\common\digit.bmp
# End Source File
# Begin Source File

SOURCE=.\res\common\face.ico
# End Source File
# Begin Source File

SOURCE=.\res\common\flower.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Game.ico
# End Source File
# Begin Source File

SOURCE=.\res\Game.rc2
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\common\nothing.ico
# End Source File
# Begin Source File

SOURCE=.\res\common\player.bmp
# End Source File
# Begin Source File

SOURCE=.\res\common\player.ico
# End Source File
# Begin Source File

SOURCE=.\res\selected.bmp
# End Source File
# Begin Source File

SOURCE=.\res\common\silver.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
