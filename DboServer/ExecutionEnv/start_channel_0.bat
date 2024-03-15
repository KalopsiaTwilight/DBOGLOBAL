@echo off
start GameServer.exe .\\config\\GameServer.ini

for /F "TOKENS=1,2,*" %%a in ('tasklist /FI "IMAGENAME eq GameServer.exe"') do set MyPID=%%b
echo %MyPID% > gameserver.pid

exit