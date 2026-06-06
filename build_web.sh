#!/usr/bin/env bash
# Build C Snake Game for the browser (Emscripten -> WebAssembly)

set -eu

cd "$(dirname "$0")"

echo "========================================"
echo "  C Snake Game - Web Build (Emscripten)"
echo "========================================"

if ! command -v emcc >/dev/null 2>&1; then
    echo "[ERROR] emcc not found on PATH"
    exit 1
fi

echo "Using emcc: $(command -v emcc)"
emcc --version 2>&1 | head -1 || true

CORE_SRC="init.c snake.c queue.c stack.c board.c memory.c game_ui.c game_config.c obstacles.c score.c audio.c level.c editor.c"
WEB_SRC="main_web.c platform_web.c"

echo "Compiling to web/snake.js + web/snake.wasm ..."
emcc -std=c99 -O2 \
    $WEB_SRC $CORE_SRC \
    -o web/snake.js \
    -s WASM=1 \
    -s ENVIRONMENT=web \
    -s NO_EXIT_RUNTIME=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s ASSERTIONS=1

echo "[OK] Web build complete."
ls -la web/snake.js web/snake.wasm
