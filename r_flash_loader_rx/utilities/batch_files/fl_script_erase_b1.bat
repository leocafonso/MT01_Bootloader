@echo OFF
CLS
SET PY=C:\Python27\python.exe
SET WS=..\..\r_flash_loader_rx\utilities\python

%PY% -u "%WS%\r_fl_serial_flash_loader.py" -p3 -b1 -cerase

ECHO DONE
pause
