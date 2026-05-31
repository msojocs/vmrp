/*
 * test_got_snapshot.c — 验证 hook_got_write 的 GOT 快照保留逻辑
 *
 * 背景：netpay 等嵌套插件会覆写 wrapper GOT 中的 bridge 指针为自己的
 * 回调地址。dump0 restore 后需要从快照恢复这些指针。旧代码会在非 bridge
 * 值写入时清除快照、并让 got_snapshot_base 被其他 R9 抢占，导致恢复失败。
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define EXT_BASE_ADDR    0x00010000u
#define EXT_TABLE_ADDR   EXT_BASE_ADDR
#define EXT_TABLE_COUNT  150u

/* 模拟 hook_got_write 的核心逻辑（与 arm_ext_executor.c 保持一致） */
typedef struct {
    uint32_t got_snapshot_base;
    uint32_t got_snapshot[EXT_TABLE_COUNT];
} GotState;

static void got_write(GotState *s, uint32_t r9, uint32_t address,
                      uint32_t value) {
    uint32_t got_size = EXT_TABLE_COUNT * 4;
    if (!r9 || address < r9 || address >= r9 + got_size) return;
    uint32_t idx = (address - r9) / 4;
    if (idx >= EXT_TABLE_COUNT) return;
    if (value >= EXT_TABLE_ADDR &&
        value < EXT_TABLE_ADDR + EXT_TABLE_COUNT * 4) {
        if (!s->got_snapshot_base || s->got_snapshot_base == r9) {
            s->got_snapshot_base = r9;
        }
        if (s->got_snapshot_base == r9) {
            s->got_snapshot[idx] = value;
        }
    }
    /* 非 bridge 值写入时保留已有快照，不清除 */
}

/* GOT 修复逻辑（与 table[44] read 后的修复循环一致） */
static void got_fixup(GotState *s, uint8_t *mem, uint32_t mem_base,
                      uint32_t region_start, uint32_t region_end) {
    if (!s->got_snapshot_base) return;
    uint32_t got_base = s->got_snapshot_base;
    for (uint32_t i = 0; i < EXT_TABLE_COUNT; i++) {
        uint32_t addr = got_base + i * 4;
        if (s->got_snapshot[i] >= EXT_TABLE_ADDR &&
            s->got_snapshot[i] < EXT_TABLE_ADDR + EXT_TABLE_COUNT * 4 &&
            addr >= region_start && addr + 4 <= region_end) {
            memcpy(mem + (addr - mem_base), &s->got_snapshot[i], 4);
        }
    }
}

static int failures = 0;

#define ASSERT_EQ(desc, actual, expected) do { \
    if ((actual) != (expected)) { \
        printf("  FAIL: %s: got 0x%X, expected 0x%X\n", desc, \
               (unsigned)(actual), (unsigned)(expected)); \
        failures++; \
    } \
} while(0)

/* ---------- 测试用例 ---------- */

int test_got_bridge_write_records_snapshot(void) {
    printf("[TEST] bridge write records snapshot\n");
    GotState s = {0};
    uint32_t wrapper_r9 = 0x645C40;
    uint32_t bridge_38 = EXT_TABLE_ADDR + 38 * 4;  /* table[38] = 0x10098 */

    got_write(&s, wrapper_r9, wrapper_r9 + 38 * 4, bridge_38);

    ASSERT_EQ("base set to wrapper R9", s.got_snapshot_base, wrapper_r9);
    ASSERT_EQ("snapshot[38] recorded", s.got_snapshot[38], bridge_38);
    return 0;
}

int test_got_nonbridge_preserves_snapshot(void) {
    printf("[TEST] non-bridge write preserves existing snapshot\n");
    GotState s = {0};
    uint32_t wrapper_r9 = 0x645C40;
    uint32_t bridge_38 = EXT_TABLE_ADDR + 38 * 4;
    uint32_t netpay_callback = 0x67D240;  /* 游戏代码地址，非 bridge */

    /* 先写入 bridge 值 */
    got_write(&s, wrapper_r9, wrapper_r9 + 38 * 4, bridge_38);
    ASSERT_EQ("snapshot[38] after bridge write", s.got_snapshot[38], bridge_38);

    /* 再写入非 bridge 值（模拟 netpay 覆写） */
    got_write(&s, wrapper_r9, wrapper_r9 + 38 * 4, netpay_callback);
    ASSERT_EQ("snapshot[38] preserved after non-bridge write",
              s.got_snapshot[38], bridge_38);
    return 0;
}

