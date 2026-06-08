#!/usr/bin/env bash
# test/integration/mpc/test_exit.sh — 验证 MPC 游戏正常退出
#
# 场景：启动 → 导航到退出 → 确认退出
# 期望：进程正常退出
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
source "$SCRIPT_DIR/../lib.sh"

echo "=== mpc: clean exit ==="

# 与原 test/mpc/exit.sh 相同的点击序列
export VMRP_AUTO_CLICKS="14,308,2000;44,285,2000;14,308,2000"

run_vmrp "mythroad/mpc.mrp"

assert_not_timeout "game did not hang"
assert_exit_code 0 "exits without error"
assert_stderr_not_contains "Segmentation\|SIGSEGV\|double free" \
    "no memory errors"
assert_stderr_not_contains "FATAL\|panic\|abort" "no fatal errors"

test_summary
