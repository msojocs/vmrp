#!/usr/bin/env bash
# test/integration/gxdzc/test_pay_cancel.sh — 验证取消付费后正常退出
#
# 场景：启动 → 开始游戏 → 触发付费 → 选择取消 → 退出
# 期望：取消付费不导致崩溃或挂起
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
source "$SCRIPT_DIR/../lib.sh"

echo "=== gxdzc: pay cancel ==="

# 与原 test/gxdzc/pay-cancel.sh 相同的点击序列
export VMRP_AUTO_CLICKS="0,0,500;228,309,500;158,293,1000;133,295,3000;228,309,500;3,309,500"

run_vmrp "mythroad/gxdzc.mrp"

assert_not_timeout "game did not hang after pay cancel"
assert_exit_code 0 "exits without error"
assert_stderr_not_contains "Segmentation\|SIGSEGV\|double free" \
    "no memory errors"

test_summary
