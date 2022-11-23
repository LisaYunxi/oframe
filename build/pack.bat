@echo off
setlocal enabledelayedexpansion
xcopy ..\libs\ofserviceapi .\ofserviceapi /s /e /i /y
for /d %%i in (*) do (
	cd %%i
	if exist pack.bat (
		echo ============= pack %%i
		call pack.bat
	)	
	cd ..
)
if "%1"=="" (
	set tarname=bin.zip
) else (
	set tarname=%1.zip
)
zip -r !tarname! *\*.exe *\*.dll *\*.lib .\ofserviceapi zipoframe.bat