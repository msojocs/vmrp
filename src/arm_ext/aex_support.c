/*
 * aex_support.c —— ARM EXT 执行器纯工具单元(Phase 2 拆分第 1 步)。
 *
 * 收纳无模块状态依赖(或只读 ArmExtModule 标量)的通用帮助函数。
 * 函数体系从 arm_ext_executor.c 原样迁移,行为不变;跨单元可见性经
 * arm_ext_priv.h 声明。
 */
#include "../include/arm_ext_priv.h"

int arm_ext_bytes_contain(const uint8_t *haystack, uint32_t haystack_len,
                          const uint8_t *needle, uint32_t needle_len) {
    if (!haystack || !needle || !needle_len || needle_len > haystack_len)
        return 0;
    /* P5.2:先用 memchr 跳到下一个首字节候选再整段比对(此前逐字节推进,
     * ≥1KB payload 对整包 O(n·m) 扫描是注册路径卡顿点之一)。结果与朴素
     * 实现完全一致。 */
    uint32_t off = 0;
    uint32_t last = haystack_len - needle_len;
    while (off <= last) {
        const uint8_t *hit = (const uint8_t *)memchr(haystack + off,
                                                     needle[0],
                                                     (size_t)(last - off) + 1u);
        if (!hit) return 0;
        off = (uint32_t)(hit - haystack);
        if (memcmp(haystack + off, needle, needle_len) == 0)
            return 1;
        ++off;
    }
    return 0;
}

/* FNV-1a(32 位):pack 归属缓存的内容指纹 */
uint32_t arm_ext_fnv1a(const void *data, uint32_t len) {
    const uint8_t *p = (const uint8_t *)data;
    uint32_t h = 2166136261u;
    for (uint32_t i = 0; i < len; ++i) {
        h ^= p[i];
        h *= 16777619u;
    }
    return h;
}

/*
 * table[3](memcpy)/table[6](strncpy) 桥的重叠区间处理:真机 SDK 的
 * memcpy/strncpy 是朴素前向逐字节循环,应用会依赖前向搬移语义(sanitizer
 * 基线实测:多款应用以 dst<src 重叠 2 字节整理缓冲,talkcat 以 dst==src
 * 调 strncpy)。宿主 libc 对重叠区间是 UB(ASan *-param-overlap 命中,
 * 且 glibc 向量化实现不保证前向序)。重叠时改用与真机一致的前向逐字节
 * 循环,不重叠时仍走宿主快速路径,结果与真机语义完全一致。
 */
void arm_ext_guest_memcpy(void *dst, const void *src, size_t n) {
    uint8_t *d = (uint8_t *)dst;
    const uint8_t *s = (const uint8_t *)src;
    if (d < s + n && s < d + n) {
        for (size_t i = 0; i < n; i++) d[i] = s[i];
        return;
    }
    memcpy(d, s, n);
}

char *arm_ext_guest_strncpy(char *dst, const char *src, size_t n) {
    if (dst < src + n && src < dst + n) {
        size_t i = 0;
        for (; i < n && src[i]; i++) dst[i] = src[i];
        for (; i < n; i++) dst[i] = '\0';
        return dst;
    }
    return strncpy(dst, src, n);
}

int mrp_path_equal(const char *a, const char *b) {
    if (!a || !b) return 0;
    while (*a && *b) {
        char ca = *a == '\\' ? '/' : *a;
        char cb = *b == '\\' ? '/' : *b;
        if (ca != cb) return 0;
        ++a;
        ++b;
    }
    return *a == '\0' && *b == '\0';
}

void arm_ext_diag_preview_bytes(const void *data,
                                uint32_t len,
                                char *out,
                                size_t out_len) {
    if (!out || !out_len) return;
    out[0] = '\0';
    if (!data || !len) return;

    const uint8_t *p = (const uint8_t *)data;
    size_t pos = 0;
    uint32_t n = len < 48u ? len : 48u;
    for (uint32_t i = 0; i < n && pos + 1u < out_len; ++i) {
        unsigned char ch = p[i];
        if (ch >= 0x20u && ch < 0x7Fu && ch != '\\' && ch != '\'') {
            out[pos++] = (char)ch;
        } else if (pos + 4u < out_len) {
            int written = snprintf(out + pos, out_len - pos, "\\x%02X", ch);
            if (written <= 0) break;
            pos += (size_t)written;
        } else {
            break;
        }
    }
    out[pos] = '\0';
}

/*
 * 诊断开关进程级缓存:环境变量运行期不变,而 hook_table 等热路径每次进入
 * 都 getenv 是纯浪费。各开关语义见 docs/arm-ext-phase0-progress.md。
 */
static int arm_ext_env_flag_(const char *name, int *cache) {
    if (*cache < 0) *cache = getenv(name) ? 1 : 0;
    return *cache;
}
int arm_ext_diag_on(void) {
    static int c = -1;
    return arm_ext_env_flag_("SKYENGINE_ARM_EXT_DIAG", &c);
}
int arm_ext_trace_on(void) {
    static int c = -1;
    return arm_ext_env_flag_("SKYENGINE_ARM_EXT_TRACE", &c);
}
int arm_ext_trace_pc_on(void) {
    static int c = -1;
    return arm_ext_env_flag_("SKYENGINE_ARM_EXT_TRACE_PC", &c);
}
int arm_ext_timer_liveness_diag_on(void) {
    static int c = -1;
    return arm_ext_env_flag_("SKYENGINE_ARM_EXT_TIMER_LIVENESS_DIAG", &c);
}
int arm_ext_timer_owner_diag_on(void) {
    static int c = -1;
    return arm_ext_env_flag_("SKYENGINE_ARM_EXT_TIMER_OWNER_DIAG", &c);
}
int arm_ext_screen_diag_on(void) {
    static int c = -1;
    return arm_ext_env_flag_("SKYENGINE_ARM_EXT_SCREEN_DIAG", &c);
}
