@echo off
chcp 65001 >nul
cd /d "%~dp0"

echo ========================================
echo   C Snake Game Final - Flask Server
echo ========================================
echo.

where python >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Python not found. Install Python 3.10+ from https://www.python.org/
    pause
    exit /b 1
)

if not exist "web\snake.js" (
    echo [WARN] web\snake.js not found. Run build_web.bat first for full game.
    echo.
)

echo Installing Flask...
python -m pip install -r requirements.txt -q

echo.
echo Starting Flask at http://127.0.0.1:5000
echo.
python app.py
