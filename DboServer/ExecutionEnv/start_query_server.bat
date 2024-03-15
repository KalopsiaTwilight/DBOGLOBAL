@echo off
start QueryServer.exe .\\config\\QueryServer.ini

for /F "TOKENS=1,2,*" %%a in ('tasklist /FI "IMAGENAME eq QueryServer.exe"') do set MyPID=%%b
echo %MyPID% > queryserver.pid

exit