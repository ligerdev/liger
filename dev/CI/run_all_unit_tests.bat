SET BUIDDIR=E:\Projects\build_liger_505
SET QTPATH=D:\Qt\5.6\msvc2015_64\bin
%QTPATH%\windeployqt.exe --no-translations -core -test %BUIDDIR%\tests\bin

SET PATH=%PATH%;%BUIDDIR%\bin;%BUIDDIR%\lib\liger\plugins;

for %%f in (%BUIDDIR%\tests\bin\test_*.exe) do (
	%%f -silent -xunitxml -o "%%f.xml" > "%%f.log"
)