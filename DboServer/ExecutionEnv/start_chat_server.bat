start ChatServer.exe .\\config\\ChatServer.ini

for /F "TOKENS=1,2,*" %%a in ('tasklist /FI "IMAGENAME eq ChatServer.exe"') do set MyPID=%%b
echo %MyPID% > chatserver.pid

exit