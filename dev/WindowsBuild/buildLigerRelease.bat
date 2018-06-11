ECHO off

SET QTPATH=%1
SET JOM_BIN=%2
SET BUIDDIR=%3
SET LIGERSRC=%4

SET VCINSTALLDIR=C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC

REM add path to qt and jom
SET PATH=%PATH%;%QTPATH%
SET PATH=%PATH%;%JOM_BIN%

RD /S /Q %BUIDDIR%
MKDIR    %BUIDDIR%
CD %BUIDDIR%

qmake.exe "%LIGERSRC%\liger.pro" -r -spec win32-msvc2013
CALL "%VCINSTALLDIR%\vcvarsall.bat" amd64
jom.exe -j 20 

REM Clean up
RD /S /Q "%BUIDDIR%\src"
DEL /Q "%BUIDDIR%\Makefile"
DEL /Q "%BUIDDIR%\share\Makefile"
DEL /Q "%BUIDDIR%\share\liger\Makefile*"
RD /S /Q "%BUIDDIR%\share\liger\debug"
RD /S /Q "%BUIDDIR%\share\liger\release"
DEL /Q "%BUIDDIR%\lib\liger\*.dll"
DEL /Q "%BUIDDIR%\lib\liger\*.exp"
DEL /Q "%BUIDDIR%\lib\liger\*.lib"
DEL /Q "%BUIDDIR%\lib\liger\plugins\*.exp"
DEL /Q "%BUIDDIR%\lib\liger\plugins\*.lib"

REM copy Qt dependencies(dlls)
windeployqt.exe --no-translations -concurrent -clucene -core -enginio -gui -qthelp -multimedia -multimediawidgets -multimediaquick -network -opengl -positioning -printsupport -qml  -quick -script -sensors -sql -svg -test -webkit -webkitwidgets -widgets -xml -webchannel "%BUIDDIR%\bin\liger.exe"
REM extra dlls
XCOPY /Y /Q "%QTPATH%\icudt54.dll" "%BUIDDIR%\bin\"
XCOPY /Y /Q "%QTPATH%\icuin54.dll" "%BUIDDIR%\bin\" 
XCOPY /Y /Q "%QTPATH%\icuuc54.dll" "%BUIDDIR%\bin\"

