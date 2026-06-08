#!/usr/bin/env bash
# test/integration/gxdzc/test_exit.sh — 验证退出游戏按钮正常退出
#
# 场景：启动 → 点击开始 → 点击菜单 → 点击退出按钮
# 期望：进程正常退出，无崩溃、无内存错误
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
source "$SCRIPT_DIR/../lib.sh"

echo "=== gxdzc: clean exit ==="

# 与原 test/gxdzc/exit.sh 相同的点击序列
export VMRP_AUTO_CLICKS="0,0,500;228,309,500;76,291,500;123,291,500"

run_vmrp "mythroad/gxdzc.mrp"

assert_not_timeout "game did not hang"
assert_exit_code 0 "exits without error"
assert_stderr_not_contains "Segmentation\|SIGSEGV\|double free\|heap-use-after-free" \
    "no memory errors"
assert_stderr_not_contains "FATAL\|panic\|abort" "no fatal errors"

test_summary
