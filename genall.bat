@echo off
setlocal enabledelayedexpansion
call setenv.bat

set /p version=<version
set builddate=%date:~0,4%%date:~5,2%%date:~8,2%
set buildversion=!version!_!builddate!
echo #define INNER_BASE_VERSION "!buildversion!" > src_base\function\versionValue.h
echo #define INNER_DUE_DATE "20240630" >> src_base\function\versionValue.h

rem ��static��lib���а汾����
if exist libs\setstatic.bat (
	cd libs
	call setstatic.bat 
	cd %~dp0\
)

for /d %%i in (build*) do (
	cd %%i
	if exist gen.bat (
		echo ============= gen %%i
		call gen.bat
	)
	cd ..
)