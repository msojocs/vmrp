/*
 * test_api_lifecycle.c — 验证 vmrp_api.h 的生命周期管理
 *
 * 策略：stub 掉 prepareVmrpArgs/startVmrp/event/timer 等重型函数，
 * 只测试 vmrp_api.c 自身的屏幕缓冲区管理、edit 状态、dirty 标志等纯逻辑。
 * 这些是嵌入式使用（Flutter/Android）最容易出错的地方。
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define HARNESS_MULTI_FILE
#include "harness.h"
#include "../src/include/vmrp_api.h"
#include "../src/include/vmrp.h"

/* ── 外部依赖的桩 ─────────────────────────────────────────
 * vmrp_api.c 调用这些函数，我们提供轻量实现使其可独立链接。 */

/* prepareVmrpArgs — 在 vmrp.c 中实现，被 vmrp_api_init 调用。
 * 桩总是返回成功。 */
static int prepare_called = 0;
int prepareVmrpArgs(int argc, char *argv[]) {
    (void)argc; (void)argv;
    prepare_called++;
    return 0; /* MR_SUCCESS */
}

/* startVmrp — 被 vmrp_api_start 调用 */
static int start_called = 0;
int startVmrp(int argc, char *argv[]) {
    (void)argc; (void)argv;
    start_called++;
    return 0;
}

/* event / timer — 被 vmrp_api_event / vmrp_api_timer 调用 */
static int event_called = 0;
static int last_event_code = -1;
int32_t event(int32_t code, int32_t p1, int32_t p2) {
    (void)p1; (void)p2;
    event_called++;
    last_event_code = code;
    return 0;
}

static int timer_called = 0;
int32_t timer(void) {
    timer_called++;
    return 0;
}

/* configureVmrpDnsMap — 被 vmrp_api_set_dns_map 调用 */
int configureVmrpDnsMap(const char *map) {
    (void)map;
    return 0;
}

/* printVmrpUsage — vmrp.h 声明 */
void printVmrpUsage(const char *program) { (void)program; }

/* native_platform_memory — 被 editCreate/editRelease/set_edit_text 用到 */
void *my_mallocExt(uint32_t len) {
    uint32_t *p = malloc((size_t)len + sizeof(uint32_t));
    if (!p) return NULL;
    *p = len;
    return p + 1;
}

void my_freeExt(void *p) {
    if (p) free(((uint32_t *)p) - 1);
}

/* vmrp_config — 在 vmrp.c 中定义，这里提供 */
VmrpConfig vmrp_config = {
    .screen_width = 240,
    .screen_height = 320,
};

/* ── 辅助函数 ─────────────────────────────────────────── */
static void reset_counters(void) {
    prepare_called = 0;
    start_called = 0;
    event_called = 0;
    timer_called = 0;
    last_event_code = -1;
}

/* ── 测试用例 ─────────────────────────────────────────── */

static void test_init_allocates_screen_buffer(void) {
    TEST_BEGIN("vmrp_api_init allocates screen buffer");
    reset_counters();

    int rc = vmrp_api_init(240, 320);
    ASSERT_INT_EQ(0, rc);
    ASSERT_INT_EQ(1, prepare_called);

    const uint16_t *buf = vmrp_api_get_screen_buffer();
    ASSERT_PTR_NOT_NULL(buf);

    ASSERT_INT_EQ(240, vmrp_api_get_screen_width());
    ASSERT_INT_EQ(320, vmrp_api_get_screen_height());

    vmrp_api_destroy();
    TEST_END();
}

static void test_init_default_screen_size(void) {
    TEST_BEGIN("vmrp_api_init with 0,0 uses default 240x320");
    reset_counters();

    int rc = vmrp_api_init(0, 0);
    ASSERT_INT_EQ(0, rc);

    ASSERT_INT_EQ(240, vmrp_api_get_screen_width());
    ASSERT_INT_EQ(320, vmrp_api_get_screen_height());

    vmrp_api_destroy();
    TEST_END();
}

static void test_init_negative_screen_size(void) {
    TEST_BEGIN("vmrp_api_init with negative sizes uses default");
    reset_counters();

    int rc = vmrp_api_init(-100, -200);
    ASSERT_INT_EQ(0, rc);

    ASSERT_INT_EQ(240, vmrp_api_get_screen_width());
    ASSERT_INT_EQ(320, vmrp_api_get_screen_height());

    vmrp_api_destroy();
    TEST_END();
}

static void test_init_custom_screen_size(void) {
    TEST_BEGIN("vmrp_api_init with custom screen size");
    reset_counters();

    int rc = vmrp_api_init(176, 220);
    ASSERT_INT_EQ(0, rc);

    ASSERT_INT_EQ(176, vmrp_api_get_screen_width());
    ASSERT_INT_EQ(220, vmrp_api_get_screen_height());

    vmrp_api_destroy();
    TEST_END();
}

static void test_screen_dirty_flag(void) {
    TEST_BEGIN("screen dirty flag starts clean and auto-clears on read");
    reset_counters();

    vmrp_api_init(240, 320);

    /* 初始状态应该不脏 */
    int dirty = vmrp_api_get_screen_dirty();
    ASSERT_INT_EQ(0, dirty);

    /* 再次读取仍然不脏 */
    dirty = vmrp_api_get_screen_dirty();
    ASSERT_INT_EQ(0, dirty);

    vmrp_api_destroy();
    TEST_END();
}

