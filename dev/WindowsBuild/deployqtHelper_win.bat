@ECHO OFF
SET numArgument=0;
FOR %%x IN (%*) DO SET /A numArgument+=1

if %numArgument% NEQ 2 (
  ECHO "Usage: deployqtHelper_win <app_bin_folder> <qt_bin>"
) else (
  SET APP_BIN=%1
  SET QT_BIN=%2

  ECHO App bin folder : %APP_BIN% 
  ECHO Qt bin folder  : %QT_BIN%

  ECHO "Run windeployqt"
  %QT_BIN%\windeployqt.exe --no-translations -concurrent -clucene -core -enginio -gui -qthelp -multimedia -multimediawidgets -multimediaquick -network -opengl -positioning -printsupport -qml  -quick -sensors -sql -svg -test -webkit -webkitwidgets -widgets -xml -webchannel "%APP_BIN%\liger.exe"

  ECHO "Copy unicode library"
  XCOPY /Y "%QT_BIN%\icudt54.dll" "%APP_BIN%"
  XCOPY /Y "%QT_BIN%\icuin54.dll" "%APP_BIN%" 
  XCOPY /Y "%QT_BIN%\icuuc54.dll" "%APP_BIN%"
)
