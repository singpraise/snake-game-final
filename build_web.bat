@echo off
setlocal EnableDelayedExpansion

echo ========================================
echo   C Snake Game - Web Build (Emscripten)
echo ========================================
echo.

where emcc >nul 2>&1
if errorlevel 1 (
    if exist "C:\emsdk\upstream\emscripten\emcc.bat" (
        set "PATH=C:\emsdk\upstream\emscripten;%PATH%"
    )
)

where emcc >nul 2>&1
if errorlevel 1 (
    echo [ERROR] emcc not found.
    echo Install Emscripten from https://emscripten.org/docs/getting_started/downloads.html
    echo Or push to GitHub and let Actions build the web version.
    pause
    exit /b 1
)

echo Using emcc:
where emcc
echo.

set CORE=init.c snake.c queue.c stack.c board.c memory.c game_ui.c game_config.c obstacles.c score.c audio.c level.c editor.c
set WEB=main_web.c platform_web.c
set CFLAGS=-std=c99 -Wall -Wextra -O2 -finput-charset=UTF-8 -fexec-charset=UTF-8

echo Compiling...
emcc %CFLAGS% %WEB% %CORE% -o web\snake.js ^
    -s WASM=1 ^
    -s ENVIRONMENT=web ^
    -s NO_EXIT_RUNTIME=1 ^
    -s ALLOW_MEMORY_GROWTH=1 ^
    -s EXPORTED_FUNCTIONS=["_main","_web_send_key","_web_restart","_web_set_mode","_web_start_editor","_web_get_score","_web_is_game_over","_web_get_mode_name","_web_load_level_text"] ^
    -s EXPORTED_RUNTIME_METHODS=["ccall","cwrap"] ^
    -s ASSERTIONS=1

if errorlevel 1 (
    echo [ERROR] Web build failed.
    pause
    exit /b 1
)

echo.
echo [OK] Built web\snake.js and web\snake.wasm
echo Serve the web folder with a local HTTP server to test.
pause
