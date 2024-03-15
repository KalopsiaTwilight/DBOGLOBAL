@echo off
start MasterServer.exe .\\config\\MasterServer.ini

for /F "TOKENS=1,2,*" %%a in ('tasklist /FI "IMAGENAME eq MasterServer.exe"') do set MyPID=%%b
echo %MyPID% > masterserver.pid

exit