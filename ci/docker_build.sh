#!/usr/bin/env bash
# Fallback: build WASM inside official Emscripten Docker image
set -eux
docker run --rm \
  -v "$PWD:/src" \
  -w /src \
  emscripten/emsdk:3.1.64 \
  bash ci/build_inside_docker.sh
