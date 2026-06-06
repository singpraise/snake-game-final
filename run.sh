#!/usr/bin/env bash
cd "$(dirname "$0")"

if [[ ! -f snake.exe ]]; then
    echo "snake.exe not found. Building..."
    bash ./build.sh
fi

./snake.exe
