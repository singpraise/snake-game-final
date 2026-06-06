@echo off
chcp 65001 >nul
setlocal EnableDelayedExpansion

echo ========================================
echo   C Snake Game - Windows Build Script
echo ========================================
echo.

REM Try common MinGW / MSYS2 install locations
set "GCC="
for %%P in (
    "C:\msys64\ucrt64\bin\gcc.exe"
    "C:\msys64\mingw64\bin\gcc.exe"
    "C:\msys64\clang64\bin\gcc.exe"
    "C:\MinGW\bin\gcc.exe"
    "C:\MinGW-w64\bin\gcc.exe"
    "C:\TDM-GCC-64\bin\gcc.exe"
) do (
    if exist %%~P (
        set "GCC=%%~P"
        set "PATH=%%~dpP;%PATH%"
        goto :found_gcc
    )
)

where gcc >nul 2>&1
if not errorlevel 1 goto :found_gcc

echo [ERROR] gcc not found.
echo.
echo === How to install (pick one) ===
echo.
echo [Option A] MSYS2 (recommended)
echo   1. Download: https://www.msys2.org/
echo   2. Install, then open "MSYS2 UCRT64" terminal
echo   3. Run: pacman -S mingw-w64-ucrt-x86_64-gcc
echo   4. Add to PATH: C:\msys64\ucrt64\bin
echo   5. Restart PowerShell and run build.bat again
echo.
echo [Option B] WinLibs (portable, no pacman)
echo   1. Download: https://winlibs.com/
echo   2. Extract zip, add its \bin folder to PATH
echo.
echo [Option C] Visual Studio
echo   Run build_msvc.bat from "Developer Command Prompt"
echo.
pause
exit /b 1

:found_gcc
where gcc >nul 2>&1
if not errorlevel 1 (
    for /f "delims=" %%G in ('where gcc 2^>nul') do (
        echo Using gcc: %%G
        goto :compile
    )
)
echo Using gcc: %GCC%

:compile
REM Delete old exe (may be outdated Chinese build)
if exist snake.exe del /f snake.exe

set SRC=main.c init.c snake.c queue.c stack.c board.c memory.c game_ui.c game_config.c obstacles.c score.c audio.c level.c editor.c input_win.c
set CFLAGS=-std=c99 -Wall -Wextra -O2 -finput-charset=UTF-8 -fexec-charset=UTF-8

echo.
echo Compiling...
gcc %CFLAGS% -o snake.exe %SRC%
if errorlevel 1 (
    echo.
    echo [ERROR] Build failed.
    pause
    exit /b 1
)

echo.
echo [OK] Built snake.exe
echo.
echo PowerShell:  .\snake.exe
echo cmd:         snake.exe
echo Or:          run.bat
echo.
pause
exit /b 0
