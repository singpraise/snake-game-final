@echo off
chcp 65001 >nul
cd /d "%~dp0"

if not exist snake.exe (
    echo snake.exe not found. Running build.bat...
    call build.bat
    if errorlevel 1 exit /b 1
)

echo Starting snake.exe ...
echo.
snake.exe
if errorlevel 1 (
    echo.
    echo Program exited with error.
    pause
)
