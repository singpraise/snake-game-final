#!/usr/bin/env bash
# Build script for Git Bash / MSYS2

set -e

cd "$(dirname "$0")"

echo "========================================"
echo "  C Snake Game - Build Script (bash)"
echo "========================================"
echo

# Git Bash does not include gcc in PATH by default.
# gcc also needs its bin dir on PATH so cc1.exe can load required DLLs.
GCC_DIR=""
for dir in \
    "/c/msys64/ucrt64/bin" \
    "/c/msys64/mingw64/bin" \
    "/c/msys64/clang64/bin" \
    "/c/MinGW/bin" \
    "/c/TDM-GCC-64/bin" \
    "/mingw64/bin"
do
    if [[ -x "$dir/gcc.exe" ]]; then
        GCC_DIR="$dir"
        export PATH="$dir:$PATH"
        break
    fi
done

if [[ -n "$GCC_DIR" ]] && ! command -v gcc >/dev/null 2>&1; then
    echo "Added to PATH: $GCC_DIR"
fi

if ! command -v gcc >/dev/null 2>&1; then
    echo "[ERROR] gcc not found."
    echo
    echo "You have MSYS2 installed but gcc is not in PATH."
    echo "Try one of these:"
    echo
    echo "  1) Open 'MSYS2 UCRT64' terminal and run:"
    echo "       pacman -S mingw-w64-ucrt-x86_64-gcc"
    echo "     Then add to Windows PATH:"
    echo "       C:\\msys64\\ucrt64\\bin"
    echo
    echo "  2) Or compile directly:"
    echo "       /c/msys64/ucrt64/bin/gcc.exe -std=c99 -Wall -Wextra -O2 \\"
    echo "         -o snake.exe main.c init.c snake.c queue.c stack.c board.c memory.c input_win.c"
    echo
    exit 1
fi

echo "Using gcc: $(command -v gcc)"
gcc --version | head -1
echo

rm -f snake.exe

SRC="main.c init.c snake.c queue.c stack.c board.c memory.c game_ui.c game_config.c obstacles.c score.c audio.c level.c editor.c input_win.c"
CFLAGS="-std=c99 -Wall -Wextra -O2 -finput-charset=UTF-8 -fexec-charset=UTF-8"

echo "Compiling..."
gcc $CFLAGS -o snake.exe $SRC

if strings snake.exe 2>/dev/null | grep -q "Score:"; then
    echo "[OK] Verified: English build (Score: string found)"
elif grep -aq "Score:" snake.exe 2>/dev/null; then
    echo "[OK] Verified: English build"
else
    echo "[WARN] Could not verify English strings in snake.exe"
fi

echo
echo "[OK] Built snake.exe"
echo "Run:  ./snake.exe"
echo
