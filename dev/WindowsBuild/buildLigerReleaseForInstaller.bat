REM Inputs ====================================================================

REM Inputs Passed as arguments by the QtCreator ligerWinDeploy External Tool
REM ========================================================================
SET LIGERVERSION=%LIGER_VERSION%
SET LIGERSRC=%1
SET BUILDDIR=%2\..\Liger_%LIGERVERSION%_Installation_Files
SET QTPATH=%3
SET JOM_PATH=%3\..\..\..\Tools\QtCreator\bin
REM ========================================================================

REM Environment Variables set in the QtCreator ligerWinDeploy External Tool
REM =======================================================================
REM SET BOOST_INCLUDE=C:\Boost\include\boost-1_66
REM SET BOOST_LIB=C:\Boost\lib\vc141
REM SET BOOST_PYTHON_LIB=C:\Boost\lib\vc141

REM SET PYTHON_DLL=C:\Windows\System32\python27.dll
REM SET VCBATCHDIR=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build
REM SET COMPILER_DIR=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.12.25827\bin\Hostx64\x64
REM Inputs For Inno Setup Builder
REM SET INNO_SETUP_DIR=C:\Program Files (x86)\Inno Setup 5
REM SET EXEC_NAME=liger.exe
REM SET APPURL=http://ligeropt.sourceforge.net/
REM SET PUBLISHER=The University of Sheffield
REM SET APPNAME=Liger
REM =======================================================================

REM Inputs ====================================================================

REM ================Do NOT Change====================================
SET PATH=%PATH%;%COMPILER_DIR%
SET PATH=%PATH%;%QTPATH%
SET PATH=%PATH%;%JOM_PATH%

echo ------ Start --------
echo ---------------------

echo %BUILDDIR%

RD /S /Q %BUILDDIR%
MKDIR    %BUILDDIR%
CD /D %BUILDDIR%
 
qmake.exe "CONFIG+=NO_TESTS" "%LIGERSRC%\liger.pro" -r -spec win32-msvc
CALL "%VCBATCHDIR%\vcvarsall.bat" amd64
CD /D %BUILDDIR%
jom.exe -j12

RD  /S /Q "%BUILDDIR%\src"
RD  /S /Q "%BUILDDIR%\tests"
RD  /Q "%BUILDDIR%\share\liger\debug"
RD  /Q "%BUILDDIR%\share\liger\release"
DEL /Q "%BUILDDIR%\Makefile"
DEL /Q "%BUILDDIR%\share\Makefile"
DEL /Q "%BUILDDIR%\share\liger\Makefile.static"
DEL /Q "%BUILDDIR%\share\liger\Makefile.static.Release"
DEL /Q "%BUILDDIR%\share\liger\Makefile.static.Debug"

windeployqt.exe --compiler-runtime --release --no-translations ^
-xml -concurrent -webenginecore -webengine -webenginewidgets -qthelp ^
-sql -printsupport "%BUILDDIR%\bin\liger.exe"

xcopy "%QTPATH%\Qt5QuickWidgets.dll" "%BUILDDIR%\bin\"
xcopy "%PYTHON_DLL%" "%BUILDDIR%\bin\"

"%INNO_SETUP_DIR%\iscc" "%LIGERSRC%\dev\WindowsBuild\LigerSetupBuilder.iss" ^
/DProjectDir="%LIGERSRC%" ^
/DBuildDir="%BUILDDIR%" ^
/DMyAppExeName="%EXEC_NAME%" ^
/DMyAppURL="%APPURL%" ^
/DMyAppPublisher="%PUBLISHER%" ^
/DMyAppVersion="%LIGERVERSION%" ^
/DMyAppName="%APPNAME%"

ECHO ----- Finished ------
ECHO ---------------------