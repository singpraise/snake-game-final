@echo off
chcp 65001 >nul
cd /d "%~dp0"

set "GIT=C:\Program Files\Git\bin\git.exe"
if not exist "%GIT%" (
    echo [ERROR] Git not found. Install from https://git-scm.com/download/win
    pause
    exit /b 1
)

echo ========================================
echo   Push to snake-game-final (GitHub)
echo ========================================
echo.
echo BEFORE running: create empty repo at
echo   https://github.com/new
echo Name: snake-game-final  (Public, no README)
echo.

"%GIT%" status
echo.
"%GIT%" remote -v
echo.
echo Pushing to origin main...
"%GIT%" push -u origin main
if errorlevel 1 (
    echo.
    echo [FAILED] Common fixes:
    echo   1. Create repo: https://github.com/new  name=snake-game-final
    echo   2. Sign in to GitHub in browser when prompted
    echo   3. Or use GitHub Desktop / Personal Access Token
    pause
    exit /b 1
)

echo.
echo [OK] Pushed! Next steps:
echo   1. https://github.com/singpraise/snake-game-final/settings/pages
echo   2. Source = GitHub Actions
echo   3. Wait Actions green, then open:
echo      https://singpraise.github.io/snake-game-final/
echo.
pause
