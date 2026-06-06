#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

echo "========================================"
echo "  C Snake Game Final - Flask Server"
echo "========================================"

if ! command -v python3 >/dev/null 2>&1; then
    echo "[ERROR] python3 not found"
    exit 1
fi

if [[ ! -f web/snake.js ]]; then
    echo "[WARN] web/snake.js missing — run: bash build_web.sh"
fi

python3 -m pip install -r requirements.txt -q
echo "Open http://127.0.0.1:5000"
python3 app.py
