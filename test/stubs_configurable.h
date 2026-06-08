/*
 * stubs_configurable.h — 在现有 stubs.c 基础上提供可注入的桩行为
 *
 * 设计原则：
 *   - 不修改 stubs.c 本体，通过全局覆写结构实现注入
 *   - 每个测试开始时调用 stubs_reset() 清零状态
 *   - 附带调用记录，用于验证被测代码的行为
 *
 * 用法：
 *   #include "stubs_configurable.h"
 *
 *   // 自定义一个总是失败的 open
 *   static int32_t my_open_fail(const char *f, uint32_t m) {
 *       (void)f; (void)m;
 *       return -1;
 *   }
 *
 *   void test_something(void) {
 *       stubs_reset();
 *       g_stub_overrides.file_open = my_open_fail;
 *       // ... 调用被测代码 ...
 *       assert(g_stub_overrides.record.file_open_count == 1);
 *   }
 */
#ifndef VMRP_STUBS_CONFIGURABLE_H
#define VMRP_STUBS_CONFIGURABLE_H

#include <stdint.h>
#include <string.h>

/* ── 覆写函数指针 ─────────────────────────────────────────── */
/* 若非 NULL，stubs.c 中的对应桩会调用这里的函数而非默认实现。
 * 函数签名与 DSM_REQUIRE_FUNCS 中的一致。 */

typedef struct {
    /* 文件 I/O 覆写 */
    int32_t (*file_open)(const char *filename, uint32_t mode);
    int32_t (*file_close)(int32_t f);
    int32_t (*file_read)(int32_t f, void *p, uint32_t l);
    int32_t (*file_write)(int32_t f, void *p, uint32_t l);
    int32_t (*file_seek)(int32_t f, int32_t pos, int method);
    int32_t (*file_info)(const char *filename);
    int32_t (*file_remove)(const char *filename);
    int32_t (*file_getLen)(const char *filename);

    /* 定时器覆写 */
    int32_t (*timerStart)(uint16_t t);
    int32_t (*timerStop)(void);
    uint32_t (*get_uptime_ms)(void);

    /* 网络覆写 */
    int32_t (*initNetwork)(void *cb, const char *mode, void *ud);
    int32_t (*getHostByName)(const char *name, void *cb, void *ud);

    /* 图形覆写 */
    void (*drawBitmap)(void *bmp, int16_t x, int16_t y, uint16_t w, uint16_t h);

    /* ── 调用记录 ─────────────────────────────────────────── */
    struct {
        /* 文件操作计数 */
        int file_open_count;
        int file_close_count;
        int file_read_count;
        int file_write_count;

        /* 最近一次 open 的参数 */
        char last_open_path[256];
        uint32_t last_open_mode;

        /* 定时器 */
        int timer_start_count;
        int timer_stop_count;
        uint16_t last_timer_interval;

        /* 图形 */
        int draw_bitmap_count;

        /* 网络 */
        int init_network_count;
        int get_host_count;
        char last_host_name[256];
    } record;
} stub_overrides_t;

/* 全局实例，在 stubs.c 中定义 */
extern stub_overrides_t g_stub_overrides;

/* 每个测试开始时调用，清零所有覆写和记录 */
static inline void stubs_reset(void) {
    memset(&g_stub_overrides, 0, sizeof(g_stub_overrides));
}

#endif /* VMRP_STUBS_CONFIGURABLE_H */
