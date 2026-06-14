#!/usr/bin/env bash
# test/integration/lib.sh — 端到端测试共享断言库
#
# 用法：
#   source "$(dirname "$0")/../integration/lib.sh"
#   run_vmrp "mythroad/game.mrp"
#   assert_exit_code 0
#   assert_stderr_not_contains "Segmentation"
#   test_summary

set -uo pipefail

# ── 配置 ──────────────────────────────────────────────────
VMRP_BIN="${VMRP_BIN:-build/vmrp}"
VMRP_TIMEOUT="${VMRP_TIMEOUT:-30}"   # 秒

# ── 状态变量 ──────────────────────────────────────────────
_PASS_COUNT=0
_FAIL_COUNT=0
_TEST_TMPDIR="$(mktemp -d)"
trap 'rm -rf "$_TEST_TMPDIR"' EXIT

# ── 核心运行函数 ──────────────────────────────────────────
# run_vmrp <mrp_file>
# 运行前设置好 VMRP_AUTO_CLICKS 等环境变量
# 运行后可用 assert_* 检查结果
# 设置: VMRP_EXIT_CODE, VMRP_STDOUT_FILE, VMRP_STDERR_FILE, VMRP_SCREEN_FILE
run_vmrp() {
    local mrp="$1"

    VMRP_STDOUT_FILE="$_TEST_TMPDIR/stdout.log"
    VMRP_STDERR_FILE="$_TEST_TMPDIR/stderr.log"
    VMRP_SCREEN_FILE="${VMRP_PPM_PATH:-$_TEST_TMPDIR/screen.ppm}"

    export VMRP_PPM_PATH="$VMRP_SCREEN_FILE"

    VMRP_EXIT_CODE=0
    timeout "$VMRP_TIMEOUT" "$VMRP_BIN" "$mrp" \
        >"$VMRP_STDOUT_FILE" 2>"$VMRP_STDERR_FILE" || VMRP_EXIT_CODE=$?

    # timeout(1) 返回 124 表示超时
    if [ "$VMRP_EXIT_CODE" -eq 124 ]; then
        echo "  WARN: vmrp timed out after ${VMRP_TIMEOUT}s"
    fi
}

# ── 断言函数 ──────────────────────────────────────────────

# assert_exit_code <expected> [label]
assert_exit_code() {
    local expected="$1"
    local label="${2:-exit code is $expected}"
    if [ "$VMRP_EXIT_CODE" -eq "$expected" ]; then
        echo "  PASS: $label"
        ((_PASS_COUNT++)) || true
    else
        echo "  FAIL: $label (expected=$expected, actual=$VMRP_EXIT_CODE)"
        ((_FAIL_COUNT++)) || true
    fi
}

# assert_exit_code_not <unexpected> [label]
assert_exit_code_not() {
    local unexpected="$1"
    local label="${2:-exit code is not $unexpected}"
    if [ "$VMRP_EXIT_CODE" -ne "$unexpected" ]; then
        echo "  PASS: $label"
        ((_PASS_COUNT++)) || true
    else
        echo "  FAIL: $label (got $VMRP_EXIT_CODE)"
        ((_FAIL_COUNT++)) || true
    fi
}

# assert_not_timeout [label]
assert_not_timeout() {
    local label="${1:-did not time out}"
    if [ "$VMRP_EXIT_CODE" -ne 124 ]; then
        echo "  PASS: $label"
        ((_PASS_COUNT++)) || true
    else
        echo "  FAIL: $label (process timed out after ${VMRP_TIMEOUT}s)"
        ((_FAIL_COUNT++)) || true
    fi
}

# assert_stdout_contains <pattern> [label]
assert_stdout_contains() {
    local pattern="$1"
    local label="${2:-stdout contains '$pattern'}"
    if grep -q "$pattern" "$VMRP_STDOUT_FILE" 2>/dev/null; then
        echo "  PASS: $label"
        ((_PASS_COUNT++)) || true
    else
        echo "  FAIL: $label"
        echo "    pattern '$pattern' not found in stdout"
        echo "    --- last 5 lines ---"
        tail -5 "$VMRP_STDOUT_FILE" 2>/dev/null | sed 's/^/    /'
        ((_FAIL_COUNT++)) || true
    fi
}

