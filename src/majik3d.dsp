# Microsoft Developer Studio Project File - Name="Majik3D" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Majik3D - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Majik3D.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Majik3D.mak" CFG="Majik3D - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Majik3D - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Majik3D - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Majik3D - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "__WIN32__" /YX /FD /c
# ADD BASE RSC /l 0x40b /d "NDEBUG"
# ADD RSC /l 0x40b /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 ws2_32.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib glut32.lib glu32.lib pui.lib ssg.lib fnt.lib sg.lib /nologo /subsystem:console /incremental:yes /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "Majik3D - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "__WIN32__" /YX /FD /GZ /c
# ADD BASE RSC /l 0x40b /d "_DEBUG"
# ADD RSC /l 0x40b /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib glut32.lib opengl32.lib glu32.lib pui.lib ssg.lib fnt.lib sg.lib /nologo /subsystem:console /map /debug /machine:I386 /out:"Majik3D.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Majik3D - Win32 Release"
# Name "Majik3D - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\mcConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\mcDebug.cpp
# End Source File
# Begin Source File

SOURCE=.\mcDisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\mcError.cpp
# End Source File
# Begin Source File

SOURCE=.\mcInput.cpp
# End Source File
# Begin Source File

SOURCE=.\mcLandscape.cpp
# End Source File
# Begin Source File

SOURCE=.\mcMapquad.cpp
# End Source File
# Begin Source File

SOURCE=.\mcMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\mcObject.cpp
# End Source File
# Begin Source File

SOURCE=.\mcOverlay.cpp
# End Source File
# Begin Source File

SOURCE=.\mcPerlin.cpp
# End Source File
# Begin Source File

SOURCE=.\mcProtocol.cpp
# End Source File
# Begin Source File

SOURCE=.\mcScene.cpp
# End Source File
# Begin Source File

SOURCE=.\mcSky.cpp
# End Source File
# Begin Source File

SOURCE=.\mcSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\mcTerrainblock.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\mcConfig.hpp
# End Source File
# Begin Source File

SOURCE=.\mcDebug.hpp
# End Source File
# Begin Source File

SOURCE=.\mcDetail.hpp
# End Source File
# Begin Source File

SOURCE=.\mcDisplay.hpp
# End Source File
# Begin Source File

SOURCE=.\mcError.hpp
# End Source File
# Begin Source File

SOURCE=.\mcInput.hpp
# End Source File
# Begin Source File

SOURCE=.\mcLandscape.hpp
# End Source File
# Begin Source File

SOURCE=.\mcMapquad.hpp
# End Source File
# Begin Source File

SOURCE=.\mcMaterial.hpp
# End Source File
# Begin Source File

SOURCE=.\mcMenu.hpp
# End Source File
# Begin Source File

SOURCE=.\mcObject.hpp
# End Source File
# Begin Source File

SOURCE=.\mcOverlay.hpp
# End Source File
# Begin Source File

SOURCE=.\mcPerlin.hpp
# End Source File
# Begin Source File

SOURCE=.\mcProtocol.hpp
# End Source File
# Begin Source File

SOURCE=.\mcScene.hpp
# End Source File
# Begin Source File

SOURCE=.\mcSky.hpp
# End Source File
# Begin Source File

SOURCE=.\mcSocket.hpp
# End Source File
# Begin Source File

SOURCE=.\mcTerrainblock.hpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
