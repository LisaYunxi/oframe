@echo off
setlocal enabledelayedexpansion
cd ..
call setenv.bat
cd %~dp0\

echo build version is: [%1]

@rem ���˿̵�ʱ���ɱ���version��ʱ��
echo #define INNER_BASE_VERSION "%1" > ..\src_base\function\versionValue.h
echo #define INNER_DUE_DATE "20240630" >> ..\src_base\function\versionValue.h

rem ��static��lib���а汾����
if exist ..\libs\setstatic.bat (
	cd ..\libs
	call setstatic.bat 
	cd %~dp0\
)

for /d %%i in (*) do (
	cd %%i
	if exist makefile.windows (
		echo ============= make %%i
		call nmake -f makefile.windows 1>output 2>&1
	)
	if exist build.bat (
		echo ============= build %%i
		call build.bat
	) else (
		for /d %%i in (*) do (
			cd %%i
			if exist makefile.windows (
				echo ============= make %%i
				call nmake -f makefile.windows 1>output 2>&1
			)
			if exist build.bat (
				echo ============= build %%i
				call build.bat
			)	
			cd ..
		)
	)
	cd ..
)