# assert_stderr_contains <pattern> [label]
assert_stderr_contains() {
    local pattern="$1"
    local label="${2:-stderr contains '$pattern'}"
    if grep -q "$pattern" "$VMRP_STDERR_FILE" 2>/dev/null; then
        echo "  PASS: $label"
        ((_PASS_COUNT++)) || true
    else
        echo "  FAIL: $label"
        echo "    pattern '$pattern' not found in stderr"
        ((_FAIL_COUNT++)) || true
    fi
}

# assert_stderr_not_contains <pattern> [label]
assert_stderr_not_contains() {
    local pattern="$1"
    local label="${2:-stderr clean of '$pattern'}"
    if grep -q "$pattern" "$VMRP_STDERR_FILE" 2>/dev/null; then
        echo "  FAIL: $label"
        grep "$pattern" "$VMRP_STDERR_FILE" | head -3 | sed 's/^/    /'
        ((_FAIL_COUNT++)) || true
    else
        echo "  PASS: $label"
        ((_PASS_COUNT++)) || true
    fi
}

# assert_file_exists <path> [label]
assert_file_exists() {
    local path="$1"
    local label="${2:-file exists: $path}"
    if [ -f "$path" ]; then
        echo "  PASS: $label"
        ((_PASS_COUNT++)) || true
    else
        echo "  FAIL: $label (file not found)"
        ((_FAIL_COUNT++)) || true
    fi
}

# assert_screen_captured [label]
assert_screen_captured() {
    local label="${1:-screen was captured}"
    if [ -f "$VMRP_SCREEN_FILE" ] && [ -s "$VMRP_SCREEN_FILE" ]; then
        echo "  PASS: $label"
        ((_PASS_COUNT++)) || true
    else
        echo "  FAIL: $label (no screen capture at $VMRP_SCREEN_FILE)"
        ((_FAIL_COUNT++)) || true
    fi
}

# assert_screen_matches_hash <baseline_hash_file> [label]
# 基准文件只包含一行 sha256 hex 字符串
assert_screen_matches_hash() {
    local baseline="$1"
    local label="${2:-screen matches baseline}"
    if [ ! -f "$VMRP_SCREEN_FILE" ]; then
        echo "  FAIL: $label (no screen capture)"
        ((_FAIL_COUNT++)) || true
        return
    fi
    if [ ! -f "$baseline" ]; then
        echo "  SKIP: $label (baseline file not found: $baseline)"
        echo "  Run 'sha256sum $VMRP_SCREEN_FILE' to generate baseline"
        return
    fi
    local actual_hash
    actual_hash=$(sha256sum "$VMRP_SCREEN_FILE" | cut -d' ' -f1)
    local expected_hash
    expected_hash=$(head -1 "$baseline" | tr -d '[:space:]')
    if [ "$actual_hash" = "$expected_hash" ]; then
        echo "  PASS: $label"
        ((_PASS_COUNT++)) || true
    else
        echo "  FAIL: $label"
        echo "    expected: $expected_hash"
        echo "    actual:   $actual_hash"
        # 保存实际截屏供人工检查
        local save_path="$_TEST_TMPDIR/actual_$(basename "$baseline" .sha256).ppm"
        cp "$VMRP_SCREEN_FILE" "$save_path" 2>/dev/null
        echo "    saved to: $save_path"
        ((_FAIL_COUNT++)) || true
    fi
}

# ── 汇总 ─────────────────────────────────────────────────
# test_summary — 打印汇总，失败时返回非零
test_summary() {
    local total=$((_PASS_COUNT + _FAIL_COUNT))
    echo ""
    echo "==== $total checks: $_PASS_COUNT passed, $_FAIL_COUNT failed ===="
    [ "$_FAIL_COUNT" -eq 0 ]
}
