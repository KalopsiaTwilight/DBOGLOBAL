@echo off
start AuthServer.exe .\\config\\AuthServer.ini

for /F "TOKENS=1,2,*" %%a in ('tasklist /FI "IMAGENAME eq AuthServer.exe"') do set MyPID=%%b
echo %MyPID% > authserver.pid

pause