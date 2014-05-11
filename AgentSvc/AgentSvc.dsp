# Microsoft Developer Studio Project File - Name="AgentSvc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AgentSvc - Win32 Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AgentSvc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AgentSvc.mak" CFG="AgentSvc - Win32 Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AgentSvc - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "AgentSvc - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AgentSvc - Win32 Unicode" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "$/"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AgentSvc - Win32 Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\Lyra\Dev"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Gr /MTd /W3 /Gm /GX /ZI /Od /I "..\MessageMaker" /I "..\AgentMessages" /I "..\AgentSvr" /I "..\Underlight" /I "..\DXSDK\SDK\inc" /I "..\RWSDK\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /out:"/lyra/dev/AgentSvcd.exe"
# Begin Custom Build - Performing registration
OutDir=\Lyra\Dev
TargetPath=\Lyra\Dev\AgentSvc.exe
InputPath=\Lyra\Dev\AgentSvc.exe
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(TargetPath)" /RegServer 
	echo Server registration done! 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AgentSvc - Win32 Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AgentSvc___Win32_Release"
# PROP BASE Intermediate_Dir "AgentSvc___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "\Lyra\Dev"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Gr /MT /W3 /GX /O1 /I "..\MessageMaker" /I "..\AgentMessages" /I "..\AgentSvr" /I "..\Underlight" /I "..\DXSDK\SDK\inc" /I "..\RWSDK\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /out:"/lyra/dev/AgentSvc.exe"
# Begin Custom Build - Performing registration
OutDir=\Lyra\Dev
TargetPath=\Lyra\Dev\AgentSvc.exe
InputPath=\Lyra\Dev\AgentSvc.exe
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(TargetPath)" /RegServer 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	echo Server registration done! 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AgentSvc - Win32 Unicode"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AgentSvc___Win32_Unicode"
# PROP BASE Intermediate_Dir "AgentSvc___Win32_Unicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Unicode"
# PROP Intermediate_Dir "Unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gr /MTd /W3 /Gm /GX /ZI /Od /I "..\MessageMaker" /I "..\AgentMessages" /I "..\AgentSvr" /I "..\Underlight" /I "..\DXSDK\SDK\inc" /I "..\RWSDK\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Gr /MTd /W3 /Gm /GX /ZI /Od /I "..\MessageMaker" /I "..\AgentMessages" /I "..\AgentSvr" /I "..\Underlight" /I "..\DXSDK\SDK\inc" /I "..\RWSDK\inc" /D "_MBCS" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /out:"/lyra/dev/AgentSvcd.exe"
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /out:"/lyra/dev/AgentSvcd.exe"
# Begin Custom Build - Performing registration
OutDir=.\Unicode
TargetPath=.\Unicode\AgentSvc.exe
InputPath=.\Unicode\AgentSvc.exe
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(TargetPath)" /RegServer 
	echo Server registration done! 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "AgentSvc - Win32 Debug"
# Name "AgentSvc - Win32 Release"
# Name "AgentSvc - Win32 Unicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AgentSvc.cpp
# End Source File
# Begin Source File

SOURCE=.\AgentSvc.idl
# ADD MTL /tlb ".\AgentSvc.tlb" /h "AgentSvc.h" /iid "AgentSvc_i.c" /Oicf
# End Source File
# Begin Source File

SOURCE=.\AgentSvc.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Constants.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\AgentSvc.rgs
# End Source File
# End Group
# End Target
# End Project
