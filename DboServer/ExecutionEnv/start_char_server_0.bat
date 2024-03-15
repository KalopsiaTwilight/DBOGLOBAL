@echo off
start CharServer.exe .\\config\\CharServer.ini

for /F "TOKENS=1,2,*" %%a in ('tasklist /FI "IMAGENAME eq CharServer.exe"') do set MyPID=%%b
echo %MyPID% > charserver.pid

exit