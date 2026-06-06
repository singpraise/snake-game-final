#!/usr/bin/env bash
# Build C Snake Game for the browser (Emscripten → WebAssembly)

set -euo pipefail

cd "$(dirname "$0")"

echo "========================================"
echo "  C Snake Game - Web Build (Emscripten)"
echo "========================================"
echo

if ! command -v emcc >/dev/null 2>&1; then
    for dir in \
        "/c/emsdk/upstream/emscripten" \
        "/c/emsdk/emscripten/1.39.20" \
        "$HOME/emsdk/upstream/emscripten"
    do
        if [[ -x "$dir/emcc" || -x "$dir/emcc.bat" ]]; then
            export PATH="$dir:$PATH"
            break
        fi
    done
fi

if ! command -v emcc >/dev/null 2>&1; then
    echo "[ERROR] emcc not found."
    echo
    echo "Install Emscripten:"
    echo "  git clone https://github.com/emscripten-core/emsdk.git"
    echo "  cd emsdk && ./emsdk install latest && ./emsdk activate latest"
    echo "  source ./emsdk_env.sh"
    echo
    echo "Or push to GitHub — Actions will build and deploy automatically."
    exit 1
fi

echo "Using emcc: $(command -v emcc)"
emcc --version | head -1
echo

CORE_SRC="init.c snake.c queue.c stack.c board.c memory.c game_ui.c game_config.c obstacles.c score.c audio.c level.c editor.c"
WEB_SRC="main_web.c platform_web.c"

CFLAGS=(
    -std=c99
    -Wall
    -Wextra
    -O2
)

LDFLAGS=(
    -o web/snake.js
    -s WASM=1
    -s ENVIRONMENT=web
    -s NO_EXIT_RUNTIME=1
    -s ALLOW_MEMORY_GROWTH=1
    -s EXPORTED_FUNCTIONS='["_main","_web_send_key","_web_restart","_web_set_mode","_web_start_editor","_web_get_score","_web_is_game_over","_web_get_mode_name","_web_load_level_text"]'
    -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]'
    -s ASSERTIONS=1
)

echo "Compiling to web/snake.js + web/snake.wasm ..."
emcc "${CFLAGS[@]}" $WEB_SRC $CORE_SRC "${LDFLAGS[@]}"

echo
echo "[OK] Web build complete."
echo "Open web/index.html via a local server, or deploy web/ to GitHub Pages."
echo
echo "  python -m http.server 8080 --directory web"
echo "  http://localhost:8080/"
echo
