@echo off
setlocal enableDelayedExpansion
set sum=0
for /f "delims=" %%i in (研究生录取名单.txt) do (
	cqzk.exe %%i 2016
	set /a sum+=1
	echo 已处理完第!sum!条信息
)
echo -------------------------------------------------------------------------
echo 共查询了%sum%条信息
PAUSE
研究生信息.txt
EXIT
