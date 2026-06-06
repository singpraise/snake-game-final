@echo off
chcp 65001 >nul
setlocal

echo ========================================
echo   C Snake Game - MSVC Build Script
echo ========================================
echo.

where cl >nul 2>&1
if errorlevel 1 (
    echo [ERROR] cl.exe not found.
    echo.
    echo Open "x64 Native Tools Command Prompt for VS" from Start Menu,
    echo then run this script again:
    echo   cd "%~dp0"
    echo   build_msvc.bat
    echo.
    echo Or install Visual Studio with "Desktop development with C++"
    pause
    exit /b 1
)

set SRC=main.c init.c snake.c queue.c stack.c board.c memory.c game_ui.c game_config.c obstacles.c score.c audio.c level.c editor.c input_win.c
set CFLAGS=/nologo /W4 /O2 /utf-8

echo Compiling with MSVC...
cl %CFLAGS% %SRC% /Fe:snake.exe
if errorlevel 1 (
    echo [ERROR] Build failed.
    pause
    exit /b 1
)

del *.obj 2>nul
echo.
echo [OK] Built snake.exe
pause
exit /b 0
