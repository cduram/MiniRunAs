@echo off & setlocal
call "%VS90COMNTOOLS%vsvars32.bat"
set libs=advapi32.lib
cl -nologo miniRunAs.cpp %libs%
if errorlevel 1 exit /b 9
del miniRunAs.obj
