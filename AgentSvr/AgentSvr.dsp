# Microsoft Developer Studio Project File - Name="AgentSvr" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AgentSvr - Win32 Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AgentSvr.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AgentSvr.mak" CFG="AgentSvr - Win32 Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AgentSvr - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "AgentSvr - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AgentSvr - Win32 Unicode" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/AgentSvr", FVBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AgentSvr - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /Gr /MTd /W3 /Gm /Gi /ZI /Od /I "..\MessageMaker" /I "..\AgentMessages" /I "..\AgentSvr" /I "..\Underlight" /I "..\DXSDK\SDK\inc" /I "..\RWSDK\inc" /I "..\AgentSvc" /D "_DEBUG" /D "UL_DEBUG" /D "USE_INLINE" /D "AGENT" /D "GAMEMASTER" /D "WIN32" /D "_WINDOWS" /D "GAME_LYR" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /Oicf /o "NUL" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ../MessageMaker/Debug/MessageMaker.lib oldnames.lib libcmtd.lib winmm.lib ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib /out:"\Lyra\Dev\AgentSvrD.exe" /MAPINFO:LINES /verbose:lib
# SUBTRACT LINK32 /profile /incremental:no /map

!ELSEIF  "$(CFG)" == "AgentSvr - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AgentSvr___Win32_Release"
# PROP BASE Intermediate_Dir "AgentSvr___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /Gr /MTd /W3 /Gm /Gi /ZI /Od /D "_DEBUG" /D "USE_INLINE" /D "WIN32" /D "_WINDOWS" /D "AGENT" /FR /YX /FD /c
# ADD CPP /nologo /G5 /Gr /MT /W3 /Gi /O1 /I "..\MPSDK\inc" /I "../Underlight" /I "../MessageMaker" /I "../AgentMessages" /I "..\MessageMaker" /I "..\AgentMessages" /I "..\AgentSvr" /I "..\Underlight" /I "..\DXSDK\SDK\inc" /I "..\RWSDK\inc" /I "..\AgentSvc" /D "USE_INLINE" /D "AGENT" /D "GAMEMASTER" /D "WIN32" /D "_WINDOWS" /D "GAME_LYR" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /Oicf /o "NUL" /win32
# SUBTRACT BASE MTL /mktyplib203
# ADD MTL /nologo /Oicf /o "NUL" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 winmm.lib ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libcmt.lib ../MessageMaker/Release/MessageMaker.lib oldnames.lib libcmt.lib winmm.lib ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no /map /machine:I386 /nodefaultlib /out:"\lyra\dev\AgentSvr.exe" /pdbtype:con /MAPINFO:LINES
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "AgentSvr - Win32 Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AgentSvr___Win32_Unicode"
# PROP BASE Intermediate_Dir "AgentSvr___Win32_Unicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Unicode"
# PROP Intermediate_Dir "Unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /Gr /MTd /W3 /Gm /Gi /ZI /Od /I "..\MessageMaker" /I "..\AgentMessages" /I "..\AgentSvr" /I "..\Underlight" /I "..\DXSDK\SDK\inc" /I "..\RWSDK\inc" /I "..\AgentSvc" /D "_DEBUG" /D "USE_INLINE" /D "WIN32" /D "_WINDOWS" /D "AGENT" /D "GAMEMASTER" /D "UL_DEBUG" /FR /YX /FD /c
# ADD CPP /nologo /G5 /Gr /MTd /W3 /Gm /Gi /ZI /Od /I "..\MessageMaker" /I "..\AgentMessages" /I "..\AgentSvr" /I "..\Underlight" /I "..\DXSDK\SDK\inc" /I "..\RWSDK\inc" /I "..\AgentSvc" /D "UL_DEBUG" /D "_DEBUG" /D "_UNICODE" /D "UNICODE" /D "USE_INLINE" /D "AGENT" /D "GAMEMASTER" /D "WIN32" /D "_WINDOWS" /D "GAME_LYR" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /Oicf /o "NUL" /win32
# SUBTRACT BASE MTL /mktyplib203
# ADD MTL /nologo /D "_DEBUG" /Oicf /o "NUL" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ../MessageMaker/Debug/MessageMaker.lib oldnames.lib libcmtd.lib winmm.lib ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no /map /debug /machine:I386 /nodefaultlib /out:"\Lyra\Dev\AgentSvrD.exe" /pdbtype:con /MAPINFO:LINES /verbose:lib
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 ../MessageMaker/Unicode/MessageMaker.lib oldnames.lib libcmtd.lib winmm.lib ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no /map /debug /machine:I386 /nodefaultlib /out:"\Lyra\Dev\AgentSvrD.exe" /pdbtype:con /MAPINFO:LINES /verbose:lib
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "AgentSvr - Win32 Debug"
# Name "AgentSvr - Win32 Release"
# Name "AgentSvr - Win32 Unicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Agents.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\BuildView.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cActor.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cActorList.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cAgentBox.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cAgentDaemon.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cAgentServer.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cAI.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cArts.cpp
# End Source File
# Begin Source File

