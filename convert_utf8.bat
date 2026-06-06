@echo off
chcp 65001 >nul
REM Convert all source files in this folder to UTF-8 (no BOM)
powershell -NoProfile -Command ^
  "$utf8 = New-Object System.Text.UTF8Encoding $false; " ^
  "$dir = '%~dp0'; " ^
  "Get-ChildItem $dir -Include *.c,*.h,*.bat,*.txt,Makefile -File -Recurse | ForEach-Object { " ^
  "  $t = [IO.File]::ReadAllText($_.FullName); " ^
  "  [IO.File]::WriteAllText($_.FullName, $t, $utf8); " ^
  "  Write-Host ('UTF-8: ' + $_.Name) " ^
  "}"
echo Done.
pause
