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
#include <unistd.h>
#ifdef _WIN32
#include <direct.h>
#define getcwd _getcwd
#endif

static void test_setenv(const char *name, const char *value) {
#ifdef _WIN32
    _putenv_s(name, value);
#else
    setenv(name, value, 1);
#endif
}

static void test_unsetenv(const char *name) {
#ifdef _WIN32
    _putenv_s(name, "");
#else
    unsetenv(name);
#endif
}

#define HARNESS_MULTI_FILE
#include "harness.h"
#include "../src/include/vmrp_api.h"
#include "../src/include/vmrp.h"

/* ── 外部依赖的桩 ─────────────────────────────────────────
 * vmrp_api.c 调用这些函数，我们提供轻量实现使其可独立链接。 */

/* startVmrp — 被 vmrp_api_start 调用 */
static int start_called = 0;
static int stop_called = 0;
static int exit_requested = 0;
int startVmrp(const VmrpArgs *args) {
    (void)args;
    start_called++;
    exit_requested = 0;
    return 0;
}

void stopVmrp(void) {
    stop_called++;
    exit_requested = 0;
}

void vmrp_request_exit(void) {
    exit_requested = 1;
}

int vmrp_is_exited(void) {
    return exit_requested;
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

/* vmrp_args.c 依赖的桩 — 链接器需要这些符号 */
int32_t my_info(const char *path) {
    (void)path;
    return 0;
}

/* native audio — vmrp_api.c 暴露 Flutter 拉流 ABI，单元测试只需验证转发和静音兜底。 */
static int audio_active = 0;
static int audio_render_called = 0;
static int audio_stop_called = 0;
int native_audio_sample_rate(void) {
    return 44100;
}

int native_audio_channels(void) {
    return 2;
}

int native_audio_is_active(void) {
    return audio_active;
}

int native_audio_render_s16le(void *buffer, int frames) {
    audio_render_called++;
    if (buffer && frames > 0) {
        memset(buffer, 0x11, (size_t)frames * 2u * sizeof(int16_t));
    }
    return frames > 0 ? frames : 0;
}

void native_audio_stop(void) {
    audio_stop_called++;
    audio_active = 0;
}

/* vmrp_config — 在 vmrp.c 中定义，这里提供 */
VmrpConfig vmrp_config = {
    .screen_width = 240,
    .screen_height = 320,
};

/* ── 辅助函数 ─────────────────────────────────────────── */
static void reset_counters(void) {
    start_called = 0;
    event_called = 0;
    timer_called = 0;
    last_event_code = -1;
    stop_called = 0;
    exit_requested = 0;
    audio_active = 0;
    audio_render_called = 0;
    audio_stop_called = 0;
}

/* ── 测试用例 ─────────────────────────────────────────── */

static void test_init_allocates_screen_buffer(void) {
    TEST_BEGIN("vmrp_api_init allocates screen buffer");
    reset_counters();

    int rc = vmrp_api_init(240, 320);
    ASSERT_INT_EQ(0, rc);

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
    ASSERT_INT_EQ(1, stop_called);

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

static void test_is_running_tracks_start_and_destroy(void) {
    TEST_BEGIN("vmrp_api_is_running tracks start and destroy");
    reset_counters();

    vmrp_api_init(240, 320);
    ASSERT_INT_EQ(0, vmrp_api_is_running());

    int rc = vmrp_api_start("/tmp/test.mrp", NULL, NULL);
    ASSERT_INT_EQ(0, rc);
    ASSERT_INT_EQ(1, vmrp_api_is_running());

    vmrp_api_destroy();
    ASSERT_INT_EQ(0, vmrp_api_is_running());
    ASSERT_INT_EQ(1, stop_called);
    TEST_END();
}

static void test_exit_stops_api_events_and_timers(void) {
    TEST_BEGIN("runtime exit stops vmrp_api_event and vmrp_api_timer");
    reset_counters();

    vmrp_api_init(240, 320);
    ASSERT_INT_EQ(0, vmrp_api_start("/tmp/test.mrp", NULL, NULL));
    ASSERT_INT_EQ(1, vmrp_api_is_running());

    vmrp_request_exit();
    ASSERT_INT_EQ(0, vmrp_api_is_running());
    ASSERT_INT_EQ(-1, vmrp_api_event(VMRP_KEY_PRESS, VMRP_KEY_5, 0));
    ASSERT_INT_EQ(-1, vmrp_api_timer());
    ASSERT_INT_EQ(0, vmrp_api_is_edit_active());
    ASSERT_INT_EQ(-1, vmrp_api_set_edit_text("ignored"));
    ASSERT_INT_EQ(-1, vmrp_api_cancel_edit());
    ASSERT_INT_EQ(0, event_called);
    ASSERT_INT_EQ(0, timer_called);
    ASSERT_INT_EQ(0, vmrp_api_get_timer_interval());

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
    ASSERT_INT_EQ(0, vmrp_api_start("/tmp/test.mrp", NULL, NULL));
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
    ASSERT_INT_EQ(0, vmrp_api_start("/tmp/test.mrp", NULL, NULL));
    vmrp_api_timer();
    ASSERT_INT_EQ(1, timer_called);

    vmrp_api_destroy();
    TEST_END();
}

static void test_audio_api_forwards_when_running(void) {
    TEST_BEGIN("vmrp_api audio forwards while running");
    reset_counters();

    int16_t frames[8] = {0};
    vmrp_api_init(240, 320);
    ASSERT_INT_EQ(0, vmrp_api_start("/tmp/test.mrp", NULL, NULL));
    audio_active = 1;

    ASSERT_INT_EQ(44100, vmrp_api_audio_sample_rate());
    ASSERT_INT_EQ(2, vmrp_api_audio_channels());
    ASSERT_INT_EQ(1, vmrp_api_audio_is_active());
    ASSERT_INT_EQ(4, vmrp_api_audio_render_s16le(frames, 4));
    ASSERT_INT_EQ(1, audio_render_called);
    ASSERT_TRUE(frames[0] != 0);

    vmrp_api_audio_stop();
    ASSERT_INT_EQ(1, audio_stop_called);
    ASSERT_INT_EQ(0, audio_active);

    vmrp_api_destroy();
    TEST_END();
}

static void test_audio_render_silences_when_not_running(void) {
    TEST_BEGIN("vmrp_api audio render returns silence when stopped");
    reset_counters();

    int16_t frames[4];
    for (size_t i = 0; i < sizeof(frames) / sizeof(frames[0]); i++) {
        frames[i] = (int16_t)0x7fff;
    }

    vmrp_api_init(240, 320);
    ASSERT_INT_EQ(0, vmrp_api_audio_is_active());
    ASSERT_INT_EQ(0, vmrp_api_audio_render_s16le(frames, 2));
    ASSERT_INT_EQ(0, audio_render_called);
    for (size_t i = 0; i < sizeof(frames) / sizeof(frames[0]); i++) {
        ASSERT_INT_EQ(0, frames[i]);
    }

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

static void test_args_parse_env_work_dir(void) {
    TEST_BEGIN("vmrp_args_parse reads VMRP_WORK_DIR");
    char cwd[PATH_MAX];
    char expected[PATH_MAX + 32];
    char *argv[] = { "vmrp" };
    VmrpArgs args;

    ASSERT_PTR_NOT_NULL(getcwd(cwd, sizeof(cwd)));
    snprintf(expected, sizeof(expected), "%s/%s", cwd, "vmrp_env_work");
    test_setenv("VMRP_WORK_DIR", "vmrp_env_work");

    ASSERT_INT_EQ(MR_SUCCESS, vmrp_args_parse(1, argv, &args));
    ASSERT_STR_EQ(expected, args.work_dir);

    test_unsetenv("VMRP_WORK_DIR");
    TEST_END();
}

static void test_args_parse_cli_work_dir_overrides_env(void) {
    TEST_BEGIN("vmrp_args_parse --work-dir overrides VMRP_WORK_DIR");
    char *argv[] = { "vmrp", "--work-dir", "/tmp/vmrp_cli_work" };
    VmrpArgs args;

    test_setenv("VMRP_WORK_DIR", "/tmp/vmrp_env_work");

    ASSERT_INT_EQ(MR_SUCCESS, vmrp_args_parse(3, argv, &args));
    ASSERT_STR_EQ("/tmp/vmrp_cli_work", args.work_dir);

    test_unsetenv("VMRP_WORK_DIR");
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
    test_is_running_tracks_start_and_destroy();
    test_exit_stops_api_events_and_timers();
    test_start_null_path_fails();
    test_start_empty_path_fails();
    test_event_forwards_to_runtime();
    test_timer_forwards_to_runtime();
    test_audio_api_forwards_when_running();
    test_audio_render_silences_when_not_running();
    test_edit_inactive_by_default();
    test_set_edit_text_without_active_fails();
    test_cancel_edit_without_active_fails();
    test_args_parse_env_work_dir();
    test_args_parse_cli_work_dir_overrides_env();

    return harness_tests_failed - before;
}
