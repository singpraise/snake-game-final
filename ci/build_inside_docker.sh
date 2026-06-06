#!/usr/bin/env bash
# Runs inside emscripten/emsdk Docker container
set -eux
emcc --version
bash build_web.sh