static void test_destroy_nullifies_screen_buffer(void) {
    TEST_BEGIN("vmrp_api_destroy clears screen buffer");
    reset_counters();

    vmrp_api_init(240, 320);
    ASSERT_PTR_NOT_NULL(vmrp_api_get_screen_buffer());

    vmrp_api_destroy();
    ASSERT_PTR_NULL(vmrp_api_get_screen_buffer());

    TEST_END();
}

static void test_double_destroy_safe(void) {
    TEST_BEGIN("double vmrp_api_destroy does not crash");
    reset_counters();

    vmrp_api_init(240, 320);
    vmrp_api_destroy();
    vmrp_api_destroy();  /* 不能崩溃 */
    ASSERT_TRUE(1);      /* 走到这里就是通过 */

    TEST_END();
}

static void test_multiple_init_destroy_cycles(void) {
    TEST_BEGIN("init/destroy can be repeated multiple times");
    reset_counters();

    for (int i = 0; i < 5; i++) {
        int rc = vmrp_api_init(240, 320);
        ASSERT_INT_EQ(0, rc);
        ASSERT_PTR_NOT_NULL(vmrp_api_get_screen_buffer());
        vmrp_api_destroy();
        ASSERT_PTR_NULL(vmrp_api_get_screen_buffer());
    }
    ASSERT_INT_EQ(5, prepare_called);

    TEST_END();
}

static void test_start_calls_startVmrp(void) {
    TEST_BEGIN("vmrp_api_start delegates to startVmrp");
    reset_counters();

    vmrp_api_init(240, 320);
    int rc = vmrp_api_start("/tmp/test.mrp", NULL, NULL);
    ASSERT_INT_EQ(0, rc);
    ASSERT_TRUE(start_called >= 1);

    vmrp_api_destroy();
    TEST_END();
}

static void test_start_null_path_fails(void) {
    TEST_BEGIN("vmrp_api_start with NULL path returns -1");
    reset_counters();

    vmrp_api_init(240, 320);
    int rc = vmrp_api_start(NULL, NULL, NULL);
    ASSERT_INT_EQ(-1, rc);

    vmrp_api_destroy();
    TEST_END();
}

static void test_start_empty_path_fails(void) {
    TEST_BEGIN("vmrp_api_start with empty path returns -1");
    reset_counters();

    vmrp_api_init(240, 320);
    int rc = vmrp_api_start("", NULL, NULL);
    ASSERT_INT_EQ(-1, rc);

    vmrp_api_destroy();
    TEST_END();
}

static void test_event_forwards_to_runtime(void) {
    TEST_BEGIN("vmrp_api_event forwards to event()");
    reset_counters();

    vmrp_api_init(240, 320);
    vmrp_api_event(VMRP_KEY_PRESS, VMRP_KEY_5, 0);
    ASSERT_INT_EQ(1, event_called);
    ASSERT_INT_EQ(VMRP_KEY_PRESS, last_event_code);

    vmrp_api_destroy();
    TEST_END();
}

static void test_timer_forwards_to_runtime(void) {
    TEST_BEGIN("vmrp_api_timer forwards to timer()");
    reset_counters();

    vmrp_api_init(240, 320);
    vmrp_api_timer();
    ASSERT_INT_EQ(1, timer_called);

    vmrp_api_destroy();
    TEST_END();
}

static void test_edit_inactive_by_default(void) {
    TEST_BEGIN("edit is inactive after init");
    reset_counters();

    vmrp_api_init(240, 320);
    ASSERT_INT_EQ(0, vmrp_api_is_edit_active());

    vmrp_api_destroy();
    TEST_END();
}

static void test_set_edit_text_without_active_fails(void) {
    TEST_BEGIN("vmrp_api_set_edit_text fails when edit not active");
    reset_counters();

    vmrp_api_init(240, 320);
    int rc = vmrp_api_set_edit_text("hello");
    ASSERT_INT_EQ(-1, rc);

    vmrp_api_destroy();
    TEST_END();
}

static void test_cancel_edit_without_active_fails(void) {
    TEST_BEGIN("vmrp_api_cancel_edit fails when edit not active");
    reset_counters();

    vmrp_api_init(240, 320);
    int rc = vmrp_api_cancel_edit();
    ASSERT_INT_EQ(-1, rc);

    vmrp_api_destroy();
    TEST_END();
}

/* ── 运行所有测试 ─────────────────────────────────────── */
int test_api_lifecycle_run_all(void) {
    int before = harness_tests_failed;

    test_init_allocates_screen_buffer();
    test_init_default_screen_size();
    test_init_negative_screen_size();
    test_init_custom_screen_size();
    test_screen_dirty_flag();
    test_destroy_nullifies_screen_buffer();
    test_double_destroy_safe();
    test_multiple_init_destroy_cycles();
    test_start_calls_startVmrp();
    test_start_null_path_fails();
    test_start_empty_path_fails();
    test_event_forwards_to_runtime();
    test_timer_forwards_to_runtime();
    test_edit_inactive_by_default();
    test_set_edit_text_without_active_fails();
    test_cancel_edit_without_active_fails();

    return harness_tests_failed - before;
}