int test_got_different_r9_no_hijack(void) {
    printf("[TEST] different R9 does not hijack base\n");
    GotState s = {0};
    uint32_t wrapper_r9 = 0x645C40;
    uint32_t netpay_r9  = 0x700000;
    uint32_t bridge_38 = EXT_TABLE_ADDR + 38 * 4;
    uint32_t bridge_40 = EXT_TABLE_ADDR + 40 * 4;

    /* wrapper 写入 bridge 值 → base 锁定为 wrapper_r9 */
    got_write(&s, wrapper_r9, wrapper_r9 + 38 * 4, bridge_38);
    ASSERT_EQ("base locked to wrapper", s.got_snapshot_base, wrapper_r9);

    /* netpay 用不同 R9 写入 bridge 值 → base 不应改变 */
    got_write(&s, netpay_r9, netpay_r9 + 40 * 4, bridge_40);
    ASSERT_EQ("base stays wrapper after netpay write",
              s.got_snapshot_base, wrapper_r9);
    ASSERT_EQ("snapshot[40] not updated by netpay",
              s.got_snapshot[40], 0);
    return 0;
}

int test_got_fixup_restores_after_memcpy(void) {
    printf("[TEST] GOT fixup restores bridge pointers after memcpy\n");
    /* 模拟 dump0 restore 场景：
     * 1. wrapper GOT 在 R9=0x100 处（简化地址）
     * 2. 快照记录了 GOT[38] = table[38] 的 bridge 地址
     * 3. memcpy 把整块内存清零（模拟 dump0 读取覆盖）
     * 4. fixup 应恢复 GOT[38] */
    uint32_t mem_base = 0x100;
    uint32_t mem_size = EXT_TABLE_COUNT * 4 + 64;
    uint8_t *mem = calloc(1, mem_size);

    GotState s = {0};
    s.got_snapshot_base = mem_base;
    s.got_snapshot[38] = EXT_TABLE_ADDR + 38 * 4;  /* 0x10098 */
    s.got_snapshot[40] = EXT_TABLE_ADDR + 40 * 4;  /* 0x100A0 */

    /* 先在 GOT 区域写入正确值 */
    uint32_t val38 = EXT_TABLE_ADDR + 38 * 4;
    uint32_t val40 = EXT_TABLE_ADDR + 40 * 4;
    memcpy(mem + 38 * 4, &val38, 4);
    memcpy(mem + 40 * 4, &val40, 4);

    /* 模拟 dump0 read 覆盖了 GOT 区域（清零） */
    memset(mem, 0, mem_size);

    /* 验证 GOT 被清零 */
    uint32_t check = 0;
    memcpy(&check, mem + 38 * 4, 4);
    ASSERT_EQ("GOT[38] zeroed after memcpy", check, 0);

    /* 执行 fixup */
    got_fixup(&s, mem, mem_base, mem_base, mem_base + mem_size);

    /* 验证 GOT 被恢复 */
    memcpy(&check, mem + 38 * 4, 4);
    ASSERT_EQ("GOT[38] restored after fixup", check, EXT_TABLE_ADDR + 38 * 4);
    memcpy(&check, mem + 40 * 4, 4);
    ASSERT_EQ("GOT[40] restored after fixup", check, EXT_TABLE_ADDR + 40 * 4);

    free(mem);
    return 0;
}

int test_got_snapshot_run_all(void) {
    int before = failures;
    test_got_bridge_write_records_snapshot();
    test_got_nonbridge_preserves_snapshot();
    test_got_different_r9_no_hijack();
    test_got_fixup_restores_after_memcpy();
    return failures - before;
}
