#!/usr/bin/env bash
# gzwdzjs 花屏手动复现脚本(不经 vitest)。
# 用法: repro.sh <输出目录> [额外环境变量 KEY=VAL ...]
set -e
OUT="${1:?usage: repro.sh <outdir>}"
shift || true
mkdir -p "$OUT"
ROOT="${VMRP_ROOT:-$(cd "$(dirname "$0")/../../.." && pwd)}"
WS=$(mktemp -d /tmp/gzwdzjs-ws-XXXXXX)
cp -rL "$ROOT/wasm/dist/fs/mythroad" "$WS/mythroad"
cp "$ROOT/test/fixtures/gzwdzjs.mrp" "$WS/"
SOCK="$OUT/e2e.sock"

for kv in "$@"; do export "$kv"; done

SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy \
VMRP_E2E_SOCKET="$SOCK" \
"$ROOT/build/vmrp" --work-dir "$WS" --memory 2M "$WS/gzwdzjs.mrp" \
  >"$OUT/stdout.log" 2>"$OUT/stderr.log" &
VMRP_PID=$!

cmd() { printf '%s\n' "$1" | timeout 30 socat - "UNIX-CONNECT:$SOCK" ; }

for i in $(seq 1 200); do [ -S "$SOCK" ] && break; sleep 0.1; done
sleep 5
cmd "SCREEN $OUT/bgm-select.ppm"
cmd "KEY RIGHT_SOFT"; sleep 3
cmd "SCREEN $OUT/menu.ppm"
for i in 1 2 3 4; do cmd "KEY ENTER"; sleep 1; done
sleep 1
cmd "SCREEN $OUT/render-check.ppm"
cmd "QUIT" || kill $VMRP_PID
wait $VMRP_PID 2>/dev/null || true
rm -rf "$WS"
echo "done: $OUT"
