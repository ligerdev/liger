@ECHO off
SET QTPATH=D:\Qt\5.6\msvc2015_64\bin
SET BUILDDIR=E:\Projects\liger-build-debug
SET PATH=%PATH%;%QTPATH%
SET PATH=%PATH%;%BUILDDIR%\bin
SET TEST_BIN=%BUILDDIR%\tests\bin

CD /d %TEST_BIN%
FOR /r %TEST_BIN% %%a in (test_*.exe) DO (
    "%%~na.exe" -xunitxml -o %%~na.xml
)

python %TEST_BIN%\failure_reporter.py %TEST_BIN%