SOURCE=.\cBackgroundMusic.cpp
# End Source File
# Begin Source File

SOURCE=.\cBanner.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cBox.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cChat.cpp
# End Source File
# Begin Source File

SOURCE=.\cControlPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\cDDraw.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cDetailGoal.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cDetailQuest.cpp
# End Source File
# Begin Source File

SOURCE=.\cDSound.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cEffects.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cGameServer.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cGif.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cGoalBook.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cGoalPosting.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cGoalQuestProcs.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cItem.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cKeymap.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cLevel.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cList.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cMissile.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cNameTag.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cNeighbor.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cOrnament.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cOutput.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cPalettes.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cParty.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cPlayer.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cPostGoal.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cPostQuest.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cQuestBuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cReadGoal.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cReadQuest.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cReadReport.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\CreateFrame.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cReportGoal.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cReviewGoals.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\cSending.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\Debug.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\Dialogs.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\Interface.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\Keyboard.cpp
# End Source File
# Begin Source File

SOURCE=..\underlight\LanguageFilter.cpp
# End Source File
# Begin Source File

SOURCE=..\underlight\LevelRoomNames.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\LoginOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\MAIN.CPP
# End Source File
# Begin Source File

SOURCE=..\Underlight\Mouse.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\Move.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\Options.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\Realm.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\RenderActor.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\RenderView.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\RLE.cpp
# End Source File
# Begin Source File

SOURCE=..\Underlight\Underlight.rc
# End Source File
# Begin Source File

SOURCE=..\Underlight\Utils.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Underlight\4dx.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\actor.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\Agent.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cActor.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cActorList.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cAgentBox.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cAgentDaemon.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cAgentServer.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cAI.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cArts.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cBackgroundMusic.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cBanner.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cChat.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cChat_singleline.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cClient.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cControlPanel.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cDDraw.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cDetailGoal.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cDSound.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cEffects.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\Central.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cGameServer.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cGif.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cGoalBook.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cGoalPosting.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cItem.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cKeymap.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cLevel.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cList.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cMissile.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cNameTag.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cNeighbor.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cOrnament.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cOutput.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cPalettes.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cParty.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cPlayer.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cPostGoal.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cReadGoal.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cReadReport.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cReportGoal.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cReviewGoals.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cSending.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\cWave.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\Dialogs.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\Effects.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\GsChecksums.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\Interface.h
# End Source File
# Begin Source File

SOURCE=.\IsAgent.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\Keyboard.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\LoginOptions.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\Main.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\Mouse.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\Move.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\Options.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\Realm.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\RenderActor.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\RenderView.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\resource.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\RLE.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\Secure.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\SharedConstants.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\txtrload.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\Utils.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\v5_structs.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\wbox.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\zconf.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\zlib.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\Underlight\Lyra.ico
# End Source File
# Begin Source File

SOURCE=..\Underlight\pmare.ico
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\Underlight\Target.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=..\Underlight\AgentSvr.rgs
# End Source File
# Begin Source File

SOURCE=..\Underlight\index.html
# End Source File
# End Target
# End Project
