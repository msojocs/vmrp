#include "./include/arm_ext_executor.h"
#include "./include/compat_msvc.h"
#include "./include/arm_ext_internal.h"
#include "./include/app_compat.h"
#include "./include/bridge.h"
#include "./include/network.h"
#include "./include/fileLib.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _MSC_VER
#include <unistd.h>
#include <sys/mman.h>
#else
#include <windows.h>
#endif
#include <zlib.h>

#include "./include/utils.h"
#include "./include/vmrp.h"

extern void mr_printf(const char *format, ...);
extern uint16 *mr_screenBuf;
extern int32 mr_screen_w;
extern int32 mr_screen_h;
extern int32 mr_rand(void);
extern void mr_free(void *p, uint32 len);
extern int32 mr_mem_get(char **mem_base, uint32 *mem_len);
extern int32 mr_timerStart(uint16 t);
extern int32 mr_timerStop(void);
extern int32 mr_state;
extern int32 mr_timer_state;
extern int32 mr_stop_ex(int16 freemem);
extern uint32 mr_getTime(void);
extern int32 mr_getDatetime(mr_datetime *datetime);
extern int32 mr_getUserInfo(void *info);
extern void mr_md5_init(void *pms);
extern void mr_md5_append(void *pms, const void *data, int nbytes);
extern void mr_md5_finish(void *pms, void *digest);
extern int32 mr_sleep(uint32 ms);
extern int32 mr_plat(int32 code, int32 param);
extern const char *mr_get_pack_filename(void);
extern const char *mr_get_start_filename(void);
extern const char *mr_get_old_pack_filename(void);
extern const char *mr_get_old_start_filename(void);
extern const char *mr_get_start_fileparameter(void);
typedef void (*MR_PLAT_EX_CB)(uint8 *output, int32 output_len);
extern int32 mr_platEx(int32 code, uint8 *input, int32 input_len,
                       uint8 **output, int32 *output_len, MR_PLAT_EX_CB *cb);
extern int32 mr_ferrno(void);
extern int32 mr_open(const char *filename, uint32 mode);
extern int32 mr_close(int32 f);
extern int32 mr_info(const char *filename);
extern int32 mr_write(int32 f, void *p, uint32 l);
extern int32 mr_read(int32 f, void *p, uint32 l);
extern int32 mr_seek(int32 f, int32 pos, int method);
extern int32 mr_getLen(const char *filename);
extern int32 mr_remove(const char *filename);
extern int32 mr_rename(const char *oldname, const char *newname);
extern int32 mr_mkDir(const char *name);
extern int32 mr_rmDir(const char *name);
extern int32 mr_findStart(const char *name, char *buffer, uint32 len);
extern int32 mr_findGetNext(int32 search_handle, char *buffer, uint32 len);
extern int32 mr_findStop(int32 search_handle);
extern int32 mr_exit(void);
extern int32 mr_startShake(int32 ms);
extern int32 mr_stopShake(void);
extern int32 mr_playSound(int type, const void *data, uint32 dataLen, int32 loop);
extern int32 mr_stopSound(int type);
extern void mr_call(char *number);
extern int32 mr_sendSms(char *pNumber, char *pContent, int32 encode);
extern int32 mr_dialogCreate(const char *title, const char *text, int32 type);
extern int32 mr_dialogRelease(int32 dialog);
extern int32 mr_dialogRefresh(int32 dialog, const char *title, const char *text, int32 type);
extern int32 mr_textCreate(const char *title, const char *text, int32 type);
extern int32 mr_textRelease(int32 text);
extern int32 mr_textRefresh(int32 handle, const char *title, const char *text);
extern int32 mr_editCreate(const char *title, const char *text, int32 type, int32 max_size);
extern int32 mr_editRelease(int32 edit);
extern const char *mr_editGetText(int32 edit);
extern void mr_drawBitmap(uint16 *bmp, int16 x, int16 y, uint16 w, uint16 h);
extern const char *mr_getCharBitmap(uint16 ch, uint16 fontSize, int *width, int *height);
extern int32 mr_getScreenInfo(void *s);
extern void mr_platDrawChar(uint16 ch, int32 x, int32 y, uint32 color);
extern int32 _DispUpEx(int16 x, int16 y, uint16 w, uint16 h);
extern uint16 *c2u(const char *cp, int *err, int *size);
extern int wstrlen(char *txt);

/* mrporting.h 中定义的 mr_info 返回值，此处直接使用数值避免引入额外头文件 */
#define MRP_IS_FILE 1
/* mr_sendSms flag requesting an asynchronous MR_SMS_RESULT event. */
#define ARM_EXT_MR_SMS_RESULT_FLAG 16

extern void _DrawPoint(int16 x, int16 y, uint16 nativecolor);
extern void _DrawBitmap(uint16 *p, int16 x, int16 y, uint16 w, uint16 h, uint16 rop, uint16 transcoler, int16 sx, int16 sy, int16 mw);
extern void DrawRect(int16 x, int16 y, int16 w, int16 h, uint8 r, uint8 g, uint8 b);
extern int32 _DrawText(char *pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode, uint16 font);
extern int _BitmapCheck(uint16 *p, int16 x, int16 y, uint16 w, uint16 h, uint16 transcoler, uint16 color_check);
extern void *_mr_readFile(const char *filename, int *filelen, int lookfor);
extern void *mr_readFile_from_ram(const char *filename, int *filelen, int lookfor, char *ram_file, int ram_file_len);
extern void *mr_readFile_from_pack(const char *pack_filename, const char *filename, int *filelen, int lookfor);
extern char *LG_mem_base;
extern char *LG_mem_end;
extern uint8 *mr_gzInBuf;
extern uint8 *mr_gzOutBuf;
extern unsigned LG_gzinptr;
extern unsigned LG_gzoutcnt;
extern int32 mr_registerAPP(uint8 *p, int32 len, int32 index);
extern int _mr_TestCom(void *L, int input0, int input1);
extern int _mr_TestCom1(void *L, int input0, char *input1, int32 len);
extern int32 mr_socket(int32 type, int32 protocol);
extern int32 mr_connect(int32 s, int32 ip, uint16 port, int32 type);
extern int32 mr_closeSocket(int32 s);
extern int32 mr_getSocketState(int32 s);
typedef int32 (*MR_INIT_NETWORK_CB_t)(int32 result);
extern int32 mr_initNetwork(MR_INIT_NETWORK_CB_t cb, const char *mode);
extern int32 mr_recv(int32 s, char *buf, int len);
extern int32 mr_recvfrom(int32 s, char *buf, int len, int32 *ip, uint16 *port);
extern int32 mr_send(int32 s, const char *buf, int len);
extern int32 mr_sendto(int32 s, const char *buf, int len, int32 ip, uint16 port);
extern int32 my_hasOpenSockets(void);
extern int32 my_openSocketCount(void);
extern const char *mr_get_last_written_mrp_path(void);
extern const uint8 *mr_get_last_written_mrp_data(uint32 *len);

static uint32_t arm_addr(ArmExtModule *m, const void *ptr) {
    if (ptr == NULL) return 0;
    const uint8_t *p = (const uint8_t *)ptr;
    if (m && m->mem && p >= m->mem && p < m->mem + EXT_MEM_SIZE)
        return (uint32_t)(p - m->mem) + EXT_BASE_ADDR;
    if (m && m->platform_mem && p >= m->platform_mem &&
        p < m->platform_mem + EXT_PLATFORM_MEM_SIZE)
        return (uint32_t)(p - m->platform_mem) + EXT_PLATFORM_MEM_ADDR;
    if (m && m->platform_io_mem && p >= m->platform_io_mem &&
        p < m->platform_io_mem + EXT_PLATFORM_IO_MEM_SIZE)
        return (uint32_t)(p - m->platform_io_mem) + EXT_PLATFORM_IO_MEM_ADDR;
    if (m && m->platform_alt_mem && p >= m->platform_alt_mem &&
        p < m->platform_alt_mem + EXT_PLATFORM_ALT_MEM_SIZE)
        return (uint32_t)(p - m->platform_alt_mem) + EXT_PLATFORM_ALT_MEM_ADDR;
    if (m && m->executor_meta_mem && p >= m->executor_meta_mem &&
        p < m->executor_meta_mem + EXT_EXECUTOR_META_SIZE)
        return (uint32_t)(p - m->executor_meta_mem) + EXT_EXECUTOR_META_ADDR;
    return 0;
}

static uint32_t align4(uint32_t v) { return (v + 3u) & ~3u; }

static uint32_t arm_ext_read_le32(const uint8_t *p) {
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

typedef int (*ArmExtMrpEntryVisitor)(const uint8_t *name, uint32_t name_len,
                                     const uint8_t *data, uint32_t data_len,
                                     void *ctx);

static int arm_ext_visit_mrp_payloads(const uint8_t *mrp, uint32_t mrp_len,
                                      ArmExtMrpEntryVisitor visitor,
                                      void *ctx) {
    uint32_t header_len;
    uint32_t pos;
    uint32_t count = 0;
    if (!mrp || mrp_len < 12u || !visitor) return 0;

    header_len = arm_ext_read_le32(mrp + 4u);
    if (header_len > 232u) {
        uint32_t index_start = arm_ext_read_le32(mrp + 12u);
        uint32_t index_end = header_len + 8u;
        if (index_start < 16u || index_start > index_end ||
            index_end > mrp_len)
            return 0;
        pos = index_start;
        while (pos + 16u <= index_end) {
            uint32_t name_len = arm_ext_read_le32(mrp + pos);
            pos += 4u;
            if (name_len == 0 || name_len >= 256u ||
                name_len > index_end - pos)
                return count > 0;

            const uint8_t *name = mrp + pos;
            pos += name_len;
            if (pos + 12u > index_end) return 0;

            uint32_t data_off = arm_ext_read_le32(mrp + pos);
            uint32_t data_len = arm_ext_read_le32(mrp + pos + 4u);
            pos += 12u;
            if (data_len == 0 || data_off > mrp_len ||
                data_len > mrp_len - data_off)
                return 0;

            /*
             * Newer disk MRP files store an index of file offsets, while RAM
             * packages often use the older sequential payload table below.
             * Visit payload bytes through both layouts so provenance remains
             * format-based instead of entry-name based.
             */
            if (!visitor(name, name_len, mrp + data_off, data_len, ctx))
                return 0;
            count++;
        }
        return count > 0;
    }

    if (header_len > mrp_len - 8u) return 0;
    pos = 8u + header_len;

    while (pos + 4u <= mrp_len) {
        uint32_t name_len = arm_ext_read_le32(mrp + pos);
        pos += 4u;
        if (name_len == 0 || name_len >= 256u || name_len > mrp_len - pos)
            return count > 0;

        const uint8_t *name = mrp + pos;
        pos += name_len;
        if (pos + 4u > mrp_len) return 0;

        uint32_t data_len = arm_ext_read_le32(mrp + pos);
        pos += 4u;
        if (data_len == 0 || data_len > mrp_len - pos) return 0;

        /*
         * MRP files carry a sequential payload table after the index.  Private
         * installers may wrap one of those payloads into a RAM package with a
         * new synthetic name, so ownership is proved by payload bytes rather
         * than by entry names.
         */
        if (!visitor(name, name_len, mrp + pos, data_len, ctx))
            return 0;
        pos += data_len;
        count++;
    }

    return count > 0;
}

static int arm_ext_gunzip_payload(const uint8_t *input, uint32_t input_len,
                                  uint8_t **output, uint32_t *output_len) {
    z_stream zs;
    uint32_t cap;
    int zret;
    if (output) *output = NULL;
    if (output_len) *output_len = 0;
    if (!input || !input_len || !output || !output_len) return 0;

    memset(&zs, 0, sizeof(zs));
    if (inflateInit2(&zs, 16 + MAX_WBITS) != Z_OK) return 0;

    cap = input_len * 4u + 1024u;
    if (cap < 4096u) cap = 4096u;
    *output = (uint8_t *)malloc(cap);
    if (!*output) {
        inflateEnd(&zs);
        return 0;
    }

    zs.next_in = (Bytef *)input;
    zs.avail_in = input_len;
    do {
        if (zs.total_out == cap) {
            uint32_t new_cap = cap * 2u;
            uint8_t *new_output;
            if (new_cap <= cap) {
                free(*output);
                *output = NULL;
                inflateEnd(&zs);
                return 0;
            }
            new_output = (uint8_t *)realloc(*output, new_cap);
            if (!new_output) {
                free(*output);
                *output = NULL;
                inflateEnd(&zs);
                return 0;
            }
            *output = new_output;
            cap = new_cap;
        }
        zs.next_out = *output + zs.total_out;
        zs.avail_out = cap - (uint32_t)zs.total_out;
        zret = inflate(&zs, Z_NO_FLUSH);
    } while (zret == Z_OK);

    if (zret != Z_STREAM_END) {
        free(*output);
        *output = NULL;
        inflateEnd(&zs);
        return 0;
    }

    *output_len = (uint32_t)zs.total_out;
    inflateEnd(&zs);
    return 1;
}

typedef struct ArmExtDecodedPayloadMatch {
    const uint8_t *needle;
    uint32_t needle_len;
    int found;
} ArmExtDecodedPayloadMatch;

static int arm_ext_payload_matches_staged_file(const uint8_t *payload,
                                               uint32_t payload_len,
                                               const uint8_t *needle,
                                               uint32_t needle_len) {
    if (!payload || !needle || !payload_len || !needle_len)
        return 0;
    if (payload_len == needle_len &&
        memcmp(payload, needle, needle_len) == 0)
        return 1;
    /*
     * Private loaders overwrite the first two words of an executable staging
     * buffer with module-record/P metadata.  The payload tail is the immutable
     * executable proof used elsewhere for nested module registration.
     */
    if (payload_len == needle_len && needle_len > 8u &&
        memcmp(payload + 8u, needle + 8u, needle_len - 8u) == 0)
        return 1;
    return 0;
}

static int arm_ext_payload_or_decoded_matches_staged(const uint8_t *payload,
                                                     uint32_t payload_len,
                                                     const uint8_t *staged,
                                                     uint32_t staged_len) {
    if (arm_ext_payload_matches_staged_file(payload, payload_len,
                                           staged, staged_len))
        return 1;

    if (payload_len >= 2u && payload[0] == 0x1f && payload[1] == 0x8b) {
        uint8_t *decoded = NULL;
        uint32_t decoded_len = 0;
        int found = 0;
        if (arm_ext_gunzip_payload(payload, payload_len,
                                   &decoded, &decoded_len)) {
            found = arm_ext_payload_matches_staged_file(decoded, decoded_len,
                                                        staged, staged_len);
        }
        free(decoded);
        return found;
    }
    return 0;
}

static int arm_ext_decoded_payload_match_cb(const uint8_t *name,
                                            uint32_t name_len,
                                            const uint8_t *data,
                                            uint32_t data_len,
                                            void *ctx) {
    (void)name;
    (void)name_len;
    ArmExtDecodedPayloadMatch *match = (ArmExtDecodedPayloadMatch *)ctx;
    if (!match || match->found) return 0;

    if (arm_ext_payload_or_decoded_matches_staged(data, data_len,
                                                  match->needle,
                                                  match->needle_len)) {
        match->found = 1;
        return 0;
    }

    return match->found ? 0 : 1;
}

static int arm_ext_host_package_matches_staged_file(ArmExtModule *m,
                                                    uint32_t file_addr,
                                                    uint32_t file_len) {
    uint32 dsm_mrp_len = 0;
    const uint8 *dsm_mrp = mr_get_last_written_mrp_data(&dsm_mrp_len);
    const uint8_t *staged;
    ArmExtDecodedPayloadMatch match;
    if (!m || !file_addr || !file_len || !dsm_mrp || !dsm_mrp_len)
        return 0;
    staged = (const uint8_t *)arm_ptr(m, file_addr);
    if (!staged) return 0;

    match.needle = staged;
    match.needle_len = file_len;
    match.found = 0;
    arm_ext_visit_mrp_payloads(dsm_mrp, dsm_mrp_len,
                               arm_ext_decoded_payload_match_cb, &match);
    return match.found;
}

typedef struct ArmExtRawPayloadMatch {
    const uint8_t *needle;
    uint32_t needle_len;
    int found;
} ArmExtRawPayloadMatch;

static int arm_ext_raw_payload_match_cb(const uint8_t *name,
                                        uint32_t name_len,
                                        const uint8_t *data,
                                        uint32_t data_len,
                                        void *ctx) {
    (void)name;
    (void)name_len;
    ArmExtRawPayloadMatch *match = (ArmExtRawPayloadMatch *)ctx;
    if (!match || match->found) return 0;
    if (data_len == match->needle_len &&
        memcmp(data, match->needle, data_len) == 0) {
        match->found = 1;
        return 0;
    }
    return 1;
}

static int arm_ext_bytes_contain(const uint8_t *haystack, uint32_t haystack_len,
                                 const uint8_t *needle, uint32_t needle_len) {
    if (!haystack || !needle || !needle_len || needle_len > haystack_len)
        return 0;
    for (uint32_t off = 0; off + needle_len <= haystack_len; ++off) {
        if (memcmp(haystack + off, needle, needle_len) == 0)
            return 1;
    }
    return 0;
}

static int arm_ext_host_package_contains_raw_payload(const uint8_t *payload,
                                                     uint32_t payload_len) {
    uint32 dsm_mrp_len = 0;
    const uint8 *dsm_mrp = mr_get_last_written_mrp_data(&dsm_mrp_len);
    ArmExtRawPayloadMatch match;
    if (!payload || !payload_len || !dsm_mrp || !dsm_mrp_len)
        return 0;
    match.needle = payload;
    match.needle_len = payload_len;
    match.found = 0;
    arm_ext_visit_mrp_payloads(dsm_mrp, dsm_mrp_len,
                               arm_ext_raw_payload_match_cb, &match);
    if (match.found)
        return 1;
    return payload_len >= 1024u &&
           arm_ext_bytes_contain(dsm_mrp, dsm_mrp_len,
                                 payload, payload_len);
}

typedef struct ArmExtRamPayloadHostMatch {
    const uint8_t *staged;
    uint32_t staged_len;
    int found;
} ArmExtRamPayloadHostMatch;

static int arm_ext_ram_payload_host_match_cb(const uint8_t *name,
                                             uint32_t name_len,
                                             const uint8_t *data,
                                             uint32_t data_len,
                                             void *ctx) {
    (void)name;
    (void)name_len;
    ArmExtRamPayloadHostMatch *match = (ArmExtRamPayloadHostMatch *)ctx;
    if (!match || match->found) return 0;

    /*
     * A RAM-package payload proves a host owner only when that same encoded
     * payload is present in the installed MRP and it decodes to the staged
     * child executable.  Shared non-executable assets cannot satisfy both
     * conditions.
     */
    if (arm_ext_host_package_contains_raw_payload(data, data_len) &&
        arm_ext_payload_or_decoded_matches_staged(data, data_len,
                                                  match->staged,
                                                  match->staged_len)) {
        match->found = 1;
        return 0;
    }
    return 1;
}

static int arm_ext_host_package_matches_ram_staged_file(ArmExtModule *m,
                                                       uint32_t ram_addr,
                                                       uint32_t ram_len,
                                                       uint32_t file_addr,
                                                       uint32_t file_len) {
    const uint8_t *ram;
    const uint8_t *staged;
    ArmExtRamPayloadHostMatch match;
    if (!m || !ram_addr || !ram_len || !file_addr || !file_len)
        return 0;
    ram = (const uint8_t *)arm_ptr(m, ram_addr);
    staged = (const uint8_t *)arm_ptr(m, file_addr);
    if (!ram || !staged) return 0;

    match.staged = staged;
    match.staged_len = file_len;
    match.found = 0;
    arm_ext_visit_mrp_payloads(ram, ram_len,
                               arm_ext_ram_payload_host_match_cb, &match);
    return match.found;
}

static int arm_ext_is_host_absolute_path(const char *path) {
    if (!path || !*path) return 0;
    if (path[0] == '/') return 1;
#ifdef _WIN32
    if (path[0] == '\\') return 1;
    if (((path[0] >= 'A' && path[0] <= 'Z') ||
         (path[0] >= 'a' && path[0] <= 'z')) &&
        path[1] == ':' && (path[2] == '/' || path[2] == '\\'))
        return 1;
#endif
    return 0;
}

static const char *arm_ext_basename(const char *path) {
    const char *base = path;
    const char *slash;
    if (!path) return "";
    slash = strrchr(path, '/');
    if (slash) base = slash + 1;
    slash = strrchr(base, '\\');
    if (slash) base = slash + 1;
    return base;
}

static void arm_ext_init_pack_names(ArmExtModule *m) {
    const char *host_path;
    const char *alias;
    char cwd[PATH_MAX];
    if (!m) return;
    host_path = mr_get_pack_filename();
    alias = host_path;
#ifndef _WIN32
    if (arm_ext_is_host_absolute_path(host_path) && getcwd(cwd, sizeof(cwd))) {
        size_t cwd_len = strlen(cwd);
        if (cwd_len && strncmp(host_path, cwd, cwd_len) == 0 &&
            host_path[cwd_len] == '/') {
            /*
             * EXT code treats table[100] as both a display/copyable package
             * name and an openable package path.  Prefer the shortest
             * host-openable spelling under cwd: it keeps old ARM helpers'
             * fixed stack buffers intact without reducing the root package to
             * an unopenable basename.
             */
            alias = host_path + cwd_len + 1u;
        }
    } else
#endif
    if (arm_ext_is_host_absolute_path(host_path)) {
        alias = arm_ext_basename(host_path);
    }
    snprintf(m->pack_host_path, sizeof(m->pack_host_path), "%s",
             host_path ? host_path : "");
    snprintf(m->pack_alias, sizeof(m->pack_alias), "%s",
             (alias && *alias) ? alias : m->pack_host_path);
}

static const char *arm_ext_pack_to_host_path(ArmExtModule *m, const char *pack) {
    if (!m || !pack || !*pack) return pack;
    if (m->pack_alias[0] && m->pack_host_path[0] &&
        strcmp(pack, m->pack_alias) == 0)
        return m->pack_host_path;
    if (!arm_ext_is_host_absolute_path(pack) && m->pack_host_path[0]) {
        size_t host_len = strlen(m->pack_host_path);
        size_t pack_len = strlen(pack);
        /*
         * Some EXT helpers need table[100] shortened to a device-style suffix
         * such as mythroad/foo.mrp because their disassembly proves a fixed
         * 32-byte package buffer.  That suffix is still the same package for
         * host I/O, so resolve it back to the absolute path when it matches on
         * a path component boundary.
         */
        if (pack_len <= host_len) {
            const char *suffix = m->pack_host_path + host_len - pack_len;
            if (strcmp(suffix, pack) == 0 &&
                (suffix == m->pack_host_path ||
                 suffix[-1] == '/' || suffix[-1] == '\\')) {
                return m->pack_host_path;
            }
        }
    }
    for (uint32_t i = 0; i < m->short_pack_alias_count; ++i) {
        if (m->short_pack_aliases[i].alias[0] &&
            strcmp(pack, m->short_pack_aliases[i].alias) == 0) {
            return m->short_pack_aliases[i].host_path;
        }
    }
    return pack;
}

static const char *arm_ext_register_short_pack_alias(ArmExtModule *m,
                                                     const char *host_path) {
    if (!m || !host_path || !host_path[0]) return "";
    for (uint32_t i = 0; i < m->short_pack_alias_count; ++i) {
        if (strcmp(m->short_pack_aliases[i].host_path, host_path) == 0) {
            return m->short_pack_aliases[i].alias;
        }
    }
    if (m->short_pack_alias_count >= m->short_pack_alias_capacity) {
        uint32_t next = m->short_pack_alias_capacity
                      ? m->short_pack_alias_capacity * 2u
                      : 8u;
        ArmExtShortPackAlias *grown = (ArmExtShortPackAlias *)realloc(
            m->short_pack_aliases, (size_t)next * sizeof(*grown));
        if (!grown) return "";
        memset(grown + m->short_pack_alias_capacity, 0,
               (size_t)(next - m->short_pack_alias_capacity) *
               sizeof(*grown));
        m->short_pack_aliases = grown;
        m->short_pack_alias_capacity = next;
    }

    ArmExtShortPackAlias *slot =
        &m->short_pack_aliases[m->short_pack_alias_count];
    int n = snprintf(slot->alias, sizeof(slot->alias), "~p%u",
                     m->short_pack_alias_count);
    if (n <= 0 || (size_t)n >= sizeof(slot->alias)) {
        slot->alias[0] = '\0';
        return "";
    }
    snprintf(slot->host_path, sizeof(slot->host_path), "%s", host_path);
    m->short_pack_alias_count++;
    return slot->alias;
}

static const char *arm_ext_short_pack_alias_for_host_path(ArmExtModule *m,
                                                          const char *host_path) {
    if (!host_path || !host_path[0]) return "";
    if (m && m->pack_alias[0] && m->pack_host_path[0] &&
        strcmp(host_path, m->pack_host_path) == 0) {
        /*
         * The root alias is normally a cwd-relative package path.  If it is
         * still too long for a child whose disassembly proves a 32-byte pack
         * copy, fall through to the bounded alias registry below.
         */
        if (strlen(m->pack_alias) < ARM_EXT_SHORT_PACK_ALIAS_MAX)
            return m->pack_alias;
    }
    if (!arm_ext_is_host_absolute_path(host_path) &&
        strlen(host_path) < ARM_EXT_SHORT_PACK_ALIAS_MAX)
        return host_path;
    /*
     * The DSM layer treats mythroad/... as a root path, so this keeps the
     * ARM-visible pack name short while remaining directly openable by the
     * native readFile bridge.
     */
    const char *root = strstr(host_path, "mythroad/");
    if (root && root[0] && strlen(root) < ARM_EXT_SHORT_PACK_ALIAS_MAX)
        return root;
    const char *name = arm_ext_basename(host_path);
    if (name && name[0] && strlen(name) < ARM_EXT_SHORT_PACK_ALIAS_MAX)
        return name;
    /*
     * The hazardous child code shape copies the package name into a fixed
     * 32-byte ARM buffer.  When no host-openable spelling fits that ABI, give
     * ARM a tiny synthetic package name and keep host I/O resolution in this
     * executor's alias table.  This preserves the root-cause invariant instead
     * of hoping a long basename happens not to overflow.
     */
    return arm_ext_register_short_pack_alias(m, host_path);
}

static const char *arm_ext_current_pack_table_name(ArmExtModule *m) {
    uint32_t packp = 0;
    if (!m) return "";
    if (!arm_ptr(m, EXT_TABLE_ADDR + 100u * 4u)) return "";
    memcpy(&packp, arm_ptr(m, EXT_TABLE_ADDR + 100u * 4u), 4);
    if (!packp) return "";
    return (const char *)arm_ptr(m, packp);
}

/* 临时诊断前置声明(定义见 hook_got_write 上方,修复后删除) */
static int arm_ext_watch_write_range(uint32_t *lo, uint32_t *hi);
static int arm_ext_watch_alloc_range(uint32_t *lo, uint32_t *hi);
static void arm_ext_watch_alloc_report(ArmExtModule *m, const char *tag,
                                       uint32_t addr, uint32_t len);
static uint32_t arm_ext_watch_hash32(const void *data, uint32_t len);
static void arm_ext_watch_hex16(const void *data, uint32_t len,
                                char *out, size_t out_size);
static void arm_ext_watch_sentinel_check(ArmExtModule *m, uint32_t idx,
                                         uint32_t r0, uint32_t r1,
                                         uint32_t r2, uint32_t r3);
static void arm_ext_watch_module_event(ArmExtModule *m, const char *tag,
                                       uint32_t file_addr, uint32_t file_len,
                                       uint32_t p_addr);
static ArmExtNestedModule *arm_ext_find_nested_module(ArmExtModule *m, uint32_t addr);
static void arm_ext_protect_registered_module_storage(ArmExtModule *m,
                                                      uint32_t file_addr,
                                                      uint32_t file_len);

static uint32_t arm_alloc(ArmExtModule *m, uint32_t len) {
    len = align4(len ? len : 1);
    /* bump 堆从 EXT_HEAP_ADDR(0x200000) 向上生长,必须跳过
     * EXT 栈区 [EXT_STACK_ADDR, EXT_CODE_ADDR) 和 wrapper 代码区
     * [EXT_CODE_ADDR, +code_len):此前只检查 16MB 总界,长流程
     * (talkcat 喝水资源包下载安装)累计分配越过 0xE00000 后,
     * SCRRAM/大缓冲直接落在 wrapper 栈里,整屏位图把返回地址踩成
     * 像素值(PC=0x6246834A 一类 FETCH_UNMAPPED)。跳过保留区后
     * [代码区末, 16MB) 仍可继续分配。 */
    uint32_t reserved_lo = EXT_STACK_ADDR;
    uint32_t reserved_hi = EXT_CODE_ADDR + align4(m->code_len);
    if (m->heap_top < reserved_hi && m->heap_top + len > reserved_lo) {
        m->heap_top = reserved_hi;
    }
    /* 屏幕缓冲迁移到顶部保留区后(hook_screen_dim_write),该区对 bump 堆
     * 关闭,否则宿主绘制会覆写堆上分配。未迁移时保持完整 16MB 可用,
     * 与旧版本布局/容量完全一致(gghjt 等长流程依赖)。 */
    uint32_t mem_end = EXT_BASE_ADDR + EXT_MEM_SIZE;
    if (m->screen_addr == mem_end - EXT_SCREEN_RESERVE) {
        mem_end -= EXT_SCREEN_RESERVE;
    }
    if (m->heap_top + len >= mem_end) return 0;
    uint32_t ret = m->heap_top;
    m->heap_top += len;
    return ret;
}

/* guest 改写 ARM 可见 mr_screen_w/h 变量(table[92/93] 指向的 u32)时触发。
 * 部分 C SDK 游戏(gtcm 的 SphinxJoy 引擎)在 init 用这两个变量配置比物理
 * 屏更大的逻辑画布(横屏 480x320):真机上 DSM framebuffer 与配置一致,而
 * 模拟器的 EXT 屏幕缓冲按物理分辨率分配在 bump 堆首,若不处理,宿主
 * DrawRect/DrawBitmap 按新尺寸写入旧缓冲会越界抹掉紧随其后的模块镜像
 * (gtcm 黑屏根因)。超出容量时把缓冲迁移到 16MB 空间顶部的专属保留区:
 * 迁移发生在写变量的瞬间,早于 graphics.ext 缓存 framebuffer 指针,guest
 * 后续读 table[91] 拿到的即是新地址;bump 堆布局保持与旧版本一致,不影响
 * 依赖既有堆地址的应用(gghjt netpay dump0 恢复等)。 */
static void hook_screen_dim_write(uc_engine *uc, uc_mem_type type,
                                  uint64_t address, int size, int64_t value,
                                  void *user_data) {
    (void)uc;
    (void)type;
    ArmExtModule *m = (ArmExtModule *)user_data;
    if (!m || size <= 0) return;

    /* 仅当 ARM 可见 mr_screenBuf(table[91])仍指向宿主屏幕缓冲时才迁移:
     * gtcm 在 init 配置 480x320 逻辑画布时 buf 未动,需要迁移扩容;gghjt
     * 滚屏离屏合成先把 table[91] 指到自有缓冲再写更宽的 w(272),此时
     * 迁移宿主缓冲反而使恢复后的 buf 与 screen_addr 失配导致花屏。 */
    {
        uint32_t slot91_ptr = arm_ext_read_u32_or_zero_(
            m, EXT_TABLE_ADDR + 91u * 4u);
        uint32_t cur_buf = arm_ext_read_u32_or_zero_(m, slot91_ptr);
        if (cur_buf != m->screen_addr) return;
    }

    uint32_t addr = (uint32_t)address;
    /* 写入尚未落盘:被写维度取 value,另一维读当前内存 */
    uint32_t w = (addr == m->screen_w_slot)
                     ? (uint32_t)value
                     : arm_ext_read_u32_or_zero_(m, m->screen_w_slot);
    uint32_t h = (addr == m->screen_h_slot)
                     ? (uint32_t)value
                     : arm_ext_read_u32_or_zero_(m, m->screen_h_slot);
    if (w == 0 || h == 0 || w > 2048 || h > 2048) return;

    uint32_t need = w * h * 2u;
    uint32_t top_addr = EXT_BASE_ADDR + EXT_MEM_SIZE - EXT_SCREEN_RESERVE;
    if (need <= m->screen_cap || m->screen_addr == top_addr ||
        need > EXT_SCREEN_RESERVE) {
        return;
    }

    uint8_t *dst = (uint8_t *)arm_ptr(m, top_addr);
    const uint8_t *src = (const uint8_t *)arm_ptr(m, m->screen_addr);
    if (!dst || !src) return;
    memset(dst, 0, need);
    /* 旧内容按行搬运(stride 变化),保留已绘制画面 */
    uint32_t copy_w = ((uint32_t)m->screen_w < w ? (uint32_t)m->screen_w : w) * 2u;
    uint32_t copy_h = (uint32_t)m->screen_h < h ? (uint32_t)m->screen_h : h;
    for (uint32_t yy = 0; yy < copy_h; ++yy) {
        memcpy(dst + yy * w * 2u, src + yy * (uint32_t)m->screen_w * 2u, copy_w);
    }
    /* 同步 ARM 可见 mr_screenBuf 变量(slot91 指向的 u32) */
    uint32_t slot91 = arm_ext_read_u32_or_zero_(m, EXT_TABLE_ADDR + 91u * 4u);
    if (slot91 && arm_ptr(m, slot91)) {
        memcpy(arm_ptr(m, slot91), &top_addr, 4);
    }
    /* 同步 table[95] 屏幕位图描述项(第 30 项: w,h,len,type,addr) */
    uint32_t bmp_array = arm_ext_read_u32_or_zero_(m, EXT_TABLE_ADDR + 95u * 4u);
    if (bmp_array && arm_ptr(m, bmp_array + 30u * 16u + 15u)) {
        uint8_t *bm = (uint8_t *)arm_ptr(m, bmp_array + 30u * 16u);
        uint16_t w16 = (uint16_t)w;
        uint16_t h16 = (uint16_t)h;
        memcpy(bm + 0, &w16, 2);
        memcpy(bm + 2, &h16, 2);
        memcpy(bm + 4, &need, 4);
        memcpy(bm + 12, &top_addr, 4);
    }
    m->screen_addr = top_addr;
    m->screen_cap = EXT_SCREEN_RESERVE;
    /* screen_w/h/len 不在此处更新:绘制入口(arm_ext_push_draw_screen_context)
     * 读取 guest 变量后在容量内统一采纳,保证与后续尺寸调整走同一路径 */
}

/* guest 读回 ARM 可见 mr_screen_w/h 时触发。gtcm 进下载/付费提示前把逻辑
 * 画布写成宿主画布的转置(按 LCD 原生竖屏 320x480,真机上伴随撤销
 * plat(101) 旋转)。展示层没有旋转能力、窗口方向固定;若放任转置尺寸被
 * 读回,提示 UI 会按竖屏 320 宽布局,横屏窗口右侧 160px 永远不被绘制。
 * 写钩子在写指令落盘前触发无法当场改写,改为在 guest 读回前把两个变量
 * 恢复成宿主尺寸,guest 按窗口方向(480x320)重新布局。仅当两值恰为宿主
 * 尺寸的转置时生效,不影响 gghjt 滚屏离屏合成等改写更宽 stride 的场景。 */
static void hook_screen_dim_read(uc_engine *uc, uc_mem_type type,
                                 uint64_t address, int size, int64_t value,
                                 void *user_data) {
    (void)uc;
    (void)type;
    (void)address;
    (void)size;
    (void)value;
    ArmExtModule *m = (ArmExtModule *)user_data;
    if (!m || m->screen_w == m->screen_h) return;

    uint32_t w = arm_ext_read_u32_or_zero_(m, m->screen_w_slot);
    uint32_t h = arm_ext_read_u32_or_zero_(m, m->screen_h_slot);
    if (w != (uint32_t)m->screen_h || h != (uint32_t)m->screen_w) return;

    /* 仅主屏幕缓冲:table[91] 指向自有离屏缓冲时不干预 */
    uint32_t slot91 = arm_ext_read_u32_or_zero_(m, EXT_TABLE_ADDR + 91u * 4u);
    uint32_t cur_buf = arm_ext_read_u32_or_zero_(m, slot91);
    if (cur_buf != m->screen_addr) return;

    uint32_t hw = (uint32_t)m->screen_w;
    uint32_t hh = (uint32_t)m->screen_h;
    memcpy(arm_ptr(m, m->screen_w_slot), &hw, 4);
    memcpy(arm_ptr(m, m->screen_h_slot), &hh, 4);
}

static uint32_t arm_ext_meta_alloc(ArmExtModule *m, uint32_t len) {
    if (!m) return 0;
    len = align4(len ? len : 1);
    if (len > EXT_EXECUTOR_META_SIZE ||
        m->executor_meta_top > EXT_EXECUTOR_META_SIZE - len) {
        return 0;
    }
    uint32_t ret = EXT_EXECUTOR_META_ADDR + m->executor_meta_top;
    m->executor_meta_top += len;
    return ret;
}

static uint32_t reg_read32(uc_engine *uc, int reg) {
    uint32_t v = 0;
    uc_reg_read(uc, reg, &v);
    return v;
}

static void arm_ext_set_pack_table_name(ArmExtModule *m, const char *name);
static void arm_ext_set_child_record_pack_name(ArmExtModule *m,
                                               uint32_t file_addr,
                                               const char *name);
static int arm_ext_current_pack_matches_staged_file(ArmExtModule *m,
                                                    uint32_t file_addr,
                                                    uint32_t file_len,
                                                    const char **pack_out);
static int arm_ext_finish_callback_state(ArmExtModule *m, uint32_t ext_chunk);
static void arm_ext_dispatch_pending_sms_result(ArmExtModule *m);
static void arm_ext_clear_foreground_screen_owner(ArmExtModule *m);
static int arm_ext_has_suspended_foreground_child(ArmExtModule *m,
                                                  uint32_t primary_ext_chunk);
static int arm_ext_has_foreground_cover(ArmExtModule *m);
static void arm_ext_finish_accepted_screen_write(ArmExtModule *m,
                                                 uint32_t claim_p_addr,
                                                 uint32_t claim_helper_addr);
static void arm_ext_note_screen_presented(ArmExtModule *m);
static int arm_ext_screen_owner_is_visible(ArmExtModule *m,
                                           uint32_t owner_p_addr,
                                           uint32_t owner_helper_addr);
static void arm_ext_claim_foreground_screen_rect(ArmExtModule *m,
                                                 uint32_t owner_p_addr,
                                                 uint32_t owner_helper_addr,
                                                 int32_t x,
                                                 int32_t y,
                                                 int32_t w,
                                                 int32_t h);
static void arm_ext_diag_dump_rw_timer_state(ArmExtModule *m,
                                             const char *tag,
                                             uint32_t rw_base);
static void arm_ext_diag_dump_wrapper_timer_state(ArmExtModule *m,
                                                  const char *tag);
static void capture_timer_dispatches(ArmExtModule *m);
static int run_arm_with_sp(ArmExtModule *m, uint32_t start, uint32_t sp);
static ArmExtNestedModule *arm_ext_resource_owner_for_lr(ArmExtModule *m,
                                                         uint32_t *owner_p,
                                                         uint32_t *owner_h);
static void sync_internal_state_to_arm(ArmExtModule *m);
static void sync_timer_state_from_arm(ArmExtModule *m);
static void enter_screen_context(ArmExtModule *m,
                                 uint16 **saved_screenBuf,
                                 uint32_t *saved_present_depth);
static void leave_screen_context(ArmExtModule *m,
                                 uint16 *saved_screenBuf,
                                 uint32_t saved_present_depth);

static int32_t arm_ext_screen_stride(ArmExtModule *m) {
    if (!m) return 0;
    return m->screen_w;
}

static inline void app_on_child_confirmed(ArmExtModule *m, uint32_t p, uint32_t h) {
    if (m && m->profile && m->profile->on_child_confirmed)
        m->profile->on_child_confirmed(m, m->app_state, p, h);
}
static inline int app_should_protect_got_addr(ArmExtModule *m, uint32_t addr) {
    if (m && m->profile && m->profile->should_protect_got_addr)
        return m->profile->should_protect_got_addr(m, m->app_state, addr);
    return 0;
}


static void reg_write32(uc_engine *uc, int reg, uint32_t v) {
    uc_reg_write(uc, reg, &v);
}

static void set_arm_mode_for_addr(ArmExtModule *m, uint32_t addr) {
    uint32_t cpsr = reg_read32(m->uc, UC_ARM_REG_CPSR);
    if (addr & 1u) {
        cpsr |= (1u << 5);
    } else {
        cpsr &= ~(1u << 5);
    }
    reg_write32(m->uc, UC_ARM_REG_CPSR, cpsr);
}

static uint32_t arm_exec_addr(uint32_t addr) {
    // Keep bit 0 so Unicorn enters Thumb mode for Thumb entry points.
    return addr;
}

static int arm_ext_pending_internal_contains(ArmExtModule *m, uint32_t addr) {
    if (!m || !m->pending_internal_file_addr || !m->pending_internal_file_len)
        return 0;
    uint32_t start = m->pending_internal_file_addr;
    uint32_t len = m->pending_internal_file_len;
    if (len > UINT32_MAX - start) return 0;
    return addr >= start && addr < start + len;
}

static int arm_ext_nested_module_overlaps_pending(ArmExtModule *m,
                                                  const ArmExtNestedModule *mod) {
    if (!m || !mod || !mod->file_addr || !mod->file_len ||
        !m->pending_internal_file_addr || !m->pending_internal_file_len) {
        return 0;
    }
    uint32_t a0 = mod->file_addr;
    uint32_t b0 = m->pending_internal_file_addr;
    uint32_t a1 = a0 + mod->file_len;
    uint32_t b1 = b0 + m->pending_internal_file_len;
    if (a1 < a0 || b1 < b0) return 0;
    return a0 < b1 && b0 < a1;
}

static int arm_ext_nested_exec_range_for_lr(ArmExtModule *m, uint32_t lr,
                                            uint32_t *file_addr,
                                            uint32_t *file_len) {
    uint32_t pc = lr & ~1u;
    if (file_addr) *file_addr = 0;
    if (file_len) *file_len = 0;
    if (!m) return 0;

    /*
     * last_file_addr is the latest read/staged file buffer.  Private loaders
     * briefly have two copies of the same bytes: the raw _mr_readFile buffer
     * and the runtime image passed to mr_cacheSync.  LR ownership must follow
     * the runtime image, otherwise later P writes and nested registration can
     * bind callbacks to the raw buffer that the loader never executed.
     */
    if (arm_ext_pending_internal_contains(m, pc)) {
        if (file_addr) *file_addr = m->pending_internal_file_addr;
        if (file_len) *file_len = m->pending_internal_file_len;
        return 1;
    }

    if (m->last_file_addr && m->last_file_len &&
        pc >= m->last_file_addr &&
        pc < m->last_file_addr + m->last_file_len) {
        if (file_addr) *file_addr = m->last_file_addr;
        if (file_len) *file_len = m->last_file_len;
        return 1;
    }

    return 0;
}

static ArmExtNestedModule *arm_ext_find_nested_module(ArmExtModule *m, uint32_t addr) {
    if (!m) return NULL;
    for (int i = m->nested_module_count - 1; i >= 0; --i) {
        ArmExtNestedModule *mod = &m->nested_modules[i];
        /*
         * Private loaders reuse low-heap executable storage before the new
         * child has filled extChunk[8] and can be registered.  During that
         * window the old module record may still overlap the fresh staging
         * range.  Do not use that stale record for PC->P/R9 ownership: the
         * new child startup code must run with the R9 value it installs.
         */
        if (arm_ext_pending_internal_contains(m, addr) &&
            arm_ext_nested_module_overlaps_pending(m, mod)) {
            continue;
        }
        if (mod->file_addr && mod->file_len &&
            addr >= mod->file_addr && addr < mod->file_addr + mod->file_len) {
            return mod;
        }
    }
    return NULL;
}

static ArmExtNestedModule *arm_ext_find_nested_module_by_p(ArmExtModule *m,
                                                           uint32_t p_addr) {
    if (!m || !p_addr) return NULL;
    for (int i = m->nested_module_count - 1; i >= 0; --i) {
        ArmExtNestedModule *mod = &m->nested_modules[i];
        if (mod->p_addr == p_addr) return mod;
    }
    return NULL;
}

static ArmExtNestedModule *arm_ext_find_nested_module_by_rw(ArmExtModule *m,
                                                            uint32_t rw_base) {
    if (!m || !rw_base) return NULL;
    /*
     * Private loaders may reuse a just-freed RW block for a new foreground
     * child. Prefer the active/current owner before falling back to the first
     * module with a matching P[0], otherwise diagnostics and bridge checks can
     * describe the stale module that used to own the same address.
     */
    ArmExtNestedModule *mod =
        arm_ext_find_nested_module_by_p(m, m->current_p_addr);
    if (mod && arm_ext_read_u32_or_zero_(m, mod->p_addr) == rw_base)
        return mod;
    mod = arm_ext_find_nested_module_by_p(m, m->active_p_addr);
    if (mod && arm_ext_read_u32_or_zero_(m, mod->p_addr) == rw_base)
        return mod;
    for (int i = m->nested_module_count - 1; i >= 0; --i) {
        mod = &m->nested_modules[i];
        if (arm_ext_read_u32_or_zero_(m, mod->p_addr) == rw_base)
            return mod;
    }
    return NULL;
}

static const char *arm_ext_staged_file_package(ArmExtModule *m,
                                               uint32_t file_addr,
                                               uint32_t file_len) {
    const uint8_t *staged;
    if (!m || !file_addr || !file_len) return "";
    if (m->last_file_addr == file_addr && m->last_file_len == file_len)
        return m->last_file_pack_host_path;
    if (!m->last_file_copy || m->last_file_len != file_len || file_len <= 8u)
        return "";
    staged = (const uint8_t *)arm_ptr(m, file_addr);
    if (!staged) return "";
    /*
     * table[131] cacheSync can stage a decoded child image at a wrapper-owned
     * runtime address before the internal loader registers it.  That loader
     * overwrites file_base[0]/[4] with module record/P metadata, so validate
     * the immutable payload tail instead of handing the last-read package to an
     * unrelated module.
     */
    if (memcmp(staged + 8u, m->last_file_copy + 8u, file_len - 8u) == 0)
        return m->last_file_pack_host_path;
    return "";
}

static int arm_ext_staged_file_ram_package(ArmExtModule *m,
                                           uint32_t file_addr,
                                           uint32_t file_len,
                                           uint32_t *ram_addr,
                                           uint32_t *ram_len) {
    const uint8_t *staged;
    const uint8_t *ram;
    if (ram_addr) *ram_addr = 0;
    if (ram_len) *ram_len = 0;
    if (!m || !file_addr || !file_len ||
        !m->last_file_pack_ram_addr || !m->last_file_pack_ram_len)
        return 0;
    if (m->last_file_addr == file_addr && m->last_file_len == file_len) {
        if (ram_addr) *ram_addr = m->last_file_pack_ram_addr;
        if (ram_len) *ram_len = m->last_file_pack_ram_len;
        return 1;
    }
    if (!m->last_file_copy || file_len <= 8u)
        return 0;
    staged = (const uint8_t *)arm_ptr(m, file_addr);
    if (!staged) return 0;
    /*
     * Use the same immutable-tail proof as host package propagation.  The
     * package context is a runtime RAM package pointer, so copying it without
     * byte validation would make later blank table[100] reads depend on stale
     * last-file state.
     */
    if (m->last_file_len == file_len &&
        memcmp(staged + 8u, m->last_file_copy + 8u, file_len - 8u) != 0)
        return 0;
    if (m->last_file_len != file_len) {
        if (m->last_file_len < file_len ||
            !m->last_file_pack_ram_addr ||
            !m->last_file_pack_ram_len ||
            m->last_file_pack_ram_len < file_len)
            return 0;
        ram = (const uint8_t *)arm_ptr(m, m->last_file_pack_ram_addr);
        if (!ram) return 0;
        /*
         * Some private loaders decode an executable child out of a RAM-backed
         * MRP and stage only that executable payload.  The last read is then the
         * container entry, not the exact staged bytes; still require the staged
         * child tail to occur verbatim in the actual RAM package before carrying
         * the RAM package owner forward.
         */
        const uint8_t *needle = staged + 8u;
        uint32_t needle_len = file_len - 8u;
        int found = 0;
        for (uint32_t off = 0; off + needle_len <= m->last_file_pack_ram_len;
             ++off) {
            if (memcmp(ram + off, needle, needle_len) == 0) {
                found = 1;
                break;
            }
        }
        if (!found)
            return 0;
    }
    if (ram_addr) *ram_addr = m->last_file_pack_ram_addr;
    if (ram_len) *ram_len = m->last_file_pack_ram_len;
    return 1;
}

static int arm_ext_current_pack_matches_staged_file(ArmExtModule *m,
                                                    uint32_t file_addr,
                                                    uint32_t file_len,
                                                    const char **pack_out) {
    const char *pack = arm_ext_current_pack_table_name(m);
    const char *host_pack = arm_ext_pack_to_host_path(m, pack);
    int32_t fl = 0;
    const uint8_t *staged;
    uint8_t *pkg;
    ArmExtDecodedPayloadMatch match;
    if (pack_out) *pack_out = "";
    if (!m || !file_addr || !file_len || !pack || !pack[0] ||
        !host_pack || !host_pack[0])
        return 0;
    staged = (const uint8_t *)arm_ptr(m, file_addr);
    if (!staged) return 0;
    fl = mr_getLen(host_pack);
    if (fl <= 0) return 0;
    pkg = (uint8_t *)malloc((size_t)fl);
    if (!pkg) return 0;
    int32_t fd = mr_open(host_pack, MR_FILE_RDONLY);
    if (fd == 0) {
        free(pkg);
        return 0;
    }
    int32_t got = mr_read(fd, pkg, (uint32_t)fl);
    mr_close(fd);
    if (got != fl) {
        free(pkg);
        return 0;
    }

    /*
     * Private loaders can switch table[100] to a freshly installed package
     * before staging a child EXT from it.  Prove that the current package owns
     * the staged child by reading the package entry bytes, rather than trusting
     * a path name or the last network artifact.
     */
    match.needle = staged;
    match.needle_len = file_len;
    match.found = 0;
    arm_ext_visit_mrp_payloads(pkg, (uint32_t)fl,
                               arm_ext_decoded_payload_match_cb, &match);
    free(pkg);
    if (!match.found) return 0;
    if (pack_out) *pack_out = host_pack;
    return 1;
}

static const char *arm_ext_child_package_context(ArmExtModule *m,
                                                 uint32_t file_addr,
                                                 uint32_t file_len,
                                                 uint32_t *ram_addr,
                                                 uint32_t *ram_len) {
    const char *package_host_path =
        arm_ext_staged_file_package(m, file_addr, file_len);
    const char *current_pack_path = "";
    uint32_t package_ram_addr = 0;
    uint32_t package_ram_len = 0;
    arm_ext_staged_file_ram_package(m, file_addr, file_len,
                                    &package_ram_addr, &package_ram_len);
    if (!package_host_path[0] &&
        arm_ext_current_pack_matches_staged_file(m, file_addr, file_len,
                                                 &current_pack_path)) {
        /*
         * Download/install wrappers can set table[100] to the installed MRP
         * before their private loader registers the child.  Carry that short
         * package owner forward only after the current package byte-proves
         * ownership of this staged child image.  The owner field is host I/O
         * provenance; if table[100] is a synthetic short alias, store its
         * resolved host package path rather than the ARM token.
         */
        package_host_path = current_pack_path;
    }
    const char *last_mrp_host_path = mr_get_last_written_mrp_path();
    int host_match = 0;
    if (!package_host_path[0] && package_ram_addr && package_ram_len &&
        last_mrp_host_path && last_mrp_host_path[0]) {
        /*
         * The child image is already tied to the RAM package above.  Installed
         * disk MRPs can gzip payload entries directly, or can share the RAM
         * package's encoded executable payload.  In both cases the proof must
         * resolve to the staged child bytes, not just a shared asset.
         */
        host_match =
            arm_ext_host_package_matches_staged_file(m, file_addr, file_len) ||
            arm_ext_host_package_matches_ram_staged_file(
                m, package_ram_addr, package_ram_len, file_addr, file_len);
    }
    if (!package_host_path[0] && package_ram_addr && package_ram_len &&
        last_mrp_host_path && last_mrp_host_path[0] && host_match) {
        /*
         * Download wrappers often install a full MRP on disk, then expose only
         * a RAM-backed child package containing executable entries.  When the
         * staged executable is byte-proven to be a decoded payload in the
         * installed MRP, the child owner is the host package; sibling assets
         * later arrive with table[100] cleared and must resolve through this
         * committed owner.
         */
        package_host_path = last_mrp_host_path;
        package_ram_addr = 0;
        package_ram_len = 0;
    }
    if (ram_addr) *ram_addr = package_host_path[0] ? 0 : package_ram_addr;
    if (ram_len) *ram_len = package_host_path[0] ? 0 : package_ram_len;
    return package_host_path;
}

static void arm_ext_record_nested_module(ArmExtModule *m, uint32_t file_addr,
                                         uint32_t file_len, uint32_t p_addr,
                                         uint32_t helper_addr) {
    if (!m || !file_addr || !file_len || !p_addr || !helper_addr) return;
    uint32_t package_ram_addr = 0;
    uint32_t package_ram_len = 0;
    const char *package_host_path = arm_ext_child_package_context(
        m, file_addr, file_len, &package_ram_addr, &package_ram_len);

    for (int i = 0; i < m->nested_module_count; ++i) {
        ArmExtNestedModule *mod = &m->nested_modules[i];
        if (mod->file_addr == file_addr && mod->file_len == file_len) {
            mod->p_addr = p_addr;
            mod->helper_addr = helper_addr;
            if (package_host_path[0]) {
                snprintf(mod->package_host_path, sizeof(mod->package_host_path),
                         "%s", package_host_path);
                mod->package_ram_addr = 0;
                mod->package_ram_len = 0;
            }
            if (!package_host_path[0] && package_ram_addr && package_ram_len) {
                mod->package_host_path[0] = '\0';
                mod->package_ram_addr = package_ram_addr;
                mod->package_ram_len = package_ram_len;
            }
            arm_ext_protect_registered_module_storage(m, file_addr, file_len);
            return;
        }
    }

    if (m->nested_module_count >= ARM_EXT_NESTED_MODULE_MAX) {
        if (getenv("VMRP_ARM_EXT_TRACE")) {
            printf("arm_ext_executor: nested module registry full, file=0x%X len=%u\n",
                   file_addr, file_len);
        }
        return;
    }

    ArmExtNestedModule *slot = &m->nested_modules[m->nested_module_count++];
    /* 临时诊断:记录模块注册(修复后删除) */
    arm_ext_watch_module_event(m, "register", file_addr, file_len, p_addr);
    slot->file_addr = file_addr;
    slot->file_len = file_len;
    slot->p_addr = p_addr;
    slot->helper_addr = helper_addr;
    snprintf(slot->package_host_path, sizeof(slot->package_host_path), "%s",
             package_host_path);
    slot->package_ram_addr = package_host_path[0] ? 0 : package_ram_addr;
    slot->package_ram_len = package_host_path[0] ? 0 : package_ram_len;
    slot->got_memcpy_off = 0;
    slot->pack_name_addr = 0;
    arm_ext_protect_registered_module_storage(m, file_addr, file_len);
}

/*
 * 通用 private-loader child GOT bridge 修复。
 *
 * 背景：少数 wrapper（如 gghjt 的 cfunction.ext）用私有 loader 加载子模块，不走标准
 * table[25] (_mr_c_function_new) 路径。私有 loader 会分配并清零一个 module record——
 * 它是 EXT_TABLE 的逐槽镜像（record[off] 对应 EXT_TABLE[off/4]）——但不填 bridge 源槽，
 * 子模块 entry 随后把空白源槽拷进自己的 RW，导致通过 GOT 调用标准函数时跳到空指针。
 * 因为这些 bridge 值从未被 ARM 写出，通用的 hook_got_write 快照机制捕获不到。
 *
 * 这里的修复完全数据驱动，无任何 app 指纹/文件名/魔数索引：
 *  (a) 扫描 record，凡 master EXT_TABLE[idx] 为自指针 bridge（== EXT_TABLE_ADDR+idx*4）
 *      且当前 record 槽为空白(0) 的，填回该 bridge 值；
 *  (b) 子模块 RW 段里 bridge 槽的目标偏移属于该子模块自身的数据段布局，无法从 EXT_TABLE
 *      推导，故只对已有反汇编和运行时锚点证明的布局做声明式镜像。
 * record 源槽填值可通用应用；RW 镜像不能把其它子模块的实验性偏移当作全局规则，否则会
 * 覆盖 private child 自己的状态字段（gghjt 的 verdload 下载路径即暴露了这一点）。
 */
#define PRIVATE_CHILD_RECORD_SPAN 0x248u

typedef struct {
    uint32_t record_src_off; /* run 起始 record 偏移 */
    uint32_t rw_dst_off;     /* run 起始 RW 镜像偏移 */
    uint32_t count;          /* 4 字节槽数量          */
} PrivateChildRwRun;

typedef struct {
    const char *note;
    const PrivateChildRwRun *runs;
    uint32_t run_count;
} PrivateChildRwLayout;

/* 私有 loader bridge-copy 基准布局：idx26→rw+0x16C（mr_printf）、
 * idx3..19→rw+0x170..0x1B0（memcpy..strtoul）。源 record 偏移 =
 * idx*4（record 是 EXT_TABLE 镜像）。真实 RW 偏移仍由子模块自身的
 * Thumb bridge-copy 反汇编结构确认并平移，不能只凭内存区间重叠套用。 */
static const PrivateChildRwRun std_bridge_copy_rw_runs[] = {
    { 0x68u, 0x16Cu, 1u  },
    { 0x0Cu, 0x170u, 17u },
};
static const PrivateChildRwLayout private_child_rw_layouts[] = {
    { "std-bridge-copy", std_bridge_copy_rw_runs,
      sizeof(std_bridge_copy_rw_runs) / sizeof(std_bridge_copy_rw_runs[0]) },
};

static int arm_ext_private_child_rw_layout_is_safe(const PrivateChildRwLayout *lay,
                                                   int32_t shift) {
    if (!lay || !lay->run_count) return 0;
    uint32_t min_dst = UINT32_MAX;
    uint32_t max_dst = 0;
    for (uint32_t r = 0; r < lay->run_count; ++r) {
        const PrivateChildRwRun *run = &lay->runs[r];
        if (!run->count) return 0;
        int32_t dst_signed = (int32_t)run->rw_dst_off + shift;
        if (dst_signed < 0) return 0;
        uint32_t dst = (uint32_t)dst_signed;
        uint32_t end = dst + run->count * 4u;
        if (end < dst) return 0;
        if (dst < min_dst) min_dst = dst;
        if (end > max_dst) max_dst = end;
    }

    /*
     * These mirrored values are callable bridge pointers, not generic state.
     * Private loaders can reuse RW blocks between children; a shifted pattern
     * that lands in the low control/state window would seed the next child with
     * nonzero state such as downloader flags.  Accept only layouts that live
     * past that low mutable area.
     */
    return min_dst >= 0x80u && max_dst > min_dst;
}

static uint16_t arm_ext_code_u16(const uint8_t *code, uint32_t off) {
    return (uint16_t)code[off] | (uint16_t)((uint16_t)code[off + 1u] << 8);
}

static uint32_t arm_ext_code_u32(const uint8_t *code, uint32_t off) {
    return (uint32_t)code[off] |
           ((uint32_t)code[off + 1u] << 8) |
           ((uint32_t)code[off + 2u] << 16) |
           ((uint32_t)code[off + 3u] << 24);
}

static void arm_ext_diag_preview_bytes(const void *data,
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

static void arm_ext_diag_fd_set(ArmExtModule *m, int32_t handle,
                                const char *name) {
    if (!m || handle <= 0 || !name) return;
    ArmExtDiagFd *free_slot = NULL;
    for (uint32_t i = 0; i < ARM_EXT_DIAG_FD_MAX; ++i) {
        if (m->diag_fds[i].handle == handle) {
            snprintf(m->diag_fds[i].name, sizeof(m->diag_fds[i].name),
                     "%s", name);
            return;
        }
        if (!free_slot && m->diag_fds[i].handle == 0)
            free_slot = &m->diag_fds[i];
    }
    if (!free_slot)
        free_slot = &m->diag_fds[(uint32_t)handle % ARM_EXT_DIAG_FD_MAX];
    free_slot->handle = handle;
    snprintf(free_slot->name, sizeof(free_slot->name), "%s", name);
}

static const char *arm_ext_diag_fd_name(ArmExtModule *m, int32_t handle) {
    if (!m || handle <= 0) return "";
    for (uint32_t i = 0; i < ARM_EXT_DIAG_FD_MAX; ++i) {
        if (m->diag_fds[i].handle == handle)
            return m->diag_fds[i].name;
    }
    return "";
}

static void arm_ext_diag_fd_clear(ArmExtModule *m, int32_t handle) {
    if (!m || handle <= 0) return;
    for (uint32_t i = 0; i < ARM_EXT_DIAG_FD_MAX; ++i) {
        if (m->diag_fds[i].handle == handle) {
            m->diag_fds[i].handle = 0;
            m->diag_fds[i].name[0] = '\0';
            return;
        }
    }
}

static void arm_ext_diag_owner_for_lr(ArmExtModule *m,
                                      uint32_t *owner_p,
                                      uint32_t *owner_h,
                                      uint32_t *owner_file,
                                      uint32_t *owner_len) {
    if (owner_p) *owner_p = 0;
    if (owner_h) *owner_h = 0;
    if (owner_file) *owner_file = 0;
    if (owner_len) *owner_len = 0;
    ArmExtNestedModule *owner =
        arm_ext_resource_owner_for_lr(m, owner_p, owner_h);
    if (owner_file) *owner_file = owner ? owner->file_addr : 0;
    if (owner_len) *owner_len = owner ? owner->file_len : 0;
}

static int arm_ext_thumb_ldr_literal_u32(const uint8_t *code,
                                         uint32_t len,
                                         uint32_t insn_off,
                                         uint32_t *value_out) {
    if (!code || insn_off + 2u > len) return 0;
    uint16_t insn = arm_ext_code_u16(code, insn_off);
    if ((insn & 0xF800u) != 0x4800u) return 0;

    uint32_t lit_off = ((insn_off + 4u) & ~3u) +
                       ((uint32_t)(insn & 0x00FFu) * 4u);
    if (lit_off + 4u > len) return 0;
    if (value_out) *value_out = arm_ext_code_u32(code, lit_off);
    return 1;
}

static int arm_ext_thumb_ldr_word_imm(uint16_t insn, uint32_t record_src_off,
                                      uint32_t *rt_out) {
    if ((insn & 0xF800u) != 0x6800u) return 0;
    uint32_t imm = ((insn >> 6) & 0x1Fu) * 4u;
    uint32_t rn = (insn >> 3) & 0x7u;
    if (rn != 0 || imm != record_src_off) return 0;
    if (rt_out) *rt_out = insn & 0x7u;
    return 1;
}

static int arm_ext_thumb_str_word_imm(uint16_t insn, uint32_t rt,
                                      uint32_t *base_reg_out,
                                      uint32_t *dst_imm_out) {
    if ((insn & 0xF800u) != 0x6000u) return 0;
    if ((insn & 0x7u) != rt) return 0;
    if (base_reg_out) *base_reg_out = (insn >> 3) & 0x7u;
    if (dst_imm_out) *dst_imm_out = ((insn >> 6) & 0x1Fu) * 4u;
    return 1;
}

static int arm_ext_thumb_word_ref_has_imm(uint16_t insn, uint32_t imm) {
    if ((insn & 0xF800u) != 0x6000u && (insn & 0xF800u) != 0x6800u)
        return 0;
    return ((uint32_t)((insn >> 6) & 0x1Fu) * 4u) == imm;
}

static int arm_ext_thumb_bl_pair(uint16_t first, uint16_t second) {
    return (first & 0xF800u) == 0xF000u &&
           (second & 0xD000u) == 0xD000u;
}

static int arm_ext_thumb_bridge_base_offset(const uint8_t *code,
                                            uint32_t file_len,
                                            uint32_t before_off,
                                            uint32_t base_reg,
                                            uint32_t *base_off_out) {
    uint32_t scan_start = before_off > 96u ? before_off - 96u : 0u;
    for (uint32_t ldr_off = before_off; ldr_off-- > scan_start;) {
        if (ldr_off & 1u) continue;
        uint16_t insn = arm_ext_code_u16(code, ldr_off);
        if ((insn & 0xF800u) != 0x4800u ||
            ((insn >> 8) & 0x7u) != base_reg)
            continue;

        uint32_t lit_off = ((ldr_off + 4u) & ~3u) + ((insn & 0xFFu) * 4u);
        if (lit_off + 4u > file_len) continue;
        int64_t base = (int64_t)arm_ext_code_u32(code, lit_off);
        int saw_r9_add = 0;
        for (uint32_t off = ldr_off + 2u; off + 2u <= before_off; off += 2u) {
            uint16_t op = arm_ext_code_u16(code, off);
            if (op == (uint16_t)(0x4448u | base_reg)) {
                saw_r9_add = 1;
            } else if ((op & 0xFF00u) == (uint16_t)(0x3800u | (base_reg << 8))) {
                base -= (int64_t)(op & 0xFFu);
            } else if ((op & 0xFF00u) == (uint16_t)(0x3000u | (base_reg << 8))) {
                base += (int64_t)(op & 0xFFu);
            }
        }
        if (!saw_r9_add || base < 0 || base > 0x4000) continue;
        *base_off_out = (uint32_t)base;
        return 1;
    }
    return 0;
}

static int arm_ext_thumb_bridge_run_matches(const uint8_t *code,
                                            uint32_t file_len,
                                            uint32_t start_off,
                                            uint32_t base_reg,
                                            uint32_t base_off,
                                            const PrivateChildRwRun *run,
                                            int32_t shift) {
    if (!run || !run->count) return 0;
    if (start_off + run->count * 4u > file_len) return 0;
    for (uint32_t i = 0; i < run->count; ++i) {
        uint32_t rt = 0, store_base = 0, store_imm = 0;
        if (!arm_ext_thumb_ldr_word_imm(
                arm_ext_code_u16(code, start_off + i * 4u),
                run->record_src_off + i * 4u, &rt))
            return 0;
        if (!arm_ext_thumb_str_word_imm(
                arm_ext_code_u16(code, start_off + i * 4u + 2u),
                rt, &store_base, &store_imm))
            return 0;
        if (store_base != base_reg) return 0;
        uint32_t expected = (uint32_t)((int32_t)run->rw_dst_off + shift) + i * 4u;
        if (base_off + store_imm != expected) return 0;
    }
    return 1;
}

static int arm_ext_discover_private_child_rw_shift(ArmExtModule *m,
                                                   uint32_t file_addr,
                                                   uint32_t file_len,
                                                   const PrivateChildRwLayout *lay,
                                                   int32_t *shift_out) {
    if (!m || !file_addr || !file_len || !lay || !shift_out) return 0;
    const uint8_t *code = (const uint8_t *)arm_ptr(m, file_addr);
    if (!code || file_len < 16u) return 0;

    const PrivateChildRwRun *anchor = NULL;
    for (uint32_t r = 0; r < lay->run_count; ++r) {
        if (lay->runs[r].record_src_off == 0x0Cu && lay->runs[r].count >= 2u) {
            anchor = &lay->runs[r];
            break;
        }
    }
    if (!anchor) return 0;

    for (uint32_t off = 0; off + anchor->count * 4u <= file_len; off += 2u) {
        uint32_t rt = 0, base_reg = 0, store_imm = 0, base_off = 0;
        if (!arm_ext_thumb_ldr_word_imm(arm_ext_code_u16(code, off),
                                        anchor->record_src_off, &rt))
            continue;
        if (!arm_ext_thumb_str_word_imm(arm_ext_code_u16(code, off + 2u),
                                        rt, &base_reg, &store_imm))
            continue;
        if (!arm_ext_thumb_bridge_base_offset(code, file_len, off,
                                              base_reg, &base_off))
            continue;

        int32_t shift = (int32_t)(base_off + store_imm) -
                        (int32_t)anchor->rw_dst_off;
        int ok = 1;
        for (uint32_t r = 0; r < lay->run_count && ok; ++r) {
            const PrivateChildRwRun *run = &lay->runs[r];
            int found = 0;
            uint32_t span_start = off > 24u ? off - 24u : 0u;
            uint32_t span_end = off + anchor->count * 4u + 24u;
            if (span_end > file_len) span_end = file_len;
            for (uint32_t roff = span_start;
                 roff + run->count * 4u <= span_end;
                 roff += 2u) {
                if (arm_ext_thumb_bridge_run_matches(code, file_len, roff,
                                                      base_reg, base_off,
                                                      run, shift)) {
                    found = 1;
                    break;
                }
            }
            if (!found) ok = 0;
        }
        if (ok) {
            *shift_out = shift;
            return 1;
        }
    }
    return 0;
}

static int arm_ext_private_child_record_src_is_unsafe(
    ArmExtModule *m,
    uint32_t file_addr,
    uint32_t file_len,
    uint32_t src_off) {
    if (!m || !file_addr || !file_len) return 0;

    for (size_t L = 0; L < sizeof(private_child_rw_layouts) /
                           sizeof(private_child_rw_layouts[0]); ++L) {
        const PrivateChildRwLayout *lay = &private_child_rw_layouts[L];
        int32_t shift = 0;
        if (!arm_ext_discover_private_child_rw_shift(m, file_addr, file_len,
                                                     lay, &shift))
            continue;
        if (arm_ext_private_child_rw_layout_is_safe(lay, shift))
            continue;
        for (uint32_t r = 0; r < lay->run_count; ++r) {
            const PrivateChildRwRun *run = &lay->runs[r];
            uint32_t run_start = run->record_src_off;
            uint32_t run_end = run_start + run->count * 4u;
            if (run_end >= run_start && src_off >= run_start &&
                src_off < run_end) {
                return 1;
            }
        }
    }
    return 0;
}

static void arm_ext_repair_private_child_record_bridges(ArmExtModule *m,
                                                        uint32_t record,
                                                        uint32_t file_addr,
                                                        uint32_t file_len) {
    if (!m || !record) return;

    /*
     * A private loader builds a per-child module record that mirrors the low
     * EXT table.  Some loaders leave bridge slots blank and expect the child
     * startup code to copy them into its RW area.  Fill only blank slots whose
     * master table entry is the standard self-pointer bridge, before the child
     * has a chance to snapshot the record into RW.  If the current child proves
     * that those source slots land in low mutable RW state instead of a bridge
     * table, leave them blank; the safe high-table mirror below still repairs
     * confirmed bridge tables after the child has a real RW base.
     */
    for (uint32_t off = 0; off + 4u <= PRIVATE_CHILD_RECORD_SPAN; off += 4u) {
        uint32_t idx = off / 4u;
        if (idx >= EXT_TABLE_COUNT) break;
        if (!arm_ptr(m, record + off)) break;
        uint32_t ext = arm_ext_read_u32_or_zero_(m, EXT_TABLE_ADDR + idx * 4u);
        if (ext != EXT_TABLE_ADDR + idx * 4u) continue;
        if (arm_ext_read_u32_or_zero_(m, record + off) != 0) continue;
        if (arm_ext_private_child_record_src_is_unsafe(m, file_addr, file_len,
                                                       off))
            continue;
        memcpy(arm_ptr(m, record + off), &ext, 4);
    }
}

static int arm_ext_private_child_has_safe_rw_bridge_layout(ArmExtModule *m,
                                                           uint32_t file_addr,
                                                           uint32_t file_len) {
    if (!m || !file_addr || !file_len) return 0;

    for (size_t L = 0; L < sizeof(private_child_rw_layouts) /
                           sizeof(private_child_rw_layouts[0]); ++L) {
        const PrivateChildRwLayout *lay = &private_child_rw_layouts[L];
        int32_t shift = 0;
        if (arm_ext_discover_private_child_rw_shift(m, file_addr, file_len,
                                                     lay, &shift) &&
            arm_ext_private_child_rw_layout_is_safe(lay, shift)) {
            return 1;
        }
    }
    return 0;
}

static void arm_ext_scrub_private_child_low_bridge_state(ArmExtModule *m,
                                                         uint32_t file_addr,
                                                         uint32_t file_len,
                                                         uint32_t rw_base) {
    if (!m || !rw_base ||
        !arm_ext_private_child_has_safe_rw_bridge_layout(m, file_addr,
                                                         file_len)) {
        return;
    }
    if (!arm_ptr(m, rw_base + 0x38u) || !arm_ptr(m, rw_base + 0x7Fu))
        return;

    /*
     * Private loaders can recycle an RW block from a previous child whose
     * startup copied standard EXT bridge self-pointers into a low mutable
     * state window.  For the new child, a proven high bridge-copy layout means
     * those low self-pointers are stale state, not callable slots.  Clear only
     * canonical self-pointers so real counters, flags and owner pointers stay
     * intact.
     */
    for (uint32_t off = 0x38u; off < 0x80u; off += 4u) {
        uint32_t v = arm_ext_read_u32_or_zero_(m, rw_base + off);
        if (v < EXT_TABLE_ADDR || v >= EXT_TABLE_ADDR + EXT_TABLE_COUNT * 4u)
            continue;
        uint32_t idx = (v - EXT_TABLE_ADDR) / 4u;
        if (v != EXT_TABLE_ADDR + idx * 4u) continue;
        uint32_t zero = 0;
        memcpy(arm_ptr(m, rw_base + off), &zero, 4);
    }
}

static int arm_ext_should_skip_got_snapshot_restore(ArmExtModule *m,
                                                    uint32_t addr) {
    if (!m || !m->got_snapshot_base || addr < m->got_snapshot_base)
        return 0;

    uint32_t off = addr - m->got_snapshot_base;
    if (off < 0x38u || off >= 0x80u) return 0;

    ArmExtNestedModule *owner =
        arm_ext_find_nested_module_by_rw(m, m->got_snapshot_base);
    if (!owner) return 0;
    if (!arm_ext_private_child_has_safe_rw_bridge_layout(m, owner->file_addr,
                                                         owner->file_len)) {
        return 0;
    }

    /*
     * A global GOT snapshot is keyed by R9, but private loaders can recycle
     * the same RW base for a later child.  When the current child proves a
     * high bridge-copy layout, the low 0x38..0x7f window is mutable state, not
     * a callable bridge table.  Do not restore stale self-pointers there after
     * the child intentionally clears the window with memset.
     */
    if (getenv("VMRP_ARM_EXT_DIAG")) {
        static uint32_t skip_diag_count = 0;
        if (skip_diag_count < 64u) {
            printf("DIAG got_restore_skip addr=0x%X off=0x%X rw=0x%X ownerFile=0x%X/%u ownerP=0x%X ownerH=0x%X\n",
                   addr, off, m->got_snapshot_base, owner->file_addr,
                   owner->file_len, owner->p_addr, owner->helper_addr);
        } else if (skip_diag_count == 64u) {
            printf("DIAG got_restore_skip truncated after 64 entries\n");
        }
        skip_diag_count++;
    }
    return 1;
}

static int arm_ext_child_has_compact_r9_state_list(const uint8_t *code,
                                                   uint32_t file_len) {
    if (!code || file_len < 256u) return 0;
    for (uint32_t off = 0; off + 768u <= file_len; off += 2u) {
        if (arm_ext_code_u16(code, off) != 0xB5F0u) continue;

        uint32_t r9_setups = 0;
        uint32_t span80 = off + 80u;
        for (uint32_t p = off + 2u; p + 2u <= span80; p += 2u) {
            if ((arm_ext_code_u16(code, p) & 0xFFF8u) == 0x4448u)
                r9_setups++;
        }
        if (r9_setups < 2u) continue;

        int has_node_alloc = 0;
        int has_node_clear = 0;
        int has_r9_table_call = 0;
        uint32_t node48_refs = 0;
        uint32_t window_end = off + 768u;
        for (uint32_t p = off + 2u; p + 8u <= window_end; p += 2u) {
            /*
             * verdload.ext 0x2CBCFC is a compact private-child parser.  It
             * allocates fixed 0x4c-byte nodes (0x2CBDC4), zeroes them through
             * an R9-relative table callback (0x2CBDD2), then links nodes by
             * their +0x48 next field (0x2CBF4E..0x2CBF84).  With a long host
             * pack_filename, the same R9-relative state window held
             * "c:/mythroad/" bytes at rw+0x9c and later fed brwmain's list
             * insert at 0x2CF5CA.  Treat this ABI shape like the explicit
             * 32-byte copy sites without naming a package or payload length.
             */
            if (arm_ext_code_u16(code, p) == 0x204Cu)
                has_node_alloc = 1;
            if (arm_ext_code_u16(code, p) == 0x224Cu)
                has_node_clear = 1;
            if ((arm_ext_code_u16(code, p) & 0xFFF8u) == 0x4448u &&
                arm_ext_code_u16(code, p + 2u) == 0x681Bu &&
                arm_ext_code_u16(code, p + 4u) == 0x2100u &&
                arm_ext_code_u16(code, p + 6u) == 0x1C20u) {
                has_r9_table_call = 1;
            }
            if (arm_ext_thumb_word_ref_has_imm(arm_ext_code_u16(code, p),
                                               0x48u)) {
                node48_refs++;
            }
        }
        if (has_node_alloc && has_node_clear && has_r9_table_call &&
            node48_refs >= 5u) {
            return 1;
        }
    }
    return 0;
}

static int arm_ext_child_has_compact_timer_walker(const uint8_t *code,
                                                  uint32_t file_len) {
    if (!code || file_len < 64u) return 0;
    static const uint8_t pat[] = {
        0x00, 0x48, 0xF8, 0xB5, 0x78, 0x44, 0x80, 0x6B,
        0x80, 0x30, 0x40, 0x68, 0x80, 0x47, 0x00, 0x25,
        0x00, 0x4E, 0x4E, 0x44, 0x31, 0x69, 0x35, 0x61,
        0x41, 0x1A, 0xB0, 0x68, 0x0B, 0x1C, 0x00, 0x28,
    };
    for (uint32_t off = 0; off + sizeof(pat) <= file_len; off += 2u) {
        int match = 1;
        for (uint32_t i = 0; i < sizeof(pat); ++i) {
            /*
             * Compact helper code=2 walkers load an R9-relative scheduler
             * base, then use +8/+12/+16 as queue/current/last-tick.  The LDR
             * literal immediates and the base value move with code layout, so
             * only those bytes are wildcarded.
             */
            if (i == 0 || i == 16) continue;
            if (code[off + i] != pat[i]) {
                match = 0;
                break;
            }
        }
        if (match) return 1;
    }
    return 0;
}

static int arm_ext_child_reads_record100_to_compact_r9_buffer(
    const uint8_t *code,
    uint32_t file_len) {
    if (!code || file_len < 128u) return 0;
    for (uint32_t off = 0; off + 96u <= file_len; off += 2u) {
        if (arm_ext_code_u16(code, off) != 0xB530u ||
            arm_ext_code_u16(code, off + 2u) != 0xB083u) {
            continue;
        }

        int has_r9_buffer_clear = 0;
        int has_record100_read = 0;
        for (uint32_t p = off + 4u; p + 16u < off + 96u; p += 2u) {
            /*
             * verdload.ext 0x2CA58A clears seven words at an R9-relative
             * buffer, then calls the record-read shim at 0x2CD810 with
             * r0=1, r1=100, stack[1]=that buffer.  Runtime showed the long
             * package path from record[100] landing in this compact state
             * area before brwmain consumed the bytes as list pointers.
             */
            if (arm_ext_code_u16(code, p) == 0x2100u &&
                arm_ext_code_u16(code, p + 2u) == 0x2200u &&
                arm_ext_code_u16(code, p + 4u) == 0x2300u &&
                (arm_ext_code_u16(code, p + 6u) & 0xFFF8u) == 0x4448u &&
                arm_ext_code_u16(code, p + 8u) == 0xC00Eu &&
                arm_ext_code_u16(code, p + 10u) == 0xC00Eu &&
                arm_ext_code_u16(code, p + 12u) == 0xC008u) {
                has_r9_buffer_clear = 1;
            }
            if ((arm_ext_code_u16(code, p) & 0xFFF8u) == 0x4448u &&
                arm_ext_code_u16(code, p + 2u) == 0x9101u &&
                arm_ext_code_u16(code, p + 4u) == 0x2164u &&
                arm_ext_code_u16(code, p + 6u) == 0x9000u &&
                arm_ext_code_u16(code, p + 8u) == 0x2001u &&
                arm_ext_code_u16(code, p + 10u) == 0x9202u &&
                arm_ext_thumb_bl_pair(arm_ext_code_u16(code, p + 12u),
                                      arm_ext_code_u16(code, p + 14u))) {
                has_record100_read = 1;
            }
        }
        if (has_r9_buffer_clear && has_record100_read) return 1;
    }
    return 0;
}

static int arm_ext_child_needs_short_pack_alias(ArmExtModule *m,
                                                uint32_t file_addr,
                                                uint32_t file_len) {
    if (!m || !file_addr || file_len < 96u) return 0;
    const uint8_t *code = (const uint8_t *)arm_ptr(m, file_addr);
    if (!code) return 0;

    for (uint32_t off = 0; off + 96u <= file_len; off += 2u) {
        if (arm_ext_code_u16(code, off) != 0xB5FFu ||
            arm_ext_code_u16(code, off + 2u) != 0xB089u) {
            continue;
        }

        int has_small_stack_pack_copy = 0;
        int has_output_store = 0;
        for (uint32_t p = off + 4u; p + 8u < off + 96u; p += 2u) {
            /*
             * frame.ext proves the hazardous shape: a 32-byte local buffer at
             * sp+4 is used as the package-name destination before the wrapper
             * calls readFile, and the function later writes through its
             * caller-provided output pointer.  One observed frame.ext clears
             * that buffer with two STMIA bursts and calls the pack getter with
             * r0=sp+4; older DOTA frame.ext used the A801 spelling.  Use this
             * code shape instead of a filename branch so other helpers keep
             * the full package path unless they have the same fixed-buffer ABI.
             */
            if (arm_ext_code_u16(code, p) == 0x2120u &&
                arm_ext_code_u16(code, p + 2u) == 0xA801u) {
                has_small_stack_pack_copy = 1;
            }
            if (arm_ext_code_u16(code, p) == 0xC70Fu &&
                arm_ext_code_u16(code, p + 2u) == 0xC70Fu &&
                arm_ext_code_u16(code, p + 4u) == 0x3F20u) {
                has_small_stack_pack_copy = 1;
            }
            if (arm_ext_code_u16(code, p) == 0x444Au &&
                arm_ext_code_u16(code, p + 2u) == 0x6812u &&
                arm_ext_code_u16(code, p + 4u) == 0xA801u &&
                arm_ext_code_u16(code, p + 6u) == 0x4790u) {
                has_small_stack_pack_copy = 1;
            }
            if (arm_ext_code_u16(code, p) == 0x444Au &&
                arm_ext_code_u16(code, p + 2u) == 0x6812u &&
                arm_ext_code_u16(code, p + 4u) == 0x1C38u &&
                arm_ext_code_u16(code, p + 6u) == 0x4790u) {
                has_small_stack_pack_copy = 1;
            }
            if ((arm_ext_code_u16(code, p) & 0xFFC7u) == 0x6000u) {
                has_output_store = 1;
            }
        }

        if (has_small_stack_pack_copy && has_output_store) return 1;
    }

    for (uint32_t off = 0; off + 72u <= file_len; off += 2u) {
        /*
         * Some private children copy table[100] into a fixed 32-byte
         * R9-relative global buffer during startup:
         *   memset(global, 0, 0x20); len = strlen(pack);
         *   memcpy(global, pack, len)
         * The native pack_filename is a short mutable DSM path.  Exposing a
         * host absolute path here overflows that buffer into neighboring
         * globals, so detect the ABI shape from code instead of naming apps.
         */
        if (arm_ext_code_u16(code, off) != 0xB5B0u) continue; /* push */

        int has_clear32 = 0;
        int has_pack_len_call = 0;
        int has_copy_len_call = 0;
        for (uint32_t p = off + 2u; p + 18u < off + 72u; p += 2u) {
            if (arm_ext_code_u16(code, p) == 0x2220u &&
                arm_ext_code_u16(code, p + 2u) == 0x2100u) {
                has_clear32 = 1;
            }
            if (arm_ext_code_u16(code, p) == 0x1C20u &&
                (arm_ext_code_u16(code, p + 2u) & 0xF800u) == 0x4800u &&
                arm_ext_code_u16(code, p + 4u) == 0x4449u &&
                arm_ext_code_u16(code, p + 6u) == 0x6809u &&
                arm_ext_code_u16(code, p + 8u) == 0x4788u) {
                has_pack_len_call = 1;
            }
            if (arm_ext_code_u16(code, p) == 0x1C02u &&
                arm_ext_code_u16(code, p + 2u) == 0x1C21u &&
                arm_ext_code_u16(code, p + 4u) == 0x1C28u &&
                (arm_ext_code_u16(code, p + 6u) & 0xF800u) == 0x4800u &&
                arm_ext_code_u16(code, p + 8u) == 0x444Bu &&
                arm_ext_code_u16(code, p + 10u) == 0x681Bu &&
                arm_ext_code_u16(code, p + 12u) == 0x4798u) {
                has_copy_len_call = 1;
            }
        }
        if (has_clear32 && has_pack_len_call && has_copy_len_call) return 1;
    }

    if (arm_ext_child_reads_record100_to_compact_r9_buffer(code, file_len))
        return 1;

    if (arm_ext_child_has_compact_r9_state_list(code, file_len)) return 1;

    return 0;
}

static void arm_ext_apply_short_pack_alias_for_private_child(ArmExtModule *m,
                                                             uint32_t file_addr,
                                                             uint32_t file_len,
                                                             uint32_t p_addr) {
    if (!m || !file_addr || !file_len) return;
    if (!arm_ext_child_needs_short_pack_alias(m, file_addr, file_len)) return;

    uint32_t package_ram_addr = 0;
    uint32_t package_ram_len = 0;
    const char *owner = "";
    ArmExtNestedModule *mod =
        p_addr ? arm_ext_find_nested_module_by_p(m, p_addr) : NULL;
    if (mod && mod->package_host_path[0]) {
        owner = mod->package_host_path;
    } else {
        owner = arm_ext_child_package_context(m, file_addr, file_len,
                                              &package_ram_addr,
                                              &package_ram_len);
        if (!owner || !owner[0])
            owner = m->pack_host_path;
    }

    const char *alias = arm_ext_short_pack_alias_for_host_path(m, owner);
    if (!alias || !alias[0]) return;
    const char *current = arm_ext_current_pack_table_name(m);
    /*
     * Private-loader startup code can copy record[100]/table[100] before its
     * helper is confirmed.  brwmain.ext 0x2D1E0C proves that the startup copy
     * is the hazardous 32-byte ABI, so install the short alias during
     * mr_cacheSync staging as well as after confirmed sync.
     */
    if (!current || strcmp(current, alias) != 0) {
        arm_ext_set_pack_table_name(m, alias);
    }
    arm_ext_set_child_record_pack_name(m, file_addr, alias);
}

static void arm_ext_repair_private_child_bridges(ArmExtModule *m,
                                                 uint32 file_addr,
                                                 uint32 file_len,
                                                 uint32 rw_base) {
    if (!m || !rw_base) return;
    uint32_t record = arm_ext_read_u32_or_zero_(m, file_addr); /* file_base[0] */
    if (!record) return;

    /* (a) 数据驱动 record 源槽填值：仅空白的自指针 bridge 槽。 */
    arm_ext_repair_private_child_record_bridges(m, record, file_addr,
                                                file_len);

    /* (b) 声明式 RW 镜像：先用子模块自身的 Thumb bridge-copy 结构证明布局，
     * 再整体 in-bounds 拟合并逐槽镜像空白目标。
     *
     * DOTA 浏览器插件的 frame.ext 反汇编在 0x2C97C8 把第三参数当输出指针，
     * 0x2C97FC 执行 str r0,[r4]；旧逻辑仅因它与 game.ext 代码区间重叠，就借用
     * game.ext 的 got_memcpy_off，把本描述符平移到 rw+0x34..0x74，随后 frame
     * 把 0x202C7831 这样的代码/标志值当输出指针写崩。区间重叠只能说明地址复用，
     * 不能证明 child RW bridge 布局相同；必须看到当前 child 代码中存在
     * record[0x68]/record[0x0C..0x4C] 复制到同一 R9-relative RW 区的指令序列。 */
    for (size_t L = 0; L < sizeof(private_child_rw_layouts) /
                           sizeof(private_child_rw_layouts[0]); ++L) {
        const PrivateChildRwLayout *lay = &private_child_rw_layouts[L];
        int32_t shift = 0;
        if (!arm_ext_discover_private_child_rw_shift(m, file_addr, file_len,
                                                     lay, &shift))
            continue;
        if (!arm_ext_private_child_rw_layout_is_safe(lay, shift))
            continue;
        int fits = 1;
        for (uint32_t r = 0; r < lay->run_count && fits; ++r) {
            const PrivateChildRwRun *run = &lay->runs[r];
            int32_t dst_signed = (int32_t)run->rw_dst_off + shift;
            if (dst_signed < 0) { fits = 0; break; }
            uint32_t dst = (uint32_t)dst_signed;
            if (!run->count ||
                !arm_ptr(m, record + run->record_src_off + (run->count - 1u) * 4u) ||
                !arm_ptr(m, rw_base + dst + (run->count - 1u) * 4u))
                fits = 0;
            for (uint32_t i = 0; i < run->count && fits; ++i) {
                uint32_t src_off = run->record_src_off + i * 4u;
                uint32_t v = arm_ext_read_u32_or_zero_(m, record + src_off);
                uint32_t expected = EXT_TABLE_ADDR + src_off;
                if (v != expected) fits = 0;
            }
        }
        if (!fits) continue;
        for (uint32_t r = 0; r < lay->run_count; ++r) {
            const PrivateChildRwRun *run = &lay->runs[r];
            uint32_t dst = (uint32_t)((int32_t)run->rw_dst_off + shift);
            for (uint32_t i = 0; i < run->count; ++i) {
                uint32_t v = arm_ext_read_u32_or_zero_(
                    m, record + run->record_src_off + i * 4u);
                if (!v) continue;
                if (arm_ext_read_u32_or_zero_(m, rw_base + dst + i * 4u))
                    continue; /* 只写空白 RW 槽，保证 no-op 安全 */
                memcpy(arm_ptr(m, rw_base + dst + i * 4u), &v, 4);
                if (getenv("VMRP_ARM_EXT_DIAG")) {
                    printf("DIAG private-child bridge repair[%s]: shift=%d rw+0x%X = 0x%X\n",
                           lay->note, shift, dst + i * 4u, v);
                }
            }
        }
    }
}

typedef struct ArmExtInternalLoaderTuple {
    uint32_t helper_addr;
    uint32_t p_addr;
    uint32_t rw_base;
    uint32_t record;
} ArmExtInternalLoaderTuple;

static int arm_ext_read_internal_loader_tuple(ArmExtModule *m,
                                              uint32_t ext_chunk,
                                              uint32_t file_addr,
                                              uint32_t file_len,
                                              int require_confirmed,
                                              ArmExtInternalLoaderTuple *tuple) {
    if (tuple) memset(tuple, 0, sizeof(*tuple));
    if (!m || !ext_chunk || !file_addr || file_len < 8u ||
        !arm_ptr(m, ext_chunk + 0x37u) ||
        !arm_ptr(m, file_addr + 7u)) {
        return 0;
    }

    uint32_t magic = 0, helper_addr = 0, chunk_file = 0, chunk_len = 0;
    uint32_t init_func = 0, chunk_rw = 0, chunk_rw_len = 0, p_addr = 0;
    uint32_t p_len = 0, record = 0;
    uint32_t chunk_record = 0, file_p_addr = 0;
    uint32_t rw_base = 0, p_ext_chunk = 0;
    memcpy(&magic, arm_ptr(m, ext_chunk), 4);
    if (magic != EXT_CHUNK_MAGIC) return 0;
    memcpy(&init_func, arm_ptr(m, ext_chunk + 0x04u), 4);
    memcpy(&helper_addr, arm_ptr(m, ext_chunk + 0x08u), 4);
    memcpy(&chunk_file, arm_ptr(m, ext_chunk + 0x0Cu), 4);
    memcpy(&chunk_len, arm_ptr(m, ext_chunk + 0x10u), 4);
    memcpy(&chunk_rw, arm_ptr(m, ext_chunk + 0x14u), 4);
    memcpy(&chunk_rw_len, arm_ptr(m, ext_chunk + 0x18u), 4);
    memcpy(&p_addr, arm_ptr(m, ext_chunk + 0x1Cu), 4);
    memcpy(&p_len, arm_ptr(m, ext_chunk + 0x20u), 4);
    memcpy(&chunk_record, arm_ptr(m, ext_chunk + 0x2Cu), 4);
    memcpy(&record, arm_ptr(m, file_addr), 4);
    memcpy(&file_p_addr, arm_ptr(m, file_addr + 4u), 4);
    if (chunk_file != file_addr || chunk_len != file_len ||
        init_func != file_addr + 8u ||
        !p_addr || file_p_addr != p_addr ||
        !record || !arm_ptr(m, record + PRIVATE_CHILD_RECORD_SPAN - 1u) ||
        chunk_record != record ||
        !arm_ptr(m, p_addr + sizeof(mr_c_function_P_t) - 1u) ||
        (require_confirmed && (!helper_addr ||
                               !arm_ptr(m, helper_addr & ~1u) ||
                               (helper_addr & ~1u) < file_addr ||
                               (helper_addr & ~1u) >= file_addr + file_len ||
                               p_len < sizeof(mr_c_function_P_t)))) {
        return 0;
    }

    memcpy(&rw_base, arm_ptr(m, p_addr), 4);
    memcpy(&p_ext_chunk, arm_ptr(m, p_addr + 0x0Cu), 4);
    /*
     * cfunction.ext private loader at 0xE8339C..0xE833CA stores child entry
     * (file_base+8), file_base, file_len, module record and P before
     * mr_cacheSync. The child later fills extChunk[8] and P[0] during entry;
     * extChunk[0x14]/[0x18] are only copied back after that BLX returns.
     * Split staging validation from confirmed helper/RW ownership, but keep
     * both tied to the SDK P->mrc_extChunk field at P+0x0C.
     */
    if (p_ext_chunk != ext_chunk ||
        (require_confirmed && !rw_base) ||
        (chunk_rw && rw_base && chunk_rw != rw_base)) {
        return 0;
    }
    if (require_confirmed && chunk_rw_len) {
        uint32_t rw_len = 0;
        memcpy(&rw_len, arm_ptr(m, p_addr + 4u), 4);
        if (rw_len != chunk_rw_len) return 0;
    }
    if (require_confirmed && rw_base &&
        !arm_ptr(m, rw_base)) {
        return 0;
    }
    if (tuple) {
        tuple->helper_addr = helper_addr;
        tuple->p_addr = p_addr;
        tuple->rw_base = rw_base;
        tuple->record = record;
    }
    return 1;
}

static int arm_ext_has_internal_loader_chunk(ArmExtModule *m,
                                             uint32_t file_addr,
                                             uint32_t file_len) {
    if (!m || !file_addr || !file_len) return 0;

    for (uint32_t ext_chunk = EXT_HEAP_ADDR;
         ext_chunk + 0x38 <= m->heap_top;
         ext_chunk += 4) {
        if (arm_ext_read_internal_loader_tuple(m, ext_chunk,
                                               file_addr, file_len, 0, NULL)) {
            return 1;
        }
    }

    return 0;
}

static int arm_ext_sync_internal_nested_module(ArmExtModule *m,
                                               uint32_t file_addr,
                                               uint32_t file_len) {
    if (!m || !file_addr || !file_len) return 0;

    for (uint32_t ext_chunk = EXT_HEAP_ADDR;
         ext_chunk + 0x38 <= m->heap_top;
         ext_chunk += 4) {
        ArmExtInternalLoaderTuple tuple;
        if (!arm_ext_read_internal_loader_tuple(m, ext_chunk, file_addr,
                                                file_len, 1, &tuple)) {
            continue;
        }
        uint32_t helper_addr = tuple.helper_addr;
        uint32_t p_addr = tuple.p_addr;
        uint32_t rw_base = tuple.rw_base;
        arm_ext_repair_private_child_bridges(m, file_addr, file_len, rw_base);
        arm_ext_scrub_private_child_low_bridge_state(m, file_addr, file_len,
                                                     rw_base);

        /*
         * cfunction.ext's internal loader does not call table[25].  The
         * disassembly at 0xE8339C..0xE833CA writes file_base+8, file_base,
         * file_len and P into an extChunk, calls mr_cacheSync, then BLXs the
         * child entry.  The child fills extChunk[8] with its callback helper.
         * Recording that structure gives host-side R9/timer ownership the
         * same module boundaries the wrapper uses internally.
         */
        arm_ext_record_nested_module(m, file_addr, file_len, p_addr, helper_addr);
        arm_ext_apply_short_pack_alias_for_private_child(m, file_addr,
                                                         file_len, p_addr);
        app_on_child_confirmed(m, p_addr, helper_addr);
        /*
         * Host-visible nested loads (table[25]) make every loaded child the
         * active foreground module.  cfunction.ext's private loader builds
         * the same extChunk/node shape without crossing table[25], so mirror
         * that ownership here once the helper slot is populated.
        */
        m->active_helper_addr = helper_addr;
        m->active_p_addr = p_addr;
        arm_ext_clear_foreground_screen_owner(m);
        if (!m->primary_helper_addr) {
            /*
             * Host table[25] uses the first nested EXT as the foreground app
             * module. cfunction.ext's internal table[25] follows the same
             * wrapper layout but bypasses the host hook, so mirror only that
             * ownership metadata here. The wrapper has already BLXed the child
             * entry and issued its setup helper calls; this block must not
             * initialize or dispatch the child again.
             */
            m->primary_helper_addr = helper_addr;
            m->primary_p_addr = p_addr;
            m->primary_file_addr = file_addr;
            m->primary_file_len = file_len;
            m->primary_host_init_pending = 0;
        }
        m->pending_internal_file_addr = 0;
        m->pending_internal_file_len = 0;
        /* 不在此处清除 primary_child_reopen_timer_needed 标志。
         * 该标志由模态框关闭时设置(arm_ext_call code=1 post-processing)，
         * 表示后续需要重启定时器驱动新加载的子插件。此函数在 run_arm_with_sp
         * 内部执行（如 game 的 code=2 回调中加载子模块），此时无法启动宿主
         * 定时器。如果在这里清除标志，arm_ext_call 返回后的定时器检查就无法
         * 发现需要重启，导致子插件界面无法显示。标志由 arm_ext_call 的
         * post-processing 在确认定时器已重启后清除。 */
        if (getenv("VMRP_ARM_EXT_DIAG")) {
            uint32_t ext_type = 0, is_pause = 0;
            uint32_t p_rw = 0, p_rw_len = 0;
            uint32_t chunk_rw = 0, chunk_rw_len = 0;
            memcpy(&ext_type, arm_ptr(m, p_addr + 8), 4);
            memcpy(&is_pause, arm_ptr(m, ext_chunk + 0x34), 4);
            memcpy(&p_rw, arm_ptr(m, p_addr), 4);
            memcpy(&p_rw_len, arm_ptr(m, p_addr + 4), 4);
            memcpy(&chunk_rw, arm_ptr(m, ext_chunk + 0x14), 4);
            memcpy(&chunk_rw_len, arm_ptr(m, ext_chunk + 0x18), 4);
            printf("DIAG synced file=0x%X len=%u chunk=0x%X P=0x%X H=0x%X p_rw=0x%X p_rw_len=%u chunk_rw=0x%X chunk_rw_len=%u ext_type=%u pause=%u activeP=0x%X activeH=0x%X primaryP=0x%X primaryH=0x%X timerP=0x%X timerH=0x%X\n",
                   file_addr, file_len, ext_chunk, p_addr, helper_addr,
                   p_rw, p_rw_len, chunk_rw, chunk_rw_len, ext_type, is_pause,
                   m->active_p_addr, m->active_helper_addr, m->primary_p_addr,
                   m->primary_helper_addr, m->timer_p_addr, m->timer_helper_addr);
            arm_ext_diag_dump_rw_timer_state(m, "sync-child", rw_base);
        }
        if (getenv("VMRP_ARM_EXT_TRACE")) {
            printf("arm_ext_executor: synced internal nested helper=0x%X P=0x%X file=0x%X len=%u primary=0x%X/0x%X\n",
                   helper_addr, p_addr, file_addr, file_len,
                   m->primary_helper_addr, m->primary_p_addr);
        }
        return 1;
    }

    return 0;
}




static int arm_ext_ranges_overlap(uint32_t a, uint32_t a_len,
                                  uint32_t b, uint32_t b_len) {
    uint64_t a_start = a;
    uint64_t a_end = a_start + a_len;
    uint64_t b_start = b;
    uint64_t b_end = b_start + b_len;
    return a_start < b_end && b_start < a_end;
}

static int arm_ext_range_contains(uint32_t outer, uint32_t outer_len,
                                  uint32_t inner, uint32_t inner_len) {
    uint64_t outer_start = outer;
    uint64_t outer_end = outer_start + outer_len;
    uint64_t inner_start = inner;
    uint64_t inner_end = inner_start + inner_len;
    return outer_start <= inner_start && inner_end <= outer_end;
}

static void arm_ext_consider_code_overlap(uint32_t range_addr,
                                          uint32_t range_len,
                                          uint64_t window_start,
                                          uint64_t window_end,
                                          int *found,
                                          uint32_t *best_lo,
                                          uint32_t *best_hi) {
    if (!range_addr || !range_len) return;
    uint64_t range_start = range_addr;
    uint64_t range_end = range_start + range_len;
    uint64_t lo = range_start > window_start ? range_start : window_start;
    uint64_t hi = range_end < window_end ? range_end : window_end;
    if (lo >= hi) return;
    if (!*found || lo < *best_lo) {
        *found = 1;
        *best_lo = (uint32_t)lo;
        *best_hi = (uint32_t)hi;
    }
}

static int arm_ext_find_first_registered_code_overlap(ArmExtModule *m,
                                                      uint32_t addr,
                                                      uint32_t len,
                                                      uint32_t *overlap_lo,
                                                      uint32_t *overlap_hi) {
    if (overlap_lo) *overlap_lo = 0;
    if (overlap_hi) *overlap_hi = 0;
    if (!m || !addr || !len) return 0;

    uint64_t window_start = addr;
    uint64_t window_end = window_start + len;
    int found = 0;
    uint32_t best_lo = 0;
    uint32_t best_hi = 0;

    /*
     * Once an EXT image has been registered or is in the mr_cacheSync staging
     * window, its file bytes are executable storage.  The Mythroad LG_mem head
     * is guest-visible, so allocation has to treat these ranges as logical
     * holes even if guest code writes a free-list node that spans them.
     */
    arm_ext_consider_code_overlap(m->primary_file_addr, m->primary_file_len,
                                  window_start, window_end, &found,
                                  &best_lo, &best_hi);
    arm_ext_consider_code_overlap(m->pending_internal_file_addr,
                                  m->pending_internal_file_len,
                                  window_start, window_end, &found,
                                  &best_lo, &best_hi);
    for (int i = 0; i < m->nested_module_count; ++i) {
        ArmExtNestedModule *mod = &m->nested_modules[i];
        arm_ext_consider_code_overlap(mod->file_addr, mod->file_len,
                                      window_start, window_end, &found,
                                      &best_lo, &best_hi);
    }

    if (!found) return 0;
    if (overlap_lo) *overlap_lo = best_lo;
    if (overlap_hi) *overlap_hi = best_hi;
    return 1;
}

static int arm_ext_first_unprotected_subrange(ArmExtModule *m,
                                              uint32_t range_addr,
                                              uint32_t range_len,
                                              uint32_t need,
                                              uint32_t *alloc_addr) {
    if (alloc_addr) *alloc_addr = 0;
    if (!range_addr || !range_len || !need) return 0;
    uint64_t range_end = (uint64_t)range_addr + range_len;
    uint32_t cursor = range_addr;

    while ((uint64_t)cursor + need <= range_end) {
        uint32_t code_lo = 0;
        uint32_t code_hi = 0;
        uint32_t probe_len = (uint32_t)(range_end - cursor);
        if (!arm_ext_find_first_registered_code_overlap(m, cursor, probe_len,
                                                        &code_lo, &code_hi)) {
            if (alloc_addr) *alloc_addr = cursor;
            return 1;
        }
        if ((uint64_t)cursor + need <= code_lo) {
            if (alloc_addr) *alloc_addr = cursor;
            return 1;
        }
        if (code_hi <= cursor) return 0;
        cursor = code_hi;
    }
    return 0;
}

static int arm_ext_addr_range_mapped(ArmExtModule *m,
                                     uint32_t addr,
                                     uint32_t len) {
    if (!m || !addr || !len) return 0;
    if (len - 1u > UINT32_MAX - addr) return 0;
    return arm_ptr(m, addr) && arm_ptr(m, addr + len - 1u);
}

static int arm_ext_mirror_read_file_to_adjacent_slot(ArmExtModule *m,
                                                     uint32_t len_slot,
                                                     const void *data,
                                                     uint32_t len,
                                                     uint32_t ret_addr,
                                                     uint32_t *slot_out) {
    if (slot_out) *slot_out = 0;
    if (!m || !len_slot || !data || !len) return 0;
    uint32_t wrapper_rw = arm_ext_wrapper_rw_base_(m);
    if (!wrapper_rw || reg_read32(m->uc, UC_ARM_REG_R9) != wrapper_rw)
        return 0;
    if (!arm_ptr(m, len_slot) || !arm_ptr(m, len_slot + 4u)) return 0;
    if (arm_ext_read_u32_or_zero_(m, len_slot) != len) return 0;
    /*
     * This wrapper parser reaches table[125] from its 0xE80B82 call site, and
     * the caller frame stores that Thumb return at lenSlot+8.  Require this
     * marker so ordinary mr_readFile(&len) callers are not interpreted as the
     * wrapper's private {fileLen,dataPtr} ABI.
     */
    if (arm_ext_read_u32_or_zero_(m, len_slot + 8u) != 0x00000B82u)
        return 0;

    uint32_t slot = arm_ext_read_u32_or_zero_(m, len_slot + 4u);
    if (slot_out) *slot_out = slot;
    if (!slot || slot == ret_addr) return 0;
    if (!arm_ext_addr_range_mapped(m, slot, len)) return 0;

    /*
     * Some cfunction.ext parsers pass table[125] a stack pair shaped as
     * {fileLen, dataPtr} and then use dataPtr, not the readFile return value,
     * for the follow-up materialize/write.  Mirror the decoded bytes into that
     * caller-owned buffer so the ARM-side parser state matches native readFile.
     */
    void *dst = arm_ptr(m, slot);
    if (!dst) return 0;
    if (memcmp(dst, data, len) == 0) return 0;
    memcpy(dst, data, len);
    return 1;
}

static void arm_ext_drop_overlapping_stale_nested_modules(ArmExtModule *m,
                                                          uint32_t file_addr,
                                                          uint32_t file_len) {
    if (!m || !file_addr || !file_len) return;
    int out = 0;
    for (int i = 0; i < m->nested_module_count; ++i) {
        ArmExtNestedModule mod = m->nested_modules[i];
        int is_primary = m->primary_file_addr &&
                         mod.file_addr == m->primary_file_addr &&
                         mod.file_len == m->primary_file_len;
        int overlaps = mod.file_addr && mod.file_len &&
                       arm_ext_ranges_overlap(mod.file_addr, mod.file_len,
                                              file_addr, file_len);
        if (!is_primary && overlaps) {
            /*
             * Private loaders may stage a replacement child into the same
             * low-heap code range before the new extChunk has a helper.  Once
             * mr_cacheSync proves the bytes were rewritten, the old module
             * record no longer describes that address range and must not drive
             * R9, timer-owner or screen-owner recovery.
             */
            if (m->active_p_addr == mod.p_addr &&
                m->active_helper_addr == mod.helper_addr) {
                m->active_p_addr = 0;
                m->active_helper_addr = 0;
            }
            if (m->timer_p_addr == mod.p_addr &&
                m->timer_helper_addr == mod.helper_addr) {
                m->timer_p_addr = 0;
                m->timer_helper_addr = 0;
            }
            if (getenv("VMRP_ARM_EXT_DIAG")) {
                printf("DIAG stale_child_drop oldFile=0x%X/%u oldP=0x%X oldH=0x%X newFile=0x%X/%u\n",
                       mod.file_addr, mod.file_len, mod.p_addr,
                       mod.helper_addr, file_addr, file_len);
            }
            /* 临时诊断:记录 stale 模块卸载(修复后删除) */
            arm_ext_watch_module_event(m, "drop_stale", mod.file_addr,
                                       mod.file_len, mod.p_addr);
            continue;
        }
        m->nested_modules[out++] = mod;
    }
    m->nested_module_count = out;
}

static void arm_ext_retire_modules_overwritten_by_data_read(ArmExtModule *m,
                                                            uint32_t read_addr,
                                                            uint32_t read_len) {
    if (!m || !read_addr || !read_len) return;

    uint32_t code_lo = 0;
    uint32_t code_hi = 0;
    if (!arm_ext_find_first_registered_code_overlap(m, read_addr, read_len,
                                                    &code_lo, &code_hi)) {
        return;
    }

    /*
     * table[44] writes directly into a caller-supplied ARM buffer, bypassing the
     * app allocator.  If that buffer overlaps a registered child image, the
     * image bytes are no longer executable ownership for native semantics: later
     * first-fit readFile calls may intentionally reuse the same LG_mem address.
     * Retire only non-primary children; primary dump/restore keeps its dedicated
     * reconciliation path below.
     */
    uc_ctl_remove_cache(m->uc, read_addr, read_addr + read_len);
    arm_ext_drop_overlapping_stale_nested_modules(m, read_addr, read_len);
    if (m->pending_internal_file_addr && m->pending_internal_file_len &&
        arm_ext_ranges_overlap(m->pending_internal_file_addr,
                               m->pending_internal_file_len,
                               read_addr, read_len)) {
        m->pending_internal_file_addr = 0;
        m->pending_internal_file_len = 0;
    }
    if (!m->active_p_addr && m->primary_p_addr && m->primary_helper_addr) {
        m->active_p_addr = m->primary_p_addr;
        m->active_helper_addr = m->primary_helper_addr;
        arm_ext_clear_foreground_screen_owner(m);
    }
}

static void arm_ext_restore_primary_mapping_after_dump0(ArmExtModule *m,
                                                       uint32_t read_addr,
                                                       uint32_t read_len) {
    if (!m || !m->primary_file_addr || !m->primary_file_len ||
        !m->primary_p_addr || !m->primary_helper_addr) {
        return;
    }
    if (!arm_ext_range_contains(read_addr, read_len,
                                m->primary_file_addr, m->primary_file_len)) {
        return;
    }
    /*
     * gxdzc/netpay 取消路径反汇编定位：
     *   0xE836A7 通过 table[44] 把 plugins/dump0 读回 0x6460D4，长度 0x96000；
     *   随后 game.ext 在 0x6643CD/0x6643D7 重新 stop/start 自己的 timer。
     * 读回 dump0 会把 0x6460F8 上临时加载的 netpay.ext 覆盖回 game.ext，
     * 但执行器的 nested_modules 仍把同一地址范围登记为 netpay，导致
     * table[31] 把 timer owner 记录成 netpay helper(0x64D531)，下一次
     * code=2 进入已被覆盖的插件，最终在 wrapper 队列 0xE83B48..0xE83B56
     * 弹出坏 PC=0x9A000004。这里按真实内存恢复结果重建代码归属，不在
     * run_arm_with_sp() 里吞异常；兼容性：只在大块读回完整覆盖 primary
     * ext 映像时触发，普通插件 staging 仍由 table[131]/mr_cacheSync 处理。
     * 验证：gxdzc 相关 e2e 不再触发 invalid memory。
     */
    int out = 0;
    for (int i = 0; i < m->nested_module_count; ++i) {
        ArmExtNestedModule mod = m->nested_modules[i];
        int is_primary = mod.file_addr == m->primary_file_addr &&
                         mod.file_len == m->primary_file_len &&
                         mod.helper_addr == m->primary_helper_addr;
        if (!is_primary &&
            arm_ext_ranges_overlap(mod.file_addr, mod.file_len, read_addr, read_len)) {
            continue;
        }
        m->nested_modules[out++] = mod;
    }
    m->nested_module_count = out;
    arm_ext_record_nested_module(m, m->primary_file_addr, m->primary_file_len,
                                 m->primary_p_addr, m->primary_helper_addr);

    if (m->active_helper_addr &&
        m->active_helper_addr != m->primary_helper_addr &&
        arm_ext_range_contains(read_addr, read_len, m->active_helper_addr & ~1u, 2)) {
        m->active_helper_addr = m->primary_helper_addr;
        m->active_p_addr = m->primary_p_addr;
        arm_ext_clear_foreground_screen_owner(m);
    }
    if (m->timer_helper_addr &&
        m->timer_helper_addr != m->primary_helper_addr &&
        arm_ext_range_contains(read_addr, read_len, m->timer_helper_addr & ~1u, 2)) {
        m->timer_helper_addr = m->primary_helper_addr;
        m->timer_p_addr = m->primary_p_addr;
    }
    if (m->dispatch_timer_start &&
        arm_ext_range_contains(read_addr, read_len, m->dispatch_timer_start & ~1u, 2)) {
        m->dispatch_timer_start = 0;
    }
    if (m->dispatch_timer_stop &&
        arm_ext_range_contains(read_addr, read_len, m->dispatch_timer_stop & ~1u, 2)) {
        m->dispatch_timer_stop = 0;
    }
}

static uint32_t arm_ext_p_for_code_addr(ArmExtModule *m, uint32_t addr,
                                        uint32_t *helper_addr) {
    uint32_t pc = addr & ~1u;
    if (helper_addr) *helper_addr = 0;
    if (!m) return 0;

    if (pc >= EXT_CODE_ADDR && pc < EXT_CODE_ADDR + m->code_len) {
        if (helper_addr) *helper_addr = m->helper_addr;
        return m->p_addr;
    }

    ArmExtNestedModule *mod = arm_ext_find_nested_module(m, pc);
    if (mod) {
        if (helper_addr) *helper_addr = mod->helper_addr;
        return mod->p_addr;
    }

    return 0;
}

static int arm_ext_has_foreground_child(ArmExtModule *m) {
    return m &&
           m->primary_p_addr && m->primary_helper_addr &&
           m->active_p_addr && m->active_helper_addr &&
           m->active_p_addr != m->primary_p_addr &&
           m->active_helper_addr != m->primary_helper_addr &&
           m->active_p_addr != m->p_addr &&
           m->active_helper_addr != m->helper_addr;
}

static int arm_ext_has_suspended_foreground_child(ArmExtModule *m,
                                                  uint32_t primary_ext_chunk) {
    if (!arm_ext_has_foreground_child(m) ||
        !primary_ext_chunk ||
        !arm_ptr(m, primary_ext_chunk + 0x34u)) {
        return 0;
    }
    uint32_t suspend_depth = 0;
    memcpy(&suspend_depth, arm_ptr(m, primary_ext_chunk + 0x34u), 4);
    return suspend_depth > 0;
}

static uint32_t arm_ext_active_rw_base(ArmExtModule *m) {
    uint32_t rw_base = 0;
    if (m && m->active_p_addr && arm_ptr(m, m->active_p_addr)) {
        memcpy(&rw_base, arm_ptr(m, m->active_p_addr), 4);
    }
    return rw_base;
}

static int arm_ext_frame_timer_node_is_valid(ArmExtModule *m,
                                             uint32_t node) {
    if (!m || !node || !arm_ptr(m, node) || !arm_ptr(m, node + 0x1Cu))
        return 0;
    /*
     * frame.ext allocates timer nodes at 0x2C9486 and stamps the first word
     * with 0x79ABBCCF before 0x2C9538 links them into the R9+0x94 queue.
     * Require that structural marker so unrelated child RW data is not
     * mistaken for a private timer queue.
     */
    return arm_ext_read_u32_or_zero_(m, node) == 0x79ABBCCFu;
}

static int arm_ext_foreground_child_has_frame_timer_queue(ArmExtModule *m) {
    if (!arm_ext_has_foreground_child(m)) return 0;
    uint32_t rw_base = arm_ext_active_rw_base(m);
    if (!rw_base || !arm_ptr(m, rw_base + 0x94u) ||
        !arm_ptr(m, rw_base + 0xA0u)) {
        return 0;
    }
    /*
     * DOTA frame.ext code=2 (0x2C96A0) consumes the queue at R9+0x94:
     * [base+8] is the queued head and [base+12] is the active/current list.
     * If both are empty, direct child code=2 immediately returns and can
     * starve the wrapper-owned loading queue that installed the child.
     */
    uint32_t queued = arm_ext_read_u32_or_zero_(m, rw_base + 0x9Cu);
    uint32_t active = arm_ext_read_u32_or_zero_(m, rw_base + 0xA0u);
    return arm_ext_frame_timer_node_is_valid(m, queued) ||
           arm_ext_frame_timer_node_is_valid(m, active);
}

static int arm_ext_compact_timer_node_is_valid(ArmExtModule *m,
                                               uint32_t node) {
    return m && node && arm_ptr(m, node) && arm_ptr(m, node + 0x1Cu);
}

static int arm_ext_compact_timer_queue_at(ArmExtModule *m,
                                          uint32_t rw_base,
                                          uint32_t scheduler_off) {
    if (!rw_base || !arm_ptr(m, rw_base + scheduler_off + 0x10u))
        return 0;

    /*
     * Disassembled compact helpers use an R9-relative scheduler header:
     * [base+8] is the queued head and [base+12] is the active/current list.
     * Two SDK layouts observed so far place that header at R9+0xC0 and
     * R9+0x248; both share the same walker byte pattern above.
     */
    uint32_t queued = arm_ext_read_u32_or_zero_(
        m, rw_base + scheduler_off + 0x08u);
    uint32_t current = arm_ext_read_u32_or_zero_(
        m, rw_base + scheduler_off + 0x0Cu);
    return arm_ext_compact_timer_node_is_valid(m, queued) ||
           arm_ext_compact_timer_node_is_valid(m, current);
}

static int arm_ext_foreground_child_has_compact_timer_queue(ArmExtModule *m) {
    if (!arm_ext_has_foreground_child(m)) return 0;
    ArmExtNestedModule *mod = arm_ext_find_nested_module_by_p(
        m, m->active_p_addr);
    if (!mod || !mod->file_addr || !mod->file_len) return 0;
    const uint8_t *code = (const uint8_t *)arm_ptr(m, mod->file_addr);
    if (!arm_ext_child_has_compact_timer_walker(code, mod->file_len))
        return 0;

    /*
     * The compact walker returns immediately when its queue/current heads are
     * null; the timestamp word is cleared before that return and is not
     * runnable work by itself.  Only real node pointers justify stealing a
     * wrapper-owned host timer tick for the foreground child.
     */
    uint32_t rw_base = arm_ext_active_rw_base(m);
    return arm_ext_compact_timer_queue_at(m, rw_base, 0x0C0u) ||
           arm_ext_compact_timer_queue_at(m, rw_base, 0x248u);
}

static int arm_ext_wrapper_dispatch_is_busy(ArmExtModule *m) {
    /*
     * wrapper_rw+4 is only a queue/busy flag in the older direct-dispatch
     * layout identified by find_wrapper_timer_dispatch().  The 19KB
     * chain-walker wrapper keeps unrelated nonzero data at the same offset, so
     * using it as generic liveness pins stale wrapper timer ownership forever.
     */
    if (!m || !m->wrapper_timer_dispatch_addr) return 0;
    if (m->wrapper_compact_timer_scheduler_off) {
        /*
         * Compact wrapper walkers load an R9-relative scheduler literal and
         * consume real node lists from that header.  In that ABI wrapper_rw+4
         * is broader state, not runnable timer evidence; using it after the
         * scheduler is empty can keep code=2 pinned to the wrapper forever.
         */
        return 0;
    }
    uint32_t wrapper_rw = arm_ext_wrapper_rw_base_(m);
    if (!wrapper_rw || !arm_ptr(m, wrapper_rw + 0x04u)) return 0;
    /*
     * cfunction.ext wrapper dispatch uses wrapper_rw+4 as its queue/busy flag
     * (see the 0x1F50 dispatch notes).  Read it only as liveness evidence:
     * mutating this flag changes wrapper queue semantics and previously left
     * pending events undrained.
     */
    return arm_ext_read_u32_or_zero_(m, wrapper_rw + 0x04u) != 0;
}

static int arm_ext_wrapper_timer_node_is_valid(ArmExtModule *m,
                                               uint32_t node) {
    if (!m || !node || !arm_ptr(m, node) || !arm_ptr(m, node + 0x20u))
        return 0;
    /*
     * optwar's cfunction.ext timer queue at 0xE84920 stores next/prev at
     * node+0/+4, stamps 0x79ABBCCF at node+8, compares due time at +0x0C,
     * and invokes callback/data fields through +0x14/+0x18/+0x1C.
     */
    return arm_ext_read_u32_or_zero_(m, node + 0x08u) == 0x79ABBCCFu;
}

static int arm_ext_wrapper_compact_timer_node_is_valid(ArmExtModule *m,
                                                       uint32_t node) {
    if (!m || !node || !arm_ptr(m, node) || !arm_ptr(m, node + 0x1Cu))
        return 0;
    /*
     * The compact wrapper add/remove path compares node[0] with this SDK
     * marker before linking through node+0x18/node+0x1c.
     */
    return arm_ext_read_u32_or_zero_(m, node) == 0x79ABBCCFu;
}

static int arm_ext_wrapper_compact_timer_queue_at(ArmExtModule *m,
                                                  uint32_t wrapper_rw,
                                                  uint32_t scheduler_off) {
    if (!wrapper_rw || !scheduler_off ||
        !arm_ptr(m, wrapper_rw + scheduler_off + 0x14u)) {
        return 0;
    }

    /*
     * The matched wrapper walker loads R6=R9+scheduler_off, then consumes
     * [R6+0x0c] as the sorted queue, [R6+0x10] as the active/due chain, and
     * [R6+0x14] as its last-tick timestamp.
     */
    uint32_t queued = arm_ext_read_u32_or_zero_(
        m, wrapper_rw + scheduler_off + 0x0Cu);
    uint32_t current = arm_ext_read_u32_or_zero_(
        m, wrapper_rw + scheduler_off + 0x10u);
    return arm_ext_wrapper_compact_timer_node_is_valid(m, queued) ||
           arm_ext_wrapper_compact_timer_node_is_valid(m, current);
}

static int arm_ext_wrapper_has_timer_queue(ArmExtModule *m) {
    uint32_t wrapper_rw = arm_ext_wrapper_rw_base_(m);
    if (!wrapper_rw) {
        return 0;
    }

    if (arm_ext_wrapper_compact_timer_queue_at(
            m, wrapper_rw, m->wrapper_compact_timer_scheduler_off)) {
        return 1;
    }

    if (arm_ptr(m, wrapper_rw + 0x3C8u) &&
        arm_ptr(m, wrapper_rw + 0x3D8u)) {
        uint32_t queued = arm_ext_read_u32_or_zero_(m, wrapper_rw + 0x3C8u);
        uint32_t current = arm_ext_read_u32_or_zero_(m, wrapper_rw + 0x3D8u);
        return arm_ext_wrapper_timer_node_is_valid(m, queued) ||
               arm_ext_wrapper_timer_node_is_valid(m, current);
    }
    return 0;
}

static int arm_ext_screen_owner_is_visible(ArmExtModule *m,
                                           uint32_t owner_p_addr,
                                           uint32_t owner_helper_addr) {
    if (!m || !owner_p_addr) return 1;
    if (!arm_ext_has_foreground_child(m)) return 1;
    /*
     * A loaded child helper is not a visible occlusion by itself.  Private
     * loaders also register support EXTs through the same table[25] ABI; only
     * an accepted child present creates foreground cover rows that must block
     * lower-layer framebuffer writes.
     */
    if (!arm_ext_has_foreground_cover(m)) return 1;
    return owner_p_addr == m->active_p_addr ||
           owner_helper_addr == m->active_helper_addr;
}

static int arm_ext_current_screen_owner_is_visible(ArmExtModule *m) {
    if (!m) return 1;
    uint32_t helper_addr = 0;
    uint32_t p_addr =
        arm_ext_p_for_code_addr(m, reg_read32(m->uc, UC_ARM_REG_PC),
                                &helper_addr);
    if (!p_addr) {
        p_addr = m->current_p_addr;
        helper_addr = m->current_helper_addr;
    }
    return arm_ext_screen_owner_is_visible(m, p_addr, helper_addr);
}

static void arm_ext_sync_r9_for_code_addr(ArmExtModule *m, uint32_t addr) {
    uint32_t pc = addr & ~1u;
    uint32_t p_addr = 0;
    if (pc >= EXT_CODE_ADDR && pc < EXT_CODE_ADDR + m->code_len) {
        uint32_t current_r9 = reg_read32(m->uc, UC_ARM_REG_R9);
        for (int i = 0; i < m->nested_module_count; ++i) {
            uint32_t nested_rw = 0;
            if (arm_ptr(m, m->nested_modules[i].p_addr)) {
                memcpy(&nested_rw, arm_ptr(m, m->nested_modules[i].p_addr), 4);
            }
            if (nested_rw && current_r9 == nested_rw) {
                p_addr = m->p_addr;
                break;
            }
        }
    } else {
        ArmExtNestedModule *mod = arm_ext_find_nested_module(m, pc);
        if (mod) {
            p_addr = mod->p_addr;
            /*
             * 同一份 EXT 代码被 wrapper 实例化多次时（dota 的 screen 模块
             * 0x661AC0 同时有 loading / menu 等多个实例，file_addr 相同、rw 各异），
             * arm_ext_find_nested_module 只能按 PC 区间匹配，返回“最后登记”的实例。
             * 当正在执行的是一个 wrapper 刚 BLX 进去、但尚未登记进 nested_modules
             * 的新实例时（game 自己已经把 sb 装成新实例的 rw），按“最后登记实例”纠
             * R9 会把 game 设好的正确值覆盖成旧实例的 rw，使该模块所有 sb 相对全局
             * 访问错位。dota 实证：menu 屏消息处理器以 loading 实例的 R9 运行，
             * memset(sb-0xA44) 落到 primary 共享的 cg_1 背景条结构、清 0 其指针，
             * 主界面 y=80..159 变黑带。
             *
             * 判据（数据驱动，无 app 指纹）：仅当该代码段被同 file_addr 的多个实例
             * 共享（歧义），且当前 R9 不等于任何“已知上下文”的 rw —— 既不是任何
             * 已登记 nested 实例、也不是 primary、也不是 wrapper(m->p_addr) 的 rw ——
             * 时，认定它是 game 刚设好的“新建未登记实例”的 sb，予以信任、不覆盖。
             * 残留的旧 R9 必然等于某个已知上下文的 rw，仍走原纠正逻辑；单实例模块
             * （same_code==1）完全不受影响。验证：dota/gghjt/gxdzc e2e
             * 回归通过。
             */
            int same_code = 0;
            for (int i = 0; i < m->nested_module_count; ++i) {
                if (m->nested_modules[i].file_addr == mod->file_addr &&
                    m->nested_modules[i].file_len)
                    same_code++;
            }
            if (same_code > 1) {
                uint32_t cur_r9 = reg_read32(m->uc, UC_ARM_REG_R9);
                int recognized = 0;
                if (cur_r9 == arm_ext_read_u32_or_zero_(m, m->primary_p_addr) ||
                    cur_r9 == arm_ext_read_u32_or_zero_(m, m->p_addr))
                    recognized = 1;
                for (int i = 0; !recognized && i < m->nested_module_count; ++i) {
                    if (cur_r9 ==
                        arm_ext_read_u32_or_zero_(m, m->nested_modules[i].p_addr))
                        recognized = 1;
                }
                if (cur_r9 && !recognized) return; /* 新建未登记实例的 sb，信任 game */
            }
        }
    }
    if (!p_addr || !arm_ptr(m, p_addr)) return;

    uint32_t rw_base = 0;
    memcpy(&rw_base, arm_ptr(m, p_addr), 4);
    if (!rw_base) return;

    uint32_t current_r9 = reg_read32(m->uc, UC_ARM_REG_R9);
    if (current_r9 != rw_base) {
        reg_write32(m->uc, UC_ARM_REG_R9, rw_base);
    }
}

static void trace_pc(uc_engine *uc, uint64_t address, uint32_t size, void *user_data) {
    (void)size;
    ArmExtModule *m = (ArmExtModule *)user_data;
    uint32_t pos = m->pc_ring_pos++ % EXT_TRACE_PC_RING;
    m->pc_ring[pos] = (uint32_t)address;
    m->cpsr_ring[pos] = reg_read32(uc, UC_ARM_REG_CPSR);
}

static void dump_pc_ring(ArmExtModule *m) {
    if (!getenv("VMRP_ARM_EXT_TRACE_PC")) return;
    uint32_t total = m->pc_ring_pos < EXT_TRACE_PC_RING ? m->pc_ring_pos : EXT_TRACE_PC_RING;
    printf("arm_ext_executor: recent PCs:\n");
    for (uint32_t i = 0; i < total; ++i) {
        uint32_t idx = (m->pc_ring_pos - total + i) % EXT_TRACE_PC_RING;
        printf("  #%02u pc=0x%X cpsr=0x%X %s\n",
               i, m->pc_ring[idx], m->cpsr_ring[idx],
               (m->cpsr_ring[idx] & (1u << 5)) ? "thumb" : "arm");
    }
}

static void arm_ext_mark_unhandled_intr(ArmExtModule *m, uint32_t intno) {
    if (!m) return;
    m->pending_intr_no = intno;
    m->pending_intr_pc = reg_read32(m->uc, UC_ARM_REG_PC) & ~1u;
    m->pending_intr_r0 = reg_read32(m->uc, UC_ARM_REG_R0);
    m->pending_intr_r1 = reg_read32(m->uc, UC_ARM_REG_R1);
}

static void hook_intr(uc_engine *uc, uint32_t intno, void *user_data) {
    ArmExtModule *m = (ArmExtModule *)user_data;
    uint32_t pc = reg_read32(uc, UC_ARM_REG_PC) & ~1u;
    uint32_t r0 = reg_read32(uc, UC_ARM_REG_R0);
    uint32_t r1 = reg_read32(uc, UC_ARM_REG_R1);
    /*
     * ARM semihosting uses SVC #0xAB in Thumb state.  Vendor browser
     * components keep a tiny debug putchar stub in their runtime image:
     *   movs r0,#3; mov r1,sp; svc #0xAB
     * where operation 3 is SYS_WRITEC and r1 points at one byte.  Unicorn
     * reports the trap through UC_HOOK_INTR and does not emulate the ABI, so
     * consume only this documented print operation.  Other SVC/HVC traps are
     * preserved as real execution errors by stopping with pending_intr_* set.
     */
    if (intno == 2u && r0 == 3u && arm_ptr(m, r1)) {
        if (getenv("VMRP_ARM_EXT_DIAG")) {
            static uint32_t semihost_diag_count = 0;
            if (semihost_diag_count < 64u) {
                unsigned ch = arm_ext_read_u8_or_zero_(m, r1);
                printf("DIAG semihost_writec pc=0x%X ch=0x%02X r1=0x%X\n",
                       pc, ch, r1);
            } else if (semihost_diag_count == 64u) {
                printf("DIAG semihost_writec truncated after 64 entries\n");
            }
            semihost_diag_count++;
        }
        return;
    }

    arm_ext_mark_unhandled_intr(m, intno);
    uc_emu_stop(uc);
}

static uint32_t arg_read(ArmExtModule *m, unsigned n) {
    if (n < 4) return reg_read32(m->uc, UC_ARM_REG_R0 + n);
    uint32_t sp = reg_read32(m->uc, UC_ARM_REG_SP);
    uint32_t v = 0;
    uc_mem_read(m->uc, sp + (n - 4) * 4, &v, 4);
    return v;
}

static int run_arm_with_sp(ArmExtModule *m, uint32_t start, uint32_t sp) {
    set_arm_mode_for_addr(m, start);
    m->pending_intr_no = 0;
    m->pending_intr_pc = 0;
    m->pending_intr_r0 = 0;
    m->pending_intr_r1 = 0;
    reg_write32(m->uc, UC_ARM_REG_SP, sp);
    reg_write32(m->uc, UC_ARM_REG_LR, EXT_STOP_ADDR);
    uc_err err = uc_emu_start(m->uc, arm_exec_addr(start), EXT_STOP_ADDR, 0, 0);
    if (err == UC_ERR_OK && m->pending_intr_no) {
        err = UC_ERR_EXCEPTION;
        reg_write32(m->uc, UC_ARM_REG_PC, m->pending_intr_pc);
    }
    /*
     * Some nested .mrp plug-ins (e.g. netpay.mrp loaded by gghjt.mrp) keep
     * function pointers without the Thumb bit, so a BLX into them lands in
     * ARM mode while the bytes are Thumb-2.  Unicorn reports that mismatch as
     * UC_ERR_INSN_INVALID for some Thumb instruction pairs and UC_ERR_EXCEPTION
     * for others, so do the same one-shot mode correction for both errors. If
     * the same PC still fails after being retried in Thumb mode, it is real
     * invalid code/data, not a missing Thumb bit.  gghjt's 60s timeout-return
     * path exposes this after dump0 restore: a copied timer node (e.g.
     * table[3] to 0x738B34) is data, and an unbounded retry here pins the UI on
     * “请稍等”.  Let the normal invalid-PC handling below stop the callback
     * cleanly instead of spinning forever.
     */
    if (err == UC_ERR_INSN_INVALID || err == UC_ERR_EXCEPTION) {
        uint32_t pc = reg_read32(m->uc, UC_ARM_REG_PC);
        if (pc != EXT_STOP_ADDR && pc != 0) {
            uint32_t cpsr = reg_read32(m->uc, UC_ARM_REG_CPSR);
            if ((cpsr & (1u << 5)) == 0) {
                cpsr |= (1u << 5);
                reg_write32(m->uc, UC_ARM_REG_CPSR, cpsr);
                if (getenv("VMRP_ARM_EXT_TRACE")) {
                    printf("arm_ext_executor: retrying in Thumb mode at pc=0x%X\n", pc);
                }
                /*
                 * Unicorn selects Thumb entry semantics from the low address
                 * bit on uc_emu_start().  Changing CPSR.T alone is not enough
                 * when a BLX target lost bit 0 and stopped on valid Thumb
                 * bytes, so restart the callback at the Thumb-tagged PC.
                 */
                err = uc_emu_start(m->uc, pc | 1u, EXT_STOP_ADDR, 0, 0);
                if (err == UC_ERR_OK && m->pending_intr_no) {
                    err = UC_ERR_EXCEPTION;
                    reg_write32(m->uc, UC_ARM_REG_PC, m->pending_intr_pc);
                }
            }
        }
    }
    if (err != UC_ERR_OK) {
        if (reg_read32(m->uc, UC_ARM_REG_PC) == EXT_STOP_ADDR) return MR_SUCCESS;
        uint32_t pc = reg_read32(m->uc, UC_ARM_REG_PC) & ~1u;
        uint32_t cur_sp = reg_read32(m->uc, UC_ARM_REG_SP);
        if (m->pending_intr_no && getenv("VMRP_ARM_EXT_DIAG")) {
            printf("DIAG unhandled_intr intno=%u pc=0x%X r0=0x%X r1=0x%X start=0x%X\n",
                   m->pending_intr_no, m->pending_intr_pc,
                   m->pending_intr_r0, m->pending_intr_r1, start);
        }
        if (err == UC_ERR_INSN_INVALID && pc >= EXT_CODE_ADDR && pc < EXT_CODE_ADDR + m->code_len) {
            /*
             * ARM 侧栈漂进了代码段后无法继续安全解释。这里只结束当前
             * callback，让平台层保持原状态继续调度；退出/重启必须来自
             * ARM 程序自己的 mr_exit/mr_state 路径，不能在异常处理里推断。
             */
            if (getenv("VMRP_ARM_EXT_TRACE")) {
                printf("arm_ext_executor: ARM callback stopped at non-instruction pc=0x%X sp=0x%X\n", pc, cur_sp);
                dump_pc_ring(m);
            }
            reg_write32(m->uc, UC_ARM_REG_PC, EXT_STOP_ADDR);
            return MR_SUCCESS;
        }
        /*
         * Nested plug-ins loaded into the heap (e.g. netpay.mrp inside
         * gghjt.mrp) sometimes encode return addresses that drop us a
         * halfword into a 32-bit Thumb-2 instruction. The fetched bytes
         * are then undefined, but a quick check confirms PC-2 starts a
         * valid 32-bit Thumb-2 op -- treat it as a clean stop instead of
         * spamming the console; the caller already handles MR_SUCCESS the
         * same way it handles a normal return.
         */
        if (err == UC_ERR_INSN_INVALID && pc >= EXT_HEAP_ADDR && pc + 2 < EXT_BASE_ADDR + EXT_MEM_SIZE) {
            uint8_t *p = (uint8_t *)arm_ptr(m, pc - 2);
            if (p) {
                uint16_t prev_hw = (uint16_t)(p[0] | (p[1] << 8));
                uint32_t top5 = prev_hw >> 11;
                if (top5 == 0x1d || top5 == 0x1e || top5 == 0x1f) {
                    if (getenv("VMRP_ARM_EXT_TRACE")) {
                        printf("arm_ext_executor: plugin returned to mid-Thumb2 pc=0x%X sp=0x%X (treated as clean exit)\n", pc, cur_sp);
                        dump_pc_ring(m);
                    }
                    reg_write32(m->uc, UC_ARM_REG_PC, EXT_STOP_ADDR);
                    return MR_SUCCESS;
                }
            }
        }
        /*
         * gghjt 60s 超时返回路径：wrapper 的定时器内联分发跳转到堆上的
         * 定时器/链表节点（如 0x738B58 含 FB FF FF FF …）而非代码。
         *
         * 正常流程中 wrapper 在分发前会先完成 dump0 读取（0x96000 字节
         * → 0x646100），还原游戏内存后分发到的回调指向有效游戏代码。
         * 超时路径中 dump0 虽已打开但大块读取尚未发生就崩溃了。
         *
         * 修复：从宿主侧完成 dump0 读取 + RW 区清理（与正常路径中
         * table[14] memset(0x645C70,0,0x80) / memset(0x645CF0,0,0x20) 等
         * 效），然后终止本次 ARM 执行。下一次定时器触发时 wrapper 会
         * 基于已恢复的游戏状态分发到有效游戏代码，继续加载 pak 资源，
         * 实现「返回游戏菜单」而非重启。
         */
        if (err == UC_ERR_INSN_INVALID && pc >= EXT_HEAP_ADDR && pc < EXT_BASE_ADDR + EXT_MEM_SIZE) {
            uint32_t lr = reg_read32(m->uc, UC_ARM_REG_LR);
            /* LR 在 wrapper 代码范围内，说明是 wrapper 内联分发到了堆上
             * 的无效回调地址——netpay 超时返回的标志性崩溃模式 */
            int lr_in_wrapper = (lr & ~1u) >= EXT_CODE_ADDR &&
                                (lr & ~1u) < EXT_CODE_ADDR + m->code_len;
            /* wrapper 内部 loader BLX 到 game code 时 LR 自然指向 wrapper，
             * 不能把这种合法跳转当作无效回调。game code 在 pending file 范围内
             * 时排除 lr_in_wrapper 判定，让后续 "fallback to LR" 继续执行。 */
            if (lr_in_wrapper && m->pending_internal_file_addr &&
                pc >= m->pending_internal_file_addr &&
                pc < m->pending_internal_file_addr + m->pending_internal_file_len) {
                lr_in_wrapper = 0;
            }
            uint8_t *pcp = (uint8_t *)arm_ptr(m, pc);
            if (pcp && ((pcp[0] == 0xFF && pcp[1] == 0xFF) ||
                       (pcp[0] == 0x00 && pcp[1] == 0x00 && pcp[2] == 0x00 && pcp[3] == 0x00) ||
                       lr_in_wrapper)) {
                if (getenv("VMRP_ARM_EXT_TRACE")) {
                    printf("arm_ext_executor: plugin returned to heap data pc=0x%X sp=0x%X (treated as clean exit)\n", pc, cur_sp);
                    dump_pc_ring(m);
                }
                reg_write32(m->uc, UC_ARM_REG_PC, EXT_STOP_ADDR);
                return MR_SUCCESS;
            }
        }
        /*
         * Some plug-in callbacks (e.g. the netpay timer event) corrupt
         * their own stack in our emulator, popping a return address that
         * lands inside ARM ext data (commonly a copy of the mrp pathname).
         * Detect that pattern -- PC sitting on the stack page near SP --
         * and treat it as a clean stop so the caller can carry on; we'd
         * otherwise just spam an Unhandled CPU exception line every tick.
         */
        if (err == UC_ERR_EXCEPTION) {
            uint32_t sp_diff = (pc > cur_sp) ? pc - cur_sp : cur_sp - pc;
            if (sp_diff < 0x4000) {
                if (getenv("VMRP_ARM_EXT_TRACE")) {
                    printf("arm_ext_executor: stack-corrupt plugin callback pc=0x%X sp=0x%X (treated as clean exit)\n", pc, cur_sp);
                    dump_pc_ring(m);
                }
                reg_write32(m->uc, UC_ARM_REG_PC, EXT_STOP_ADDR);
                return MR_SUCCESS;
            }
        }
        printf("arm_ext_executor: uc_emu_start(0x%X) failed: %u (%s)\n", start, err, uc_strerror(err));
        uint8_t *pc_mem = arm_ptr(m, pc);
        if (pc_mem) {
            printf("arm_ext_executor: pc bytes @0x%X:", pc);
            for (uint32_t i = 0; i < 16; ++i) printf(" %02X", pc_mem[i]);
            printf("\n");
        }
        dump_pc_ring(m);
        dumpREG(m->uc);
        return MR_FAILED;
    }
    return MR_SUCCESS;
}

static int run_arm(ArmExtModule *m, uint32_t start) {
    return run_arm_with_sp(m, start, EXT_STACK_ADDR + EXT_STACK_SIZE);
}

static void restore_ext_r9(ArmExtModule *m) {
    if (!m) return;
    uint32_t p_addr = m->active_p_addr ? m->active_p_addr : m->p_addr;
    if (!p_addr) return;
    uint32_t rw_base = 0;
    memcpy(&rw_base, arm_ptr(m, p_addr), 4);
    if (rw_base) reg_write32(m->uc, UC_ARM_REG_R9, rw_base);
}

static char *arm_str(ArmExtModule *m, uint32_t addr) {
    char *p = (char *)arm_ptr(m, addr);
    return p ? p : (char *)"";
}

static ArmExtNestedModule *arm_ext_resource_owner_for_lr(ArmExtModule *m,
                                                         uint32_t *owner_p,
                                                         uint32_t *owner_helper) {
    uint32_t helper = 0;
    uint32_t p = 0;
    ArmExtNestedModule *owner = NULL;
    if (owner_p) *owner_p = 0;
    if (owner_helper) *owner_helper = 0;
    if (!m) return NULL;

    p = arm_ext_p_for_code_addr(m, reg_read32(m->uc, UC_ARM_REG_LR), &helper);
    owner = arm_ext_find_nested_module_by_p(m, p);
    if (owner_p) *owner_p = p;
    if (owner_helper) *owner_helper = helper;
    return owner;
}

static void *arm_ext_read_child_resource(ArmExtModule *m,
                                         ArmExtNestedModule *owner,
                                         const char *read_name,
                                         int *file_len,
                                         int lookfor,
                                         char *read_pack_host_path,
                                         uint32_t *read_pack_ram_addr,
                                         uint32_t *read_pack_ram_len) {
    void *hp = NULL;
    if (!m || !owner || !read_name) return NULL;
    /*
     * Private loaders give each child a stable package owner when the staged
     * executable is byte-proven.  A later shared table[100] value can still be
     * the outer app because wrappers reuse one writable pack_filename buffer;
     * child resource calls are scoped by LR to the child's recorded owner.
     */
    if (owner->package_ram_addr && owner->package_ram_len &&
        arm_ptr(m, owner->package_ram_addr)) {
        hp = mr_readFile_from_ram(read_name, file_len, lookfor,
                                  arm_ptr(m, owner->package_ram_addr),
                                  (int)owner->package_ram_len);
        if (hp) {
            if (read_pack_ram_addr) *read_pack_ram_addr = owner->package_ram_addr;
            if (read_pack_ram_len) *read_pack_ram_len = owner->package_ram_len;
        }
        return hp;
    }
    if (owner->package_host_path[0]) {
        hp = mr_readFile_from_pack(owner->package_host_path, read_name,
                                   file_len, lookfor);
        if (hp && read_pack_host_path) {
            snprintf(read_pack_host_path, PATH_MAX, "%s",
                     owner->package_host_path);
        }
    }
    return hp;
}

static int arm_ext_child_has_package_owner(ArmExtModule *m,
                                           ArmExtNestedModule *owner) {
    if (!owner) return 0;
    if (owner->package_host_path[0]) return 1;
    return owner->package_ram_addr && owner->package_ram_len &&
           arm_ptr(m, owner->package_ram_addr);
}


static int format_arm(ArmExtModule *m, char *dst, size_t dst_size, const char *fmt, unsigned first_arg) {
    size_t out = 0;
    unsigned ai = first_arg;
    for (const char *p = fmt; *p && out + 1 < dst_size; ++p) {
        if (*p != '%') {
            dst[out++] = *p;
            continue;
        }
        p++;
        if (*p == '%') { dst[out++] = '%'; continue; }
        char spec[32];
        size_t si = 0;
        spec[si++] = '%';
        while (*p && strchr("-+ #0", *p) && si + 1 < sizeof(spec)) spec[si++] = *p++;
        while (*p && isdigit((unsigned char)*p) && si + 1 < sizeof(spec)) spec[si++] = *p++;
        if (*p == 'l' && si + 1 < sizeof(spec)) spec[si++] = *p++;
        if (!*p) break;
        spec[si++] = *p;
        spec[si] = '\0';
        char tmp[512];
        switch (*p) {
            case 's': {
                uint32_t av = arg_read(m, ai++);
                snprintf(tmp, sizeof(tmp), spec, arm_str(m, av));
            } break;
            case 'c': {
                uint32_t av = arg_read(m, ai++);
                snprintf(tmp, sizeof(tmp), spec, (int)av);
            } break;
            case 'p': {
                uint32_t av = arg_read(m, ai++);
                snprintf(tmp, sizeof(tmp), "0x%X", av);
            } break;
            case 'd': case 'i': case 'u': case 'x': case 'X': case 'o': {
                uint32_t av = arg_read(m, ai++);
                snprintf(tmp, sizeof(tmp), spec, av);
            } break;
            default:
                /* 原生 sprintf(src/mythroad/printf.c 的 default 分支)对
                 * 未知转换符只输出该字符本身且【不消耗可变参数】。此前
                 * 这里把未知 spec 直接交给宿主 snprintf:glibc 会把 %m
                 * 展开成 strerror(errno)(真实视频驱动的 socket I/O 常置
                 * errno,展开成 33 字节长串),且多消耗一个参数使后续
                 * %d 读到错位的表槽值(0x10044→"65604")。talkcat 的
                 * "%m%d.jpg" 路径名因此从原生的 "m5.jpg" 级长度膨胀到
                 * 42 字节,溢出 guest 30 字节栈缓冲,覆盖相邻指针后在
                 * 定时器回调触发 UC_MEM_WRITE_UNMAPPED addr=0x3036353A
                 * (= 溢出文本 "6560"+4)。验证:真实视频驱动下启动
                 * talkcat 不再于 ~4.5s 崩溃,headless PPM 正常。 */
                tmp[0] = *p;
                tmp[1] = '\0';
                break;
        }
        size_t l = strlen(tmp);
        if (l > dst_size - out - 1) l = dst_size - out - 1;
        memcpy(dst + out, tmp, l);
        out += l;
    }
    dst[out] = '\0';
    return (int)out;
}

/* ---- MRP 文件缓存实现 ---- */

static uint32_t mrp_rd32le(const uint8_t *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static void mrp_cache_free(ArmExtModule *m);

static const char *path_basename(const char *path) {
    const char *p = strrchr(path, '/');
    const char *q = strrchr(path, '\\');
    if (q && (!p || q > p)) p = q;
    return p ? p + 1 : path;
}

static int path_has_directory_component(const char *path) {
    if (!path) return 0;
    for (const char *p = path; *p; ++p) {
        if (*p == '/' || *p == '\\')
            return 1;
    }
    return 0;
}

static int mrp_path_equal(const char *a, const char *b) {
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

/* 解析 MRP 并缓存所有条目（解压 gzip） */
static void parse_mrp_cache(ArmExtModule *m, const char *mrp_path) {
    mrp_cache_free(m);
    m->mrp_cache_count = 0;
    if (!mrp_path || !*mrp_path) return;

    FILE *fp = vmrp_host_fopen(mrp_path, "rb");
    if (!fp) return;
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (sz <= 240) { fclose(fp); return; }

    uint8_t *raw = (uint8_t *)malloc((size_t)sz);
    if (!raw) { fclose(fp); return; }
    if (fread(raw, 1, (size_t)sz, fp) != (size_t)sz) { free(raw); fclose(fp); return; }
    fclose(fp);

    if (memcmp(raw, "MRPG", 4) != 0) { free(raw); return; }

    uint32_t pos = 240;
    while (pos + 16 <= (uint32_t)sz) {
        uint32_t name_len = mrp_rd32le(raw + pos);
        pos += 4;
        if (name_len == 0 || name_len > 255 || pos + name_len + 12 > (uint32_t)sz) break;
        char entry_name[256];
        uint32_t copy_len = name_len < sizeof(entry_name) - 1 ? name_len : sizeof(entry_name) - 1;
        memcpy(entry_name, raw + pos, copy_len);
        entry_name[copy_len] = '\0';
        /* 去除尾部 null（MRP 条目名含 null 终止符算在 name_len 中） */
        if (copy_len > 0 && entry_name[copy_len - 1] == '\0') copy_len--;
        pos += name_len;
        uint32_t off = mrp_rd32le(raw + pos);
        uint32_t packed_len = mrp_rd32le(raw + pos + 4);
        pos += 12;
        if (off >= (uint32_t)sz || packed_len > (uint32_t)sz - off) continue;

        if (m->mrp_cache_count == m->mrp_cache_capacity) {
            int new_capacity = m->mrp_cache_capacity ? m->mrp_cache_capacity * 2 : 32;
            MrpCacheEntry *new_cache = (MrpCacheEntry *)realloc(
                m->mrp_cache, (size_t)new_capacity * sizeof(*new_cache));
            if (!new_cache) break;
            /* New slots must start empty because cleanup walks only committed
             * entries; zeroing also keeps partially decoded entries harmless
             * until mrp_cache_count is advanced. */
            memset(new_cache + m->mrp_cache_capacity, 0,
                   (size_t)(new_capacity - m->mrp_cache_capacity) * sizeof(*new_cache));
            m->mrp_cache = new_cache;
            m->mrp_cache_capacity = new_capacity;
        }

        MrpCacheEntry *e = &m->mrp_cache[m->mrp_cache_count];
        memset(e, 0, sizeof(*e));
        strncpy(e->name, entry_name, sizeof(e->name) - 1);
        e->name[sizeof(e->name) - 1] = '\0';

        /* gzip 压缩的条目在宿主侧解压 */
        if (packed_len >= 2 && raw[off] == 0x1f && raw[off + 1] == 0x8b) {
            z_stream zs;
            memset(&zs, 0, sizeof(zs));
            if (inflateInit2(&zs, 16 + MAX_WBITS) != Z_OK) continue;
            uint32_t cap = packed_len * 4 + 4096;
            uint8_t *out = (uint8_t *)malloc(cap);
            if (!out) { inflateEnd(&zs); continue; }
            zs.next_in = raw + off;
            zs.avail_in = packed_len;
            int zret;
            do {
                if (zs.total_out == cap) {
                    uint32_t new_cap = cap * 2;
                    uint8_t *nb = (uint8_t *)realloc(out, new_cap);
                    if (!nb) { free(out); out = NULL; break; }
                    out = nb;
                    cap = new_cap;
                }
                zs.next_out = out + zs.total_out;
                zs.avail_out = cap - (uint32_t)zs.total_out;
                zret = inflate(&zs, Z_NO_FLUSH);
            } while (zret == Z_OK);
            if (!out || zret != Z_STREAM_END) { free(out); inflateEnd(&zs); continue; }
            e->data = out;
            e->data_len = (uint32_t)zs.total_out;
            inflateEnd(&zs);
        } else {
            e->data = (uint8_t *)malloc(packed_len);
            if (!e->data) continue;
            memcpy(e->data, raw + off, packed_len);
            e->data_len = packed_len;
        }
        m->mrp_cache_count++;
    }
    free(raw);
}

/* 按 basename 在缓存中查找条目 */
static MrpCacheEntry *mrp_cache_find(ArmExtModule *m, const char *filename) {
    if (!filename || !*filename) return NULL;
    for (int i = 0; i < m->mrp_cache_count; i++) {
        if (mrp_path_equal(m->mrp_cache[i].name, filename))
            return &m->mrp_cache[i];
    }

    /* Only basename-match direct resource names.  A request containing a
     * directory component is a filesystem-state probe on real devices; answering
     * it from the package cache can make first-run extractors believe their
     * persistent files already exist while companion files are still missing. */
    if (path_has_directory_component(filename))
        return NULL;

    const char *base = path_basename(filename);
    for (int i = 0; i < m->mrp_cache_count; i++) {
        if (strcmp(path_basename(m->mrp_cache[i].name), base) == 0)
            return &m->mrp_cache[i];
    }
    return NULL;
}

/* 分配一个虚拟 fd 并绑定到指定数据 */
static uint32_t mrp_vfd_open(ArmExtModule *m, const uint8_t *data, uint32_t len) {
    for (int i = 0; i < MRP_VFD_MAX; i++) {
        if (!m->mrp_vfds[i].in_use) {
            m->mrp_vfds[i].in_use = 1;
            m->mrp_vfds[i].data = data;
            m->mrp_vfds[i].data_len = len;
            m->mrp_vfds[i].pos = 0;
            return MRP_VFD_BASE + (uint32_t)i + 1;
        }
    }
    return 0;
}

static MrpVirtualFd *mrp_vfd_get(ArmExtModule *m, uint32_t fd) {
    if (fd <= MRP_VFD_BASE || fd > MRP_VFD_BASE + MRP_VFD_MAX) return NULL;
    MrpVirtualFd *v = &m->mrp_vfds[fd - MRP_VFD_BASE - 1];
    return v->in_use ? v : NULL;
}

static void mrp_cache_free(ArmExtModule *m) {
    for (int i = 0; i < m->mrp_cache_count; i++) {
        free(m->mrp_cache[i].data);
        m->mrp_cache[i].data = NULL;
    }
    free(m->mrp_cache);
    m->mrp_cache = NULL;
    m->mrp_cache_count = 0;
    m->mrp_cache_capacity = 0;
}

/* ---- 结束 MRP 文件缓存 ---- */

static void write_table_entry(ArmExtModule *m, uint32_t index, uint32_t value) {
    memcpy((uint8_t *)arm_ptr(m, EXT_TABLE_ADDR + index * 4), &value, 4);
}

static uint32_t alloc_u32_slot(ArmExtModule *m, uint32_t value) {
    uint32_t slot = arm_alloc(m, 4);
    if (slot) memcpy(arm_ptr(m, slot), &value, 4);
    return slot;
}

static uint32_t alloc_string(ArmExtModule *m, const char *value) {
    if (!value) value = "";
    size_t len = strlen(value) + 1;
    uint32_t slot = arm_alloc(m, (uint32_t)len);
    if (slot) memcpy(arm_ptr(m, slot), value, len);
    return slot;
}

static uint32_t alloc_bytes(ArmExtModule *m, const void *value, uint32_t len) {
    uint32_t slot = arm_alloc(m, len);
    if (slot && value && len) memcpy(arm_ptr(m, slot), value, len);
    return slot;
}

static uint32_t arm_ext_pack_table_slot(ArmExtModule *m) {
    if (!m) return 0;
    if (!m->pack_table_addr) {
        m->pack_table_addr = arm_alloc(m, ARM_EXT_PACK_TABLE_SIZE);
    }
    return m->pack_table_addr;
}

static void arm_ext_set_pack_table_name(ArmExtModule *m, const char *name) {
    uint32_t slot = arm_ext_pack_table_slot(m);
    char *dst;
    if (!m || !slot) return;
    dst = (char *)arm_ptr(m, slot);
    if (!dst) return;
    /*
     * Native table[100] points at writable pack_filename[128], not an
     * immutable exact-length string.  EXT helpers copy shorter package names
     * such as c:/mythroad/ into that storage; zero-fill first so stale bytes
     * from a previous longer host path cannot survive past the new NUL.
     */
    memset(dst, 0, ARM_EXT_PACK_TABLE_SIZE);
    snprintf(dst, ARM_EXT_PACK_TABLE_SIZE, "%s", name ? name : "");
    write_table_entry(m, 100, slot);
}

static void arm_ext_set_child_record_pack_name(ArmExtModule *m,
                                               uint32_t file_addr,
                                               const char *name) {
    if (!m || !file_addr || !name || !name[0]) return;
    uint32_t record = arm_ext_read_u32_or_zero_(m, file_addr);
    if (!record || !arm_ptr(m, record + 100u * 4u)) return;
    ArmExtNestedModule *mod = arm_ext_find_nested_module(m, file_addr + 8u);
    uint32_t slot = (mod && mod->file_addr == file_addr)
                  ? mod->pack_name_addr
                  : 0;
    if (!slot) {
        uint32_t existing = arm_ext_read_u32_or_zero_(
            m, record + 100u * 4u);
        if (existing >= EXT_EXECUTOR_META_ADDR &&
            existing < EXT_EXECUTOR_META_ADDR + EXT_EXECUTOR_META_SIZE &&
            arm_ptr(m, existing)) {
            slot = existing;
        }
    }
    if (!slot) {
        slot = arm_ext_meta_alloc(m, ARM_EXT_PACK_TABLE_SIZE);
    }
    if (!slot) return;
    if (mod && mod->file_addr == file_addr)
        mod->pack_name_addr = slot;
    char *dst = (char *)arm_ptr(m, slot);
    if (!dst) return;
    /*
     * Private loaders give each child a per-module record that mirrors the
     * EXT table.  DOTA frame.ext's package getter at 0x2C916C reads
     * record[100], not the shared EXT_TABLE[100], before the fixed 32-byte
     * copies at 0x2C9810.  brwmain.ext startup repeats the same ABI hazard at
     * 0x2D1E0C: memset 32 bytes, strlen(pack), memcpy(strlen).  Keep that
     * ARM-visible record slot short too; host readFile ownership still comes
     * from the recorded package provenance.
     * The pointer is executor metadata, not a late low-heap allocation: the
     * browser crash proved record[100] at child P+0x14 can be executed as a
     * wrapper callback when it is placed immediately after the private P.
     */
    memset(dst, 0, ARM_EXT_PACK_TABLE_SIZE);
    snprintf(dst, ARM_EXT_PACK_TABLE_SIZE, "%s", name);
    memcpy(arm_ptr(m, record + 100u * 4u), &slot, 4);
}

static void internal_slot_write(ArmExtModule *m, uint32_t slot, uint32_t value) {
    if (slot) memcpy(arm_ptr(m, slot), &value, 4);
}

static int internal_slot_read(ArmExtModule *m, uint32_t slot, uint32_t *value) {
    if (!m || !slot || !value || !arm_ptr(m, slot)) return 0;
    memcpy(value, arm_ptr(m, slot), 4);
    return 1;
}

static uint32_t arm_ext_map_read_file_host_ptr(ArmExtModule *m,
                                               const void *host_ptr,
                                               const void *read_file_ret,
                                               uint32_t arm_ret) {
    if (!m || !host_ptr) return 0;
    if (read_file_ret && host_ptr == read_file_ret) return arm_ret;
    return arm_addr(m, host_ptr);
}

static void arm_ext_sync_read_file_gzip_slots(ArmExtModule *m,
                                              const void *read_file_ret,
                                              uint32_t arm_ret) {
    if (!m || !read_file_ret || !arm_ret) return;

    uint32_t arm_gzin = arm_ext_map_read_file_host_ptr(
        m, mr_gzInBuf, read_file_ret, arm_ret);
    uint32_t arm_gzout = arm_ext_map_read_file_host_ptr(
        m, mr_gzOutBuf, read_file_ret, arm_ret);
    if (arm_gzin) {
        internal_slot_write(m, m->mr_gzin_buf_slot, arm_gzin);
    }
    if (arm_gzout) {
        internal_slot_write(m, m->mr_gzout_buf_slot, arm_gzout);
    }
    internal_slot_write(m, m->lg_gzinptr_slot, (uint32_t)LG_gzinptr);
    internal_slot_write(m, m->lg_gzoutcnt_slot, (uint32_t)LG_gzoutcnt);
}

static int arm_ext_host_lg_mem_contains(const void *p, uint32_t len) {
    const char *c = (const char *)p;
    if (!c || !LG_mem_base || !LG_mem_end || c < LG_mem_base || c >= LG_mem_end)
        return 0;
    if (!len) return 1;
    return (size_t)(LG_mem_end - c) >= (size_t)len;
}

static void arm_ext_release_host_read_file_buffer(const void *hp,
                                                  uint32_t len) {
    /*
     * table[125] copies host _mr_readFile output into ARM-visible LG_mem before
     * returning.  The original host mr_malloc buffer is therefore only bridge
     * staging; ARM code can later free the ARM copy, but it can never free this
     * host allocation.  Releasing it here keeps repeated download/cancel loops
     * from exhausting host LG_mem while preserving direct RAM-package pointers.
     */
    if (len && arm_ext_host_lg_mem_contains(hp, len)) {
        mr_free((void *)hp, len);
    }
}

/*
 * 已删除 arm_ext_mirror_read_file_to_ram_source():该逻辑在 table[125]
 * (mr_readFile_from_ram, pack='$') 返回后,把解压输出(fl 字节)memcpy 回
 * guest 传入的压缩源缓冲区(raml 字节)。原生实现(src/mythroad/mythroad.c
 * _mr_readFile 的 '$' 分支)解压到新分配的 mr_gzOutBuf 并返回该指针,
 * 从不改写 RAM 源镜像。由于压缩数据必然 fl > raml,该回写每次越界
 * fl-raml 字节,连续踩坏 guest 堆中相邻的位图数据(talkcat 主界面短横线
 * 花屏)和 wrapper 元数据(偶发 0xE83A55 定时器派发后 PC 落入栈页的
 * UC_ERR_INSN_INVALID 崩溃)。验证:清空 workdir 运行 talkcat.mrp,
 * PPM 无横线噪点且无 uc_emu_start 失败。
 */

static void sync_internal_state_to_arm(ArmExtModule *m) {
    if (!m) return;
    internal_slot_write(m, m->mr_state_slot, (uint32_t)mr_state);
    internal_slot_write(m, m->mr_timer_state_slot, (uint32_t)mr_timer_state);
}

static void sync_timer_state_from_arm(ArmExtModule *m) {
    enum { ARM_TIMER_IDLE = 0, ARM_TIMER_RUNNING = 1 };
    uint32_t arm_timer_state = 0;
    if (!internal_slot_read(m, m ? m->mr_timer_state_slot : 0, &arm_timer_state)) {
        return;
    }
    if (arm_timer_state <= 3) {
        if (m->host_timer_pending &&
            mr_timer_state == ARM_TIMER_RUNNING &&
            arm_timer_state == ARM_TIMER_IDLE) {
            internal_slot_write(m, m->mr_timer_state_slot, ARM_TIMER_RUNNING);
            return;
        }
        mr_timer_state = (int32)arm_timer_state;
        if (mr_timer_state != ARM_TIMER_RUNNING) {
            m->host_timer_pending = 0;
        }
    }
}

static void arm_ext_record_timer_owner(ArmExtModule *m) {
    if (!m) return;
    uint32_t lr = reg_read32(m->uc, UC_ARM_REG_LR) & ~1u;
    uint32_t owner_p = 0;
    uint32_t owner_helper = 0;
    int owner_from_current_dispatch = 0;

    owner_p = arm_ext_p_for_code_addr(m, lr, &owner_helper);
    if (owner_p == m->p_addr && owner_helper == m->helper_addr &&
        m->current_p_addr && m->current_helper_addr &&
        (m->current_p_addr != m->p_addr ||
         m->current_helper_addr != m->helper_addr)) {
        /*
         * Some wrapper EXT builds expose table[31] through a cfunction.ext
         * veneer.  While dispatching a child helper, LR then points into the
         * wrapper even though the timer belongs to the child that made the
         * call.  Prefer the active helper context only for that wrapper-veneer
         * case; direct calls from game/child code keep the LR-derived owner.
         */
        owner_p = m->current_p_addr;
        owner_helper = m->current_helper_addr;
        owner_from_current_dispatch = 1;
    }
    if (!owner_p || !owner_helper) {
        /*
         * table[31] owner is normally identified from LR's code range. The
         * current/active path is kept for host-synthetic entries whose LR is
         * outside every loaded EXT image. Verification: gxdzc e2e payment
         * routing remains stable.
         */
        owner_p = m->current_p_addr ? m->current_p_addr :
                  (m->active_p_addr ? m->active_p_addr : m->p_addr);
        owner_helper = m->current_helper_addr ? m->current_helper_addr :
                       (m->active_helper_addr ? m->active_helper_addr : m->helper_addr);
        owner_from_current_dispatch = m->current_p_addr && m->current_helper_addr;
    }

    if (owner_p && owner_helper) {
        m->timer_p_addr = owner_p;
        m->timer_helper_addr = owner_helper;
        if (getenv("VMRP_ARM_EXT_DIAG")) {
            printf("DIAG timer_owner lr=0x%X ownerP=0x%X ownerH=0x%X fromCurrent=%d activeP=0x%X activeH=0x%X currentP=0x%X currentH=0x%X\n",
                   lr, owner_p, owner_helper, owner_from_current_dispatch,
                   m->active_p_addr, m->active_helper_addr,
                   m->current_p_addr, m->current_helper_addr);
        }
    }
}

/* origin_mem 统计同步。
 * 宿主 table[0]/table[1]/table[2] 使用 arm_alloc 而非 origin_mem
 * 池本身来分配内存，ARM 侧自带的 free-list 分配器因此不会运行，
 * 对应的 slot 值也不会被 ARM 代码更新。这里在宿主侧 accounting
 * 后立即将最新值写回 ARM slot，让 ext 读到一致的统计。 */
static void sync_origin_mem_slots(ArmExtModule *m) {
    if (m->origin_mem_left_slot)
        memcpy(arm_ptr(m, m->origin_mem_left_slot), &m->origin_mem_left, 4);
    if (m->origin_mem_min_slot)
        memcpy(arm_ptr(m, m->origin_mem_min_slot), &m->origin_mem_min, 4);
    if (m->origin_mem_top_slot)
        memcpy(arm_ptr(m, m->origin_mem_top_slot), &m->origin_mem_top, 4);
}

/*
 * Guest 侧 LG_mem first-fit 分配器,链表操作逐语句对应 src/mythroad/mem.c
 * 的 mr_malloc/mr_free。原生 ABI 中 table[0]/[1]/[2] 就是这三个函数,且
 * table[146] 直接暴露 &LG_mem_free 头节点,应用可以读改空闲链表。
 * mythroad.c:1290 记载的"骚操作"依赖该语义:应用先 mr_free 一块自有
 * 内存到空闲链表,再调 readFile(table[125]),其内部 mr_malloc(解压
 * 输出)按 first-fit 必然落回该块,应用随后【丢弃 readFile 返回值】
 * 直接从预测地址读取像素(talkcat 图标路径,cfunction.ext 0xE82744
 * 反汇编确认 0xE827A8 调用后 r0 未被保存)。
 * 此前 table[0] 用 arm_alloc(bump,从不复用)、table[1] 仅做统计,
 * 空闲链表从不运转,预测地址处残留的 MRP 索引字节被当作位图渲染,
 * 即 talkcat 主界面短横线花屏的根因。
 *
 * 容量设计(兼容性权衡,经 e2e 回归校准):
 * - 空闲链表只管理 512KB origin_mem 池,首次尝试从池内 first-fit 分配,
 *   使"free 后 readFile 复用"的地址预测在池内成立;
 * - 池内无合适块时由调用方退回 arm_alloc(bump)。此前 table[0] 一直是
 *   bump(实际容量 16MB),dota/opbzqe/optwar 等游戏的内存检测和资源
 *   加载依赖远超 512KB 的累计分配,若像真机一样在池满时直接失败会
 *   黑屏(e2e 全量回归证实);bump 后备保持与修复前完全相同的容量。
 * - LG_mem_left/min/top 统计沿用 note_origin_mem_alloc/free 的既有账本
 *   (所有 table[0]/[1] 调用无论落在池内或 bump 都记账),与修复前的
 *   ARM 可见值一致,不影响依赖该预算的游戏(见 origin_mem_len 注释)。
 * 验证:talkcat.mrp 主界面 PPM 无花屏;e2e 全量回归与基线一致。
 */

/* mem.c realLGmemSize:空闲节点 {next,len} 占 8 字节,块长按 8 对齐 */
#define ARM_EXT_LG_MEM_ALIGN(x) (((x) + 7u) & ~7u)

static uint32_t arm_ext_guest_mem_read_u32(ArmExtModule *m, uint32_t addr) {
    uint32_t v = 0;
    void *p = arm_ptr(m, addr);
    if (p) memcpy(&v, p, 4);
    return v;
}

static void arm_ext_guest_mem_write_u32(ArmExtModule *m,
                                        uint32_t addr,
                                        uint32_t v) {
    void *p = arm_ptr(m, addr);
    if (p) memcpy(p, &v, 4);
}

static void note_origin_mem_alloc(ArmExtModule *m, uint32_t len) {
    if (!m->origin_mem_addr) return;
    len = align4(len ? len : 1);
    m->origin_mem_left = len < m->origin_mem_left ? m->origin_mem_left - len : 0;
    if (m->origin_mem_left < m->origin_mem_min) {
        m->origin_mem_min = m->origin_mem_left;
        m->origin_mem_top = m->origin_mem_len - m->origin_mem_min;
    }
    sync_origin_mem_slots(m);
}

static void note_origin_mem_free(ArmExtModule *m, uint32_t len) {
    if (!m->origin_mem_addr) return;
    m->origin_mem_left += align4(len);
    if (m->origin_mem_left > m->origin_mem_len) m->origin_mem_left = m->origin_mem_len;
    sync_origin_mem_slots(m);
}

static uint32_t arm_ext_guest_mem_malloc(ArmExtModule *m, uint32_t len) {
    if (!m || !m->origin_mem_addr || !m->origin_mem_head_addr) return 0;
    uint32_t base = m->origin_mem_addr;
    uint32_t end = base + m->origin_mem_len;
    len = ARM_EXT_LG_MEM_ALIGN(len);
    if (!len) return 0; /* mem.c: invalid memory request */
    if (base + arm_ext_guest_mem_read_u32(m, m->origin_mem_head_addr) > end) {
        return 0; /* mem.c: corrupted memory */
    }
    /* previous 为头节点或空闲节点地址,两者均为 {next@+0, len@+4} 布局 */
    uint32_t previous = m->origin_mem_head_addr;
    uint32_t nextfree = base + arm_ext_guest_mem_read_u32(m, previous);
    /* 节点最小 8 字节,合法链表节点数不可能超过 pool_len/8;超过即链表
     * 成环(guest 已破坏链表),按 mem.c 的 corrupted memory 处理返回 0,
     * 避免宿主死循环。 */
    uint32_t iter_limit = m->origin_mem_len / 8u + 2u;
    while (nextfree < end && iter_limit--) {
        uint32_t protected_lo = 0;
        uint32_t protected_hi = 0;
        if (arm_ext_find_first_registered_code_overlap(m, nextfree, 8u,
                                                       &protected_lo,
                                                       &protected_hi)) {
            return 0; /* protected executable bytes cannot be free-list metadata */
        }
        uint32_t node_next = arm_ext_guest_mem_read_u32(m, nextfree);
        uint32_t node_len = arm_ext_guest_mem_read_u32(m, nextfree + 4);
        if (!node_len || node_len > end - nextfree) {
            return 0; /* mem.c: corrupted memory */
        }
        uint32_t alloc_addr = 0;
        if (!arm_ext_first_unprotected_subrange(m, nextfree, node_len, len,
                                                &alloc_addr)) {
            previous = nextfree;
            nextfree = base + node_next;
            continue;
        }
        if (alloc_addr == nextfree && node_len == len) {
            arm_ext_guest_mem_write_u32(m, previous, node_next);
            return nextfree;
        }
        if (alloc_addr == nextfree) {
            /* 拆分:剩余部分在 nextfree+len 处形成新空闲节点 */
            uint32_t node_end = nextfree + node_len;
            uint32_t l = nextfree + len;
            uint32_t right_len = node_end - l;
            uint32_t protected_lo = 0;
            uint32_t protected_hi = 0;
            if (right_len &&
                arm_ext_find_first_registered_code_overlap(
                    m, l, right_len, &protected_lo, &protected_hi) &&
                protected_lo < l + 8u) {
                l = protected_hi;
                right_len = l < node_end ? node_end - l : 0;
            }
            if (right_len) {
                arm_ext_guest_mem_write_u32(m, l, node_next);
                arm_ext_guest_mem_write_u32(m, l + 4, right_len);
                arm_ext_guest_mem_write_u32(m, previous, l - base);
            } else {
                arm_ext_guest_mem_write_u32(m, previous, node_next);
            }
            return nextfree;
        }
        {
            uint32_t node_end = nextfree + node_len;
            uint32_t alloc_end = alloc_addr + len;
            uint32_t left_len = alloc_addr - nextfree;
            uint32_t right_len = node_end - alloc_end;
            arm_ext_guest_mem_write_u32(m, nextfree + 4, left_len);
            if (right_len) {
                uint32_t protected_lo = 0;
                uint32_t protected_hi = 0;
                if (arm_ext_find_first_registered_code_overlap(
                        m, alloc_end, right_len, &protected_lo,
                        &protected_hi) &&
                    protected_lo < alloc_end + 8u) {
                    alloc_end = protected_hi;
                    right_len = alloc_end < node_end ? node_end - alloc_end : 0;
                }
            }
            if (right_len) {
                arm_ext_guest_mem_write_u32(m, alloc_end, node_next);
                arm_ext_guest_mem_write_u32(m, alloc_end + 4, right_len);
                arm_ext_guest_mem_write_u32(m, nextfree, alloc_end - base);
            } else {
                arm_ext_guest_mem_write_u32(m, nextfree, node_next);
            }
            return alloc_addr;
        }
        previous = nextfree;
        nextfree = base + node_next;
    }
    return 0; /* mem.c: no memory2(池内无足够大的块,调用方退回 bump) */
}

static void arm_ext_guest_mem_free_unprotected(ArmExtModule *m, uint32_t p,
                                               uint32_t len) {
    if (!m || !m->origin_mem_addr || !m->origin_mem_head_addr) return;
    uint32_t base = m->origin_mem_addr;
    uint32_t end = base + m->origin_mem_len;
    len = ARM_EXT_LG_MEM_ALIGN(len);
    if (!len || !p || p < base || p >= end || p + len > end ||
        p + len <= base) {
        return; /* mem.c: mr_free invalid */
    }
    /* 按地址序找到插入位置:freep < p <= n */
    uint32_t freep = m->origin_mem_head_addr;
    uint32_t n = base + arm_ext_guest_mem_read_u32(m, freep);
    uint32_t iter_limit = m->origin_mem_len / 8u + 2u;
    while (n < end && n < p && iter_limit--) {
        uint32_t protected_lo = 0;
        uint32_t protected_hi = 0;
        if (arm_ext_find_first_registered_code_overlap(m, n, 8u,
                                                       &protected_lo,
                                                       &protected_hi)) {
            return; /* existing list node overlaps executable storage */
        }
        freep = n;
        n = base + arm_ext_guest_mem_read_u32(m, n);
    }
    if (!(iter_limit + 1u)) return; /* 链表成环,同 malloc 的 corrupted 处理 */
    if (n < end) {
        uint32_t protected_lo = 0;
        uint32_t protected_hi = 0;
        if (arm_ext_find_first_registered_code_overlap(m, n, 8u,
                                                       &protected_lo,
                                                       &protected_hi)) {
            return;
        }
    }
    if (p == freep || p == n) return; /* mem.c: already free */
    if (freep != m->origin_mem_head_addr &&
        freep + arm_ext_guest_mem_read_u32(m, freep + 4) == p) {
        /* 与前一空闲块连续:并入 */
        arm_ext_guest_mem_write_u32(
            m, freep + 4,
            arm_ext_guest_mem_read_u32(m, freep + 4) + len);
    } else {
        /* 插入新空闲节点 */
        arm_ext_guest_mem_write_u32(m, freep, p - base);
        arm_ext_guest_mem_write_u32(m, p, n - base);
        arm_ext_guest_mem_write_u32(m, p + 4, len);
        freep = p;
    }
    if (n < end && p + len == n) {
        /* 与后一空闲块连续:并入 */
        arm_ext_guest_mem_write_u32(
            m, freep, arm_ext_guest_mem_read_u32(m, n));
        arm_ext_guest_mem_write_u32(
            m, freep + 4,
            arm_ext_guest_mem_read_u32(m, freep + 4) +
                arm_ext_guest_mem_read_u32(m, n + 4));
    }
    /* 统计由调用方的 note_origin_mem_free 记账(见分配器头注释) */
}

static void arm_ext_guest_mem_free(ArmExtModule *m, uint32_t p, uint32_t len) {
    if (!m || !m->origin_mem_addr || !m->origin_mem_head_addr) return;
    uint32_t base = m->origin_mem_addr;
    uint32_t end = base + m->origin_mem_len;
    len = ARM_EXT_LG_MEM_ALIGN(len);
    if (!len || !p || p < base || p >= end || p + len > end ||
        p + len <= base) {
        return; /* mem.c: mr_free invalid */
    }

    /*
     * Do not write LG_mem free-list nodes over registered executable images.
     * Some wrappers expose subranges of a large app arena through table[146];
     * once such a subrange becomes an EXT image, freeing the containing arena
     * must return only the non-code fragments to app allocation.
     */
    uint32_t cursor = p;
    uint32_t range_end = p + len;
    while (cursor < range_end) {
        uint32_t protected_lo = 0;
        uint32_t protected_hi = 0;
        uint32_t remaining = range_end - cursor;
        if (!arm_ext_find_first_registered_code_overlap(m, cursor, remaining,
                                                        &protected_lo,
                                                        &protected_hi)) {
            if (remaining >= 8u)
                arm_ext_guest_mem_free_unprotected(m, cursor, remaining);
            break;
        }
        if (cursor < protected_lo && protected_lo - cursor >= 8u) {
            arm_ext_guest_mem_free_unprotected(m, cursor,
                                               protected_lo - cursor);
        }
        if (protected_hi <= cursor) break;
        cursor = protected_hi;
    }
}

/*
 * bump 后备块回收器(宿主侧,见 arm_ext_internal.h 中 bump_live 注释)。
 * 原生设备 LG_mem 仅 512KB,应用长流程完全依赖 free 后复用;此前
 * bump 后备块的 free 是空操作,talkcat 喝水资源包安装的反复大块
 * readFile 使 bump 只增不减耗尽 16MB 并撞入 EXT 栈区。live 表登记
 * 应用可见的 bump 块;free 只回收登记过的地址(长度取登记值),
 * guest 无法借伪造 free 回收执行器内部分配。空闲表按地址有序并
 * 合并相邻块,复用时 first-fit 拆分。
 * 验证:talkcat 喝水资源包下载安装 e2e 通过,全量 e2e 回归。
 */

static int arm_ext_bump_block_append(ArmExtBumpBlock **arr,
                                     uint32_t *count,
                                     uint32_t *cap,
                                     uint32_t idx,
                                     uint32_t addr,
                                     uint32_t len) {
    if (*count == *cap) {
        uint32_t new_cap = *cap ? *cap * 2 : 32;
        ArmExtBumpBlock *p =
            (ArmExtBumpBlock *)realloc(*arr, new_cap * sizeof(**arr));
        if (!p) return 0;
        *arr = p;
        *cap = new_cap;
    }
    memmove(*arr + idx + 1, *arr + idx, (*count - idx) * sizeof(**arr));
    (*arr)[idx].addr = addr;
    (*arr)[idx].len = len;
    (*count)++;
    return 1;
}

static void arm_ext_bump_block_remove(ArmExtBumpBlock *arr,
                                      uint32_t *count,
                                      uint32_t idx) {
    memmove(arr + idx, arr + idx + 1, (*count - idx - 1) * sizeof(*arr));
    (*count)--;
}

static int arm_ext_bump_free_insert(ArmExtModule *m, uint32_t addr,
                                    uint32_t len) {
    if (!m || !addr || !len) return 0;
    uint32_t i = 0;
    while (i < m->bump_free_count && m->bump_free_blocks[i].addr < addr) i++;
    if (i > 0 && m->bump_free_blocks[i - 1].addr +
                     m->bump_free_blocks[i - 1].len == addr) {
        m->bump_free_blocks[i - 1].len += len;
        if (i < m->bump_free_count &&
            m->bump_free_blocks[i - 1].addr + m->bump_free_blocks[i - 1].len ==
                m->bump_free_blocks[i].addr) {
            m->bump_free_blocks[i - 1].len += m->bump_free_blocks[i].len;
            arm_ext_bump_block_remove(m->bump_free_blocks,
                                      &m->bump_free_count, i);
        }
        return 1;
    }
    if (i < m->bump_free_count &&
        addr + len == m->bump_free_blocks[i].addr) {
        m->bump_free_blocks[i].addr = addr;
        m->bump_free_blocks[i].len += len;
        return 1;
    }
    return arm_ext_bump_block_append(&m->bump_free_blocks,
                                     &m->bump_free_count,
                                     &m->bump_free_cap, i, addr, len);
}

static void arm_ext_bump_block_remove_range(ArmExtBumpBlock **arr,
                                            uint32_t *count,
                                            uint32_t *cap,
                                            uint32_t range_addr,
                                            uint32_t range_len) {
    if (!arr || !*arr || !count || !range_addr || !range_len) return;
    uint64_t range_start = range_addr;
    uint64_t range_end = range_start + range_len;
    for (uint32_t i = 0; i < *count;) {
        ArmExtBumpBlock *b = &(*arr)[i];
        uint64_t block_start = b->addr;
        uint64_t block_end = block_start + b->len;
        uint64_t lo = block_start > range_start ? block_start : range_start;
        uint64_t hi = block_end < range_end ? block_end : range_end;
        if (lo >= hi) {
            i++;
            continue;
        }
        if (lo <= block_start && hi >= block_end) {
            arm_ext_bump_block_remove(*arr, count, i);
            continue;
        }
        if (lo <= block_start) {
            b->addr = (uint32_t)hi;
            b->len = (uint32_t)(block_end - hi);
            i++;
            continue;
        }
        if (hi >= block_end) {
            b->len = (uint32_t)(lo - block_start);
            i++;
            continue;
        }
        uint32_t right_addr = (uint32_t)hi;
        uint32_t right_len = (uint32_t)(block_end - hi);
        b->len = (uint32_t)(lo - block_start);
        if (cap) {
            arm_ext_bump_block_append(arr, count, cap, i + 1,
                                      right_addr, right_len);
        }
        i += 2;
    }
}

static int arm_ext_bump_track(ArmExtModule *m, uint32_t addr, uint32_t len) {
    return arm_ext_bump_block_append(&m->bump_live, &m->bump_live_count,
                                     &m->bump_live_cap, m->bump_live_count,
                                     addr, align4(len ? len : 1));
}

static uint32_t arm_ext_bump_reuse(ArmExtModule *m, uint32_t len) {
    len = align4(len ? len : 1);
    for (uint32_t i = 0; i < m->bump_free_count; ++i) {
        ArmExtBumpBlock *b = &m->bump_free_blocks[i];
        if (b->len < len) continue;
        uint32_t addr = b->addr;
        if (b->len - len >= 8) {
            b->addr += len;
            b->len -= len;
        } else {
            /* 剩余不足以成块,整块给出(与池分配器拆分粒度一致) */
            len = b->len;
            arm_ext_bump_block_remove(m->bump_free_blocks,
                                      &m->bump_free_count, i);
        }
        if (!arm_ext_bump_track(m, addr, len)) return 0;
        return addr;
    }
    return 0;
}

static int arm_ext_bump_recycle(ArmExtModule *m, uint32_t addr) {
    uint32_t live_idx = 0;
    uint32_t len = 0;
    for (; live_idx < m->bump_live_count; ++live_idx) {
        if (m->bump_live[live_idx].addr == addr) {
            len = m->bump_live[live_idx].len;
            break;
        }
    }
    if (!len) return 0; /* 未登记的地址:不属于应用的 bump 块,忽略 */
    arm_ext_bump_block_remove(m->bump_live, &m->bump_live_count, live_idx);

    /* 按地址有序插入空闲表并与前后相邻块合并,但剔除已登记 EXT 映像。
     * bump 后备块也可能承载 readFile 得到的子模块;注册为代码后不能再
     * 因应用 free 而复用成像素/资源缓冲。 */
    uint32_t cursor = addr;
    uint32_t range_end = addr + len;
    while (cursor < range_end) {
        uint32_t protected_lo = 0;
        uint32_t protected_hi = 0;
        uint32_t remaining = range_end - cursor;
        if (!arm_ext_find_first_registered_code_overlap(m, cursor, remaining,
                                                        &protected_lo,
                                                        &protected_hi)) {
            arm_ext_bump_free_insert(m, cursor, remaining);
            break;
        }
        if (cursor < protected_lo)
            arm_ext_bump_free_insert(m, cursor, protected_lo - cursor);
        if (protected_hi <= cursor) break;
        cursor = protected_hi;
    }
    return 1;
}

static void arm_ext_protect_registered_module_storage(ArmExtModule *m,
                                                      uint32_t file_addr,
                                                      uint32_t file_len) {
    if (!m || !file_addr || !file_len) return;
    /*
     * A registered module's file range is instruction/literal storage, even if
     * it originally came from an app-visible readFile allocation.  Drop that
     * interval from bump live/free tracking so later app frees cannot recycle
     * executable bytes as ordinary data.
     */
    arm_ext_bump_block_remove_range(&m->bump_live, &m->bump_live_count,
                                    &m->bump_live_cap, file_addr, file_len);
    arm_ext_bump_block_remove_range(&m->bump_free_blocks, &m->bump_free_count,
                                    &m->bump_free_cap, file_addr, file_len);
}

/* 应用可见内存分配/释放的统一入口(table[0]/[2]/[125] 输出):
 * 池 first-fit(地址预测成立)→ bump 空闲块复用 → bump 新块。 */
static uint32_t arm_ext_app_mem_malloc(ArmExtModule *m, uint32_t len) {
    uint32_t ret = arm_ext_guest_mem_malloc(m, len);
    if (ret) arm_ext_watch_alloc_report(m, "pool_malloc", ret, len);
    if (!ret) {
        ret = arm_ext_bump_reuse(m, len);
        if (ret) arm_ext_watch_alloc_report(m, "bump_reuse", ret, len);
    }
    if (!ret) {
        ret = arm_alloc(m, len);
        if (ret) arm_ext_watch_alloc_report(m, "bump_new", ret, len);
        if (ret && !arm_ext_bump_track(m, ret, len)) {
            /* 登记失败(宿主内存不足)时块仍有效,只是不可回收 */
        }
    }
    return ret;
}

static void arm_ext_app_mem_free(ArmExtModule *m, uint32_t p, uint32_t len) {
    if (!m || !p) return;
    if (m->origin_mem_addr && p >= m->origin_mem_addr &&
        p < m->origin_mem_addr + m->origin_mem_len) {
        arm_ext_watch_alloc_report(m, "pool_free", p, len);
        arm_ext_guest_mem_free(m, p, len);
        return;
    }
    arm_ext_watch_alloc_report(m, "bump_recycle", p, len ? len : 4);
    arm_ext_bump_recycle(m, p);
}

static int arm_ext_is_standard_ext_self_pointer(uint32_t value) {
    if (value < EXT_TABLE_ADDR ||
        value >= EXT_TABLE_ADDR + EXT_TABLE_COUNT * 4u) {
        return 0;
    }
    uint32_t idx = (value - EXT_TABLE_ADDR) / 4u;
    return value == EXT_TABLE_ADDR + idx * 4u;
}

/* 读取 game 的定时器链表头。不同版本的 game.ext SDK 把 timer head 写在
 * game_rw 的不同偏移（0x8C 或 0x88），这里依次尝试两个已知偏移。 */
static uint32_t read_game_timer_head(ArmExtModule *m, uint32_t grw) {
    if (!grw) return 0;
    uint32_t val = 0;
    if (arm_ptr(m, grw + GAME_TIMER_HEAD_OFFSET))
        memcpy(&val, arm_ptr(m, grw + GAME_TIMER_HEAD_OFFSET), 4);
    if (val && !arm_ext_is_standard_ext_self_pointer(val)) return val;
    val = 0;
    if (arm_ptr(m, grw + GAME_TIMER_HEAD_OFFSET_ALT))
        memcpy(&val, arm_ptr(m, grw + GAME_TIMER_HEAD_OFFSET_ALT), 4);
    return arm_ext_is_standard_ext_self_pointer(val) ? 0 : val;
}

/* 写回 game 的定时器链表头。使用与 read 相同的偏移搜索逻辑：
 * 首先尝试主偏移 0x8C，若该处本身就有值则写到 0x8C；
 * 否则写到备选偏移 0x88。 */
static void write_game_timer_head(ArmExtModule *m, uint32_t grw, uint32_t val) {
    if (!grw) return;
    uint32_t cur = 0;
    if (arm_ptr(m, grw + GAME_TIMER_HEAD_OFFSET))
        memcpy(&cur, arm_ptr(m, grw + GAME_TIMER_HEAD_OFFSET), 4);
    if ((cur && !arm_ext_is_standard_ext_self_pointer(cur)) ||
        !arm_ptr(m, grw + GAME_TIMER_HEAD_OFFSET_ALT)) {
        memcpy(arm_ptr(m, grw + GAME_TIMER_HEAD_OFFSET), &val, 4);
    } else {
        memcpy(arm_ptr(m, grw + GAME_TIMER_HEAD_OFFSET_ALT), &val, 4);
    }
}

static void arm_ext_free_row_spans(ArmExtRowSpans *spans) {
    if (!spans) return;
    free(spans->min_x);
    free(spans->max_x);
    spans->min_x = NULL;
    spans->max_x = NULL;
    spans->rows = 0;
}

static void arm_ext_clear_row_spans(ArmExtRowSpans *spans) {
    if (!spans || !spans->min_x || !spans->max_x) return;
    for (uint32_t y = 0; y < spans->rows; ++y) {
        spans->min_x[y] = UINT16_MAX;
        spans->max_x[y] = 0;
    }
}

static int arm_ext_ensure_row_spans(ArmExtRowSpans *spans, uint32_t rows) {
    if (!spans || rows == 0) return 0;
    if (spans->rows == rows && spans->min_x && spans->max_x) return 1;

    arm_ext_free_row_spans(spans);
    spans->min_x = (uint16_t *)malloc(rows * sizeof(uint16_t));
    spans->max_x = (uint16_t *)malloc(rows * sizeof(uint16_t));
    if (!spans->min_x || !spans->max_x) {
        arm_ext_free_row_spans(spans);
        return 0;
    }
    spans->rows = rows;
    arm_ext_clear_row_spans(spans);
    return 1;
}

static int arm_ext_has_row_spans(const ArmExtRowSpans *spans) {
    if (!spans || !spans->min_x || !spans->max_x) return 0;
    for (uint32_t y = 0; y < spans->rows; ++y) {
        if (spans->min_x[y] < spans->max_x[y]) return 1;
    }
    return 0;
}

static int arm_ext_ensure_screen_regions(ArmExtModule *m) {
    if (!m || m->screen_h <= 0) return 0;
    uint32_t rows = (uint32_t)m->screen_h;
    return arm_ext_ensure_row_spans(&m->screen_damage, rows) &&
           arm_ext_ensure_row_spans(&m->screen_present, rows);
}

static void arm_ext_clear_screen_regions(ArmExtModule *m) {
    if (!arm_ext_ensure_screen_regions(m)) return;
    arm_ext_clear_row_spans(&m->screen_damage);
    arm_ext_clear_row_spans(&m->screen_present);
}

static int arm_ext_ensure_foreground_cover_regions(ArmExtModule *m) {
    if (!m || m->screen_h <= 0) return 0;
    if (!arm_ext_ensure_row_spans(&m->foreground_cover, (uint32_t)m->screen_h))
        return 0;
    if (!arm_ext_has_row_spans(&m->foreground_cover))
        arm_ext_clear_row_spans(&m->foreground_cover);
    return 1;
}

static void arm_ext_clear_foreground_cover_regions(ArmExtModule *m) {
    if (!arm_ext_ensure_foreground_cover_regions(m)) return;
    arm_ext_clear_row_spans(&m->foreground_cover);
}

static void arm_ext_diag_dump_layer_state(ArmExtModule *m, const char *tag) {
    if (!m || !getenv("VMRP_ARM_EXT_DIAG")) return;
    uint32_t primary_chunk = 0;
    uint32_t active_chunk = 0;
    uint32_t primary_rw = 0;
    uint32_t active_rw = 0;
    if (m->primary_p_addr && arm_ptr(m, m->primary_p_addr + 12)) {
        memcpy(&primary_rw, arm_ptr(m, m->primary_p_addr), 4);
        memcpy(&primary_chunk, arm_ptr(m, m->primary_p_addr + 12), 4);
    }
    if (m->active_p_addr && arm_ptr(m, m->active_p_addr + 12)) {
        memcpy(&active_rw, arm_ptr(m, m->active_p_addr), 4);
        memcpy(&active_chunk, arm_ptr(m, m->active_p_addr + 12), 4);
    }
    uint32_t pc = reg_read32(m->uc, UC_ARM_REG_PC);
    uint32_t lr = reg_read32(m->uc, UC_ARM_REG_LR);
    uint32_t r9 = reg_read32(m->uc, UC_ARM_REG_R9);
    uint16_t cover0 = UINT16_MAX;
    uint16_t cover1 = 0;
    if (arm_ext_ensure_foreground_cover_regions(m) &&
        m->foreground_cover.rows > 27) {
        cover0 = m->foreground_cover.min_x[27];
        cover1 = m->foreground_cover.max_x[27];
    }
    uint32_t p24 = 0, p28 = 0, p34 = 0;
    uint32_t a24 = 0, a28 = 0, a34 = 0;
    if (primary_chunk) {
        p24 = arm_ext_read_u32_or_zero_(m, primary_chunk + 0x24);
        p28 = arm_ext_read_u32_or_zero_(m, primary_chunk + 0x28);
        p34 = arm_ext_read_u32_or_zero_(m, primary_chunk + 0x34);
    }
    if (active_chunk) {
        a24 = arm_ext_read_u32_or_zero_(m, active_chunk + 0x24);
        a28 = arm_ext_read_u32_or_zero_(m, active_chunk + 0x28);
        a34 = arm_ext_read_u32_or_zero_(m, active_chunk + 0x34);
    }
    printf("DIAG layer %s pc=0x%X lr=0x%X r9=0x%X activeP=0x%X activeH=0x%X activeRW=0x%X activeChunk=0x%X active[24]=0x%X active[28]=0x%X active[34]=0x%X primaryP=0x%X primaryH=0x%X primaryRW=0x%X primaryChunk=0x%X primary[24]=0x%X primary[28]=0x%X primary[34]=0x%X timerP=0x%X timerH=0x%X cover27=%u..%u fgOwnerP=0x%X fgOwnerH=0x%X fgValid=%d hostTimer=%d mrTimer=%d\n",
           tag ? tag : "?",
           pc, lr, r9,
           m->active_p_addr, m->active_helper_addr, active_rw, active_chunk,
           a24, a28, a34,
           m->primary_p_addr, m->primary_helper_addr, primary_rw, primary_chunk,
           p24, p28, p34,
           m->timer_p_addr, m->timer_helper_addr,
           cover0, cover1,
           m->foreground_screen_owner_p_addr,
           m->foreground_screen_owner_helper_addr,
           (m->foreground_screen_owner_p_addr != 0),
           m->host_timer_pending, mr_timer_state);
}

static void arm_ext_diag_dump_timer_node(ArmExtModule *m,
                                         const char *tag,
                                         const char *name,
                                         uint32_t node) {
    if (!m || !getenv("VMRP_ARM_EXT_DIAG") || !node) return;
    if (!arm_ptr(m, node) || !arm_ptr(m, node + 0x1Cu)) {
        printf("DIAG timer_node %s %s=0x%X unmapped\n",
               tag ? tag : "?", name ? name : "?", node);
        return;
    }
    uint32_t w[8];
    for (uint32_t i = 0; i < 8; ++i)
        w[i] = arm_ext_read_u32_or_zero_(m, node + i * 4u);
    printf("DIAG timer_node %s %s=0x%X [00]=0x%X [04]=0x%X [08]=0x%X [0C]=0x%X [10]=0x%X [14]=0x%X [18]=0x%X [1C]=0x%X\n",
           tag ? tag : "?", name ? name : "?", node,
           w[0], w[1], w[2], w[3], w[4], w[5], w[6], w[7]);
}

static void arm_ext_diag_dump_wrapper_compact_timer_nodes(ArmExtModule *m,
                                                         const char *tag) {
    if (!m || !getenv("VMRP_ARM_EXT_TIMER_LIVENESS_DIAG") ||
        !m->wrapper_compact_timer_scheduler_off) {
        return;
    }

    uint32_t wrapper_rw = arm_ext_wrapper_rw_base_(m);
    uint32_t off = m->wrapper_compact_timer_scheduler_off;
    if (!wrapper_rw || !arm_ptr(m, wrapper_rw + off + 0x14u)) return;

    uint32_t queued = arm_ext_read_u32_or_zero_(m, wrapper_rw + off + 0x0Cu);
    uint32_t current = arm_ext_read_u32_or_zero_(m, wrapper_rw + off + 0x10u);
    uint32_t last_tick = arm_ext_read_u32_or_zero_(m, wrapper_rw + off + 0x14u);
    printf("DIAG wrapper_compact_nodes %s rw=0x%X off=0x%X queued=0x%X current=0x%X lastTick=0x%X\n",
           tag ? tag : "?", wrapper_rw, off, queued, current, last_tick);

    uint32_t node = queued;
    for (uint32_t i = 0; i < 4 && node && arm_ptr(m, node + 0x1Cu); ++i) {
        uint32_t marker = arm_ext_read_u32_or_zero_(m, node);
        uint32_t delay = arm_ext_read_u32_or_zero_(m, node + 0x08u);
        uint32_t cb = arm_ext_read_u32_or_zero_(m, node + 0x0Cu);
        uint32_t data = arm_ext_read_u32_or_zero_(m, node + 0x10u);
        uint32_t cleanup = arm_ext_read_u32_or_zero_(m, node + 0x14u);
        uint32_t next = arm_ext_read_u32_or_zero_(m, node + 0x18u);
        uint32_t due_next = arm_ext_read_u32_or_zero_(m, node + 0x1Cu);
        uint32_t cb_owner_h = 0;
        uint32_t cb_owner_p = arm_ext_p_for_code_addr(m, cb & ~1u, &cb_owner_h);
        printf("DIAG wrapper_compact_node %s list=queued idx=%u node=0x%X marker=0x%X delay=0x%X cb=0x%X data=0x%X cleanup=0x%X next=0x%X dueNext=0x%X cbP=0x%X cbH=0x%X\n",
               tag ? tag : "?", i, node, marker, delay, cb, data, cleanup,
               next, due_next, cb_owner_p, cb_owner_h);
        node = next;
    }

    node = current;
    for (uint32_t i = 0; i < 4 && node && arm_ptr(m, node + 0x1Cu); ++i) {
        uint32_t marker = arm_ext_read_u32_or_zero_(m, node);
        uint32_t delay = arm_ext_read_u32_or_zero_(m, node + 0x08u);
        uint32_t cb = arm_ext_read_u32_or_zero_(m, node + 0x0Cu);
        uint32_t data = arm_ext_read_u32_or_zero_(m, node + 0x10u);
        uint32_t cleanup = arm_ext_read_u32_or_zero_(m, node + 0x14u);
        uint32_t next = arm_ext_read_u32_or_zero_(m, node + 0x18u);
        uint32_t due_next = arm_ext_read_u32_or_zero_(m, node + 0x1Cu);
        uint32_t cb_owner_h = 0;
        uint32_t cb_owner_p = arm_ext_p_for_code_addr(m, cb & ~1u, &cb_owner_h);
        printf("DIAG wrapper_compact_node %s list=current idx=%u node=0x%X marker=0x%X delay=0x%X cb=0x%X data=0x%X cleanup=0x%X next=0x%X dueNext=0x%X cbP=0x%X cbH=0x%X\n",
               tag ? tag : "?", i, node, marker, delay, cb, data, cleanup,
               next, due_next, cb_owner_p, cb_owner_h);
        node = due_next;
    }
}

static void arm_ext_diag_dump_rw_timer_state(ArmExtModule *m,
                                             const char *tag,
                                             uint32_t rw_base) {
    if (!m || !getenv("VMRP_ARM_EXT_DIAG") || !rw_base) return;
    if (!arm_ptr(m, rw_base) || !arm_ptr(m, rw_base + 0x1Cu)) {
        printf("DIAG rw_low %s rw=0x%X unmapped\n",
               tag ? tag : "?", rw_base);
        return;
    }
    uint32_t w[8];
    for (uint32_t i = 0; i < 8; ++i)
        w[i] = arm_ext_read_u32_or_zero_(m, rw_base + i * 4u);
    /*
     * Keep the low words because older helper variants put timer queues near
     * R9+8, but do not treat them as authoritative for every private child.
     */
    printf("DIAG rw_low %s rw=0x%X +00=0x%X +04=0x%X +08=0x%X +0C=0x%X +10=0x%X +14=0x%X +18=0x%X +1C=0x%X\n",
           tag ? tag : "?", rw_base,
           w[0], w[1], w[2], w[3], w[4], w[5], w[6], w[7]);
    arm_ext_diag_dump_timer_node(m, tag, "rw+08", w[2]);
    arm_ext_diag_dump_timer_node(m, tag, "rw+0C", w[3]);
    /*
     * DOTA's browser frame helper disassembly proves a different timer base:
     * 0x2C96A0 loads literal 0x94, adds R9, then uses [base+8] for the queued
     * head, [base+12] for the active list and [base+16] for the last tick.
     * Dump that structural window for all private children so the evidence is
     * about the module ABI, not a filename or app-specific branch.
     */
    if (arm_ptr(m, rw_base + 0x94u) && arm_ptr(m, rw_base + 0xC0u)) {
        uint32_t fw[12];
        for (uint32_t i = 0; i < 12; ++i)
            fw[i] = arm_ext_read_u32_or_zero_(m, rw_base + 0x94u + i * 4u);
        printf("DIAG rw_frame %s rw=0x%X +94=0x%X +98=0x%X +9C=0x%X +A0=0x%X +A4=0x%X +A8=0x%X +AC=0x%X +B0=0x%X +B4=0x%X +B8=0x%X +BC=0x%X +C0=0x%X\n",
               tag ? tag : "?", rw_base,
               fw[0], fw[1], fw[2], fw[3], fw[4], fw[5],
               fw[6], fw[7], fw[8], fw[9], fw[10], fw[11]);
        arm_ext_diag_dump_timer_node(m, tag, "rw+9C", fw[2]);
        arm_ext_diag_dump_timer_node(m, tag, "rw+A0", fw[3]);
    }
    if (arm_ptr(m, rw_base + 0x1B0u) && arm_ptr(m, rw_base + 0x200u)) {
        uint32_t cb[8];
        for (uint32_t i = 0; i < 8; ++i)
            cb[i] = arm_ext_read_u32_or_zero_(m, rw_base + 0x1C0u + i * 4u);
        printf("DIAG rw_cb %s rw=0x%X +1C0=0x%X +1C4=0x%X +1C8=0x%X +1CC=0x%X +1D0=0x%X +1D4=0x%X +1D8=0x%X +1DC=0x%X\n",
               tag ? tag : "?", rw_base,
               cb[0], cb[1], cb[2], cb[3], cb[4], cb[5], cb[6], cb[7]);
    }
    if (arm_ptr(m, rw_base + 0x0C0u) && arm_ptr(m, rw_base + 0x0E4u)) {
        uint32_t q[10];
        for (uint32_t i = 0; i < 10; ++i)
            q[i] = arm_ext_read_u32_or_zero_(m, rw_base + 0x0C0u + i * 4u);
        /*
         * Compact scheduler variant whose walker loads R9+0xC0 before using
         * +8/+12/+16 as queue/current/last-tick state.
         */
        printf("DIAG rw_sched_low %s rw=0x%X +0C0=0x%X +0C4=0x%X +0C8=0x%X +0CC=0x%X +0D0=0x%X +0D4=0x%X +0D8=0x%X +0DC=0x%X +0E0=0x%X +0E4=0x%X\n",
               tag ? tag : "?", rw_base,
               q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8], q[9]);
        arm_ext_diag_dump_timer_node(m, tag, "rw+0C8", q[2]);
        arm_ext_diag_dump_timer_node(m, tag, "rw+0CC", q[3]);
    }
    if (arm_ptr(m, rw_base + 0x248u) && arm_ptr(m, rw_base + 0x26Cu)) {
        uint32_t q[10];
        for (uint32_t i = 0; i < 10; ++i)
            q[i] = arm_ext_read_u32_or_zero_(m, rw_base + 0x248u + i * 4u);
        /*
         * Some private modal children use a compact scheduler just past their
         * copied bridge table.  verdload.ext's code=2 walker loads R9+0x248
         * and then consumes +8/+12/+16 as queue/current/last-tick state.
         */
        printf("DIAG rw_sched %s rw=0x%X +248=0x%X +24C=0x%X +250=0x%X +254=0x%X +258=0x%X +25C=0x%X +260=0x%X +264=0x%X +268=0x%X +26C=0x%X\n",
               tag ? tag : "?", rw_base,
               q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8], q[9]);
        arm_ext_diag_dump_timer_node(m, tag, "rw+250", q[2]);
        arm_ext_diag_dump_timer_node(m, tag, "rw+254", q[3]);
    }
    ArmExtNestedModule *owner = arm_ext_find_nested_module_by_rw(m, rw_base);
    if (owner) {
        uint32_t record = arm_ext_read_u32_or_zero_(m, owner->file_addr);
        uint32_t slot0 = arm_ext_read_u32_or_zero_(m, record + 0u * 4u);
        uint32_t slot3 = arm_ext_read_u32_or_zero_(m, record + 3u * 4u);
        uint32_t slot25 = arm_ext_read_u32_or_zero_(m, record + 25u * 4u);
        uint32_t slot100 = arm_ext_read_u32_or_zero_(m, record + 100u * 4u);
        uint32_t slot125 = arm_ext_read_u32_or_zero_(m, record + 125u * 4u);
        uint32_t slot131 = arm_ext_read_u32_or_zero_(m, record + 131u * 4u);
        /*
         * frame.ext 0x2C93E0 dispatches through record[125] and 0x2C97C8
         * stores the result through its caller-provided R2.  These few slots
         * prove whether the private module record still points at wrapper
         * callbacks or has been restored to host table bridges.
        */
        printf("DIAG record %s file=0x%X record=0x%X slot0=0x%X slot3=0x%X slot25=0x%X slot100=0x%X slot125=0x%X slot131=0x%X\n",
               tag ? tag : "?", owner->file_addr, record,
               slot0, slot3, slot25, slot100, slot125, slot131);
    }
}

static void arm_ext_diag_dump_wrapper_timer_state(ArmExtModule *m,
                                                  const char *tag) {
    if (!m || !getenv("VMRP_ARM_EXT_DIAG")) return;
    uint32_t wrapper_rw = arm_ext_wrapper_rw_base_(m);
    if (!wrapper_rw) return;

    if (m->wrapper_compact_timer_scheduler_off &&
        arm_ptr(m, wrapper_rw + m->wrapper_compact_timer_scheduler_off + 0x24u)) {
        uint32_t off = m->wrapper_compact_timer_scheduler_off;
        uint32_t q[10];
        for (uint32_t i = 0; i < 10; ++i)
            q[i] = arm_ext_read_u32_or_zero_(m, wrapper_rw + off + i * 4u);
        /*
         * Discovered from the wrapper timer walker literal. The walker uses
         * +0x0c as queue head, +0x10 as active/due list, and +0x14 as last
         * tick, so dump this ABI before older fixed-offset guesses.
         */
        printf("DIAG wrapper_timer_compact %s rw=0x%X off=0x%X +00=0x%X +04=0x%X +08=0x%X +0C=0x%X +10=0x%X +14=0x%X +18=0x%X +1C=0x%X +20=0x%X +24=0x%X live=%d\n",
               tag ? tag : "?", wrapper_rw, off,
               q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8], q[9],
               arm_ext_wrapper_has_timer_queue(m));
        char label_queue[32];
        char label_current[32];
        snprintf(label_queue, sizeof(label_queue), "wrw+%03X+0C", off);
        snprintf(label_current, sizeof(label_current), "wrw+%03X+10", off);
        arm_ext_diag_dump_timer_node(m, tag, label_queue, q[3]);
        arm_ext_diag_dump_timer_node(m, tag, label_current, q[4]);
    }

    if (!arm_ptr(m, wrapper_rw + 0x3C8u) ||
        !arm_ptr(m, wrapper_rw + 0x3E8u)) {
        if (!m->wrapper_compact_timer_scheduler_off) {
            printf("DIAG wrapper_timer %s rw=0x%X unmapped\n",
                   tag ? tag : "?", wrapper_rw);
        }
        return;
    }

    uint32_t q[9];
    for (uint32_t i = 0; i < 9; ++i)
        q[i] = arm_ext_read_u32_or_zero_(m, wrapper_rw + 0x3C8u + i * 4u);
    /*
     * optwar cfunction.ext 0xE84920 loads R9+0x3C8 as the sorted timer queue
     * and R9+0x3D8 as the temporary due-list sentinel before invoking nodes.
     */
    printf("DIAG wrapper_timer %s rw=0x%X +3C8=0x%X +3CC=0x%X +3D0=0x%X +3D4=0x%X +3D8=0x%X +3DC=0x%X +3E0=0x%X +3E4=0x%X +3E8=0x%X live=%d\n",
           tag ? tag : "?", wrapper_rw,
           q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8],
           arm_ext_wrapper_has_timer_queue(m));
    arm_ext_diag_dump_timer_node(m, tag, "wrw+3C8", q[0]);
    arm_ext_diag_dump_timer_node(m, tag, "wrw+3D8", q[4]);
}

static void arm_ext_mark_row_spans(ArmExtRowSpans *spans,
                                   int32_t screen_w,
                                   int32_t screen_h,
                                   int32_t x,
                                   int32_t y,
                                   int32_t w,
                                   int32_t h) {
    if (!spans || !spans->min_x || !spans->max_x || spans->rows == 0 ||
        screen_w <= 0 ||
        screen_h <= 0 || w <= 0 || h <= 0) {
        return;
    }

    int32_t min_x = x < 0 ? 0 : x;
    int32_t min_y = y < 0 ? 0 : y;
    int32_t max_x = x + w;
    int32_t max_y = y + h;
    if (max_x > screen_w) max_x = screen_w;
    if (max_y > screen_h) max_y = screen_h;
    if (min_x >= max_x || min_y >= max_y) return;

    uint16_t ux0 = (uint16_t)min_x;
    uint16_t ux1 = (uint16_t)max_x;
    for (int32_t yy = min_y; yy < max_y && (uint32_t)yy < spans->rows; ++yy) {
        if (ux0 < spans->min_x[yy]) spans->min_x[yy] = ux0;
        if (ux1 > spans->max_x[yy]) spans->max_x[yy] = ux1;
    }
}

static void arm_ext_note_screen_damage_rect(ArmExtModule *m,
                                            int32_t x,
                                            int32_t y,
                                            int32_t w,
                                            int32_t h) {
    if (!m || !arm_ext_ensure_screen_regions(m)) return;
    arm_ext_mark_row_spans(&m->screen_damage, m->screen_w, m->screen_h,
                           x, y, w, h);
}

static void arm_ext_note_screen_damage_addr_range(ArmExtModule *m,
                                                  uint64_t address,
                                                  int size) {
    if (!m || !m->screen_addr || m->screen_w <= 0 || size <= 0 ||
        address < m->screen_addr) {
        return;
    }
    uint64_t start_byte = address - m->screen_addr;
    uint64_t end_byte = start_byte + (uint64_t)size - 1u;
    uint64_t max_byte = (uint64_t)m->screen_len - 1u;
    if (start_byte > max_byte) return;
    if (end_byte > max_byte) end_byte = max_byte;

    uint64_t start_pixel = start_byte / sizeof(uint16_t);
    uint64_t end_pixel = end_byte / sizeof(uint16_t);
    uint32_t screen_stride = (uint32_t)arm_ext_screen_stride(m);
    uint32_t y0 = (uint32_t)(start_pixel / screen_stride);
    uint32_t y1 = (uint32_t)(end_pixel / screen_stride);
    uint32_t x0 = (uint32_t)(start_pixel % screen_stride);
    uint32_t x1 = (uint32_t)(end_pixel % screen_stride) + 1u;
    if ((int32_t)x0 >= m->screen_w && (int32_t)x1 > m->screen_w) return;
    if ((int32_t)x0 >= m->screen_w) x0 = (uint32_t)m->screen_w;
    if ((int32_t)x1 > m->screen_w) x1 = (uint32_t)m->screen_w;

    if (y0 == y1) {
        arm_ext_note_screen_damage_rect(m, (int32_t)x0, (int32_t)y0,
                                        (int32_t)(x1 - x0), 1);
        return;
    }
    arm_ext_note_screen_damage_rect(m, (int32_t)x0, (int32_t)y0,
                                    m->screen_w - (int32_t)x0, 1);
    for (uint32_t y = y0 + 1u; y < y1; ++y) {
        arm_ext_note_screen_damage_rect(m, 0, (int32_t)y, m->screen_w, 1);
    }
    arm_ext_note_screen_damage_rect(m, 0, (int32_t)y1, (int32_t)x1, 1);
}

static void arm_ext_note_screen_present_rect(ArmExtModule *m,
                                             int32_t x,
                                             int32_t y,
                                             int32_t w,
                                             int32_t h) {
    if (!m || !arm_ext_ensure_screen_regions(m)) return;
    arm_ext_mark_row_spans(&m->screen_present, m->screen_w, m->screen_h,
                           x, y, w, h);
}

static void arm_ext_note_foreground_cover_rect(ArmExtModule *m,
                                               int32_t x,
                                               int32_t y,
                                               int32_t w,
                                               int32_t h) {
    if (!m || !arm_ext_ensure_foreground_cover_regions(m)) return;
    arm_ext_mark_row_spans(&m->foreground_cover, m->screen_w, m->screen_h,
                           x, y, w, h);
}

static int arm_ext_has_foreground_cover(ArmExtModule *m) {
    if (!m || !arm_ext_has_foreground_child(m) ||
        !arm_ext_ensure_foreground_cover_regions(m)) {
        return 0;
    }
    return arm_ext_has_row_spans(&m->foreground_cover);
}

static int arm_ext_suspend_depth_for_p(ArmExtModule *m,
                                       uint32_t p_addr,
                                       uint32_t *suspend_depth) {
    uint32_t ext_chunk = 0;
    if (suspend_depth) *suspend_depth = 0;
    if (m && p_addr && arm_ptr(m, p_addr + 12)) {
        memcpy(&ext_chunk, arm_ptr(m, p_addr + 12), 4);
    }
    if (!ext_chunk || !arm_ptr(m, ext_chunk + 0x34) || !suspend_depth) return 0;
    memcpy(suspend_depth, arm_ptr(m, ext_chunk + 0x34), 4);
    return 1;
}

static int arm_ext_foreground_cover_is_nonmodal(ArmExtModule *m) {
    uint32_t suspend_depth = 0;
    return arm_ext_has_foreground_child(m) &&
           arm_ext_suspend_depth_for_p(m, m->active_p_addr, &suspend_depth) &&
           suspend_depth == 0;
}

static void arm_ext_retire_nonmodal_foreground_cover_rect(ArmExtModule *m,
                                                          int32_t x,
                                                          int32_t y,
                                                          int32_t w,
                                                          int32_t h) {
    if (!arm_ext_has_foreground_cover(m) ||
        !arm_ext_foreground_cover_is_nonmodal(m)) {
        return;
    }

    int32_t min_x = x < 0 ? 0 : x;
    int32_t min_y = y < 0 ? 0 : y;
    int32_t max_x = x + w;
    int32_t max_y = y + h;
    if (max_x > m->screen_w) max_x = m->screen_w;
    if (max_y > m->screen_h) max_y = m->screen_h;
    if (min_x >= max_x || min_y >= max_y) return;

    int changed = 0;
    for (int32_t yy = min_y; yy < max_y; ++yy) {
        uint16_t c0 = m->foreground_cover.min_x[yy];
        uint16_t c1 = m->foreground_cover.max_x[yy];
        if (c0 >= c1 || max_x <= c0 || min_x >= c1) continue;

        /*
         * A readable extChunk[0x34] == 0 marks a non-modal overlay, not a
         * suspended page.  When the covered layer later explicitly presents a
         * rectangle over the overlay span, the visible owner for that span has
         * moved back to the covered layer.  Unknown or modal depths keep the
         * existing cover until wrapper resume clears active ownership.
         * Compatibility impact is limited to explicit present calls; cache-only
         * writes remain invisible behind foreground cover.  Verification is the
         * opbzqe RIGHT-key PPM check plus the existing plugin and pixel e2e
         * cases listed in docs/opbzqe-advbar-black-region-debug.md.
         */
        if (min_x <= c0 && max_x >= c1) {
            m->foreground_cover.min_x[yy] = UINT16_MAX;
            m->foreground_cover.max_x[yy] = 0;
            changed = 1;
        } else if (min_x <= c0) {
            m->foreground_cover.min_x[yy] = (uint16_t)max_x;
            changed = 1;
        } else if (max_x >= c1) {
            m->foreground_cover.max_x[yy] = (uint16_t)min_x;
            changed = 1;
        }
    }

    if (changed && !arm_ext_has_foreground_cover(m)) {
        m->foreground_screen_owner_p_addr = 0;
        m->foreground_screen_owner_helper_addr = 0;
    }
}

static int arm_ext_owner_is_foreground_child(ArmExtModule *m,
                                             uint32_t owner_p_addr,
                                             uint32_t owner_helper_addr) {
    return arm_ext_has_foreground_child(m) &&
           owner_p_addr &&
           (owner_p_addr == m->active_p_addr ||
            owner_helper_addr == m->active_helper_addr);
}

static int arm_ext_owner_is_covered_by_foreground(ArmExtModule *m,
                                                  uint32_t owner_p_addr,
                                                  uint32_t owner_helper_addr) {
    return arm_ext_has_foreground_cover(m) &&
           !arm_ext_owner_is_foreground_child(m, owner_p_addr,
                                              owner_helper_addr);
}

static void arm_ext_clear_empty_nonmodal_foreground_owner(ArmExtModule *m) {
    if (!m || !m->foreground_screen_owner_p_addr) return;
    if (m->foreground_screen_owner_p_addr != m->active_p_addr ||
        m->foreground_screen_owner_helper_addr != m->active_helper_addr) {
        return;
    }
    if (arm_ext_has_foreground_cover(m) ||
        !arm_ext_foreground_cover_is_nonmodal(m)) {
        return;
    }

    /*
     * Non-modal overlays such as advbar keep their EXT module active after the
     * covered game presents over the overlay rows.  Cache-only writes from the
     * still-active child can refresh foreground_screen_owner_* without adding
     * any foreground_cover rows, but owner metadata alone is not a visible
     * occlusion mask.  Once the row spans are empty, the lower layer must be
     * allowed to present normally; modal children keep using suspend depth.
     */
    m->foreground_screen_owner_p_addr = 0;
    m->foreground_screen_owner_helper_addr = 0;
}

static uint32_t arm_ext_screen_caller_owner(ArmExtModule *m,
                                            uint32_t *caller_helper_addr) {
    uint32_t helper_addr = 0;
    uint32_t p_addr = 0;
    if (m) {
        p_addr = arm_ext_p_for_code_addr(m, reg_read32(m->uc, UC_ARM_REG_LR),
                                         &helper_addr);
        if (!p_addr) {
            p_addr = m->current_p_addr;
            helper_addr = m->current_helper_addr;
        }
    }
    if (caller_helper_addr) *caller_helper_addr = helper_addr;
    return p_addr;
}

static void arm_ext_note_visible_present_rect(ArmExtModule *m,
                                              int32_t x,
                                              int32_t y,
                                              int32_t w,
                                              int32_t h) {
    arm_ext_note_screen_presented(m);
    arm_ext_note_screen_present_rect(m, x, y, w, h);
}

static uint16_t arm_ext_sample_present_pixel(ArmExtModule *m,
                                             const uint16_t *bmp,
                                             int32_t x,
                                             int32_t y) {
    if (!m || !bmp || x < 0 || y < 0 ||
        x >= m->screen_w || y >= m->screen_h) {
        return 0;
    }
    return bmp[(uint32_t)y * (uint32_t)arm_ext_screen_stride(m) +
               (uint32_t)x];
}

static void arm_ext_diag_visible_present(ArmExtModule *m,
                                         const char *tag,
                                         const uint16_t *bmp,
                                         int32_t x,
                                         int32_t y,
                                         int32_t w,
                                         int32_t h,
                                         int covered_by_foreground) {
    if (!m || !getenv("VMRP_ARM_EXT_SCREEN_DIAG")) return;
    if (!arm_ext_has_foreground_child(m) &&
        !(w >= m->screen_w && h >= m->screen_h)) {
        return;
    }

    uint32_t owner_helper = 0;
    uint32_t owner_p = arm_ext_screen_caller_owner(m, &owner_helper);
    uint16_t c_tl = arm_ext_sample_present_pixel(m, bmp, x, y);
    uint16_t c_mid = arm_ext_sample_present_pixel(
        m, bmp, m->screen_w / 2, m->screen_h / 2);
    uint16_t c_probe = arm_ext_sample_present_pixel(
        m, bmp, (m->screen_w * 2) / 3, (m->screen_h * 9) / 20);
    /*
     * Narrow visible-present signature for foreground handoff bugs.  It logs
     * only explicit host-visible submits, not every ARM framebuffer write.
     */
    printf("DIAG visible_present %s x=%d y=%d w=%d h=%d ownerP=0x%X ownerH=0x%X activeP=0x%X activeH=0x%X primaryP=0x%X primaryH=0x%X fgOwnerP=0x%X fgOwnerH=0x%X covered=%d samples=%04X/%04X/%04X\n",
           tag ? tag : "?", x, y, w, h, owner_p, owner_helper,
           m->active_p_addr, m->active_helper_addr,
           m->primary_p_addr, m->primary_helper_addr,
           m->foreground_screen_owner_p_addr,
           m->foreground_screen_owner_helper_addr,
           covered_by_foreground, c_tl, c_mid, c_probe);
}

typedef int (*ArmExtPresentSegmentFn)(ArmExtModule *m,
                                      void *ctx,
                                      int32_t x,
                                      int32_t y,
                                      int32_t w);

typedef struct ArmExtBitmapPresentCtx {
    uint16_t *bmp;
    int32_t dst_x;
    int32_t dst_y;
    int32_t source_stride;
    int32_t source_x;
    int32_t source_y;
} ArmExtBitmapPresentCtx;

typedef struct ArmExtDispupPresentCtx {
    uint16_t *screen;
    int32_t source_stride;
} ArmExtDispupPresentCtx;

static int arm_ext_submit_uncovered_present_segments(
    ArmExtModule *m,
    int32_t x,
    int32_t y,
    int32_t w,
    int32_t h,
    ArmExtPresentSegmentFn submit,
    void *submit_ctx) {
    if (!m || !submit || w <= 0 || h <= 0) return 0;

    int32_t min_x = x < 0 ? 0 : x;
    int32_t min_y = y < 0 ? 0 : y;
    int32_t max_x = x + w;
    int32_t max_y = y + h;
    if (max_x > m->screen_w) max_x = m->screen_w;
    if (max_y > m->screen_h) max_y = m->screen_h;
    if (min_x >= max_x || min_y >= max_y) return 0;

    int drew = 0;
    for (int32_t yy = min_y; yy < max_y; ++yy) {
        uint16_t c0 = m->foreground_cover.min_x[yy];
        uint16_t c1 = m->foreground_cover.max_x[yy];
        if (c0 >= c1) {
            if (submit(m, submit_ctx, min_x, yy, max_x - min_x)) {
                arm_ext_note_visible_present_rect(m, min_x, yy,
                                                  max_x - min_x, 1);
                drew = 1;
            }
            continue;
        }
        if (min_x < c0) {
            int32_t sx1 = c0 < max_x ? c0 : max_x;
            if (min_x < sx1 &&
                submit(m, submit_ctx, min_x, yy, sx1 - min_x)) {
                arm_ext_note_visible_present_rect(m, min_x, yy,
                                                  sx1 - min_x, 1);
                drew = 1;
            }
        }
        if (c1 < max_x) {
            int32_t sx0 = c1 > min_x ? c1 : min_x;
            if (sx0 < max_x &&
                submit(m, submit_ctx, sx0, yy, max_x - sx0)) {
                arm_ext_note_visible_present_rect(m, sx0, yy,
                                                  max_x - sx0, 1);
                drew = 1;
            }
        }
    }
    return drew;
}

static int arm_ext_submit_bitmap_segment(ArmExtModule *m,
                                         void *ctx,
                                         int32_t x,
                                         int32_t y,
                                         int32_t w) {
    (void)m;
    ArmExtBitmapPresentCtx *bitmap_ctx = (ArmExtBitmapPresentCtx *)ctx;
    if (!bitmap_ctx || !bitmap_ctx->bmp) return 0;
    guiDrawBitmapWithStride(
        bitmap_ctx->bmp, x, y, w, 1, bitmap_ctx->source_stride,
        bitmap_ctx->source_x + (x - bitmap_ctx->dst_x),
        bitmap_ctx->source_y + (y - bitmap_ctx->dst_y));
    return 1;
}

static int arm_ext_submit_dispup_segment(ArmExtModule *m,
                                         void *ctx,
                                         int32_t x,
                                         int32_t y,
                                         int32_t w) {
    (void)m;
    ArmExtDispupPresentCtx *dispup_ctx = (ArmExtDispupPresentCtx *)ctx;
    if (!dispup_ctx || !dispup_ctx->screen || dispup_ctx->source_stride <= 0) {
        return 0;
    }
    guiDrawBitmapWithStride(dispup_ctx->screen, x, y, w, 1,
                            dispup_ctx->source_stride, x, y);
    return 1;
}

static int arm_ext_present_bitmap_rect(ArmExtModule *m,
                                       uint16_t *bmp,
                                       int32_t x,
                                       int32_t y,
                                       int32_t w,
                                       int32_t h,
                                       int32_t source_stride,
                                       int32_t source_x,
                                       int32_t source_y,
                                       int covered_by_foreground) {
    if (!bmp || w <= 0 || h <= 0 || source_stride <= 0) return 0;
    if (covered_by_foreground) {
        arm_ext_retire_nonmodal_foreground_cover_rect(m, x, y, w, h);
        covered_by_foreground = arm_ext_has_foreground_cover(m);
    }
    if (!covered_by_foreground || !arm_ext_has_foreground_cover(m)) {
        arm_ext_diag_visible_present(m, "bitmap", bmp, x, y, w, h,
                                     covered_by_foreground);
        guiDrawBitmapWithStride(bmp, x, y, w, h, source_stride,
                                source_x, source_y);
        arm_ext_note_visible_present_rect(m, x, y, w, h);
        return 1;
    }

    ArmExtBitmapPresentCtx bitmap_ctx = {
        .bmp = bmp,
        .dst_x = x,
        .dst_y = y,
        .source_stride = source_stride,
        .source_x = source_x,
        .source_y = source_y,
    };
    return arm_ext_submit_uncovered_present_segments(
        m, x, y, w, h, arm_ext_submit_bitmap_segment, &bitmap_ctx);
}

static int arm_ext_dispup_rect(ArmExtModule *m,
                               int32_t x,
                               int32_t y,
                               int32_t w,
                               int32_t h,
                               int covered_by_foreground) {
    if (w <= 0 || h <= 0) return 0;
    if (covered_by_foreground) {
        arm_ext_retire_nonmodal_foreground_cover_rect(m, x, y, w, h);
        covered_by_foreground = arm_ext_has_foreground_cover(m);
    }
    if (!covered_by_foreground || !arm_ext_has_foreground_cover(m)) {
        uint16_t *screen = (uint16_t *)arm_ptr(m, m->screen_addr);
        arm_ext_diag_visible_present(m, "dispup", screen,
                                     x, y, w, h, covered_by_foreground);
        guiDrawBitmapWithStride(screen, x, y, w, h,
                                arm_ext_screen_stride(m), x, y);
        arm_ext_note_visible_present_rect(m, x, y, w, h);
        return MR_SUCCESS;
    }

    ArmExtDispupPresentCtx dispup_ctx = {
        .screen = (uint16_t *)arm_ptr(m, m->screen_addr),
        .source_stride = arm_ext_screen_stride(m),
    };
    return arm_ext_submit_uncovered_present_segments(
        m, x, y, w, h, arm_ext_submit_dispup_segment, &dispup_ctx)
        ? MR_SUCCESS : 0;
}

static int arm_ext_has_screen_damage(ArmExtModule *m) {
    if (!m || !arm_ext_ensure_screen_regions(m)) return 0;
    for (uint32_t y = 0; y < m->screen_damage.rows; ++y) {
        if (m->screen_damage.min_x[y] < m->screen_damage.max_x[y]) {
            return 1;
        }
    }
    return 0;
}

static void arm_ext_note_screen_damage_diff(ArmExtModule *m,
                                            const uint16_t *before) {
    if (!m || !before || !m->screen_addr || !arm_ext_ensure_screen_regions(m) ||
        m->screen_w <= 0 || m->screen_h <= 0) {
        return;
    }
    const uint16_t *after = (const uint16_t *)arm_ptr(m, m->screen_addr);
    if (!after) return;

    for (int32_t y = 0; y < m->screen_h; ++y) {
        int32_t min_x = m->screen_w;
        int32_t max_x = -1;
        const uint16_t *before_row =
            before + (uint32_t)y * (uint32_t)arm_ext_screen_stride(m);
        const uint16_t *after_row =
            after + (uint32_t)y * (uint32_t)arm_ext_screen_stride(m);
        for (int32_t x = 0; x < m->screen_w; ++x) {
            if (before_row[x] != after_row[x]) {
                if (x < min_x) min_x = x;
                max_x = x;
            }
        }
        if (max_x >= min_x) {
            arm_ext_note_screen_damage_rect(m, min_x, y, max_x - min_x + 1, 1);
        }
    }
}

static void arm_ext_present_uncovered_screen_damage(ArmExtModule *m,
                                                    uint16_t *screen) {
    if (!m || !screen || !m->screen_dirty || !m->screen_presented_in_callback ||
        !arm_ext_ensure_screen_regions(m)) {
        return;
    }

    /*
     * Native EXT code can compose through host screen-cache APIs, then present
     * only a smaller dirty rectangle.  The ARM framebuffer is copied back to
     * the host shadow buffer at callback exit, but SDL has only seen the
     * explicit present rectangles.  Submit the row spans that were changed by
     * host-executed drawing APIs and were not covered by those presents.
     */
    for (uint32_t y = 0; y < m->screen_damage.rows; ++y) {
        uint16_t d0 = m->screen_damage.min_x[y];
        uint16_t d1 = m->screen_damage.max_x[y];
        if (d0 >= d1) continue;

        uint16_t p0 = m->screen_present.min_x[y];
        uint16_t p1 = m->screen_present.max_x[y];
        if (p0 >= p1) {
            mr_drawBitmap(screen, d0, (int16)y, (uint16)(d1 - d0), 1);
            continue;
        }
        if (d0 < p0) {
            mr_drawBitmap(screen, d0, (int16)y, (uint16)(p0 - d0), 1);
        }
        if (p1 < d1) {
            mr_drawBitmap(screen, p1, (int16)y, (uint16)(d1 - p1), 1);
        }
    }
}

static void arm_ext_present_screen_damage(ArmExtModule *m, uint16_t *screen) {
    if (!m || !screen || !m->screen_dirty || !arm_ext_ensure_screen_regions(m)) {
        return;
    }

    for (uint32_t y = 0; y < m->screen_damage.rows; ++y) {
        uint16_t d0 = m->screen_damage.min_x[y];
        uint16_t d1 = m->screen_damage.max_x[y];
        if (d0 < d1) {
            mr_drawBitmap(screen, d0, (int16)y, (uint16)(d1 - d0), 1);
        }
    }
}

static void arm_ext_merge_uncovered_screen_damage(ArmExtModule *m,
                                                  uint32_t owner_p_addr,
                                                  uint32_t owner_helper_addr) {
    if (!m || !m->screen_dirty || !m->screen_presented_in_callback ||
        !arm_ext_ensure_screen_regions(m)) {
        return;
    }

    for (uint32_t y = 0; y < m->screen_damage.rows; ++y) {
        uint16_t d0 = m->screen_damage.min_x[y];
        uint16_t d1 = m->screen_damage.max_x[y];
        if (d0 >= d1) continue;

        uint16_t p0 = m->screen_present.min_x[y];
        uint16_t p1 = m->screen_present.max_x[y];
        if (p0 >= p1) {
            arm_ext_claim_foreground_screen_rect(m, owner_p_addr,
                                                 owner_helper_addr,
                                                 d0, (int32_t)y,
                                                 d1 - d0, 1);
            continue;
        }
        if (d0 < p0) {
            arm_ext_claim_foreground_screen_rect(m, owner_p_addr,
                                                 owner_helper_addr,
                                                 d0, (int32_t)y,
                                                 p0 - d0, 1);
        }
        if (p1 < d1) {
            arm_ext_claim_foreground_screen_rect(m, owner_p_addr,
                                                 owner_helper_addr,
                                                 p1, (int32_t)y,
                                                 d1 - p1, 1);
        }
    }
}

static void enter_screen_context(ArmExtModule *m,
                                 uint16 **saved_screen,
                                 uint32_t *present_depth_before) {
    *saved_screen = mr_screenBuf;
    if (m) {
        if (present_depth_before) {
            *present_depth_before = m->screen_present_depth;
        }
        m->screen_presented_in_callback = 0;
        arm_ext_clear_screen_regions(m);
    } else if (present_depth_before) {
        *present_depth_before = 0;
    }
    if (m->screen_addr) mr_screenBuf = (uint16 *)arm_ptr(m, m->screen_addr);
}

static void arm_ext_note_screen_presented(ArmExtModule *m) {
    if (m) {
        m->screen_presented_in_callback = 1;
        m->screen_present_depth++;
    }
}

static uint16_t *arm_ext_snapshot_screen(ArmExtModule *m) {
    if (!m || !m->screen_addr || !m->screen_len || !arm_ptr(m, m->screen_addr))
        return NULL;
    uint16_t *snapshot = (uint16_t *)malloc(m->screen_len);
    if (!snapshot) return NULL;
    memcpy(snapshot, arm_ptr(m, m->screen_addr), m->screen_len);
    return snapshot;
}

static void leave_screen_context(ArmExtModule *m,
                                 uint16 *saved_screen,
                                 uint32_t present_depth_before) {
    if (m->screen_addr && saved_screen && m->screen_len) {
        uint16_t *arm_screen = (uint16_t *)arm_ptr(m, m->screen_addr);
        int32_t stride = arm_ext_screen_stride(m);
        if (arm_screen && stride == m->screen_w) {
            memcpy(saved_screen, arm_screen,
                   (size_t)m->screen_w * (size_t)m->screen_h *
                   sizeof(uint16_t));
        } else if (arm_screen && stride > 0) {
            for (int32_t y = 0; y < m->screen_h; ++y) {
                memcpy(saved_screen + (size_t)y * (size_t)m->screen_w,
                       arm_screen + (size_t)y * (size_t)stride,
                       (size_t)m->screen_w * sizeof(uint16_t));
            }
        }
    }
    mr_screenBuf = saved_screen;
    if (saved_screen && m && m->screen_dirty && m->screen_presented_in_callback) {
        arm_ext_merge_uncovered_screen_damage(m, m->foreground_screen_owner_p_addr,
                                              m->foreground_screen_owner_helper_addr);
        arm_ext_present_uncovered_screen_damage(m, saved_screen);
        m->screen_dirty = 0;
    }
    /*
     * Keep foreground screen ownership as visible-layer metadata only.
     * A foreground overlay may present its own covered rows and later proxy
     * still-visible rows from the shared ARM screen cache.  Auto-restoring a
     * host-side copy into that cache would erase the producer's next frame.
     */
    /*
     * Native apps often clear the whole screen buffer, repaint only dirty
     * regions, then explicitly present those regions with mr_drawBitmap or
     * DispUpEx.  A forced full-screen present at callback exit would expose
     * cleared-but-not-presented pixels, so callback-exit synthesis is limited
     * to row spans that were explicitly tracked as screen damage.
     */
    if (saved_screen && m && m->screen_dirty &&
        m->screen_present_depth == present_depth_before &&
        arm_ext_has_screen_damage(m)) {
        arm_ext_present_screen_damage(m, saved_screen);
    }
    m->screen_dirty = 0;
    if (m) m->screen_presented_in_callback = 0;
}

typedef struct ArmExtScreenContext {
    uint16 *saved_screen;
    int32 saved_w;
    int32 saved_h;
    uint32_t target_addr;
    int active;
} ArmExtScreenContext;

static int arm_ext_read_table_u32_slot(ArmExtModule *m, uint32_t idx,
                                       uint32_t *value) {
    if (value) *value = 0;
    if (!m || idx >= EXT_TABLE_COUNT) return 0;

    uint32_t screen_slot = 0;
    void *slotp = arm_ptr(m, EXT_TABLE_ADDR + idx * 4);
    if (!slotp) return 0;
    memcpy(&screen_slot, slotp, 4);
    if (!screen_slot || !arm_ptr(m, screen_slot)) return 0;

    uint32_t slot_value = 0;
    memcpy(&slot_value, arm_ptr(m, screen_slot), 4);
    if (value) *value = slot_value;
    return 1;
}

static int arm_ext_push_draw_screen_context(ArmExtModule *m,
                                            ArmExtScreenContext *ctx) {
    if (!ctx) return 0;
    memset(ctx, 0, sizeof(*ctx));
    ctx->saved_screen = mr_screenBuf;
    ctx->saved_w = mr_screen_w;
    ctx->saved_h = mr_screen_h;
    if (!m) return 0;

    uint32_t screen_addr = 0;
    uint32_t screen_w = 0;
    uint32_t screen_h = 0;
    if (!arm_ext_read_table_u32_slot(m, 91, &screen_addr) ||
        !screen_addr || !arm_ptr(m, screen_addr)) {
        return 0;
    }
    if (!arm_ext_read_table_u32_slot(m, 92, &screen_w) ||
        !arm_ext_read_table_u32_slot(m, 93, &screen_h) ||
        screen_w == 0 || screen_h == 0) {
        return 0;
    }

    /*
     * guest 可改写 ARM 可见 mr_screen_w/h(table[92/93])配置逻辑画布(gtcm
     * 横屏 480x320):hook_screen_dim_write 已在写入瞬间把缓冲迁移到顶部
     * 保留区。仅当缓冲确实已迁移(screen_addr 落在保留区)时才把新尺寸
     * 持久采纳进 m->screen_w/h/len——present 的 stride/行数与 damage 跟踪
     * 必须与迁移后的绘制一致。未迁移时保持原有语义:guest 尺寸只作用于
     * 本次绘制(函数末尾的 mr_screen_w/h 赋值),但超出宿主缓冲容量的
     * 尺寸必须回退,避免宿主绘制越界抹掉缓冲之后的模块镜像(gtcm 黑屏
     * 根因);写成宿主画布转置的尺寸同样回退——那是 gtcm 撤销 LCD 旋转
     * 的竖屏请求,hook_screen_dim_read 会在 guest 读回前恢复宿主尺寸,
     * 宿主绘制若在这个窗口内采纳转置 stride 会交错花屏。guest 把
     * table[91] 指向自有缓冲时(滚动背景离屏合成)不受影响。
     */
    if (screen_addr == m->screen_addr) {
        uint32_t cap = m->screen_cap ? m->screen_cap : m->screen_len;
        int migrated = m->screen_addr ==
                       EXT_BASE_ADDR + EXT_MEM_SIZE - EXT_SCREEN_RESERVE;
        int transposed = screen_w == (uint32_t)m->screen_h &&
                         screen_h == (uint32_t)m->screen_w &&
                         screen_w != screen_h;
        if (transposed || screen_w > 2048 || screen_h > 2048 ||
            screen_w * screen_h * 2u > cap) {
            /* 越界/转置保护:退回宿主已知尺寸 */
            screen_w = (uint32_t)m->screen_w;
            screen_h = (uint32_t)m->screen_h;
        } else if (migrated &&
                   ((int32_t)screen_w != m->screen_w ||
                    (int32_t)screen_h != m->screen_h)) {
            m->screen_w = (int32_t)screen_w;
            m->screen_h = (int32_t)screen_h;
            m->screen_len = screen_w * screen_h * 2u;
        }
    }

    /*
     * table[91/92/93] are the ARM-visible mr_screenBuf/mr_screen_w/mr_screen_h
     * variables. Native code may repoint them to an off-screen cache with a
     * wider stride while composing a scrolling background, so host-rendered
     * screen-cache APIs must use the current ARM values for the duration of
     * each call.
     */
    mr_screenBuf = (uint16 *)arm_ptr(m, screen_addr);
    mr_screen_w = (int32)screen_w;
    mr_screen_h = (int32)screen_h;
    ctx->target_addr = screen_addr;
    ctx->active = 1;
    return 1;
}

static void arm_ext_pop_draw_screen_context(ArmExtScreenContext *ctx) {
    if (!ctx || !ctx->active) return;
    mr_screenBuf = ctx->saved_screen;
    mr_screen_w = ctx->saved_w;
    mr_screen_h = ctx->saved_h;
}

static int arm_ext_screen_context_targets_primary(ArmExtModule *m,
                                                  const ArmExtScreenContext *ctx) {
    return m && ctx && ctx->active && ctx->target_addr == m->screen_addr;
}

static int arm_ext_should_track_screen_cache_damage(ArmExtModule *m) {
    /*
     * The screen cache is backing storage.  A foreground child protects lower
     * layers only after it has explicitly presented cover rows; table[25] alone
     * can also register non-visual support helpers, and suppressing cache
     * damage in that state leaves later primary redraws invisible.
     */
    return !arm_ext_has_foreground_cover(m);
}

static void arm_ext_finish_screen_cache_write(ArmExtModule *m,
                                              const ArmExtScreenContext *ctx,
                                              uint32_t claim_p_addr,
                                              uint32_t claim_helper_addr) {
    if (!arm_ext_screen_context_targets_primary(m, ctx)) return;
    if (!arm_ext_should_track_screen_cache_damage(m)) return;
    m->screen_dirty = 1;
    arm_ext_finish_accepted_screen_write(m, claim_p_addr, claim_helper_addr);
}

static void arm_ext_clear_foreground_screen_owner(ArmExtModule *m) {
    if (!m) return;
    m->foreground_screen_owner_p_addr = 0;
    m->foreground_screen_owner_helper_addr = 0;
    arm_ext_clear_foreground_cover_regions(m);
}

static int arm_ext_claim_foreground_screen_owner(ArmExtModule *m,
                                                 uint32_t owner_p_addr,
                                                 uint32_t owner_helper_addr) {
    if (!m || !owner_p_addr) {
        return 0;
    }
    m->foreground_screen_owner_p_addr = owner_p_addr;
    m->foreground_screen_owner_helper_addr = owner_helper_addr;
    return 1;
}

static void arm_ext_claim_foreground_screen_rect(ArmExtModule *m,
                                                 uint32_t owner_p_addr,
                                                 uint32_t owner_helper_addr,
                                                 int32_t x,
                                                 int32_t y,
                                                 int32_t w,
                                                 int32_t h) {
    (void)x;
    (void)y;
    (void)w;
    (void)h;
    arm_ext_claim_foreground_screen_owner(m, owner_p_addr,
                                          owner_helper_addr);
}

static void arm_ext_claim_foreground_screen_diff(ArmExtModule *m,
                                                 uint32_t owner_p_addr,
                                                 uint32_t owner_helper_addr,
                                                 const uint16_t *before) {
    (void)before;
    arm_ext_claim_foreground_screen_owner(m, owner_p_addr,
                                          owner_helper_addr);
}

static int arm_ext_should_accept_screen_write(ArmExtModule *m,
                                              uint32_t *claim_p_addr,
                                              uint32_t *claim_helper_addr) {
    if (claim_p_addr) *claim_p_addr = 0;
    if (claim_helper_addr) *claim_helper_addr = 0;
    if (!m) return 1;
    if (!m->primary_p_addr || !m->active_p_addr || !m->active_helper_addr) return 1;
    if (m->active_p_addr == m->primary_p_addr ||
        m->active_helper_addr == m->primary_helper_addr ||
        m->active_p_addr == m->p_addr ||
        m->active_helper_addr == m->helper_addr) {
        return 1;
    }

    uint32_t caller_helper_addr = 0;
    uint32_t caller_p_addr =
        arm_ext_p_for_code_addr(m, reg_read32(m->uc, UC_ARM_REG_LR),
                                &caller_helper_addr);
    if (!caller_p_addr) {
        caller_p_addr = m->current_p_addr;
        caller_helper_addr = m->current_helper_addr;
    }
    if (!caller_p_addr) return 1;
    if (caller_p_addr == m->active_p_addr) {
        if (claim_p_addr) *claim_p_addr = caller_p_addr;
        if (claim_helper_addr) *claim_helper_addr = caller_helper_addr;
        if (getenv("VMRP_ARM_EXT_DIAG")) {
            static int screen_claim_diag_count = 0;
            if (screen_claim_diag_count++ < 40) {
                printf("DIAG screen_claim callerP=0x%X callerH=0x%X activeP=0x%X activeH=0x%X valid=%d\n",
                       caller_p_addr, caller_helper_addr,
                       m->active_p_addr, m->active_helper_addr,
                       (m->foreground_screen_owner_p_addr != 0));
            }
        }
        return 1;
    }
    arm_ext_clear_empty_nonmodal_foreground_owner(m);
    if (m->foreground_screen_owner_p_addr != m->active_p_addr ||
        m->foreground_screen_owner_helper_addr != m->active_helper_addr) {
        return 1;
    }

    /*
     * cfunction.ext suspend/resume manages extChunk[0x34]:
     *   0xE83098/0xE83514 increment it and detach extChunk[0x24].
     *   0xE83590/0xE835FC decrement it and relink extChunk[0x24].
     * The field may be incremented after the child has already rendered, so
     * use the first actual child screen write as the host-side foreground
     * claim.  gghjt's verdload path shows the child rendering the download UI,
     * then the wrapper returns to game.ext and redraws the menu in the same
     * timer callback.  run_arm_with_sp() is still executing the wrapper helper
     * in that nested call, so classify each draw by LR's code range instead of
     * current_p_addr.  The host has no real window stack, so keep the active
     * child framebuffer authoritative until the wrapper resumes the primary
     * layer or another active child claims the screen.
    */
    if (caller_p_addr == m->primary_p_addr) {
        if (getenv("VMRP_ARM_EXT_DIAG")) {
            printf("DIAG screen_reject primary callerP=0x%X callerH=0x%X activeP=0x%X activeH=0x%X valid=%d\n",
                   caller_p_addr, caller_helper_addr,
                   m->active_p_addr, m->active_helper_addr,
                   (m->foreground_screen_owner_p_addr != 0));
        }
        return 0;
    }
    if (caller_p_addr == m->p_addr &&
        caller_helper_addr == m->helper_addr) {
        if (getenv("VMRP_ARM_EXT_DIAG")) {
            printf("DIAG screen_reject wrapper callerP=0x%X callerH=0x%X activeP=0x%X activeH=0x%X valid=%d\n",
                   caller_p_addr, caller_helper_addr,
                   m->active_p_addr, m->active_helper_addr,
                   (m->foreground_screen_owner_p_addr != 0));
        }
        return 0;
    }
    return 1;
}

static int arm_ext_should_accept_visible_present(ArmExtModule *m,
                                                 uint32_t *claim_p_addr,
                                                 uint32_t *claim_helper_addr) {
    if (arm_ext_should_accept_screen_write(m, claim_p_addr,
                                           claim_helper_addr)) {
        return 1;
    }

    uint32_t owner_helper_addr = 0;
    uint32_t owner_p_addr = arm_ext_screen_caller_owner(m, &owner_helper_addr);
    /*
     * A foreground child can own only part of the visible screen, for example
     * a top overlay while the primary game remains visible
     * underneath.  Explicit presents from the covered layer are therefore not
     * discarded; the draw path clips them against the child-owned cover rows.
     * Cache-only writes still use arm_ext_should_accept_screen_write() above
     * and remain invisible until an explicit present reaches this path.
     */
    if (!arm_ext_owner_is_covered_by_foreground(m, owner_p_addr,
                                                owner_helper_addr)) {
        return 0;
    }
    if (claim_p_addr) *claim_p_addr = owner_p_addr;
    if (claim_helper_addr) *claim_helper_addr = owner_helper_addr;
    return 1;
}

static void arm_ext_finish_accepted_screen_write(ArmExtModule *m,
                                                 uint32_t claim_p_addr,
                                                 uint32_t claim_helper_addr) {
    if (!claim_p_addr) return;
    /*
     * Foreground ownership is metadata only.  Visible pixels are protected by
     * foreground_cover row spans; keeping an extra full-screen snapshot made
     * the advbar fix harder to audit and no longer feeds any restore path.
     * Compatibility impact: cache-only writes still stay hidden behind cover,
     * while explicit presents update cover/present spans as before.  Verified
     * with the opbzqe RIGHT-key PPM/vitest flow and related plugin e2e cases.
     */
    arm_ext_claim_foreground_screen_owner(m, claim_p_addr,
                                          claim_helper_addr);
}

static void arm_ext_mirror_draw_bitmap_to_screen(ArmExtModule *m,
                                                 uint32_t bmp_addr,
                                                 int16_t x,
                                                 int16_t y,
                                                 uint16_t w,
                                                 uint16_t h,
                                                 int32_t source_stride,
                                                 int32_t source_x,
                                                 int32_t source_y) {
    if (!m || !m->screen_addr || !m->screen_len ||
        !arm_ptr(m, m->screen_addr) || !arm_ptr(m, bmp_addr) ||
        source_stride <= 0) {
        return;
    }

    int32_t screen_w = m->screen_w;
    int32_t screen_h = m->screen_h;
    if (screen_w <= 0 || screen_h <= 0) return;

    int32_t min_x = x < 0 ? 0 : x;
    int32_t min_y = y < 0 ? 0 : y;
    int32_t max_x = (int32_t)x + (int32_t)w;
    int32_t max_y = (int32_t)y + (int32_t)h;
    if (max_x > screen_w) max_x = screen_w;
    if (max_y > screen_h) max_y = screen_h;
    if (min_x >= max_x || min_y >= max_y) return;

    uint16_t *dst_screen = (uint16_t *)arm_ptr(m, m->screen_addr);
    uint32_t row_pixels = (uint32_t)(max_x - min_x);
    for (int32_t yy = min_y; yy < max_y; ++yy) {
        uint32_t offset_pixels = (uint32_t)yy *
                                 (uint32_t)arm_ext_screen_stride(m) +
                                 (uint32_t)min_x;
        int32_t sx = source_x + (min_x - x);
        int32_t sy = source_y + (yy - y);
        if (sx < 0 || sy < 0 ||
            sx + (int32_t)row_pixels > source_stride) {
            return;
        }
        uint64_t source_offset_pixels =
            (uint64_t)(uint32_t)sy * (uint64_t)(uint32_t)source_stride +
            (uint64_t)(uint32_t)sx;
        uint64_t row_addr64 = (uint64_t)bmp_addr +
                              source_offset_pixels * sizeof(uint16_t);
        uint64_t row_end64 = row_addr64 +
                             (uint64_t)row_pixels * sizeof(uint16_t) - 1u;
        if (row_end64 > 0xFFFFFFFFu) return;
        uint16_t *src_row = (uint16_t *)arm_ptr(m, (uint32_t)row_addr64);
        if (!src_row || !arm_ptr(m, (uint32_t)row_end64)) return;
        /*
         * Host-visible bitmap presents can use either local bitmap rows or the
         * full screen cache as their source. Mirror the exact submitted source
         * rows into the ARM framebuffer so later context exit cannot restore the
         * covered layer with stale pixels.
         * Do not mark it dirty here: the explicit present already reached SDL,
         * and screen_dirty is reserved for cache writes that still need a
         * synthesized submit.
         */
        memcpy(dst_screen + offset_pixels, src_row,
               (size_t)row_pixels * sizeof(uint16_t));
    }
}

static int arm_ext_bitmap_source_uses_screen_stride(ArmExtModule *m,
                                                    uint32_t bmp_addr) {
    if (!m || !bmp_addr) return 0;
    if (m->screen_addr && bmp_addr == m->screen_addr) return 1;

    uint32_t current_screen_addr = 0;
    if (arm_ext_read_table_u32_slot(m, 91, &current_screen_addr) &&
        current_screen_addr && bmp_addr == current_screen_addr) {
        return 1;
    }
    return 0;
}

static void capture_timer_dispatches(ArmExtModule *m) {
    if (!m || !m->primary_helper_addr) return;
    uint32_t t31a = EXT_TABLE_ADDR + 31 * 4;
    uint32_t t32a = EXT_TABLE_ADDR + 32 * 4;
    uint32_t t31v = 0, t32v = 0;
    memcpy(&t31v, arm_ptr(m, t31a), 4);
    memcpy(&t32v, arm_ptr(m, t32a), 4);
    uint32_t primary_start = m->primary_file_addr;
    uint32_t primary_end = primary_start + m->primary_file_len;
    int t31_primary = primary_start && t31v >= primary_start && t31v < primary_end;
    int t32_primary = primary_start && t32v >= primary_start && t32v < primary_end;

    if (t31v != t31a && arm_ptr(m, t31v)) {
        /* 嵌套 EXT 会把 SDK 的 timerStart 表项改成自己的 dispatch veneer
         * （mpc/game.ext 反汇编为 str ..., [table,#0x7c]）。模拟器必须保留
         * table hook，同时记录真实 veneer，用 wrapper R9 转调。 */
        int captured = (!m->dispatch_timer_start || t31_primary);
        if (captured) {
            m->dispatch_timer_start = t31v;
        }
        memcpy(arm_ptr(m, t31a), &t31a, 4);
        if (getenv("VMRP_ARM_EXT_TRACE")) {
            printf("arm_ext_executor: %s timerStart dispatch=0x%X\n",
                   captured ? "captured" : "ignored", t31v);
        }
    }
    if (t32v != t32a && arm_ptr(m, t32v)) {
        /* 同上，timerStop 也可能被嵌套 EXT 替换为 stop dispatch veneer。 */
        int captured = (!m->dispatch_timer_stop || t32_primary);
        if (captured) {
            m->dispatch_timer_stop = t32v;
        }
        memcpy(arm_ptr(m, t32a), &t32a, 4);
        if (getenv("VMRP_ARM_EXT_TRACE")) {
            printf("arm_ext_executor: %s timerStop dispatch=0x%X\n",
                   captured ? "captured" : "ignored", t32v);
        }
    }
}

static void cb_ret(ArmExtModule *m, uint32_t ret) {
    reg_write32(m->uc, UC_ARM_REG_R0, ret);
    uint32_t lr = reg_read32(m->uc, UC_ARM_REG_LR);
    set_arm_mode_for_addr(m, lr);
    reg_write32(m->uc, UC_ARM_REG_PC, lr);
}

/* ARM ext 跳转到低地址区（0x0 ~ EXT_LOW_TABLE_SIZE）时的处理。
 *
 * 两种场景会到这里：
 * a) 未经重定位的嵌套 ext 用低地址 table 指针（如 0x8 = table[2]），
 *    发生在宿主侧预解压后跳过了 ARM 侧的重定位。此时应将执行重定向
 *    到主表对应条目，由 hook_table 正常分发。
 * b) 无效跳转（如 0x1000），按 return-to-LR 兜底。 */
static void hook_low_zero(uc_engine *uc, uint64_t address, uint32_t size, void *user_data) {
    (void)size;
    ArmExtModule *m = (ArmExtModule *)user_data;
    uint32_t lr = reg_read32(uc, UC_ARM_REG_LR);
    /* 场景 a：未经重定位的 ext 通过低地址调用 table 函数。
     * 不做真实分发（参数可能是给嵌套 ext 函数而非 table 函数的），
     * 安全地返回 0（NULL/失败），让调用方走错误处理路径。 */
    if (address < EXT_TABLE_COUNT * 4) {
        if (m && getenv("VMRP_ARM_EXT_DIAG")) {
            static uint32_t low_zero_diag_count = 0;
            uint32_t idx = (uint32_t)(address / 4u);
            uint32_t lr_helper = 0;
            uint32_t lr_p = arm_ext_p_for_code_addr(m, lr, &lr_helper);
            ArmExtNestedModule *lr_mod =
                arm_ext_find_nested_module_by_p(m, lr_p);
            /*
             * Downloader disassembly calls SDK bridges through BLX-loaded
             * table slots.  If a private child still holds an unresolved low
             * self-pointer (for example 81*4 for mr_initNetwork), this hook is
             * the only place it can disappear before hook_table sees it.
             * Keep the diagnostic bounded because bad bridge state can loop.
             */
            if (low_zero_diag_count < 512u) {
                printf("DIAG low_zero slot=%u addr=0x%llX lr=0x%X lrP=0x%X lrH=0x%X lrFile=0x%X/%u r0=0x%X r1=0x%X r2=0x%X r3=0x%X r9=0x%X activeP=0x%X activeH=0x%X currentP=0x%X currentH=0x%X\n",
                       idx, (unsigned long long)address, lr, lr_p,
                       lr_helper, lr_mod ? lr_mod->file_addr : 0,
                       lr_mod ? lr_mod->file_len : 0,
                       reg_read32(uc, UC_ARM_REG_R0),
                       reg_read32(uc, UC_ARM_REG_R1),
                       reg_read32(uc, UC_ARM_REG_R2),
                       reg_read32(uc, UC_ARM_REG_R3),
                       reg_read32(uc, UC_ARM_REG_R9),
                       m->active_p_addr, m->active_helper_addr,
                       m->current_p_addr, m->current_helper_addr);
            } else if (low_zero_diag_count == 512u) {
                printf("DIAG low_zero truncated after 512 entries\n");
            }
            low_zero_diag_count++;
        }
        reg_write32(uc, UC_ARM_REG_R0, 0);
    }
    /* 场景 b：超出 table 范围 → return to LR */
    if (lr) {
        set_arm_mode_for_addr(m, lr);
        reg_write32(uc, UC_ARM_REG_PC, lr);
    } else {
        reg_write32(uc, UC_ARM_REG_PC, EXT_STOP_ADDR);
        uc_emu_stop(uc);
    }
}

static void hook_table(uc_engine *uc, uint64_t address, uint32_t size, void *user_data) {
    (void)size;
    ArmExtModule *m = (ArmExtModule *)user_data;
    if (address < EXT_TABLE_ADDR || address >= EXT_TABLE_ADDR + EXT_TABLE_COUNT * 4) return;
    uint32_t idx = (uint32_t)((address - EXT_TABLE_ADDR) / 4);
    if (m->pending_internal_file_addr) {
        arm_ext_sync_internal_nested_module(m, m->pending_internal_file_addr,
                                            m->pending_internal_file_len);
    }
    capture_timer_dispatches(m);
    uint32_t r0 = arg_read(m, 0), r1 = arg_read(m, 1), r2 = arg_read(m, 2), r3 = arg_read(m, 3);
    /* 临时诊断:哨兵完整性校验(捕获宿主侧覆写,修复后删除) */
    arm_ext_watch_sentinel_check(m, idx, r0, r1, r2, r3);
    uint32_t ret = MR_SUCCESS;
    int trace_table = getenv("VMRP_ARM_EXT_TRACE") != NULL;
    if (trace_table) {
        uint32_t lr = reg_read32(m->uc, UC_ARM_REG_LR);
        printf("arm_ext_executor: table[%u](0x%X,0x%X,0x%X,0x%X) lr=0x%X\n", idx, r0, r1, r2, r3, lr);
    }
    /* ARM ext 常用 mr_getTime 忙等等待动画/定时器间隔。timerStart/Stop
     * 可能夹在同一段等待循环里，因此与 getTime 一起保留连续计数。 */
    if (idx != 33 && idx != 31 && idx != 32) {
        m->busy_wait_count = 0;
        m->busy_wait_start_ms = 0;
    }

    switch (idx) {
        case 0:
            /* 原生 table[0] = mr_malloc(first-fit,mem.c)。先走 guest
             * 空闲链表使地址预测成立,池内无合适块再走 bump 空闲块
             * 复用/新块(见分配器头注释与 bump 回收器注释)。 */
            ret = arm_ext_app_mem_malloc(m, r0);
            m->last_alloc_addr = ret;
            m->last_alloc_len = r0;
            if (ret) note_origin_mem_alloc(m, r0);
            {
                uint32_t lr = reg_read32(m->uc, UC_ARM_REG_LR) & ~1u;
                uint32_t nested_file_addr = 0;
                uint32_t nested_file_len = 0;
                if (m->nested_loading &&
                    arm_ext_nested_exec_range_for_lr(m, lr, &nested_file_addr,
                                                     &nested_file_len) &&
                    r0 > 0x1000) {
                    if (!m->outer_r9) {
                        m->outer_r9 = reg_read32(m->uc, UC_ARM_REG_R9);
                    }
                    if (!m->nested_return_addr) {
                        uint32_t sp = reg_read32(m->uc, UC_ARM_REG_SP);
                        uint32_t saved_lr = 0;
                        if (arm_ptr(m, sp + 12)) {
                            uc_mem_read(m->uc, sp + 12, &saved_lr, 4);
                            /*
                             * The private loader temporarily switches R9 from
                             * wrapper RW to the child RW while child startup
                             * allocates its own data.  Restore the wrapper R9
                             * only at the real wrapper return site.  A saved LR
                             * inside the staged child is just an internal
                             * Thumb return and restoring there makes the child
                             * run with wrapper globals.
                             */
                            uint32_t ret_pc = saved_lr & ~1u;
                            if (ret_pc >= EXT_CODE_ADDR &&
                                ret_pc < EXT_CODE_ADDR + m->code_len) {
                                m->nested_return_addr = saved_lr;
                            }
                        }
                    }
                    reg_write32(m->uc, UC_ARM_REG_R9, ret + 4);
                    m->nested_loading = 0;
                    if (getenv("VMRP_ARM_EXT_TRACE")) {
                        printf("arm_ext_executor: nested R9=0x%X after malloc 0x%X outer=0x%X return=0x%X\n",
                               ret + 4, r0, m->outer_r9, m->nested_return_addr);
                    }
                }
            }
            break;
        case 1:
            /* 原生 table[1] = mr_free(p,len):池内块插回空闲链表并合并
             * ("先 free 再让 readFile 复用该块"的原生模式据此成立);
             * bump 后备块进宿主侧回收器(只认登记过的地址)。 */
            arm_ext_app_mem_free(m, r0, r1);
            note_origin_mem_free(m, r1);
            ret = MR_SUCCESS;
            break;
        case 2: {
            /* 原生 table[2] = mr_realloc(p,oldlen,newlen),mem.c:
             * p==NULL 等价 malloc;newlen==0 等价 free 返回 NULL;
             * 其余 malloc→拷贝 min(oldlen,newlen)→free(p,oldlen)。
             * 分配同 table[0] 池优先、bump 后备;统计与修复前一致仅记
             * 新块(note_origin_mem_alloc)。 */
            if (!r0) {
                ret = arm_ext_app_mem_malloc(m, r2);
                if (ret) note_origin_mem_alloc(m, r2);
                break;
            }
            if (!r2) { arm_ext_app_mem_free(m, r0, r1); ret = 0; break; }
            uint32_t p = arm_ext_app_mem_malloc(m, r2);
            if (p) {
                note_origin_mem_alloc(m, r2);
                memmove(arm_ptr(m, p), arm_ptr(m, r0), r1 < r2 ? r1 : r2);
                arm_ext_app_mem_free(m, r0, r1);
            }
            ret = p;
        } break;
        case 3:
            memcpy(arm_ptr(m, r0), arm_ptr(m, r1), r2);
            /* memcpy 后修复 GOT 中的 bridge 指针 */
            if (m->got_snapshot_base) {
                uint32_t got_base = m->got_snapshot_base;
                uint32_t cpy_end = r0 + r2;
                for (uint32_t i = 0; i < EXT_TABLE_COUNT; i++) {
                    uint32_t addr = got_base + i * 4;
                    if (m->got_snapshot[i] >= EXT_TABLE_ADDR &&
                        m->got_snapshot[i] < EXT_TABLE_ADDR + EXT_TABLE_COUNT * 4 &&
                        addr >= r0 && addr + 4 <= cpy_end &&
                        !arm_ext_should_skip_got_snapshot_restore(m, addr) &&
                        !app_should_protect_got_addr(m, addr)) {
                        memcpy(arm_ptr(m, addr), &m->got_snapshot[i], 4);
                    }
                }
            }
            ret = r0; break;
        case 4: memmove(arm_ptr(m, r0), arm_ptr(m, r1), r2); ret = r0; break;
        case 5: strcpy(arm_ptr(m, r0), arm_str(m, r1)); ret = r0; break;
        case 6: strncpy(arm_ptr(m, r0), arm_str(m, r1), r2); ret = r0; break;
        case 7: strcat(arm_ptr(m, r0), arm_str(m, r1)); ret = r0; break;
        case 8: strncat(arm_ptr(m, r0), arm_str(m, r1), r2); ret = r0; break;
        case 9: ret = memcmp(arm_ptr(m, r0), arm_ptr(m, r1), r2); break;
        case 10: ret = strcmp(arm_str(m, r0), arm_str(m, r1)); break;
        case 11: ret = strncmp(arm_str(m, r0), arm_str(m, r1), r2); break;
        case 13: { void *p = memchr(arm_ptr(m, r0), (int)r1, r2); ret = p ? arm_addr(m, p) : 0; } break;
        case 14:
            memset(arm_ptr(m, r0), (int)r1, r2);
            /* memset 后修复 GOT 中的 bridge 指针 */
            if (m->got_snapshot_base) {
                uint32_t got_base = m->got_snapshot_base;
                uint32_t set_end = r0 + r2;
                for (uint32_t i = 0; i < EXT_TABLE_COUNT; i++) {
                    uint32_t addr = got_base + i * 4;
                    if (m->got_snapshot[i] >= EXT_TABLE_ADDR &&
                        m->got_snapshot[i] < EXT_TABLE_ADDR + EXT_TABLE_COUNT * 4 &&
                        addr >= r0 && addr + 4 <= set_end &&
                        !arm_ext_should_skip_got_snapshot_restore(m, addr) &&
                        !app_should_protect_got_addr(m, addr)) {
                        memcpy(arm_ptr(m, addr), &m->got_snapshot[i], 4);
                    }
                }
            }
            ret = r0; break;
        case 12: ret = strcoll(arm_str(m, r0), arm_str(m, r1)); break;
        case 15: ret = strlen(arm_str(m, r0)); break;
        case 16: {
            char *hay = (char *)arm_ptr(m, r0);
            char *nee = (char *)arm_ptr(m, r1);
            if (!hay || !nee) { ret = 0; break; }
            char *p = strstr(hay, nee);
            ret = p ? arm_addr(m, p) : 0;
        } break;
        case 17: { char buf[1024]; ret = format_arm(m, buf, sizeof(buf), arm_str(m, r1), 2); strcpy(arm_ptr(m, r0), buf); } break;
        case 18: ret = atoi(arm_str(m, r0)); break;
        case 19: ret = (uint32_t)strtoul(arm_str(m, r0), NULL, r1); break;
        case 20: ret = mr_rand(); break;
        case 22:
            ret = mr_stop_ex((int16)r0);
            internal_slot_write(m, m->mr_state_slot, 0);
            internal_slot_write(m, m->mr_timer_state_slot, 0);
            m->host_timer_pending = 0;
            m->timer_p_addr = 0;
            m->timer_helper_addr = 0;
            break;
        case 25: {
            uint32_t lr = reg_read32(m->uc, UC_ARM_REG_LR);
            uint32_t nested_file_addr = 0;
            uint32_t nested_file_len = 0;
            int nested = arm_ext_nested_exec_range_for_lr(
                m, lr, &nested_file_addr, &nested_file_len);
            uint32_t p_addr = nested ? m->nested_p_addr : 0;
            int reuse_nested_p = p_addr && r1 == sizeof(mr_c_function_P_t) && arm_ptr(m, p_addr);
            uint32_t p_len = r1;
            /* wrapper ext 的 dispatch 入口会将 SP 设为 P + 0x20000（见 gxdzc 的
             * 0x800FC: mov sp, P+0x20000），无论 ext 请求的 P 大小是多少都假定
             * 有 128KB 栈空间。确保分配至少 EXT_WRAPPER_STACK_SIZE 避免栈溢出
             * 到代码段导致数据损坏。 */
            if (!nested && p_len < EXT_WRAPPER_STACK_SIZE + sizeof(mr_c_function_P_t)) {
                p_len = EXT_WRAPPER_STACK_SIZE + sizeof(mr_c_function_P_t);
            }
            if (!p_addr || r1 != sizeof(mr_c_function_P_t) || !arm_ptr(m, p_addr)) {
                p_addr = arm_alloc(m, p_len);
            }
            if (p_addr && !reuse_nested_p) memset(arm_ptr(m, p_addr), 0, p_len);
            if (nested) {
                memcpy(arm_ptr(m, nested_file_addr + 4), &p_addr, 4);
                uint32_t wrapper_rw = 0;
                memcpy(&wrapper_rw, arm_ptr(m, m->p_addr), 4);
                if (wrapper_rw) {
                    uint32_t callback_slot = wrapper_rw + 0x1A0u;
                    if (arm_ptr(m, callback_slot))
                        memcpy(arm_ptr(m, callback_slot), &r0, 4);
                }
                m->active_helper_addr = r0;
                m->active_p_addr = p_addr;
                arm_ext_clear_foreground_screen_owner(m);
                arm_ext_record_nested_module(m, nested_file_addr,
                                             nested_file_len, p_addr, r0);
                if (!m->primary_helper_addr) {
                    m->primary_helper_addr = r0;
                    m->primary_p_addr = p_addr;
                    m->primary_file_addr = nested_file_addr;
                    m->primary_file_len = nested_file_len;
                    /* Host table[25] only registers the child helper. Unlike
                     * cfunction.ext's internal loader, it has not called the
                     * child lifecycle handler yet, so mythroad.c must issue
                     * one foreground code=0 call after the wrapper returns. */
                    m->primary_host_init_pending = 1;
                    /* wrapper 的 suspend/resume 通过 extChunk[8] 向 game
                     * 派发 pauseApp/resumeApp。真机上 wrapper 在加载 nested
                     * ext 时设置此字段，VMRP 下可能未被初始化（保持为 0），
                     * 导致 suspend/resume 的 blx extChunk[8] 跳转到 0。
                     * 此处在首次设置 primary_helper 时补写。 */
                    uint32_t np = p_addr;
                    uint32_t ec = 0;
                    if (np && arm_ptr(m, np + 12))
                        memcpy(&ec, arm_ptr(m, np + 12), 4);
                    /* （extChunk[8] 修复移到 modal dispatch 路由中动态执行，
                     * 因为 wrapper ARM 代码会在运行中覆盖此字段） */
                } else if (m->primary_file_addr) {
                    memcpy(arm_ptr(m, m->primary_file_addr + 4), &p_addr, 4);
                }
                if (getenv("VMRP_ARM_EXT_TRACE")) {
                    printf("arm_ext_executor: nested helper=0x%X P=0x%X len=%u primary=0x%X/0x%X\n",
                           r0, p_addr, r1, m->primary_helper_addr, m->primary_p_addr);
                }
                m->nested_loading = 1;
                m->nested_p_addr = 0;
            } else {
                m->helper_addr = r0;
                m->p_addr = p_addr;
                m->active_helper_addr = r0;
                m->active_p_addr = p_addr;
                arm_ext_clear_foreground_screen_owner(m);
                memcpy(arm_ptr(m, EXT_CODE_ADDR + 4), &m->p_addr, 4);
            }
            ret = p_addr ? MR_SUCCESS : MR_FAILED;
        } break;
        case 26: {
            char buf[1024];
            const char *fmt = arm_str(m, r0);
            format_arm(m, buf, sizeof(buf), fmt, 1);
            mr_printf("%s", buf);
            ret = 0;
        } break;
        case 27: {
            uint32_t base = m->origin_mem_addr;
            uint32_t len = m->origin_mem_len;
            if (r0) memcpy(arm_ptr(m, r0), &base, 4);
            if (r1) memcpy(arm_ptr(m, r1), &len, 4);
            ret = base ? MR_SUCCESS : MR_FAILED;
        } break;
        case 28: ret = MR_SUCCESS; break;
        case 29: {
            void *bmp = arm_ptr(m, r0);
            uint16_t h = (uint16_t)arg_read(m, 4);
            uint32_t claim_p = 0, claim_helper = 0;
            if (bmp) {
                int screen_stride_source =
                    arm_ext_bitmap_source_uses_screen_stride(m, r0);
                int32_t source_stride = screen_stride_source ?
                    arm_ext_screen_stride(m) : (int32_t)(uint16)r3;
                int32_t source_x = screen_stride_source ? (int16)r1 : 0;
                int32_t source_y = screen_stride_source ? (int16)r2 : 0;
                int accept = arm_ext_should_accept_visible_present(m, &claim_p,
                                                                   &claim_helper);
                if (getenv("VMRP_ARM_EXT_DIAG")) {
                    int covered_diag = arm_ext_owner_is_covered_by_foreground(
                        m, claim_p, claim_helper);
                    printf("DIAG present29 x=%d y=%d w=%u h=%u accept=%d claimP=0x%X claimH=0x%X covered=%d\n",
                           (int16)r1, (int16)r2, (uint16)r3, h, accept,
                           claim_p, claim_helper, covered_diag);
                    arm_ext_diag_dump_layer_state(m, "present29");
                }
                if (accept) {
                    int covered = arm_ext_owner_is_covered_by_foreground(
                        m, claim_p, claim_helper);
                    arm_ext_present_bitmap_rect(m, bmp, (int16)r1, (int16)r2,
                                                (uint16)r3, h,
                                                source_stride, source_x,
                                                source_y, covered);
                }
                /*
                 * A rejected foreground present is still a write into the
                 * shared backing screen cache when its source is a bitmap.
                 * Foreground overlays can later proxy uncovered regions from
                 * that cache; skipping this mirror leaves those regions black
                 * even though the visible submit was correctly blocked.
                 */
                arm_ext_mirror_draw_bitmap_to_screen(m, r0, (int16)r1,
                                                     (int16)r2,
                                                     (uint16)r3, h,
                                                     source_stride, source_x,
                                                     source_y);
                if (accept) {
                    arm_ext_finish_accepted_screen_write(m, claim_p,
                                                         claim_helper);
                    arm_ext_claim_foreground_screen_rect(m, claim_p,
                                                         claim_helper,
                                                         (int16)r1,
                                                         (int16)r2,
                                                         (uint16)r3, h);
                    if (arm_ext_owner_is_foreground_child(m, claim_p,
                                                          claim_helper)) {
                        arm_ext_note_foreground_cover_rect(m, (int16)r1,
                                                           (int16)r2,
                                                           (uint16)r3, h);
                    }
                }
            }
            ret = MR_SUCCESS;
        } break;
        case 30: {
            int width = 0;
            int height = 0;
            const char *bitmap = mr_getCharBitmap((uint16)r0, (uint16)r1, &width, &height);
            // 根据字体实际尺寸计算位图大小：gb12=18字节(12行×12位紧凑), gb16=32字节(16行×2字节)
            int bitmap_size = ((width * height) + 7) >> 3;
            if (r2 && arm_ptr(m, r2)) memcpy(arm_ptr(m, r2), &width, 4);
            if (r3 && arm_ptr(m, r3)) memcpy(arm_ptr(m, r3), &height, 4);
            if (bitmap) {
                if (!m->char_bitmap_addr) m->char_bitmap_addr = arm_alloc(m, 32);
                if (m->char_bitmap_addr) {
                    memcpy(arm_ptr(m, m->char_bitmap_addr), bitmap, bitmap_size);
                    ret = m->char_bitmap_addr;
                } else {
                    ret = 0;
                }
            } else {
                ret = 0;
            }
        } break;
        case 31: {
            uint32_t pc_diag = 0;
            uint32_t lr_diag = 0;
            if (getenv("VMRP_ARM_EXT_DIAG")) {
                pc_diag = reg_read32(m->uc, UC_ARM_REG_PC);
                lr_diag = reg_read32(m->uc, UC_ARM_REG_LR);
                printf("DIAG table31_pre t=%u pc=0x%X lr=0x%X r0=0x%X r1=0x%X r2=0x%X r3=0x%X r4=0x%X r5=0x%X r6=0x%X r7=0x%X r9=0x%X activeP=0x%X activeH=0x%X currentP=0x%X currentH=0x%X timerP=0x%X timerH=0x%X\n",
                       (uint16)r0, pc_diag, lr_diag,
                       reg_read32(m->uc, UC_ARM_REG_R0),
                       reg_read32(m->uc, UC_ARM_REG_R1),
                       reg_read32(m->uc, UC_ARM_REG_R2),
                       reg_read32(m->uc, UC_ARM_REG_R3),
                       reg_read32(m->uc, UC_ARM_REG_R4),
                       reg_read32(m->uc, UC_ARM_REG_R5),
                       reg_read32(m->uc, UC_ARM_REG_R6),
                       reg_read32(m->uc, UC_ARM_REG_R7),
                       reg_read32(m->uc, UC_ARM_REG_R9),
                       m->active_p_addr, m->active_helper_addr,
                       m->current_p_addr, m->current_helper_addr,
                       m->timer_p_addr, m->timer_helper_addr);
            }
            ret = mr_timerStart((uint16)r0);
            internal_slot_write(m, m->mr_timer_state_slot, 1);
            mr_timer_state = 1;
            m->host_timer_pending = 1;
            arm_ext_record_timer_owner(m);
            if (getenv("VMRP_ARM_EXT_DIAG")) {
                printf("DIAG table31_post ret=0x%X pc=0x%X lr=0x%X r0=0x%X r1=0x%X r2=0x%X r3=0x%X r4=0x%X r5=0x%X r6=0x%X r7=0x%X r9=0x%X activeP=0x%X activeH=0x%X currentP=0x%X currentH=0x%X timerP=0x%X timerH=0x%X\n",
                       ret, reg_read32(m->uc, UC_ARM_REG_PC),
                       reg_read32(m->uc, UC_ARM_REG_LR),
                       reg_read32(m->uc, UC_ARM_REG_R0),
                       reg_read32(m->uc, UC_ARM_REG_R1),
                       reg_read32(m->uc, UC_ARM_REG_R2),
                       reg_read32(m->uc, UC_ARM_REG_R3),
                       reg_read32(m->uc, UC_ARM_REG_R4),
                       reg_read32(m->uc, UC_ARM_REG_R5),
                       reg_read32(m->uc, UC_ARM_REG_R6),
                       reg_read32(m->uc, UC_ARM_REG_R7),
                       reg_read32(m->uc, UC_ARM_REG_R9),
                       m->active_p_addr, m->active_helper_addr,
                       m->current_p_addr, m->current_helper_addr,
                       m->timer_p_addr, m->timer_helper_addr);
            }
            break;
        }
        case 32: {
            ret = mr_timerStop();
            internal_slot_write(m, m->mr_timer_state_slot, 0);
            mr_timer_state = 0;
            m->host_timer_pending = 0;
            m->timer_p_addr = 0;
            m->timer_helper_addr = 0;
        } break;
        case 33: {
            ret = mr_getTime();
            /* 部分 ext 用忙等循环反复调 mr_getTime() 等待真实时间流逝（如
             * 菜单切换动画、定时器主循环）。真机上 OS 抢占让时钟自然推进；
             * 模拟器里需 usleep 让宿主时间走动。连续 200 次无其它 table 调
             * 用即视为忙等，每轮 usleep 5ms；超 3 秒强制停止 ARM 执行，
             * 让宿主侧定时器有机会触发再重新进入。 */
            m->busy_wait_count++;
            if (m->busy_wait_count == 1) {
                m->busy_wait_start_ms = ret;
            }
            if (m->busy_wait_count >= 200) {
                usleep(5 * 1000);
                m->busy_wait_count = 100;
                uint32_t elapsed_ms = ret - m->busy_wait_start_ms;
                if (elapsed_ms >= 3000) {
                    m->busy_wait_count = 0;
                    m->busy_wait_start_ms = 0;
                    reg_write32(m->uc, UC_ARM_REG_PC, EXT_STOP_ADDR);
                    uc_emu_stop(m->uc);
                }
            }
        } break;
        case 34: ret = mr_getDatetime(arm_ptr(m, r0)); break;
        case 35: ret = mr_getUserInfo(arm_ptr(m, r0)); break;
        case 36: ret = mr_sleep(r0); break;
        case 37:
            ret = mr_plat((int32)r0, (int32)r1);
            if (getenv("VMRP_ARM_EXT_DIAG")) {
                uint32_t owner_p = 0, owner_h = 0;
                uint32_t owner_file = 0, owner_len = 0;
                arm_ext_diag_owner_for_lr(m, &owner_p, &owner_h,
                                          &owner_file, &owner_len);
                printf("DIAG table37 code=%d param=0x%X ret=0x%X lr=0x%X ownerP=0x%X ownerH=0x%X ownerFile=0x%X ownerLen=%u activeP=0x%X primaryP=0x%X\n",
                       (int32_t)r0, r1, ret,
                       reg_read32(m->uc, UC_ARM_REG_LR),
                       owner_p, owner_h, owner_file, owner_len,
                       m->active_p_addr, m->primary_p_addr);
            }
            break;
        case 38: {
            int diag_enabled = getenv("VMRP_ARM_EXT_DIAG") != NULL;
            uint32_t diag_lr = 0;
            uint32_t diag_owner_p = 0, diag_owner_h = 0;
            uint32_t diag_owner_file = 0, diag_owner_len = 0;
            char diag_input_preview[192];
            diag_input_preview[0] = '\0';
            if (diag_enabled) {
                diag_lr = reg_read32(m->uc, UC_ARM_REG_LR);
                arm_ext_diag_owner_for_lr(m, &diag_owner_p, &diag_owner_h,
                                          &diag_owner_file, &diag_owner_len);
                arm_ext_diag_preview_bytes(arm_ptr(m, r1), r2,
                                           diag_input_preview,
                                           sizeof(diag_input_preview));
            }
            /*
             * MR_MALLOC_SCRRAM/MR_FREE_SCRRAM provide scratch RAM that native
             * code treats as ARM-visible storage, often for large off-screen
             * framebuffers. Allocate it from the emulated ARM heap so later
             * pointer arithmetic and blits can access the returned address.
             */
            if (r0 == 1014) {
                uint32_t outp = r3, outlenp = arg_read(m, 4);
                uint32_t want = (uint32_t)r2;
                uint32_t a = 0;
                if (want) {
                    if (m->exram_addr && want <= m->exram_len) {
                        a = m->exram_addr;          /* 复用已有 exRam（幂等） */
                    } else {
                        a = arm_alloc(m, want);
                        if (a) {
                            m->exram_addr = a;
                            m->exram_len = want;
                            void *ep = arm_ptr(m, a);
                            if (ep) memset(ep, 0, want);
                        }
                    }
                }
                if (a) {
                    if (outp) uc_mem_write(m->uc, outp, &a, 4);
                    if (outlenp) uc_mem_write(m->uc, outlenp, &want, 4);
                }
                ret = a ? MR_SUCCESS : MR_FAILED;
                if (diag_enabled) {
                    printf("DIAG table38 code=%d input=0x%X inputLen=%u outputp=0x%X outputLenp=0x%X ret=0x%X out=0x%X outLen=%u inputPreview='%s' lr=0x%X ownerP=0x%X ownerH=0x%X ownerFile=0x%X ownerLen=%u activeP=0x%X primaryP=0x%X\n",
                           (int32_t)r0, r1, r2, r3, arg_read(m, 4), ret,
                           a, want, diag_input_preview, diag_lr,
                           diag_owner_p, diag_owner_h, diag_owner_file,
                           diag_owner_len, m->active_p_addr,
                           m->primary_p_addr);
                }
                break;
            }
            if (r0 == 1015) {
                ret = MR_SUCCESS;
                if (diag_enabled) {
                    printf("DIAG table38 code=%d input=0x%X inputLen=%u outputp=0x%X outputLenp=0x%X ret=0x%X out=0x0 outLen=0 inputPreview='%s' lr=0x%X ownerP=0x%X ownerH=0x%X ownerFile=0x%X ownerLen=%u activeP=0x%X primaryP=0x%X\n",
                           (int32_t)r0, r1, r2, r3, arg_read(m, 4), ret,
                           diag_input_preview, diag_lr, diag_owner_p,
                           diag_owner_h, diag_owner_file, diag_owner_len,
                           m->active_p_addr, m->primary_p_addr);
                }
                break;
            }
            uint32_t outputp_addr = r3;
            uint32_t output_lenp_addr = arg_read(m, 4);
            uint32_t arm_output = 0;
            int32 host_output_len = 0;
            uint8 *host_output = NULL;
            uint8 *input = (uint8 *)arm_ptr(m, r1);

            if (outputp_addr) {
                uc_mem_read(m->uc, outputp_addr, &arm_output, 4);
                host_output = arm_output ? (uint8 *)arm_ptr(m, arm_output) : NULL;
            }
            if (output_lenp_addr) {
                uc_mem_read(m->uc, output_lenp_addr, &host_output_len, 4);
            }

            ret = mr_platEx((int32)r0, input, (int32)r2,
                            outputp_addr ? &host_output : NULL,
                            output_lenp_addr ? &host_output_len : NULL,
                            NULL);

            if (outputp_addr) {
                uint32_t new_arm_output = arm_output;
                if (host_output && host_output_len > 0) {
                    void *old_arm_ptr = arm_output ? arm_ptr(m, arm_output) : NULL;
                    if (host_output != old_arm_ptr) {
                        new_arm_output = arm_alloc(m, (uint32_t)host_output_len + 1);
                        if (new_arm_output) {
                            memcpy(arm_ptr(m, new_arm_output), host_output, (uint32_t)host_output_len);
                            ((uint8 *)arm_ptr(m, new_arm_output))[host_output_len] = '\0';
                        }
                    }
                } else {
                    new_arm_output = 0;
                }
                uc_mem_write(m->uc, outputp_addr, &new_arm_output, 4);
            }
            if (output_lenp_addr) {
                uc_mem_write(m->uc, output_lenp_addr, &host_output_len, 4);
            }
            if (diag_enabled) {
                uint32_t new_arm_output = 0;
                int32_t new_output_len = 0;
                char output_preview[192];
                output_preview[0] = '\0';
                if (outputp_addr)
                    uc_mem_read(m->uc, outputp_addr, &new_arm_output, 4);
                if (output_lenp_addr)
                    uc_mem_read(m->uc, output_lenp_addr, &new_output_len, 4);
                arm_ext_diag_preview_bytes(arm_ptr(m, new_arm_output),
                                           new_output_len > 0 ?
                                           (uint32_t)new_output_len : 0,
                                           output_preview,
                                           sizeof(output_preview));
                printf("DIAG table38 code=%d input=0x%X inputLen=%u outputp=0x%X outputLenp=0x%X ret=0x%X out=0x%X outLen=%d inputPreview='%s' outputPreview='%s' lr=0x%X ownerP=0x%X ownerH=0x%X ownerFile=0x%X ownerLen=%u activeP=0x%X primaryP=0x%X\n",
                       (int32_t)r0, r1, r2, outputp_addr,
                       output_lenp_addr, ret, new_arm_output,
                       new_output_len, diag_input_preview, output_preview,
                       diag_lr, diag_owner_p, diag_owner_h,
                       diag_owner_file, diag_owner_len, m->active_p_addr,
                       m->primary_p_addr);
            }
        } break;
        case 39: ret = mr_ferrno(); break;
        case 40: {
            const char *open_name = arm_str(m, r0);
            ret = mr_open(arm_ext_pack_to_host_path(m, open_name), r1);
            /* 磁盘上找不到时尝试从 MRP 缓存提供虚拟 fd */
            if (ret == 0 && m->mrp_cache_count > 0) {
                MrpCacheEntry *ce = mrp_cache_find(m, open_name);
                if (ce) {
                    ret = mrp_vfd_open(m, ce->data, ce->data_len);
                }
            }
            if ((int32_t)ret > 0) {
                arm_ext_diag_fd_set(m, (int32_t)ret, open_name);
            }
            if (getenv("VMRP_ARM_EXT_DIAG")) {
                uint32_t owner_p = 0, owner_h = 0;
                ArmExtNestedModule *owner =
                    arm_ext_resource_owner_for_lr(m, &owner_p, &owner_h);
                printf("DIAG table40 name='%s' flags=0x%X ret=0x%X lr=0x%X ownerP=0x%X ownerH=0x%X ownerFile=0x%X ownerLen=%u activeP=0x%X primaryP=0x%X\n",
                       open_name, r1, ret, reg_read32(m->uc, UC_ARM_REG_LR),
                       owner_p, owner_h, owner ? owner->file_addr : 0,
                       owner ? owner->file_len : 0, m->active_p_addr,
                       m->primary_p_addr);
            }
        } break;
        case 41: {
            const char *diag_name = arm_ext_diag_fd_name(m, (int32_t)r0);
            MrpVirtualFd *vf = mrp_vfd_get(m, r0);
            if (vf) { vf->in_use = 0; ret = MR_SUCCESS; }
            else {
                ret = mr_close((int32)r0);
            }
            if (getenv("VMRP_ARM_EXT_DIAG")) {
                uint32_t owner_p = 0, owner_h = 0, owner_file = 0, owner_len = 0;
                arm_ext_diag_owner_for_lr(m, &owner_p, &owner_h,
                                          &owner_file, &owner_len);
                printf("DIAG table41 fd=%d name='%s' ret=0x%X lr=0x%X ownerP=0x%X ownerH=0x%X ownerFile=0x%X ownerLen=%u activeP=0x%X primaryP=0x%X\n",
                       (int32_t)r0, diag_name, ret,
                       reg_read32(m->uc, UC_ARM_REG_LR),
                       owner_p, owner_h, owner_file, owner_len,
                       m->active_p_addr, m->primary_p_addr);
            }
            if (ret == MR_SUCCESS) {
                arm_ext_diag_fd_clear(m, (int32_t)r0);
            }
        } break;
        case 42: {
            const char *info_name = arm_str(m, r0);
            ret = mr_info(arm_ext_pack_to_host_path(m, info_name));
            /* 磁盘上不存在时检查 MRP 缓存 */
            if (ret != MRP_IS_FILE && m->mrp_cache_count > 0) {
                if (mrp_cache_find(m, info_name))
                    ret = MRP_IS_FILE;
            }
            if (getenv("VMRP_ARM_EXT_DIAG")) {
                uint32_t owner_p = 0, owner_h = 0;
                ArmExtNestedModule *owner =
                    arm_ext_resource_owner_for_lr(m, &owner_p, &owner_h);
                printf("DIAG table42 name='%s' ret=0x%X lr=0x%X ownerP=0x%X ownerH=0x%X ownerFile=0x%X ownerLen=%u activeP=0x%X primaryP=0x%X\n",
                       info_name, ret, reg_read32(m->uc, UC_ARM_REG_LR),
                       owner_p, owner_h, owner ? owner->file_addr : 0,
                       owner ? owner->file_len : 0, m->active_p_addr,
                       m->primary_p_addr);
            }
        } break;
        case 43: {
            void *src = arm_ptr(m, r1);
            uint32_t len = r2;
            int substituted = 0;
            void *new_src = NULL;
            uint32_t new_len = 0;
            if (m->profile && m->profile->intercept_write &&
                m->profile->intercept_write(m, m->app_state, r0, r1, r2,
                                            &new_src, &new_len)) {
                src = new_src;
                len = new_len;
                substituted = 1;
            }
            ret = mr_write((int32)r0, src, len);
            if (substituted) {
                if (m->profile && m->profile->post_write_cleanup)
                    m->profile->post_write_cleanup(m->app_state);
                if (ret == (int32_t)len) ret = (int32_t)r2;
            }
            if (getenv("VMRP_ARM_EXT_DIAG")) {
                char preview[192];
                uint32_t owner_p = 0, owner_h = 0, owner_file = 0, owner_len = 0;
                arm_ext_diag_preview_bytes(src, len, preview, sizeof(preview));
                arm_ext_diag_owner_for_lr(m, &owner_p, &owner_h,
                                          &owner_file, &owner_len);
                printf("DIAG table43 fd=%d name='%s' src=0x%X len=%u ret=0x%X subst=%d preview='%s' lr=0x%X ownerP=0x%X ownerH=0x%X ownerFile=0x%X ownerLen=%u activeP=0x%X primaryP=0x%X\n",
                       (int32_t)r0, arm_ext_diag_fd_name(m, (int32_t)r0),
                       r1, r2, ret, substituted, preview,
                       reg_read32(m->uc, UC_ARM_REG_LR),
                       owner_p, owner_h, owner_file, owner_len,
                       m->active_p_addr, m->primary_p_addr);
            }
        } break;
        case 44: {
            void *hp = arm_ptr(m, r1);
            MrpVirtualFd *vf44 = mrp_vfd_get(m, r0);
            if (vf44) {
                uint32_t avail = vf44->pos < vf44->data_len ? vf44->data_len - vf44->pos : 0;
                uint32_t n = r2 < avail ? r2 : avail;
                if (n && hp) memcpy(hp, vf44->data + vf44->pos, n);
                vf44->pos += n;
                ret = (int32_t)n;
            } else {
                ret = mr_read((int32)r0, hp, r2);
            }
            if (getenv("VMRP_ARM_EXT_DIAG")) {
                char preview[192];
                uint32_t owner_p = 0, owner_h = 0, owner_file = 0, owner_len = 0;
                uint32_t overlap_lo = 0, overlap_hi = 0;
                uint32_t preview_len = (int32_t)ret > 0 ? (uint32_t)ret : 0;
                arm_ext_diag_preview_bytes(hp, preview_len, preview,
                                           sizeof(preview));
                arm_ext_diag_owner_for_lr(m, &owner_p, &owner_h,
                                          &owner_file, &owner_len);
                arm_ext_find_first_registered_code_overlap(
                    m, r1, preview_len, &overlap_lo, &overlap_hi);
                printf("DIAG table44 fd=%d name='%s' want=%u ret=0x%X dst=0x%X preview='%s' lr=0x%X ownerP=0x%X ownerH=0x%X ownerFile=0x%X ownerLen=%u activeP=0x%X primaryP=0x%X codeOverlap=0x%X..0x%X\n",
                       (int32_t)r0, arm_ext_diag_fd_name(m, (int32_t)r0),
                       r2, ret, r1, preview,
                       reg_read32(m->uc, UC_ARM_REG_LR),
                       owner_p, owner_h, owner_file, owner_len,
                       m->active_p_addr, m->primary_p_addr,
                       overlap_lo, overlap_hi);
            }
            if (m->profile && m->profile->post_read_hook)
                m->profile->post_read_hook(m, m->app_state, r1, r2, ret, hp);
            if ((int32_t)ret > 0)
                arm_ext_retire_modules_overwritten_by_data_read(
                    m, r1, (uint32_t)ret);
            /* dump/restore 读回整块模块内存后，需要：
             * 1) 修复 GOT 中的 bridge 函数指针（文件数据中是原始未重定位的值）
             * 2) invalidate Unicorn TB cache（否则执行旧翻译） */
            if ((int32_t)ret > 0 && (uint32_t)ret > 0x1000) {
                /* 仅在 dump0 恢复时（目标地址匹配）才恢复间隙数据 */
                int read_covers_primary =
                    m->primary_file_addr && m->primary_file_len &&
                    arm_ext_range_contains(r1, (uint32_t)ret,
                                           m->primary_file_addr,
                                           m->primary_file_len);
                uc_ctl_remove_cache(m->uc, r1, r1 + (uint32_t)ret);
                arm_ext_restore_primary_mapping_after_dump0(m, r1, (uint32_t)ret);
                /*
                 * table[44] also reads downloaded packages and RAM staging
                 * buffers.  Only rewrite bridge GOT slots when this read is
                 * proven to be an executable dump/restore of the primary
                 * image; otherwise compressed payload bytes can be corrupted.
                 */
                if (read_covers_primary && m->got_snapshot_base) {
                    uint32_t got_base = m->got_snapshot_base;
                    uint32_t read_end = r1 + (uint32_t)ret;
                    for (uint32_t i = 0; i < EXT_TABLE_COUNT; i++) {
                        uint32_t addr = got_base + i * 4;
                        if (m->got_snapshot[i] >= EXT_TABLE_ADDR &&
                            m->got_snapshot[i] < EXT_TABLE_ADDR + EXT_TABLE_COUNT * 4 &&
                            addr >= r1 && addr + 4 <= read_end &&
                            !arm_ext_should_skip_got_snapshot_restore(m, addr) &&
                            !app_should_protect_got_addr(m, addr)) {
                            memcpy(arm_ptr(m, addr), &m->got_snapshot[i], 4);
                        }
                    }
                }
            }
        } break;
        case 45: {
            MrpVirtualFd *vf45 = mrp_vfd_get(m, r0);
            if (vf45) {
                /* SEEK_SET=0, SEEK_CUR=1, SEEK_END=2 */
                int32_t new_pos;
                if ((int)r2 == 0) new_pos = (int32_t)r1;
                else if ((int)r2 == 1) new_pos = (int32_t)vf45->pos + (int32_t)r1;
                else new_pos = (int32_t)vf45->data_len + (int32_t)r1;
                if (new_pos < 0) new_pos = 0;
                if ((uint32_t)new_pos > vf45->data_len) new_pos = (int32_t)vf45->data_len;
                vf45->pos = (uint32_t)new_pos;
                ret = MR_SUCCESS;
            } else {
                ret = mr_seek((int32)r0, (int32)r1, (int)r2);
            }
        } break;
        case 46: {
            const char *len_name = arm_str(m, r0);
            ret = mr_getLen(arm_ext_pack_to_host_path(m, len_name));
            if (ret < 0 && m->mrp_cache_count > 0) {
                MrpCacheEntry *ce = mrp_cache_find(m, len_name);
                if (ce) ret = (int32_t)ce->data_len;
            }
            if (getenv("VMRP_ARM_EXT_DIAG")) {
                uint32_t owner_p = 0, owner_h = 0, owner_file = 0, owner_len = 0;
                arm_ext_diag_owner_for_lr(m, &owner_p, &owner_h,
                                          &owner_file, &owner_len);
                printf("DIAG table46 name='%s' ret=0x%X lr=0x%X ownerP=0x%X ownerH=0x%X ownerFile=0x%X ownerLen=%u activeP=0x%X primaryP=0x%X\n",
                       len_name, ret, reg_read32(m->uc, UC_ARM_REG_LR),
                       owner_p, owner_h, owner_file, owner_len,
                       m->active_p_addr, m->primary_p_addr);
            }
        } break;
        case 47: ret = mr_remove(arm_str(m, r0)); break;
        case 48: {
            const char *old_name = arm_str(m, r0);
            const char *new_name = arm_str(m, r1);
            ret = mr_rename(old_name, new_name);
        } break;
        case 49: ret = mr_mkDir(arm_str(m, r0)); break;
        case 50: ret = mr_rmDir(arm_str(m, r0)); break;
        case 51: ret = mr_findStart(arm_str(m, r0), arm_ptr(m, r1), r2); break;
        case 52: ret = mr_findGetNext((int32)r0, arm_ptr(m, r1), r2); break;
        case 53: ret = mr_findStop((int32)r0); break;
        case 54: ret = mr_exit(); break;
        case 55: ret = mr_startShake((int32)r0); break;
        case 56: ret = mr_stopShake(); break;
        case 57: ret = mr_playSound((int)r0, arm_ptr(m, r1), r2, (int32)r3); break;
        case 58: ret = mr_stopSound((int)r0); break;
        /* table[59] mr_sendSms(pNumber, pContent, encode) */
        case 59: {
            int32 flags = (int32)r2;
            ret = mr_sendSms(arm_str(m, r0), arm_str(m, r1), flags);
            if (ret == MR_SUCCESS && (flags & ARM_EXT_MR_SMS_RESULT_FLAG)) {
                m->pending_sms_result = 1;
                m->pending_sms_result_value = ret;
            }
        } break;
        case 60: mr_call(arm_str(m, r0)); ret = MR_SUCCESS; break;
        /* table[61] mr_getNetworkID()：返回 0 表示移动网络（MR_NET_ID_MOBILE） */
        case 61: ret = 0; break;
        /* table[81] mr_initNetwork(cb, mode)：读取 mode 并设置 isCMWAP 标志，
         * 同步返回成功。桌面端没有真实 GPRS 网络，但需要 isCMWAP 控制后续
         * send/recv 的代理模拟逻辑。 */
        case 81: {
            const char *mode_str = arm_str(m, r1);
            ret = mr_initNetwork(NULL, mode_str);
        } break;
        /* table[82] mr_closeNetwork()：return success，跟 mr_initNetwork 配对。 */
        case 82: ret = MR_SUCCESS; break;
        /* table[83] mr_getHostByName(name, cb)。
         * 异步回调 cb 是 ARM 虚拟地址，不能在宿主线程调用（会把 ARM
         * 地址当宿主函数指针，段错误）。绕过 DSM 回调链直接调用底层
         * my_getHostByName(cb=NULL) 走同步 DNS 解析路径，返回 IP。 */
        case 83: {
            const char *host = arm_str(m, r0);
            ret = my_getHostByName(NULL, host, NULL, NULL);
        } break;
        case 84: ret = mr_socket((int32)r0, (int32)r1); break;
        case 85: ret = mr_connect((int32)r0, (int32)r1, (uint16)r2, (int32)r3); break;
        case 86:
            ret = mr_closeSocket((int32)r0);
            if (getenv("VMRP_ARM_EXT_DIAG")) {
                uint32_t owner_p = 0, owner_h = 0, owner_file = 0, owner_len = 0;
                arm_ext_diag_owner_for_lr(m, &owner_p, &owner_h,
                                          &owner_file, &owner_len);
                printf("DIAG table86 socket=%d ret=0x%X lr=0x%X ownerP=0x%X ownerH=0x%X ownerFile=0x%X ownerLen=%u activeP=0x%X primaryP=0x%X\n",
                       (int32_t)r0, ret, reg_read32(m->uc, UC_ARM_REG_LR),
                       owner_p, owner_h, owner_file, owner_len,
                       m->active_p_addr, m->primary_p_addr);
            }
            break;
        case 87: {
            void *dst = arm_ptr(m, r1);
            ret = mr_recv((int32)r0, dst, (int)r2);
            if (getenv("VMRP_ARM_EXT_DIAG")) {
                char preview[192];
                uint32_t owner_p = 0, owner_h = 0, owner_file = 0, owner_len = 0;
                uint32_t preview_len = (int32_t)ret > 0 ? (uint32_t)ret : 0;
                arm_ext_diag_preview_bytes(dst, preview_len, preview,
                                           sizeof(preview));
                arm_ext_diag_owner_for_lr(m, &owner_p, &owner_h,
                                          &owner_file, &owner_len);
                printf("DIAG table87 socket=%d want=%u ret=0x%X dst=0x%X preview='%s' lr=0x%X ownerP=0x%X ownerH=0x%X ownerFile=0x%X ownerLen=%u activeP=0x%X primaryP=0x%X openSockets=%d\n",
                       (int32_t)r0, r2, ret, r1, preview,
                       reg_read32(m->uc, UC_ARM_REG_LR),
                       owner_p, owner_h, owner_file, owner_len,
                       m->active_p_addr, m->primary_p_addr,
                       my_openSocketCount());
            }
        } break;
        case 88: { uint32_t a4 = arg_read(m, 4);
                   ret = mr_recvfrom((int32)r0, arm_ptr(m, r1), (int)r2,
                                    (int32 *)arm_ptr(m, r3), (uint16 *)arm_ptr(m, a4));
                 } break;
        case 89: {
            void *src = arm_ptr(m, r1);
            if (getenv("VMRP_ARM_EXT_DIAG")) {
                char preview[192];
                uint32_t owner_p = 0, owner_h = 0, owner_file = 0, owner_len = 0;
                arm_ext_diag_preview_bytes(src, r2, preview, sizeof(preview));
                arm_ext_diag_owner_for_lr(m, &owner_p, &owner_h,
                                          &owner_file, &owner_len);
                printf("DIAG table89_pre socket=%d len=%u src=0x%X preview='%s' lr=0x%X ownerP=0x%X ownerH=0x%X ownerFile=0x%X ownerLen=%u activeP=0x%X primaryP=0x%X\n",
                       (int32_t)r0, r2, r1, preview,
                       reg_read32(m->uc, UC_ARM_REG_LR),
                       owner_p, owner_h, owner_file, owner_len,
                       m->active_p_addr, m->primary_p_addr);
            }
            ret = mr_send((int32)r0, src, (int)r2);
            if (getenv("VMRP_ARM_EXT_DIAG")) {
                printf("DIAG table89_post socket=%d len=%u ret=0x%X openSockets=%d\n",
                       (int32_t)r0, r2, ret, my_openSocketCount());
            }
        } break;
        case 90: { uint32_t a4 = arg_read(m, 4);
                   ret = mr_sendto((int32)r0, arm_ptr(m, r1), (int)r2, (int32)r3, (uint16)a4);
                 } break;
        case 69: ret = mr_dialogCreate(arm_str(m, r0), arm_str(m, r1), (int32)r2); break;
        case 70: ret = mr_dialogRelease((int32)r0); break;
        case 71: ret = mr_dialogRefresh((int32)r0, arm_str(m, r1), arm_str(m, r2), (int32)r3); break;
        case 72: ret = mr_textCreate(arm_str(m, r0), arm_str(m, r1), (int32)r2); break;
        case 73: ret = mr_textRelease((int32)r0); break;
        case 74: ret = mr_textRefresh((int32)r0, arm_str(m, r1), arm_str(m, r2)); break;
        case 75: ret = mr_editCreate(arm_str(m, r0), arm_str(m, r1), (int32)r2, (int32)r3); break;
        case 76: ret = mr_editRelease((int32)r0); break;
        case 77: {
            const char *text = mr_editGetText((int32)r0);
            /* mr_editGetText returns UCS2-BE text.  ASCII therefore contains a
             * zero high byte before every character, so strlen() would copy only
             * the first byte and make ARM browser editors see an empty string. */
            ret = text ? alloc_bytes(m, text, (uint32_t)wstrlen((char *)text) + 2) : 0;
        } break;
        /* table[78] mr_winCreate / table[79] mr_winRelease: 窗口功能不支持 */
        case 78: ret = MR_IGNORE; break;
        case 79: ret = MR_IGNORE; break;
        case 80: ret = mr_getScreenInfo(arm_ptr(m, r0)); break;
        case 113: {
            void *p = arm_ptr(m, r0);
            if (p) mr_md5_init(p);
            ret = 0;
        } break;
        case 114: {
            void *p = arm_ptr(m, r0);
            void *d = arm_ptr(m, r1);
            if (p && d) mr_md5_append(p, d, (int)r2);
            ret = 0;
        } break;
        case 115: {
            void *p = arm_ptr(m, r0);
            void *digest = arm_ptr(m, r1);
            if (p && digest) mr_md5_finish(p, digest);
            ret = 0;
        } break;
        case 118:
            if (getenv("VMRP_ARM_EXT_DIAG")) {
                printf("DIAG DispUpEx x=%d y=%d w=%u h=%u currentP=0x%X currentH=0x%X activeP=0x%X activeH=0x%X\n",
                       (int16)r0, (int16)r1, (uint16)r2, (uint16)r3,
                       m->current_p_addr, m->current_helper_addr,
                       m->active_p_addr, m->active_helper_addr);
            }
            {
                uint32_t claim_p = 0, claim_helper = 0;
                int accept = arm_ext_should_accept_visible_present(m, &claim_p,
                                                                   &claim_helper);
                if (getenv("VMRP_ARM_EXT_DIAG")) {
                    int covered_diag = arm_ext_owner_is_covered_by_foreground(
                        m, claim_p, claim_helper);
                    printf("DIAG present118 x=%d y=%d w=%u h=%u accept=%d claimP=0x%X claimH=0x%X covered=%d\n",
                           (int16)r0, (int16)r1, (uint16)r2, (uint16)r3,
                           accept, claim_p, claim_helper, covered_diag);
                    arm_ext_diag_dump_layer_state(m, "present118");
                }
                if (accept) {
                    ArmExtScreenContext screen_ctx;
                    if (arm_ext_push_draw_screen_context(m, &screen_ctx) &&
                        arm_ext_screen_context_targets_primary(m, &screen_ctx)) {
                        int covered = arm_ext_owner_is_covered_by_foreground(
                            m, claim_p, claim_helper);
                        ret = arm_ext_dispup_rect(m, (int16)r0, (int16)r1,
                                                  (uint16)r2, (uint16)r3,
                                                  covered);
                        arm_ext_finish_accepted_screen_write(m, claim_p,
                                                             claim_helper);
                        arm_ext_claim_foreground_screen_rect(m, claim_p,
                                                             claim_helper,
                                                             (int16)r0,
                                                             (int16)r1,
                                                             (uint16)r2,
                                                             (uint16)r3);
                        if (arm_ext_owner_is_foreground_child(m, claim_p,
                                                              claim_helper)) {
                            arm_ext_note_foreground_cover_rect(m, (int16)r0,
                                                               (int16)r1,
                                                               (uint16)r2,
                                                               (uint16)r3);
                        }
                    } else {
                        ret = 0;
                    }
                    arm_ext_pop_draw_screen_context(&screen_ctx);
                } else {
                    ret = 0;
                }
            }
            break;
        case 119:
            {
                uint32_t claim_p = 0, claim_helper = 0;
                if (!arm_ext_should_accept_screen_write(m, &claim_p,
                                                        &claim_helper)) {
                    claim_p = 0;
                    claim_helper = 0;
                }
                ArmExtScreenContext screen_ctx;
                if (arm_ext_push_draw_screen_context(m, &screen_ctx)) {
                    _DrawPoint((int16)r0, (int16)r1, (uint16)r2);
                    arm_ext_pop_draw_screen_context(&screen_ctx);
                    arm_ext_note_screen_damage_rect(m, (int16)r0,
                                                    (int16)r1, 1, 1);
                    arm_ext_claim_foreground_screen_rect(m, claim_p,
                                                         claim_helper,
                                                         (int16)r0,
                                                         (int16)r1, 1, 1);
                    arm_ext_finish_screen_cache_write(m, &screen_ctx,
                                                      claim_p,
                                                      claim_helper);
                }
            }
            ret = 0;
            break;
        case 120:
            {
                uint32_t claim_p = 0, claim_helper = 0;
                uint16_t h = (uint16_t)arg_read(m, 4);
                uint16_t rop = (uint16_t)arg_read(m, 5);
                uint16_t trans = (uint16_t)arg_read(m, 6);
                int16_t sx = (int16)arg_read(m, 7);
                int16_t sy = (int16)arg_read(m, 8);
                int16_t mw = (int16)arg_read(m, 9);
                if (!arm_ext_should_accept_screen_write(m, &claim_p,
                                                        &claim_helper)) {
                    claim_p = 0;
                    claim_helper = 0;
                }
                ArmExtScreenContext screen_ctx;
                if (arm_ext_push_draw_screen_context(m, &screen_ctx)) {
                    uint16_t *before = arm_ext_snapshot_screen(m);
                    _DrawBitmap(arm_ptr(m, r0), (int16)r1, (int16)r2,
                                (uint16)r3, h, rop, trans, sx, sy, mw);
                    arm_ext_pop_draw_screen_context(&screen_ctx);
                    arm_ext_note_screen_damage_diff(m, before);
                    arm_ext_claim_foreground_screen_diff(m, claim_p,
                                                         claim_helper,
                                                         before);
                    free(before);
                    arm_ext_finish_screen_cache_write(m, &screen_ctx,
                                                      claim_p,
                                                      claim_helper);
                }
            }
            ret = 0;
            break;
        case 122:
            {
                uint32_t claim_p = 0, claim_helper = 0;
                if (!arm_ext_should_accept_screen_write(m, &claim_p,
                                                        &claim_helper)) {
                    claim_p = 0;
                    claim_helper = 0;
                }
                ArmExtScreenContext screen_ctx;
                if (arm_ext_push_draw_screen_context(m, &screen_ctx)) {
                    DrawRect((int16)r0, (int16)r1, (int16)r2, (int16)r3,
                             (uint8)arg_read(m, 4),
                             (uint8)arg_read(m, 5),
                             (uint8)arg_read(m, 6));
                    arm_ext_pop_draw_screen_context(&screen_ctx);
                    arm_ext_note_screen_damage_rect(m, (int16)r0,
                                                    (int16)r1,
                                                    (int16)r2,
                                                    (int16)r3);
                    arm_ext_claim_foreground_screen_rect(m, claim_p,
                                                         claim_helper,
                                                         (int16)r0,
                                                         (int16)r1,
                                                         (int16)r2,
                                                         (int16)r3);
                    arm_ext_finish_screen_cache_write(m, &screen_ctx,
                                                      claim_p,
                                                      claim_helper);
                }
            }
            ret = 0;
            break;
        case 123:
            {
                uint32_t claim_p = 0, claim_helper = 0;
                if (!arm_ext_should_accept_screen_write(m, &claim_p,
                                                        &claim_helper)) {
                    claim_p = 0;
                    claim_helper = 0;
                }
                ArmExtScreenContext screen_ctx;
                if (arm_ext_push_draw_screen_context(m, &screen_ctx)) {
                    uint16_t *before = arm_ext_snapshot_screen(m);
                    ret = _DrawText(arm_str(m, r0), (int16)r1, (int16)r2,
                                    (uint8)r3, (uint8)arg_read(m, 4),
                                    (uint8)arg_read(m, 5),
                                    (int)arg_read(m, 6),
                                    (uint16)arg_read(m, 7));
                    arm_ext_pop_draw_screen_context(&screen_ctx);
                    arm_ext_note_screen_damage_diff(m, before);
                    arm_ext_claim_foreground_screen_diff(m, claim_p,
                                                         claim_helper,
                                                         before);
                    free(before);
                    arm_ext_finish_screen_cache_write(m, &screen_ctx,
                                                      claim_p,
                                                      claim_helper);
                } else {
                    ret = 0;
                }
            }
            break;
        case 124: {
            ArmExtScreenContext screen_ctx;
            if (arm_ext_push_draw_screen_context(m, &screen_ctx)) {
                ret = _BitmapCheck(arm_ptr(m, r0), (int16)r1, (int16)r2,
                                   (uint16)r3, (uint16)arg_read(m, 4),
                                   (uint16)arg_read(m, 5),
                                   (uint16)arg_read(m, 6));
                arm_ext_pop_draw_screen_context(&screen_ctx);
            } else {
                ret = 0;
            }
        } break;
        case 125: {
            int fl = 0;
            uint32_t packp = 0, ramp_slot = 0, raml_slot = 0, ramp = 0, raml = 0;
            uint32_t read_pack_ram_addr = 0, read_pack_ram_len = 0;
            char read_pack_host_path[PATH_MAX];
            read_pack_host_path[0] = '\0';
            const char *read_name = arm_str(m, r0);
            uc_mem_read(m->uc, EXT_TABLE_ADDR + 100 * 4, &packp, 4);
            uc_mem_read(m->uc, EXT_TABLE_ADDR + 104 * 4, &ramp_slot, 4);
            uc_mem_read(m->uc, EXT_TABLE_ADDR + 105 * 4, &raml_slot, 4);
            if (ramp_slot) uc_mem_read(m->uc, ramp_slot, &ramp, 4);
            if (raml_slot) uc_mem_read(m->uc, raml_slot, &raml, 4);

            void *hp = NULL;
            const char *pack = arm_str(m, packp);
            const char *host_pack = arm_ext_pack_to_host_path(m, pack);
            ArmExtNestedModule *owner =
                arm_ext_resource_owner_for_lr(m, NULL, NULL);
            uint32_t owner_p_diag = owner ? owner->p_addr : 0;
            uint32_t owner_h_diag = owner ? owner->helper_addr : 0;
            int pack_is_root =
                pack[0] && host_pack && m->pack_host_path[0] &&
                strcmp(host_pack, m->pack_host_path) == 0;
            int child_scoped_pack =
                arm_ext_child_has_package_owner(m, owner) &&
                (!pack[0] || pack_is_root);
            if (child_scoped_pack) {
                hp = arm_ext_read_child_resource(
                    m, owner, read_name, &fl, (int)r2,
                    read_pack_host_path, &read_pack_ram_addr,
                    &read_pack_ram_len);
            }
            if (!hp && !child_scoped_pack && pack[0] == '$' && ramp && raml) {
                hp = mr_readFile_from_ram(read_name, &fl, (int)r2, arm_ptr(m, ramp), (int)raml);
                if (hp) {
                    read_pack_ram_addr = ramp;
                    read_pack_ram_len = raml;
                    const char *last_mrp_path = mr_get_last_written_mrp_path();
                    uint32 last_mrp_len = 0;
                    const uint8 *last_mrp =
                        mr_get_last_written_mrp_data(&last_mrp_len);
                    if (last_mrp && last_mrp_len == raml &&
                        last_mrp_path && last_mrp_path[0] &&
                        memcmp(arm_ptr(m, ramp), last_mrp, raml) == 0) {
                        snprintf(read_pack_host_path,
                                 sizeof(read_pack_host_path), "%s",
                                 last_mrp_path);
                    }
                }
            } else if (!hp && !child_scoped_pack && !pack[0]) {
                /*
                 * Native pack_filename[128] is writable, and private child
                 * helpers may clear it after their loader has installed the
                 * child.  Once the LR maps to a registered child module, that
                 * module's package is the ABI context for table[125]; otherwise
                 * a blank table[100] means the VM-global package.
                 */
                hp = _mr_readFile(read_name, &fl, (int)r2);
            } else if (!hp && !child_scoped_pack) {
                /*
                 * EXT modules expose their current MRP through table[100].
                 * Nested download/install helpers may switch that slot to a
                 * transient package while the VM-level pack_filename still
                 * names the outer app. Bind the host read to the ARM-visible
                 * package for this bridge call, matching the native C table ABI.
                 */
                hp = mr_readFile_from_pack(host_pack, read_name, &fl, (int)r2);
                if (hp && host_pack && host_pack[0]) {
                    snprintf(read_pack_host_path, sizeof(read_pack_host_path),
                             "%s", host_pack);
                }
            }
            if (getenv("VMRP_ARM_EXT_DIAG")) {
                char ram_preview[192];
                char out_preview[192];
                ram_preview[0] = '\0';
                out_preview[0] = '\0';
                if (ramp && raml && arm_ptr(m, ramp)) {
                    arm_ext_diag_preview_bytes(arm_ptr(m, ramp), raml,
                                               ram_preview,
                                               sizeof(ram_preview));
                }
                if (hp && fl > 0) {
                    arm_ext_diag_preview_bytes(hp, (uint32_t)fl,
                                               out_preview,
                                               sizeof(out_preview));
                }
                printf("DIAG table125 name='%s' lookfor=%u pack='%s' host_pack='%s' childScoped=%d ownerP=0x%X ownerH=0x%X ownerFile=0x%X ownerLen=%u ramp=0x%X raml=%u found=%d fl=%d readPack='%s' readRam=0x%X/%u lr=0x%X\n",
                       read_name, r2, pack, host_pack ? host_pack : "",
                       child_scoped_pack, owner_p_diag, owner_h_diag,
                       owner ? owner->file_addr : 0,
                       owner ? owner->file_len : 0, ramp, raml,
                       hp != NULL, fl, read_pack_host_path,
                       read_pack_ram_addr, read_pack_ram_len,
                       reg_read32(m->uc, UC_ARM_REG_LR));
            }
            if (!hp) {
                uint32_t watch_lo = 0, watch_hi = 0;
                if (arm_ext_watch_alloc_range(&watch_lo, &watch_hi)) {
                    printf("WATCH_READFILE_FAIL name='%s' reason=no_host_data fl=%d lookfor=%u pack='%s' host_pack='%s' childScoped=%d ownerP=0x%X ownerH=0x%X lenSlot=0x%X sp=0x%X r9=0x%X lr=0x%X\n",
                           read_name, fl, r2, pack, host_pack ? host_pack : "",
                           child_scoped_pack, owner_p_diag, owner_h_diag, r1,
                           reg_read32(m->uc, UC_ARM_REG_SP),
                           reg_read32(m->uc, UC_ARM_REG_R9),
                           reg_read32(m->uc, UC_ARM_REG_LR));
                    fflush(stdout);
                }
                ret = 0;
                break;
            }
            /* 原生 asm_mr_readFile 的输出缓冲来自 mr_malloc(LG_mem
             * first-fit),应用会预先 free 一块自有内存构造空闲链表,让
             * 该分配按 first-fit 落在预测地址,并丢弃返回值直接读预测
             * 地址(talkcat 图标花屏根因)。先走 guest 空闲链表;池内
             * 无合适块时与 table[0] 一样退回 bump(修复前行为)。
             * 若该缓冲随后经 mr_cacheSync 注册为 EXT 映像,注册路径会把
             * 文件范围标记为不可再分配的代码区。 */
            uint32_t ap = 0;
            int direct_ram_package_ptr = 0;
            uint32_t mapped_hp = fl > 0 ? arm_addr(m, hp) : 0;
            if (mapped_hp && read_pack_ram_addr && read_pack_ram_len &&
                arm_ext_range_contains(read_pack_ram_addr, read_pack_ram_len,
                                       mapped_hp, (uint32_t)fl)) {
                /*
                 * Native _mr_readFile returns uncompressed '*'/'$' package
                 * entries as pointers inside the package buffer.  Do the same
                 * when the host result already maps to ARM-visible package
                 * RAM; copying it to a new mr_malloc block gives the caller a
                 * shorter lifetime than native and lets later readFile calls
                 * recycle storage still referenced by cached bitmap records.
                 */
                ap = mapped_hp;
                direct_ram_package_ptr = 1;
            } else {
                ap = arm_ext_app_mem_malloc(m, (uint32_t)fl);
                if (!ap) {
                    uint32_t watch_lo = 0, watch_hi = 0;
                    if (arm_ext_watch_alloc_range(&watch_lo, &watch_hi)) {
                        printf("WATCH_READFILE_FAIL name='%s' reason=no_arm_buffer fl=%d lookfor=%u pack='%s' host_pack='%s' childScoped=%d ownerP=0x%X ownerH=0x%X lenSlot=0x%X sp=0x%X r9=0x%X lr=0x%X\n",
                               read_name, fl, r2, pack,
                               host_pack ? host_pack : "", child_scoped_pack,
                               owner_p_diag, owner_h_diag, r1,
                               reg_read32(m->uc, UC_ARM_REG_SP),
                               reg_read32(m->uc, UC_ARM_REG_R9),
                               reg_read32(m->uc, UC_ARM_REG_LR));
                        fflush(stdout);
                    }
                    ret = 0;
                    break;
                }
                /*
                 * Compressed package entries and filesystem reads return
                 * native mr_malloc-owned buffers (mr_gzOutBuf/filebuf).  The
                 * bridge allocation is therefore app-visible LG_mem and must
                 * update the same counters as table[0].
                 */
                note_origin_mem_alloc(m, (uint32_t)fl);
                memcpy(arm_ptr(m, ap), hp, fl);
            }
            uint32_t len_slot_value = 0;
            if (r1 && arm_ptr(m, r1)) {
                memcpy(arm_ptr(m, r1), &fl, 4);
                memcpy(&len_slot_value, arm_ptr(m, r1), 4);
            }
            {
                uint32_t watch_lo = 0, watch_hi = 0;
                if (arm_ext_watch_alloc_range(&watch_lo, &watch_hi) &&
                    ((ap < watch_hi && ap + (uint32_t)fl > watch_lo) ||
                     direct_ram_package_ptr)) {
                    uint32_t slot4 = r1 && arm_ptr(m, r1 + 4u) ?
                        arm_ext_read_u32_or_zero_(m, r1 + 4u) : 0;
                    uint32_t slot8 = r1 && arm_ptr(m, r1 + 8u) ?
                        arm_ext_read_u32_or_zero_(m, r1 + 8u) : 0;
                    char head[64];
                    arm_ext_watch_hex16(hp, (uint32_t)fl, head, sizeof(head));
                    printf("WATCH_READFILE name='%s' ret=0x%X fl=%d direct=%d hash=0x%08X head=%s lookfor=%u pack='%s' host_pack='%s' childScoped=%d ownerP=0x%X ownerH=0x%X lenSlot=0x%X lenVal=%u slot4=0x%X slot8=0x%X sp=0x%X r9=0x%X lr=0x%X\n",
                           read_name, ap, fl,
                           direct_ram_package_ptr,
                           arm_ext_watch_hash32(hp, (uint32_t)fl), head, r2,
                           pack, host_pack ? host_pack : "",
                           child_scoped_pack, owner_p_diag, owner_h_diag, r1,
                           len_slot_value, slot4, slot8,
                           reg_read32(m->uc, UC_ARM_REG_SP),
                           reg_read32(m->uc, UC_ARM_REG_R9),
                           reg_read32(m->uc, UC_ARM_REG_LR));
                    fflush(stdout);
                }
            }
            arm_ext_sync_read_file_gzip_slots(m, hp, ap);
            free(m->last_file_copy);
            m->last_file_copy = malloc((size_t)fl);
            if (m->last_file_copy) {
                memcpy(m->last_file_copy, hp, (size_t)fl);
                m->last_file_addr = ap;
                m->last_file_len = (uint32_t)fl;
                snprintf(m->last_file_pack_host_path,
                         sizeof(m->last_file_pack_host_path), "%s",
                         read_pack_host_path);
                m->last_file_pack_ram_addr = read_pack_ram_addr;
                m->last_file_pack_ram_len = read_pack_ram_len;
            }
            uint32_t mirrored_slot = 0;
            int mirrored_to_adjacent_slot =
                m->last_file_copy &&
                arm_ext_mirror_read_file_to_adjacent_slot(
                    m, r1, m->last_file_copy, (uint32_t)fl, ap,
                    &mirrored_slot);
            if (getenv("VMRP_ARM_EXT_DIAG")) {
                char ret_preview[192];
                ret_preview[0] = '\0';
                arm_ext_diag_preview_bytes(arm_ptr(m, ap), (uint32_t)fl,
                                           ret_preview,
                                           sizeof(ret_preview));
                printf("DIAG table125_ret name='%s' ret=0x%X fl=%d lenSlot=0x%X/%u mirror=%d mirrorSlot=0x%X lr=0x%X preview='%s'\n",
                       read_name, ap, fl, r1, len_slot_value,
                       mirrored_to_adjacent_slot,
                       mirrored_slot, reg_read32(m->uc, UC_ARM_REG_LR),
                       ret_preview);
            }
            if (!direct_ram_package_ptr)
                arm_ext_release_host_read_file_buffer(hp, (uint32_t)fl);
            ret = ap;
        } break;
        case 126: ret = wstrlen(arm_str(m, r0)); break;
        case 127: ret = mr_registerAPP(arm_ptr(m, r0), (int32)r1, (int32)r2); break;
        case 130: ret = _mr_TestCom(NULL, (int)r1, (int)r2); break;
        case 131:
            ret = _mr_TestCom1(NULL, (int)r1, arm_ptr(m, r2), (int32)r3);
            /*
             * cfunction.ext has a private child loader that never calls host
             * table[25].  Its disassembly at 0xE8339C..0xE833C8 proves the
             * loader writes file_base[0]=module record, file_base[4]=child P,
             * extChunk[12]=file_base and extChunk[16]=len before mr_cacheSync.
             * Once that extChunk exists, the staging image is already the
             * wrapper-owned runtime form and must not be overwritten with the
             * raw bytes cached by _mr_readFile.
             */
            int internal_loader_staging =
                (r1 == 9 && arm_ext_has_internal_loader_chunk(m, r2, r3));
            if (getenv("VMRP_ARM_EXT_DIAG")) {
                uint32_t record_addr_diag = 0;
                uint32_t p_addr_diag = 0;
                if (r2 && arm_ptr(m, r2))
                    record_addr_diag = arm_ext_read_u32_or_zero_(m, r2);
                if (r2 && arm_ptr(m, r2 + 4u))
                    p_addr_diag = arm_ext_read_u32_or_zero_(m, r2 + 4u);
                printf("DIAG table131 cmd=%u data=0x%X len=%u ret=0x%X internalLoader=%d record=0x%X P=0x%X lastFile=0x%X/%u lastPack='%s' lr=0x%X activeP=0x%X primaryP=0x%X\n",
                       r1, r2, r3, ret, internal_loader_staging,
                       record_addr_diag, p_addr_diag, m->last_file_addr,
                       m->last_file_len, m->last_file_pack_host_path,
                       reg_read32(m->uc, UC_ARM_REG_LR), m->active_p_addr,
                       m->primary_p_addr);
            }
            if (r1 == 9 && internal_loader_staging &&
                m->last_file_copy && r2 && r3 <= m->last_file_len &&
                r3 > 8 && arm_ptr(m, r2 + 8)) {
                memcpy(arm_ptr(m, r2 + 8), m->last_file_copy + 8, r3 - 8);
                uint32_t record_addr = 0;
                uint32_t p_addr = arm_ext_read_u32_or_zero_(m, r2 + 4u);
                memcpy(&record_addr, arm_ptr(m, r2), 4);
                /*
                 * The private loader writes the runtime header before
                 * mr_cacheSync, but several wrappers leave the body in a
                 * transient scratch form.  Copy only the immutable payload tail
                 * from the host-decoded readFile result; keep file_base[0]/[4]
                 * as the wrapper's record/P metadata.
                 */
                arm_ext_repair_private_child_record_bridges(m, record_addr,
                                                            r2, r3);
                /*
                 * file_base[0] 是私有 loader 写入的 module record 指针——它是
                 * EXT_TABLE 的逐槽镜像，被子模块当作自己的 C 函数表基址使用。
                 * 该 record 把 table[125] (_mr_readFile) 重定向到 wrapper 自身的
                 * readFile (0xE819xx)。wrapper 的 readFile 工作在 wrapper 的内存
                 * 模型里，返回的是挂载/索引缓冲(0x66xxxx 区)而非宿主 readFile 解码
                 * 出的位图像素；子模块据此取位图源指针时拿到的是文件索引/文件名字节，
                 * 绘制即花屏。把 record 的 readFile 槽还原成宿主 EXT_TABLE 的桥接值，
                 * 让子模块的资源读取走宿主 readFile，得到正确解码的位图。
                 * 仅还原 readFile 槽：malloc/free/loader(table[25]) 仍走 wrapper，
                 * 以保留 wrapper 对子模块内存与子加载的管理。
                 */
                if (record_addr && arm_ptr(m, record_addr + 125 * 4)) {
                    memcpy(arm_ptr(m, record_addr + 125 * 4),
                           arm_ptr(m, EXT_TABLE_ADDR + 125 * 4), 4);
                }
                arm_ext_apply_short_pack_alias_for_private_child(m, r2, r3,
                                                                 p_addr);
                /*
                 * From this point on the executable owner is the runtime image
                 * at r2, not the raw buffer returned by _mr_readFile.  Keep
                 * last_file_copy as immutable package provenance, but make
                 * later generic nested-load ownership and P-header writes use
                 * the same file_base that the private loader will BLX into.
                 */
                m->last_file_addr = r2;
                m->last_file_len = r3;
            }
            if (r1 == 9 && !internal_loader_staging &&
                m->last_file_copy && r2 && r3 <= m->last_file_len &&
                arm_ptr(m, r2)) {
                memcpy(arm_ptr(m, r2), m->last_file_copy, r3);
                uint32_t table_addr = EXT_TABLE_ADDR;
                memcpy(arm_ptr(m, r2), &table_addr, 4);
                m->last_file_addr = r2;
                m->last_file_len = r3;
                if (m->last_alloc_len == sizeof(mr_c_function_P_t) && arm_ptr(m, m->last_alloc_addr)) {
                    m->nested_p_addr = m->last_alloc_addr;
                }
                if (getenv("VMRP_ARM_EXT_TRACE")) {
                    printf("arm_ext_executor: staged nested ext at 0x%X len=%u P=0x%X\n",
                           r2, r3, m->nested_p_addr);
                }
            }
            /*
             * 关键修复：case 131 (r1=9) 是 mr_cacheSync。原意是同步指令缓存，
             * 因为 wrapper 刚把新的 .ext 二进制写到了 [r2, r2+r3) 范围。在真机
             * 上这会清掉 D-cache 并 invalidate I-cache，让后续取指看到新指令。
             *
             * 我们用 Unicorn 跑 ARM，Unicorn 通过 QEMU TCG 做 JIT 翻译并 cache
             * translation blocks（TB）。当 gghjt 把 netpay 的 smcheck/advsms
             * 等子插件 staging 到同一段内存时，TB cache 仍然是上一个插件留下的
             * 翻译结果，结果就是 PC 进了新 .ext 的代码区，执行的却是旧代码。
             *
             * 表现为 r3 不被 LDR 重新加载、movs r1/r2 立即数被跳过等"鬼指令"，
             * 最后 BLX r3 跳到一个看着像数据/字符串的地址，崩在 0x627A60 一类
             * 位置。修复方式就是 mimic 真机的 mr_cacheSync：在 staging 完成后
             * 通过 uc_ctl_remove_cache 让 Unicorn 把这段地址的 TB 翻译丢掉，
             * 下次执行强制重新翻译现在的字节。
             */
            if (r1 == 9 && r2 && r3 > 0 && arm_ptr(m, r2) && arm_ptr(m, r2 + r3 - 1)) {
                /*
                 * The internal loader overwrites the MRPGCMAP header before
                 * mr_cacheSync: file_base[0] becomes the module record and
                 * file_base[4] becomes the child P pointer.  The authoritative
                 * check is therefore the extChunk magic/file/length tuple.
                 */
                arm_ext_drop_overlapping_stale_nested_modules(m, r2, r3);
                m->pending_internal_file_addr = r2;
                m->pending_internal_file_len = r3;
                arm_ext_sync_internal_nested_module(m, r2, r3);
                uc_err cerr = uc_ctl_remove_cache(m->uc, r2, r2 + r3);
                if (cerr != UC_ERR_OK && getenv("VMRP_ARM_EXT_TRACE")) {
                    printf("arm_ext_executor: uc_ctl_remove_cache(0x%X, 0x%X) failed: %u\n",
                           r2, r2 + r3, cerr);
                }
            }
            break;
        case 132: {
            int err_pos = -1;
            int out_size = 0;
            uint16 *host = c2u(arm_str(m, r0), r1 ? &err_pos : NULL, &out_size);
            if (r1 && arm_ptr(m, r1)) memcpy(arm_ptr(m, r1), &err_pos, 4);
            if (r2 && arm_ptr(m, r2)) memcpy(arm_ptr(m, r2), &out_size, 4);
            if (host && out_size > 0) {
                uint32_t out_addr = arm_alloc(m, (uint32_t)out_size);
                if (out_addr) {
                    memcpy(arm_ptr(m, out_addr), host, (uint32_t)out_size);
                }
                mr_free(host, (uint32)out_size);
                ret = out_addr;
            } else {
                ret = 0;
            }
        } break;
        case 133: ret = ((int32)r1 == 0) ? 0 : (int32)r0 / (int32)r1; break;
        case 134: ret = ((int32)r1 == 0) ? 0 : (int32)r0 % (int32)r1; break;
        case 145:
            {
                uint32_t claim_p = 0, claim_helper = 0;
                if (!arm_ext_should_accept_screen_write(m, &claim_p,
                                                        &claim_helper)) {
                    claim_p = 0;
                    claim_helper = 0;
                }
                ArmExtScreenContext screen_ctx;
                if (arm_ext_push_draw_screen_context(m, &screen_ctx)) {
                    uint16_t *before = arm_ext_snapshot_screen(m);
                    mr_platDrawChar((uint16)r0, (int32)r1, (int32)r2,
                                    (uint32)r3);
                    arm_ext_pop_draw_screen_context(&screen_ctx);
                    arm_ext_note_screen_damage_diff(m, before);
                    arm_ext_claim_foreground_screen_diff(m, claim_p,
                                                         claim_helper,
                                                         before);
                    free(before);
                    arm_ext_finish_screen_cache_write(m, &screen_ctx,
                                                      claim_p,
                                                      claim_helper);
                }
            }
            ret = 0;
            break;
        default:
            printf("arm_ext_executor: table[%u] not implemented (r0=0x%X r1=0x%X r2=0x%X r3=0x%X)\n", idx, r0, r1, r2, r3);
            ret = MR_IGNORE;
            break;
    }
    cb_ret(m, ret);
}


static bool hook_invalid(uc_engine *uc, uc_mem_type type, uint64_t address, int size, int64_t value, void *user_data) {
    ArmExtModule *m = (ArmExtModule *)user_data;
    if (type == UC_MEM_FETCH_UNMAPPED && address == 0) {
        uint32_t lr = 0;
        uc_reg_read(uc, UC_ARM_REG_LR, &lr);
        if (getenv("VMRP_ARM_EXT_TRACE")) {
            printf("arm_ext_executor: treated fetch from 0 as return to 0x%X\n", lr);
            dumpREG(uc);
        }
        if (lr) {
            uint32_t cpsr = 0;
            uc_reg_read(uc, UC_ARM_REG_CPSR, &cpsr);
            if (lr & 1u) cpsr |= (1u << 5);
            else cpsr &= ~(1u << 5);
            uc_reg_write(uc, UC_ARM_REG_CPSR, &cpsr);
            uc_reg_write(uc, UC_ARM_REG_PC, &lr);
        } else {
            uint32_t pc = EXT_STOP_ADDR;
            uc_reg_write(uc, UC_ARM_REG_PC, &pc);
            uc_emu_stop(uc);
        }
        return true;
    }
    {
        uint32_t pc = reg_read32(uc, UC_ARM_REG_PC);
        uint32_t sp = reg_read32(uc, UC_ARM_REG_SP);
        uint32_t cpsr = reg_read32(uc, UC_ARM_REG_CPSR);
        int thumb = (cpsr >> 5) & 1;
        printf("arm_ext_executor: invalid memory %s addr=0x%llX size=%d value=0x%llX\n",
               memTypeStr(type), (unsigned long long)address, size, (unsigned long long)value);
        printf("  crash PC=0x%X (%s) last_file=0x%X..0x%X\n",
               pc, thumb ? "thumb" : "arm",
               m->last_file_addr, m->last_file_addr + m->last_file_len);
        if (getenv("VMRP_ARM_EXT_DIAG")) {
            uint32_t r9 = reg_read32(uc, UC_ARM_REG_R9);
            arm_ext_diag_dump_layer_state(m, "invalid");
            arm_ext_diag_dump_rw_timer_state(m, "invalid-r9", r9);
        }
        /* 打印 crash PC 前32字节和后16字节，便于反汇编定位上下文 */
        {
            uint32_t pre_start = (pc > 32) ? (pc - 32) : 0;
            uint32_t pre_len = pc - pre_start;
            uint8_t pre_bytes[32];
            if (pre_len > 0 && uc_mem_read(uc, pre_start, pre_bytes, pre_len) == UC_ERR_OK) {
                printf("  bytes @0x%X (before PC):", pre_start);
                for (uint32_t i = 0; i < pre_len; i++) printf(" %02X", pre_bytes[i]);
                printf("\n");
            }
        }
        uint8_t bytes[16];
        if (uc_mem_read(uc, pc, bytes, 16) == UC_ERR_OK) {
            printf("  bytes @0x%X (at PC):", pc);
            for (int i = 0; i < 16; i++) printf(" %02X", bytes[i]);
            printf("\n");
        }
        /* 导出 crash PC 前后各256字节的二进制，供 arm-none-eabi-objdump 反汇编 */
        {
            uint32_t dump_start = (pc > 256) ? (pc - 256) : 0;
            uint32_t dump_end = pc + 256;
            uint32_t dump_len = dump_end - dump_start;
            uint8_t *dump_buf = malloc(dump_len);
            if (dump_buf && uc_mem_read(uc, dump_start, dump_buf, dump_len) == UC_ERR_OK) {
                FILE *df = fopen("/tmp/vmrp_crash.bin", "wb");
                if (df) {
                    fwrite(dump_buf, 1, dump_len, df);
                    fclose(df);
                    printf("  [CRASH_DUMP] saved %u bytes [0x%X..0x%X] to /tmp/vmrp_crash.bin\n",
                           dump_len, dump_start, dump_end);
                    printf("  [CRASH_DUMP] disassemble: arm-none-eabi-objdump -D -b binary "
                           "-m arm -M force-thumb --adjust-vma=0x%X /tmp/vmrp_crash.bin\n",
                           dump_start);
                }
            }
            free(dump_buf);
        }
        /* 导出栈内容（SP 向上64字节） */
        {
            uint8_t stack_buf[64];
            if (uc_mem_read(uc, sp, stack_buf, sizeof(stack_buf)) == UC_ERR_OK) {
                printf("  stack @0x%X:", sp);
                for (int i = 0; i < 64; i += 4) {
                    uint32_t val = stack_buf[i] | (stack_buf[i+1]<<8)
                                 | (stack_buf[i+2]<<16) | (stack_buf[i+3]<<24);
                    printf(" 0x%08X", val);
                }
                printf("\n");
            }
        }
    }
    dumpREG(uc);
    return false;
}

/* GOT 写监控 */
/* ---- 临时诊断(talkcat 循环下载-取消 代码区覆写取证,修复后删除) ----
 * VMRP_WATCH_WRITE="lo,hi":监视 guest 对 [lo,hi) 的写入,打印 PC/LR。
 * VMRP_WATCH_SENTINEL="addr":在每次 table 调用入口校验 addr 处 16 字节,
 *   变化时打印上一次 table 调用(可捕获宿主 memcpy 侧的覆写并定位到桥调用)。 */
static int arm_ext_watch_write_range(uint32_t *lo, uint32_t *hi) {
    static int parsed = -1;
    static uint32_t s_lo = 0, s_hi = 0;
    if (parsed < 0) {
        const char *env = getenv("VMRP_WATCH_WRITE");
        parsed = 0;
        if (env && *env) {
            char *end = NULL;
            s_lo = (uint32_t)strtoul(env, &end, 0);
            if (end && *end == ',')
                s_hi = (uint32_t)strtoul(end + 1, NULL, 0);
            if (s_hi > s_lo) parsed = 1;
        }
    }
    if (parsed > 0) {
        *lo = s_lo;
        *hi = s_hi;
    }
    return parsed > 0;
}

static int arm_ext_watch_alloc_range(uint32_t *lo, uint32_t *hi) {
    static int parsed = -1;
    static uint32_t s_lo = 0, s_hi = 0;
    if (parsed < 0) {
        const char *env = getenv("VMRP_WATCH_ALLOC");
        parsed = 0;
        if (env && *env) {
            char *end = NULL;
            s_lo = (uint32_t)strtoul(env, &end, 0);
            if (end && *end == ',')
                s_hi = (uint32_t)strtoul(end + 1, NULL, 0);
            if (s_hi > s_lo) parsed = 1;
        }
    }
    if (parsed > 0) {
        if (lo) *lo = s_lo;
        if (hi) *hi = s_hi;
        return 1;
    }
    /*
     * Keep older watch runs useful: without VMRP_WATCH_ALLOC, allocation
     * reports follow the write watch range that triggered this diagnostic.
     */
    return arm_ext_watch_write_range(lo, hi);
}

/* 每个 uc 实例(arm_ext_load 一次)独立的诊断状态。此前版本用共享 static,
 * 两个实例互相污染(哨兵跨实例比较、写日志配额被另一实例耗尽),取证无效。 */
typedef struct {
    ArmExtModule *m;
    int id;
    uint32_t write_hits;
    uint8_t sent[16];
    int sent_primed;
    uint32_t sent_changes;
    uint32_t last_idx, last_r0, last_r1, last_r2, last_r3, last_lr, last_pc;
} ArmExtWatchState;

static ArmExtWatchState *arm_ext_watch_state(ArmExtModule *m) {
    static ArmExtWatchState slots[8];
    static int used = 0;
    for (int i = 0; i < used; ++i)
        if (slots[i].m == m) return &slots[i];
    if (used >= 8) return NULL;
    slots[used].m = m;
    slots[used].id = used;
    return &slots[used++];
}

static void arm_ext_watch_alloc_report(ArmExtModule *m, const char *tag,
                                       uint32_t addr, uint32_t len) {
    uint32_t lo, hi;
    if (!addr || !arm_ext_watch_alloc_range(&lo, &hi)) return;
    if (addr >= hi || addr + len <= lo) return;
    ArmExtWatchState *w = arm_ext_watch_state(m);
    printf("WATCH_ALLOC[m%d] %s addr=0x%X len=%u overlaps [0x%X,0x%X)\n",
           w ? w->id : -1, tag, addr, len, lo, hi);
    fflush(stdout);
}

static uint32_t arm_ext_watch_hash32(const void *data, uint32_t len) {
    const uint8_t *p = (const uint8_t *)data;
    uint32_t h = 2166136261u;
    if (!p) return 0;
    for (uint32_t i = 0; i < len; ++i) {
        h ^= p[i];
        h *= 16777619u;
    }
    return h;
}

static void arm_ext_watch_hex16(const void *data, uint32_t len,
                                char *out, size_t out_size) {
    const uint8_t *p = (const uint8_t *)data;
    size_t pos = 0;
    if (!out || !out_size) return;
    out[0] = '\0';
    if (!p) return;
    uint32_t n = len < 16u ? len : 16u;
    for (uint32_t i = 0; i < n && pos + 4 < out_size; ++i) {
        int wrote = snprintf(out + pos, out_size - pos, "%s%02X",
                             i ? " " : "", p[i]);
        if (wrote < 0) break;
        pos += (size_t)wrote;
    }
}

/* 模块注册/清除生命周期(仅 VMRP_WATCH_WRITE 设置时打印) */
static void arm_ext_watch_module_event(ArmExtModule *m, const char *tag,
                                       uint32_t file_addr, uint32_t file_len,
                                       uint32_t p_addr) {
    uint32_t lo, hi;
    if (!arm_ext_watch_write_range(&lo, &hi)) return;
    ArmExtWatchState *w = arm_ext_watch_state(m);
    printf("WATCH_MOD[m%d] %s file=0x%X+0x%X p=0x%X\n",
           w ? w->id : -1, tag, file_addr, file_len, p_addr);
    fflush(stdout);
}

/* 只记录落在"已注册嵌套模块 file 范围内"的 guest 写——模块注册后其代码区
 * 不应再被写,任何命中都是覆写嫌疑。注册前的池数据写(合法)被过滤掉。 */
static void hook_watch_write(uc_engine *uc, uc_mem_type type, uint64_t address,
                             int size, int64_t value, void *user_data) {
    (void)type;
    ArmExtModule *m = (ArmExtModule *)user_data;
    ArmExtWatchState *w = arm_ext_watch_state(m);
    if (!w || w->write_hits >= 2000) return;
    uint32_t watch_lo = 0;
    uint32_t watch_hi = 0;
    int small_data_watch =
        arm_ext_watch_write_range(&watch_lo, &watch_hi) &&
        watch_hi > watch_lo && watch_hi - watch_lo <= 0x100u;
    ArmExtNestedModule *mod = arm_ext_find_nested_module(m, (uint32_t)address);
    if (!mod && !small_data_watch) return;
    w->write_hits++;
    uint32_t pc = reg_read32(uc, UC_ARM_REG_PC);
    uint32_t lr = reg_read32(uc, UC_ARM_REG_LR);
    uint32_t sp = reg_read32(uc, UC_ARM_REG_SP);
    uint32_t r9 = reg_read32(uc, UC_ARM_REG_R9);
    uint32_t r0 = reg_read32(uc, UC_ARM_REG_R0);
    uint32_t r1 = reg_read32(uc, UC_ARM_REG_R1);
    uint32_t r2 = reg_read32(uc, UC_ARM_REG_R2);
    uint32_t r3 = reg_read32(uc, UC_ARM_REG_R3);
    uint32_t r4 = reg_read32(uc, UC_ARM_REG_R4);
    uint32_t r5 = reg_read32(uc, UC_ARM_REG_R5);
    uint32_t r6 = reg_read32(uc, UC_ARM_REG_R6);
    uint32_t r7 = reg_read32(uc, UC_ARM_REG_R7);
    printf("WATCH_WRITE[m%d] #%u addr=0x%llX size=%d value=0x%llX pc=0x%X lr=0x%X sp=0x%X r9=0x%X r0=0x%X r1=0x%X r2=0x%X r3=0x%X r4=0x%X r5=0x%X r6=0x%X r7=0x%X mod=0x%X+0x%X\n",
           w->id, w->write_hits, (unsigned long long)address, size,
           (unsigned long long)value, pc, lr, sp, r9, r0, r1, r2, r3, r4, r5,
           r6, r7, mod ? mod->file_addr : 0, mod ? mod->file_len : 0);
    if (!mod && small_data_watch) {
        uint32_t base = watch_lo;
        printf("WATCH_WRITE[m%d] data_watch [0x%X,0x%X) old_words={0x%X,0x%X,0x%X,0x%X}\n",
               w->id, watch_lo, watch_hi,
               arm_ext_read_u32_or_zero_(m, base),
               arm_ext_read_u32_or_zero_(m, base + 4u),
               arm_ext_read_u32_or_zero_(m, base + 8u),
               arm_ext_read_u32_or_zero_(m, base + 12u));
    }
    if (mod && w->write_hits <= 5) {
        /*
         * 0x226C08 is a guest-side bitmap blitter.  At the write instruction
         * LR has often been overwritten by an inner memcpy/blend callback, so
         * dump the saved entry registers and stack arguments from that frame:
         *   saved r0-r3 = destination x/y/w/h
         *   sp+0x68..0x74 = source x/y/w/h
         *   sp+0x78/0x7C = destination/source bitmap descriptors
         * This keeps the loop-cancel diagnosis tied to the disassembled ABI
         * instead of guessing from the current scratch registers.
         */
        uint32_t saved_r0 = arm_ext_read_u32_or_zero_(m, sp + 0x44u);
        uint32_t saved_r1 = arm_ext_read_u32_or_zero_(m, sp + 0x48u);
        uint32_t saved_r2 = arm_ext_read_u32_or_zero_(m, sp + 0x4Cu);
        uint32_t saved_r3 = arm_ext_read_u32_or_zero_(m, sp + 0x50u);
        uint32_t saved_lr = arm_ext_read_u32_or_zero_(m, sp + 0x64u);
        uint32_t a68 = arm_ext_read_u32_or_zero_(m, sp + 0x68u);
        uint32_t a6c = arm_ext_read_u32_or_zero_(m, sp + 0x6Cu);
        uint32_t a70 = arm_ext_read_u32_or_zero_(m, sp + 0x70u);
        uint32_t a74 = arm_ext_read_u32_or_zero_(m, sp + 0x74u);
        uint32_t dst_desc = arm_ext_read_u32_or_zero_(m, sp + 0x78u);
        uint32_t src_desc = arm_ext_read_u32_or_zero_(m, sp + 0x7Cu);
        uint32_t dst_0 = arm_ext_read_u32_or_zero_(m, dst_desc);
        uint32_t dst_len = arm_ext_read_u32_or_zero_(m, dst_desc + 4u);
        uint32_t dst_type = arm_ext_read_u32_or_zero_(m, dst_desc + 8u);
        uint32_t dst_base = arm_ext_read_u32_or_zero_(m, dst_desc + 12u);
        uint32_t src_0 = arm_ext_read_u32_or_zero_(m, src_desc);
        uint32_t src_len = arm_ext_read_u32_or_zero_(m, src_desc + 4u);
        uint32_t src_type = arm_ext_read_u32_or_zero_(m, src_desc + 8u);
        uint32_t src_base = arm_ext_read_u32_or_zero_(m, src_desc + 12u);
        uint32_t local24 = arm_ext_read_u32_or_zero_(m, sp + 0x24u);
        uint32_t local28 = arm_ext_read_u32_or_zero_(m, sp + 0x28u);
        uint32_t local2c = arm_ext_read_u32_or_zero_(m, sp + 0x2Cu);
        uint32_t local30 = arm_ext_read_u32_or_zero_(m, sp + 0x30u);
        uint32_t local38 = arm_ext_read_u32_or_zero_(m, sp + 0x38u);
        uint32_t local3c = arm_ext_read_u32_or_zero_(m, sp + 0x3Cu);
        uint32_t local40 = arm_ext_read_u32_or_zero_(m, sp + 0x40u);
        uint32_t dst_w16 = dst_0 & 0xFFFFu;
        uint32_t dst_h16 = dst_0 >> 16;
        uint32_t dst_pixel = dst_base && r4 >= dst_base ?
            (uint32_t)((r4 - dst_base) / 2u) : 0;
        uint32_t dst_col = dst_w16 ? dst_pixel % dst_w16 : 0;
        uint32_t dst_row = dst_w16 ? dst_pixel / dst_w16 : 0;
        printf("WATCH_WRITE[m%d] args saved={r0/x=0x%X,r1/y=0x%X,r2/w=0x%X,r3/h=0x%X,lr=0x%X} stack68={sx=0x%X,sy=0x%X,sw=0x%X,sh=0x%X,dst=0x%X,src=0x%X}\n",
               w->id, saved_r0, saved_r1, saved_r2, saved_r3, saved_lr,
               a68, a6c, a70, a74, dst_desc, src_desc);
        printf("WATCH_WRITE[m%d] desc dst=0x%X {w=%u,h=%u,len=0x%X,type=0x%X,base=0x%X,pos=%u,%u} src=0x%X {w=%u,h=%u,len=0x%X,type=0x%X,base=0x%X}\n",
               w->id, dst_desc, dst_w16, dst_h16, dst_len, dst_type,
               dst_base, dst_col, dst_row, src_desc, src_0 & 0xFFFFu,
               src_0 >> 16, src_len, src_type, src_base);
        printf("WATCH_WRITE[m%d] locals clip={x=0x%X,y=0x%X,rows=0x%X,dst_gap=0x%X,src_gap=0x%X,src_h=0x%X,src_w=0x%X,dst_w=0x%X}\n",
               w->id, local24, arm_ext_read_u32_or_zero_(m, sp + 0x20u),
               local28, local2c, local30, local38, local3c, local40);
    }
    fflush(stdout);
}

static void arm_ext_watch_sentinel_check(ArmExtModule *m, uint32_t idx,
                                         uint32_t r0, uint32_t r1,
                                         uint32_t r2, uint32_t r3) {
    static int parsed = -1;
    static uint32_t addr = 0;
    if (parsed < 0) {
        const char *env = getenv("VMRP_WATCH_SENTINEL");
        parsed = 0;
        if (env && *env) {
            addr = (uint32_t)strtoul(env, NULL, 0);
            if (addr) parsed = 1;
        }
    }
    if (parsed <= 0) return;
    ArmExtWatchState *w = arm_ext_watch_state(m);
    if (!w || w->sent_changes >= 20) return;
    void *p = arm_ptr(m, addr);
    if (!p) return;
    /* 哨兵地址属于已注册模块代码后才 prime,避免注册前的池数据写误报 */
    if (!w->sent_primed) {
        if (arm_ext_find_nested_module(m, addr)) {
            memcpy(w->sent, p, 16);
            w->sent_primed = 1;
            printf("WATCH_SENTINEL[m%d] primed addr=0x%X bytes=", w->id, addr);
            for (int i = 0; i < 16; ++i) printf("%02X ", w->sent[i]);
            printf("\n");
            fflush(stdout);
        }
    } else if (memcmp(w->sent, p, 16) != 0) {
        w->sent_changes++;
        printf("WATCH_SENTINEL[m%d] CHANGED#%u addr=0x%X now=", w->id,
               w->sent_changes, addr);
        for (int i = 0; i < 16; ++i) printf("%02X ", ((uint8_t *)p)[i]);
        printf("\n  prev table[%u](0x%X,0x%X,0x%X,0x%X) lr=0x%X pc=0x%X\n",
               w->last_idx, w->last_r0, w->last_r1, w->last_r2, w->last_r3,
               w->last_lr, w->last_pc);
        printf("  cur  table[%u](0x%X,0x%X,0x%X,0x%X) lr=0x%X\n",
               idx, r0, r1, r2, r3, reg_read32(m->uc, UC_ARM_REG_LR));
        fflush(stdout);
        memcpy(w->sent, p, 16); /* 重新 prime,继续捕获后续变化 */
    }
    w->last_idx = idx;
    w->last_r0 = r0;
    w->last_r1 = r1;
    w->last_r2 = r2;
    w->last_r3 = r3;
    w->last_lr = reg_read32(m->uc, UC_ARM_REG_LR);
    w->last_pc = reg_read32(m->uc, UC_ARM_REG_PC);
}
/* ---- 临时诊断结束 ---- */

static void hook_got_write(uc_engine *uc, uc_mem_type type, uint64_t address, int size, int64_t value, void *user_data) {
    (void)type;
    ArmExtModule *m = (ArmExtModule *)user_data;
    if (size != 4) return;
    uint32_t r9 = reg_read32(uc, UC_ARM_REG_R9);
    uint32_t got_size = EXT_TABLE_COUNT * 4;
    if (!r9 || (uint32_t)address < r9 || (uint32_t)address >= r9 + got_size) return;
    uint32_t idx = ((uint32_t)address - r9) / 4;
    if (idx >= EXT_TABLE_COUNT) return;
    if ((uint32_t)value >= EXT_TABLE_ADDR &&
        (uint32_t)value < EXT_TABLE_ADDR + EXT_TABLE_COUNT * 4) {
        /*
         * 观测真实重定位偏移：ARM 代码把 memcpy 桥(table[3]=EXT_TABLE_ADDR+0xC)
         * 写入某个 rw_base 的 GOT 时，记录其 R9 相对偏移到对应嵌套模块记录上。
         * 这是私有 loader 子模块 GOT 桥块真实基址的可靠信号——bridge 修复据此把
         * 描述符平移到模块代码实际读取的偏移，而不是写死的 app 专用偏移。 */
        if ((uint32_t)value == EXT_TABLE_ADDR + 0xCu) {
            for (int gi = 0; gi < m->nested_module_count; ++gi) {
                uint32_t nrw = 0;
                if (arm_ptr(m, m->nested_modules[gi].p_addr))
                    memcpy(&nrw, arm_ptr(m, m->nested_modules[gi].p_addr), 4);
                if (nrw && nrw == r9) {
                    m->nested_modules[gi].got_memcpy_off = (uint32_t)address - r9;
                    break;
                }
            }
        }
        if (app_should_protect_got_addr(m, (uint32_t)address)) {
            return;
        }
        /* bridge 函数指针写入 → 记录快照。只在 got_snapshot_base 尚未
         * 设置或与当前 R9 一致时更新 base，防止嵌套插件（如 netpay）
         * 的 GOT 初始化覆盖 wrapper 的 snapshot base */
        if (!m->got_snapshot_base || m->got_snapshot_base == r9) {
            m->got_snapshot_base = r9;
        }
        /* 只记录属于 snapshot base 所属 GOT 的写入 */
        if (m->got_snapshot_base == r9) {
            m->got_snapshot[idx] = (uint32_t)value;
        }
    }
    /* 非 bridge 值覆盖时保留已有的快照不清除——netpay 等嵌套插件会将
     * GOT 中的 bridge 指针临时替换为自己的回调地址，但 dump0 restore
     * 读回整块内存后需要把这些槽位恢复为原始 bridge 指针 */
}

static void hook_screen_write(uc_engine *uc, uc_mem_type type, uint64_t address, int size, int64_t value, void *user_data) {
    (void)uc;
    (void)type;
    (void)address;
    (void)size;
    (void)value;
    ArmExtModule *m = (ArmExtModule *)user_data;
    m->screen_write_count++;
    /*
     * The ARM screen buffer is shared storage, not the visible layer owner.
     * When a foreground child is active, covered primary/wrapper code may
     * still repaint that memory; those writes must not synthesize a host
     * present over the child that already owns the visible screen.
     */
    if (!arm_ext_has_foreground_child(m) &&
        arm_ext_current_screen_owner_is_visible(m)) {
        m->screen_dirty = 1;
        arm_ext_note_screen_damage_addr_range(m, address, size);
    }
    if (getenv("VMRP_ARM_EXT_TRACE") && m->screen_write_count <= 20) {
        printf("arm_ext_executor: screen write #%u addr=0x%llX size=%d value=0x%llX\n",
               m->screen_write_count, (unsigned long long)address, size, (unsigned long long)value);
    }
}

static void hook_restore_r9(uc_engine *uc, uint64_t address, uint32_t size, void *user_data) {
    (void)size;
    ArmExtModule *m = (ArmExtModule *)user_data;
    if (m->outer_r9 && m->nested_return_addr && (uint32_t)address == (m->nested_return_addr & ~1u)) {
        uc_reg_write(uc, UC_ARM_REG_R9, &m->outer_r9);
        if (getenv("VMRP_ARM_EXT_TRACE")) {
            printf("arm_ext_executor: restored outer R9=0x%X at 0x%X\n",
                   m->outer_r9, (uint32_t)address);
        }
        m->outer_r9 = 0;
        m->nested_return_addr = 0;
        return;
    }
    /*
     * wrapper dispatch can BLX into callbacks retained by older nested EXTs
     * (gxdzc netpay: 0xE83B40 -> smsend.ext 0x66FFBD). Those callbacks use
     * R9-relative globals, so R9 must follow the code range currently being
     * executed rather than the last active helper. The hook runs per basic
     * block and only writes R9 when PC enters a known EXT image. Verification:
     * gxdzc e2e reaches the payment UI.
     */
    arm_ext_sync_r9_for_code_addr(m, (uint32_t)address);
}

static uint32_t find_wrapper_timer_dispatch(const uint8_t *code, uint32_t len,
                                             uint32_t *chain_thunk_out,
                                             uint32_t *compact_sched_off_out) {
    if (chain_thunk_out) *chain_thunk_out = 0;
    if (compact_sched_off_out) *compact_sched_off_out = 0;
    static const uint8_t pat[] = {
        0x00, 0x48, 0xF8, 0xB5, 0x78, 0x44, 0x80, 0x6B,
        0x80, 0x30, 0x40, 0x68, 0x80, 0x47, 0x00, 0x25,
        0x00, 0x4E, 0x4E, 0x44, 0x71, 0x69, 0x75, 0x61,
        0x41, 0x1A, 0xF0, 0x68, 0x0B, 0x1C, 0x00, 0x28,
    };

    if (!code || len < sizeof(pat)) return 0;
    for (uint32_t off = 0; off + sizeof(pat) <= len; off += 2) {
        int match = 1;
        for (uint32_t i = 0; i < sizeof(pat); ++i) {
            /* gxdzc cfunction.ext 反汇编确认 0xE83A80 是 wrapper timer
             * queue dispatcher：它从 wrapper RW+0x1FC 取队列并在 0xE83B46
             * BLX 节点回调。Thumb LDR literal 的立即数字段随代码布局变，
             * 其余指令序列稳定。兼容性：只在扫描到该队列消费函数时启用
             * wrapper timer dispatch；验证 gxdzc e2e 付费路径。 */
            if (i == 0 || i == 16) continue;
            if (code[off + i] != pat[i]) {
                match = 0;
                break;
            }
        }
        if (match) {
            uint32_t sched_off = 0;
            /*
             * Compact wrapper walkers load R6 from an R9-relative literal
             * immediately after getTime, then use [R6+0x0c]/[R6+0x10] as the
             * timer queues.  Recording the literal keeps later liveness tests
             * tied to real nodes rather than stale wrapper control flags.
             */
            if (arm_ext_thumb_ldr_literal_u32(code, len, off + 16u,
                                              &sched_off) &&
                sched_off >= 0x80u && sched_off < 0x1000u &&
                (sched_off & 3u) == 0) {
                if (compact_sched_off_out)
                    *compact_sched_off_out = sched_off;
            }
            return EXT_CODE_ADDR + off + 1u;
        }
    }

    /* gghjt/gwkdl 版 cfunction.ext（解压后 19428 字节）的同形 thunk 不是
     * 宿主 timer dispatcher。反汇编确认：
     *   0xE83B50: push {r3,lr}; bl 0xE8261C; movs r0,#0; pop {r3,pc}
     *   0xE8261C: 按 wrapper_rw+0x190 的 suspend/resume bucket 遍历节点
     *   0xE83590: 单个 resume 回调，递减 extChunk[0x34]
     *
     * 把这个 walker 当成宿主 timer 调用会在非 suspend 状态下反复递减
     * primary extChunk[0x34]，破坏 wrapper 状态机。该版本的真实 timer
     * consumer 位于 helper code=2 分支（0xE83E6C），因此这里明确不启用
     * arm_ext_call_dispatch()，让 mr_timer() 走普通 native_ext_void_event(2)
     * 路由。 */
    static const uint8_t chain_thunk_pat[] = {
        0x08, 0xB5, 0xFE, 0xF7, 0x63, 0xFD, 0x00, 0x20, 0x08, 0xBD,
    };
    for (uint32_t off = 0; off + sizeof(chain_thunk_pat) <= len; off += 2) {
        if (memcmp(code + off, chain_thunk_pat, sizeof(chain_thunk_pat)) == 0) {
            /* 记录 chain walker thunk 地址供 code=2 补发用，但不作为
             * 宿主 timer dispatcher（避免递减 suspend depth 的副作用）。 */
            *chain_thunk_out = EXT_CODE_ADDR + off + 1u;
            return 0;
        }
    }

    /* 保留 resume 回调签名校验用于诊断 pattern 漏匹配：如果只存在
     * 0xE83590 这类单节点回调而没有上面的 walker thunk，不能启用
     * arm_ext_call_dispatch()，否则会重现直接调 game helper 的崩溃。 */
    static const uint8_t pat2[] = {
        0x30, 0xB5, 0x04, 0x1C, 0x00, 0x68, 0x00, 0x49,
        0x83, 0xB0, 0x88, 0x42, 0x03, 0xD0, 0x00, 0x20,
        0xC0, 0x43, 0x03, 0xB0,
    };
    for (uint32_t off = 0; off + sizeof(pat2) <= len; off += 2) {
        int match = 1;
        for (uint32_t i = 0; i < sizeof(pat2); ++i) {
            /* byte 6 是 LDR literal 的立即数，随 literal pool 位置变化 */
            if (i == 6) continue;
            if (code[off + i] != pat2[i]) {
                match = 0;
                break;
            }
        }
        if (match) {
            /* 验证：literal pool 中应包含 EXT_CHUNK_MAGIC (0x7FD854EB)。
             * LDR 指令在 pattern 偏移 6，Thumb PC = (instr_addr + 4) & ~3 */
            uint32_t ldr_imm = (code[off + 6] & 0xFF) * 4;
            uint32_t lit_pc = (off + 6 + 4) & ~3u;
            uint32_t lit_addr = lit_pc + ldr_imm;
            if (lit_addr + 4 <= len) {
                uint32_t lit_val = 0;
                memcpy(&lit_val, code + lit_addr, 4);
                if (lit_val == EXT_CHUNK_MAGIC) {
                    return 0;
                }
            }
        }
    }
    return 0;
}

static void patch_wrapper_stack_size(ArmExtModule *m) {
    struct PatchWord {
        uint32_t old_value;
        uint32_t new_value;
    };
    static const struct PatchWord patches[] = {
        {0xe2806d80u, 0xe2806802u}, /* add r6, r0, #0x2000 -> #0x20000 */
        {0xe280bd80u, 0xe280b802u}, /* add fp, r0, #0x2000 -> #0x20000 */
        {0xe2806dc0u, 0xe2806802u}, /* add r6, r0, #0x3000 -> #0x20000 */
        {0xe280bdc0u, 0xe280b802u}, /* add fp, r0, #0x3000 -> #0x20000 */
    };
    uint32_t patch_len = m->code_len < 0x200u ? m->code_len : 0x200u;
    for (uint32_t off = 0; off + 4 <= patch_len; off += 4) {
        uint32_t value = 0;
        memcpy(&value, arm_ptr(m, EXT_CODE_ADDR + off), 4);
        for (size_t i = 0; i < sizeof(patches) / sizeof(patches[0]); ++i) {
            if (value == patches[i].old_value) {
                memcpy(arm_ptr(m, EXT_CODE_ADDR + off), &patches[i].new_value, 4);
                if (getenv("VMRP_ARM_EXT_TRACE")) {
                    printf("arm_ext_executor: patched wrapper stack at 0x%X\n", EXT_CODE_ADDR + off);
                }
                break;
            }
        }
    }
}

static void init_table(ArmExtModule *m) {
    arm_ext_init_pack_names(m);
    for (uint32_t i = 0; i < EXT_TABLE_COUNT; ++i) {
        write_table_entry(m, i, EXT_TABLE_ADDR + i * 4);
    }
    /* Shadow storage for table data slots used by C EXT code. */
    for (uint32_t i = 91; i <= 112; ++i) {
        write_table_entry(m, i, alloc_u32_slot(m, 0));
    }
    write_table_entry(m, 135, alloc_u32_slot(m, 0));
    write_table_entry(m, 136, alloc_u32_slot(m, 0));
    write_table_entry(m, 139, alloc_u32_slot(m, 0));
    write_table_entry(m, 140, alloc_u32_slot(m, 0));
    write_table_entry(m, 142, alloc_u32_slot(m, 0));
    write_table_entry(m, 143, alloc_u32_slot(m, 0));
    write_table_entry(m, 146, alloc_u32_slot(m, 0));

    m->mr_m0_files_addr = arm_alloc(m, 50 * 4);
    m->vm_state_slot = alloc_u32_slot(m, 0);
    m->mr_state_slot = alloc_u32_slot(m, 1); /* MR_STATE_RUN */
    m->bi_slot = alloc_u32_slot(m, 0);
    m->mr_timer_p_slot = alloc_u32_slot(m, 0);
    m->mr_timer_state_slot = alloc_u32_slot(m, 0); /* MR_TIMER_STATE_IDLE */
    m->mr_timer_run_without_pause_slot = alloc_u32_slot(m, 0);
    m->mr_gzin_buf_slot = alloc_u32_slot(m, 0);
    m->mr_gzout_buf_slot = alloc_u32_slot(m, 0);
    m->lg_gzinptr_slot = alloc_u32_slot(m, 0);
    m->lg_gzoutcnt_slot = alloc_u32_slot(m, 0);
    m->mr_sms_cfg_need_save_slot = alloc_u32_slot(m, 0);
    m->internal_table_addr = arm_alloc(m, 17 * 4);
    m->port_table_addr = arm_alloc(m, 4 * 4);
    if (m->internal_table_addr) {
        uint32_t internal[17] = {
            m->mr_m0_files_addr,
            m->vm_state_slot,
            m->mr_state_slot,
            m->bi_slot,
            m->mr_timer_p_slot,
            m->mr_timer_state_slot,
            m->mr_timer_run_without_pause_slot,
            m->mr_gzin_buf_slot,
            m->mr_gzout_buf_slot,
            m->lg_gzinptr_slot,
            m->lg_gzoutcnt_slot,
            m->mr_sms_cfg_need_save_slot,
            EXT_TABLE_ADDR + 12 * 4,
            EXT_TABLE_ADDR + 13 * 4,
            EXT_TABLE_ADDR + 14 * 4,
            EXT_TABLE_ADDR + 15 * 4,
            0,
        };
        memcpy(arm_ptr(m, m->internal_table_addr), internal, sizeof(internal));
        write_table_entry(m, 23, m->internal_table_addr);
    }
    if (m->port_table_addr) {
        write_table_entry(m, 24, m->port_table_addr);
    }

    int32_t sw = mr_screen_w > 0 ? mr_screen_w : vmrp_config.screen_width;
    int32_t sh = mr_screen_h > 0 ? mr_screen_h : vmrp_config.screen_height;
    int32_t bit = 16;
    m->screen_w = sw;
    m->screen_h = sh;
    m->screen_len = (uint32_t)(sw * sh * 2);
    /* 屏幕缓冲保持为 bump 堆的首个分配(地址/布局与既有应用完全一致——
     * gghjt game.ext 会扫描 RAM 定位 framebuffer,netpay dump0 恢复也依赖
     * 既有堆地址)。应用改写 ARM 可见 mr_screen_w/h 把逻辑画布改大时,由
     * slot 写监视钩子把缓冲迁移到顶部保留区(见 hook_screen_dim_write)。 */
    m->screen_cap = m->screen_len;
    m->screen_addr = arm_alloc(m, m->screen_len);
    if (m->screen_addr && mr_screenBuf) {
        memcpy(arm_ptr(m, m->screen_addr), mr_screenBuf, m->screen_len);
    }
    m->screen_dirty = 0;

    write_table_entry(m, 91, alloc_u32_slot(m, m->screen_addr));
    m->screen_w_slot = alloc_u32_slot(m, (uint32_t)sw);
    m->screen_h_slot = alloc_u32_slot(m, (uint32_t)sh);
    write_table_entry(m, 92, m->screen_w_slot);
    write_table_entry(m, 93, m->screen_h_slot);
    write_table_entry(m, 94, alloc_u32_slot(m, (uint32_t)bit));
    /* 监视 guest 对 mr_screen_w/h 变量的写入:应用(如 gtcm 的 SphinxJoy
     * 引擎)在 init 时直接改写这两个变量配置逻辑画布,必须在其后续绘制
     * (graphics.ext 会缓存 framebuffer 指针)之前完成缓冲迁移。两个 slot
     * 由连续的 alloc_u32_slot 分配,地址相邻。 */
    if (m->screen_w_slot && m->screen_h_slot) {
        uc_hook dim_hook;
        uc_hook dim_read_hook;
        uint32_t lo = m->screen_w_slot < m->screen_h_slot ? m->screen_w_slot : m->screen_h_slot;
        uint32_t hi = m->screen_w_slot < m->screen_h_slot ? m->screen_h_slot : m->screen_w_slot;
        uc_hook_add(m->uc, &dim_hook, UC_HOOK_MEM_WRITE, hook_screen_dim_write,
                    m, lo, hi + 3);
        /* 读钩子:guest 写入转置尺寸(撤销旋转)后,在读回前恢复宿主尺寸,
         * 见 hook_screen_dim_read 注释 */
        uc_hook_add(m->uc, &dim_read_hook, UC_HOOK_MEM_READ,
                    hook_screen_dim_read, m, lo, hi + 3);
    }

    const uint32_t bitmap_count = 31;
    const uint32_t bitmap_stride = 16;
    const uint32_t bitmapmax = 30;
    uint32_t bitmap_array = arm_alloc(m, bitmap_count * bitmap_stride);
    if (bitmap_array) {
        uint8_t *bm = (uint8_t *)arm_ptr(m, bitmap_array + bitmapmax * bitmap_stride);
        uint16_t sw16 = (uint16_t)sw;
        uint16_t sh16 = (uint16_t)sh;
        uint32_t type = 0;
        memcpy(bm + 0, &sw16, 2);
        memcpy(bm + 2, &sh16, 2);
        memcpy(bm + 4, &m->screen_len, 4);
        memcpy(bm + 8, &type, 4);
        memcpy(bm + 12, &m->screen_addr, 4);
        write_table_entry(m, 95, bitmap_array);
    }

    /*
     * table[100] is ARM-visible package identity and code commonly copies it
     * into fixed 32-byte buffers.  arm_ext_init_pack_names() keeps the host path
     * separately and derives the shortest openable alias, so expose the alias
     * from startup and resolve it back to the host path in file bridges.
     */
    arm_ext_set_pack_table_name(m, m->pack_alias[0] ?
                                   m->pack_alias :
                                   mr_get_pack_filename());
    write_table_entry(m, 101, alloc_string(m, mr_get_start_filename()));
    write_table_entry(m, 102, alloc_string(m, mr_get_old_pack_filename()));
    write_table_entry(m, 103, alloc_string(m, mr_get_old_start_filename()));
    write_table_entry(m, 138, alloc_string(m, mr_get_start_fileparameter()));

    /* 真机 MR 平台的应用堆一般为 512KB–1MB；origin_mem_len 过大会使
     * 部分游戏的 exRam 预算计算（= 固定值 − origin_mem_len）溢出为负，
     * 导致 SCRRAM 分配被跳过、图像资源无法渲染。默认取与真机一致的
     * 1MB(sky_istore 启动时校验堆容量,512KB 会报"内存不足");需要
     * 更大堆的应用用 --memory/VMRP_MEMORY 调整(1M-16M)。arm_alloc 仍
     * 可提供远超此值的实际内存。
     * 池在 ARM 16MB 空间的 bump 堆(0x200000 起)上分配,还需避开栈区/
     * 代码区与顶部屏幕保留区,过大的档位(16M)放不下,钳制到 8MB。 */
    m->origin_mem_len = vmrp_memory_bytes(vmrp_config.memory_mb);
    {
        uint32_t ext_pool_max = 8u * 1024u * 1024u;
        if (m->origin_mem_len > ext_pool_max) {
            printf("arm_ext_executor: clamp app heap %uMB -> 8MB (ARM address space limit)\n",
                   m->origin_mem_len >> 20);
            m->origin_mem_len = ext_pool_max;
        }
    }
    m->origin_mem_left = m->origin_mem_len;
    m->origin_mem_min = m->origin_mem_len;
    m->origin_mem_top = 0;
    /* 先分配统计 slot 和 free_head，再分配 origin_mem 池。ext 的 ARM
     * 分配器会在 origin_mem 池末尾写入元数据，如果 slot 紧跟池后面就会被覆盖。 */
    uint32_t slot108 = alloc_u32_slot(m, 0);
    uint32_t slot109 = alloc_u32_slot(m, 0);
    uint32_t slot110 = alloc_u32_slot(m, 0);
    uint32_t slot111 = alloc_u32_slot(m, 0);
    uint32_t slot135 = alloc_u32_slot(m, 0);
    uint32_t slot136 = alloc_u32_slot(m, 0);
    uint32_t free_head = arm_alloc(m, 8);
    m->origin_mem_addr = arm_alloc(m, m->origin_mem_len);
    if (m->origin_mem_addr) {
        /* mem.c _mr_mem_init:池首形成唯一空闲节点 {next=len,len=len}
         * (next==len 即"指向池尾"的链表终止哨),头节点 {next=0,len=0}
         * 指向该池首节点。头节点经 table[146] 暴露给 ARM(原生为
         * &LG_mem_free),guest first-fit 分配器(table[0]/[1]/[2] 与
         * table[125] 输出)据此运转。 */
        uint32_t free_next = m->origin_mem_len;
        uint32_t free_len = m->origin_mem_len;
        memcpy(arm_ptr(m, m->origin_mem_addr), &free_next, 4);
        memcpy((uint8_t *)arm_ptr(m, m->origin_mem_addr) + 4, &free_len, 4);
        if (free_head) {
            uint32_t zero = 0;
            memcpy(arm_ptr(m, free_head), &zero, 4);
            memcpy((uint8_t *)arm_ptr(m, free_head) + 4, &zero, 4);
        }
        m->origin_mem_head_addr = free_head;

        if (slot108) { uint32_t v = m->origin_mem_addr; memcpy(arm_ptr(m, slot108), &v, 4); }
        if (slot109) { uint32_t v = m->origin_mem_len; memcpy(arm_ptr(m, slot109), &v, 4); }
        if (slot110) { uint32_t v = m->origin_mem_addr + m->origin_mem_len; memcpy(arm_ptr(m, slot110), &v, 4); }
        if (slot111) { uint32_t v = m->origin_mem_left; memcpy(arm_ptr(m, slot111), &v, 4); }
        if (slot135) { uint32_t v = m->origin_mem_min; memcpy(arm_ptr(m, slot135), &v, 4); }
        if (slot136) { uint32_t v = m->origin_mem_top; memcpy(arm_ptr(m, slot136), &v, 4); }
        /* 保存 slot 地址，以便 note_origin_mem_alloc/free 后同步 ARM 侧 */
        m->origin_mem_left_slot = slot111;
        m->origin_mem_min_slot = slot135;
        m->origin_mem_top_slot = slot136;
        write_table_entry(m, 108, slot108);
        write_table_entry(m, 109, slot109);
        write_table_entry(m, 110, slot110);
        write_table_entry(m, 111, slot111);
        write_table_entry(m, 135, slot135);
        write_table_entry(m, 136, slot136);
        if (free_head) write_table_entry(m, 146, free_head);
    }
}

int arm_ext_load(ArmExtModule **out, const uint8 *code, uint32 len, int32 load_code, int32 *ext_ret) {
    if (!out || !code || len < 12) return MR_FAILED;
    ArmExtModule *m = calloc(1, sizeof(*m));
    if (!m) return MR_FAILED;
    /* 在 EXT_BASE_ADDR 固定地址分配 ARM 内存，使 ARM 虚拟地址等于宿主指针，
     * 与 unicorn 模式下 uc_mem_map_ptr 的行为一致——ext 返回的指针可被 Lua 直接使用 */
#ifdef _MSC_VER
    m->mem = VirtualAlloc((void *)(uintptr_t)EXT_BASE_ADDR, EXT_MEM_SIZE,
                          MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (m->mem == NULL) {
        m->mem = calloc(1, EXT_MEM_SIZE);
        m->mem_is_mmap = 0;
    } else {
        m->mem_is_mmap = 1;
        memset(m->mem, 0, EXT_MEM_SIZE);
    }
#else
    m->mem = mmap((void *)(uintptr_t)EXT_BASE_ADDR, EXT_MEM_SIZE,
                  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
    if (m->mem == MAP_FAILED) {
        m->mem = calloc(1, EXT_MEM_SIZE);
        m->mem_is_mmap = 0;
    } else {
        m->mem_is_mmap = 1;
        memset(m->mem, 0, EXT_MEM_SIZE);
    }
#endif
    if (!m->mem) goto fail;
    m->platform_mem = calloc(1, EXT_PLATFORM_MEM_SIZE);
    if (!m->platform_mem) goto fail;
    m->platform_io_mem = calloc(1, EXT_PLATFORM_IO_MEM_SIZE);
    if (!m->platform_io_mem) goto fail;
    m->platform_alt_mem = calloc(1, EXT_PLATFORM_ALT_MEM_SIZE);
    if (!m->platform_alt_mem) goto fail;
    m->executor_meta_mem = calloc(1, EXT_EXECUTOR_META_SIZE);
    if (!m->executor_meta_mem) goto fail;
    m->low_table = calloc(1, EXT_LOW_TABLE_SIZE);
    if (!m->low_table) goto fail;
    m->heap_top = EXT_HEAP_ADDR;
    m->code_len = len;
    m->host_code = code;

    arm_ext_init_pack_names(m);
    m->profile = app_compat_select(m->pack_host_path[0] ?
                                   m->pack_host_path :
                                   mr_get_pack_filename());
    if (m->profile && m->profile->init)
        m->app_state = m->profile->init(m);

    uc_err err = uc_open(UC_ARCH_ARM, UC_MODE_ARM, &m->uc);
    if (err != UC_ERR_OK) goto fail;
    err = uc_mem_map_ptr(m->uc, EXT_BASE_ADDR, EXT_MEM_SIZE, UC_PROT_ALL, m->mem);
    if (err != UC_ERR_OK) goto fail;
    err = uc_mem_map_ptr(m->uc, EXT_PLATFORM_MEM_ADDR,
                         EXT_PLATFORM_MEM_SIZE, UC_PROT_ALL,
                         m->platform_mem);
    if (err != UC_ERR_OK) goto fail;
    /*
     * MTK firmware uses 0x80000000-class addresses for ROM/MMIO/platform
     * state. Some EXT libraries probe these addresses and then keep small
     * halfword tables under that band (for example base+4, +8, ...). Keep the
     * band ARM-visible so those data probes behave like handset memory
     * instead of crashing on an unmapped read.
     */
    err = uc_mem_map_ptr(m->uc, EXT_PLATFORM_IO_MEM_ADDR,
                         EXT_PLATFORM_IO_MEM_SIZE, UC_PROT_ALL,
                         m->platform_io_mem);
    if (err != UC_ERR_OK) goto fail;
    err = uc_mem_map_ptr(m->uc, EXT_PLATFORM_ALT_MEM_ADDR,
                         EXT_PLATFORM_ALT_MEM_SIZE, UC_PROT_ALL,
                         m->platform_alt_mem);
    if (err != UC_ERR_OK) goto fail;
    err = uc_mem_map_ptr(m->uc, EXT_EXECUTOR_META_ADDR,
                         EXT_EXECUTOR_META_SIZE, UC_PROT_ALL,
                         m->executor_meta_mem);
    if (err != UC_ERR_OK) goto fail;
    init_table(m);
    /* 预解析 MRP，缓存所有条目，避免 ARM ext 做极慢的 MRP 索引扫描 */
    parse_mrp_cache(m, m->pack_host_path[0] ?
                    m->pack_host_path :
                    mr_get_pack_filename());
    memcpy(m->low_table, m->mem, EXT_TABLE_COUNT * 4);
    err = uc_mem_map_ptr(m->uc, 0, EXT_LOW_TABLE_SIZE, UC_PROT_ALL, m->low_table);
    if (err != UC_ERR_OK) goto fail;
    void *code_dst = arm_ptr(m, EXT_CODE_ADDR);
    if (!code_dst) goto fail;
    memcpy(code_dst, code, len);
    m->wrapper_timer_dispatch_addr = find_wrapper_timer_dispatch(
        code, len, &m->chain_walker_thunk_addr,
        &m->wrapper_compact_timer_scheduler_off);
    if (getenv("VMRP_ARM_EXT_TRACE")) {
        printf("arm_ext_executor: wrapper_timer_dispatch=0x%X wrapper_compact_sched=0x%X chain_walker_thunk=0x%X\n",
               m->wrapper_timer_dispatch_addr,
               m->wrapper_compact_timer_scheduler_off,
               m->chain_walker_thunk_addr);
    }
    patch_wrapper_stack_size(m);
    uint32_t table = EXT_TABLE_ADDR;
    memcpy(arm_ptr(m, EXT_CODE_ADDR), &table, 4);
    err = uc_hook_add(m->uc, &m->hook, UC_HOOK_CODE, hook_table, m, EXT_TABLE_ADDR, EXT_TABLE_ADDR + EXT_TABLE_COUNT * 4);
    if (err != UC_ERR_OK) goto fail;
    uc_hook low_zero_hook;
    err = uc_hook_add(m->uc, &low_zero_hook, UC_HOOK_CODE, hook_low_zero, m, 0, EXT_LOW_TABLE_SIZE - 1);
    if (err != UC_ERR_OK) goto fail;
    if (getenv("VMRP_ARM_EXT_TRACE_PC")) {
        uc_hook pc_hook;
        err = uc_hook_add(m->uc, &pc_hook, UC_HOOK_CODE, trace_pc, m,
                          EXT_CODE_ADDR, EXT_CODE_ADDR + len);
        if (err != UC_ERR_OK) goto fail;
    }
    uc_hook restore_hook;
    err = uc_hook_add(m->uc, &restore_hook, UC_HOOK_BLOCK, hook_restore_r9, m,
                      EXT_BASE_ADDR, EXT_BASE_ADDR + EXT_MEM_SIZE - 1);
    if (err != UC_ERR_OK) goto fail;
    uc_hook restore_platform_hook;
    err = uc_hook_add(m->uc, &restore_platform_hook, UC_HOOK_BLOCK,
                      hook_restore_r9, m,
                      EXT_PLATFORM_MEM_ADDR,
                      EXT_PLATFORM_MEM_ADDR + EXT_PLATFORM_MEM_SIZE - 1);
    if (err != UC_ERR_OK) goto fail;
    uc_hook restore_platform_io_hook;
    err = uc_hook_add(m->uc, &restore_platform_io_hook, UC_HOOK_BLOCK,
                      hook_restore_r9, m,
                      EXT_PLATFORM_IO_MEM_ADDR,
                      EXT_PLATFORM_IO_MEM_ADDR + EXT_PLATFORM_IO_MEM_SIZE - 1);
    if (err != UC_ERR_OK) goto fail;
    uc_hook restore_platform_alt_hook;
    err = uc_hook_add(m->uc, &restore_platform_alt_hook, UC_HOOK_BLOCK,
                      hook_restore_r9, m,
                      EXT_PLATFORM_ALT_MEM_ADDR,
                      EXT_PLATFORM_ALT_MEM_ADDR + EXT_PLATFORM_ALT_MEM_SIZE - 1);
    if (err != UC_ERR_OK) goto fail;
    uc_hook screen_hook;
    if (m->screen_addr && m->screen_len) {
        err = uc_hook_add(m->uc, &screen_hook, UC_HOOK_MEM_WRITE, hook_screen_write, m,
                          m->screen_addr, m->screen_addr + m->screen_len - 1);
        if (err != UC_ERR_OK) goto fail;
    }
    uc_hook invalid_hook;
    err = uc_hook_add(m->uc, &invalid_hook, UC_HOOK_MEM_INVALID, hook_invalid, m, 1, 0);
    if (err != UC_ERR_OK) goto fail;
    uc_hook intr_hook;
    err = uc_hook_add(m->uc, &intr_hook, UC_HOOK_INTR, hook_intr, m, 1, 0);
    if (err != UC_ERR_OK) goto fail;
    /* GOT bridge 指针追踪：ARM 代码向 R9 数据区写入 bridge 地址时记录，
     * 后续 dump/restore 覆盖时自动修复 */
    {
        uc_hook got_hook;
        uc_hook_add(m->uc, &got_hook, UC_HOOK_MEM_WRITE, hook_got_write, m,
                    EXT_HEAP_ADDR, EXT_BASE_ADDR + EXT_MEM_SIZE - 1);
    }
    /* 临时诊断:VMRP_WATCH_WRITE 指定范围的 guest 写监视(修复后删除) */
    {
        uint32_t watch_lo = 0, watch_hi = 0;
        if (arm_ext_watch_write_range(&watch_lo, &watch_hi)) {
            uc_hook watch_hook;
            uc_hook_add(m->uc, &watch_hook, UC_HOOK_MEM_WRITE,
                        hook_watch_write, m, watch_lo, watch_hi - 1);
            printf("arm_ext_executor: WATCH_WRITE hook [0x%X,0x%X)\n",
                   watch_lo, watch_hi);
        }
    }

    reg_write32(m->uc, UC_ARM_REG_R0, (uint32_t)load_code);
    uint16 *saved_screenBuf = NULL;
    uint32_t present_depth_before = 0;
    enter_screen_context(m, &saved_screenBuf, &present_depth_before);
    int load_ret = run_arm(m, EXT_CODE_ADDR + 8);
    leave_screen_context(m, saved_screenBuf, present_depth_before);
    if (load_ret != MR_SUCCESS) goto fail;
    capture_timer_dispatches(m);
    /* 将 ARM 代码 mr_c_function_load() 的返回值 (R0) 传给调用者，
     * 与非 native 路径中 mr_load_c_function(code) 的返回值语义一致 */
    if (ext_ret) *ext_ret = (int32_t)reg_read32(m->uc, UC_ARM_REG_R0);
    *out = m;
    return MR_SUCCESS;
fail:
    arm_ext_unload(m);
    return MR_FAILED;
}


static int arm_ext_finish_callback_state(ArmExtModule *m, uint32_t ext_chunk) {
    if (!m) return 0;

    /* ARM ext 的退出/重启逻辑会直接写 mr_state_slot 设置
     * MR_STATE_RESTART(3) 或 MR_STATE_STOP(4)。真机上 ARM 代码与宿主共享
     * 同一全局变量；Unicorn 下 ARM 内存独立，所以回调结束后要同步一次。 */
    if (m->mr_state_slot) {
        uint32_t arm_state = 0;
        memcpy(&arm_state, arm_ptr(m, m->mr_state_slot), 4);
        if (arm_state >= 3) {
            mr_exit();
            return 1;
        }
    }

    if (!ext_chunk || !arm_ptr(m, ext_chunk + 0x34)) {
        return 0;
    }

    /* wrapper 反汇编确认 extChunk[0x34] 是 suspend 深度：
     *   0xE831A4 suspend: [0x34]++，首次进入时摘掉 wrapper timer 节点
     *   0xE83220 resume:  [0x34]--，回到 0 时重新挂回 timer 节点
     * 普通 wrapper 模态/子流程会把 active_helper_addr 切到后加载的子模块；
     * gxdzc 的资源下载提示则只摘掉 game timer，并把旧链表头保存在
     * saved_game_timer_head，取消时还要恢复。真正的“返回上层/停止自驱”
     * 路径没有前台子模块、没有可恢复的 game timer，且宿主定时器空闲。
     * 模拟器没有真机平台的上层调度器，因此只在这些条件都满足时调用
     * mr_exit()，把 wrapper 状态同步到宿主平台层。 */
    enum { MR_TIMER_STATE_IDLE = 0 };
    uint32_t suspend_depth = 0;
    memcpy(&suspend_depth, arm_ptr(m, ext_chunk + 0x34), 4);
    int foreground_child =
        m->primary_helper_addr &&
        m->active_helper_addr &&
        m->active_helper_addr != m->primary_helper_addr;
    if (suspend_depth > 0 &&
        mr_timer_state == MR_TIMER_STATE_IDLE &&
        !foreground_child &&
        m->saved_game_timer_head == 0) {
        mr_exit();
        return 1;
    }
    return 0;
}

static int arm_ext_save_modal_screen_snapshot(ArmExtModule *m) {
    if (!m || !m->screen_addr || !m->screen_len ||
        !arm_ptr(m, m->screen_addr)) {
        return 0;
    }
    if (m->modal_screen_snapshot_len != m->screen_len) {
        free(m->modal_screen_snapshot);
        m->modal_screen_snapshot = NULL;
        m->modal_screen_snapshot_len = 0;
        m->modal_screen_snapshot_valid = 0;
    }
    if (!m->modal_screen_snapshot) {
        m->modal_screen_snapshot = (uint8_t *)malloc(m->screen_len);
        if (m->modal_screen_snapshot) m->modal_screen_snapshot_len = m->screen_len;
    }
    if (!m->modal_screen_snapshot) return 0;
    memcpy(m->modal_screen_snapshot, arm_ptr(m, m->screen_addr), m->screen_len);
    m->modal_screen_snapshot_valid = 0;
    return 1;
}

/* wrapper RW 前台分发区快照范围：覆盖 wrapper 在 RW 内维护的前台模块分发表
 * 与前台 helper 槽。偏移与长度来自对 cfunction.ext 事件分发路径的反汇编与
 * 运行时 RW 对比（dota 浏览器插件下载：进入子模块时 wrapper 改写 +0xF4 分发
 * 表项及 +0x18C/+0x1A0 helper 槽，关闭后未还原导致事件仍走子模块）。 */
#define MODAL_FG_SNAPSHOT_OFF 0xE0u
#define MODAL_FG_SNAPSHOT_LEN 0xD0u

static int arm_ext_save_modal_fg_snapshot(ArmExtModule *m) {
    if (!m || !m->p_addr || !arm_ptr(m, m->p_addr)) return 0;
    uint32_t rw = 0;
    memcpy(&rw, arm_ptr(m, m->p_addr), 4);
    if (!rw || !arm_ptr(m, rw + MODAL_FG_SNAPSHOT_OFF + MODAL_FG_SNAPSHOT_LEN - 1)) {
        return 0;
    }
    if (!m->modal_fg_snapshot) {
        m->modal_fg_snapshot = (uint8_t *)malloc(MODAL_FG_SNAPSHOT_LEN);
        if (!m->modal_fg_snapshot) return 0;
    }
    memcpy(m->modal_fg_snapshot, arm_ptr(m, rw + MODAL_FG_SNAPSHOT_OFF),
           MODAL_FG_SNAPSHOT_LEN);
    m->modal_fg_snapshot_rw = rw;
    m->modal_fg_snapshot_valid = 0;
    return 1;
}

/* 模态子模块关闭时还原 wrapper 前台分发区，使事件路由回到下层页面。
 * 仅当 wrapper RW 基址未变（同一 wrapper 实例）时还原，避免误写。 */
static void arm_ext_restore_modal_fg_snapshot(ArmExtModule *m) {
    if (!m || !m->modal_fg_snapshot || !m->modal_fg_snapshot_valid) return;
    uint32_t rw = 0;
    if (m->p_addr && arm_ptr(m, m->p_addr))
        memcpy(&rw, arm_ptr(m, m->p_addr), 4);
    if (rw && rw == m->modal_fg_snapshot_rw &&
        arm_ptr(m, rw + MODAL_FG_SNAPSHOT_OFF + MODAL_FG_SNAPSHOT_LEN - 1)) {
        memcpy(arm_ptr(m, rw + MODAL_FG_SNAPSHOT_OFF), m->modal_fg_snapshot,
               MODAL_FG_SNAPSHOT_LEN);
    }
    m->modal_fg_snapshot_valid = 0;
}

static void arm_ext_dispatch_pending_sms_result(ArmExtModule *m) {
    if (!m || !m->pending_sms_result || m->dispatching_sms_result) return;
    int32_t result = m->pending_sms_result_value;
    m->pending_sms_result = 0;
    m->dispatching_sms_result = 1;
    int32_t event[5] = {MR_SMS_RESULT, result, 0, 0, 0};
    uint8 *output = NULL;
    int32 output_len = 0;
    (void)arm_ext_call(m, 1, event, sizeof(event), &output, &output_len);
    m->dispatching_sms_result = 0;
}

int arm_ext_call(ArmExtModule *m, int32 code, const void *input, uint32 input_len,
                 uint8 **output, int32 *output_len) {
    if (output) *output = NULL;
    if (output_len) *output_len = 0;
    if (!m || !m->helper_addr || !m->p_addr) return MR_FAILED;
    m->busy_wait_count = 0;
    m->busy_wait_start_ms = 0;
    uint32_t input_addr = 0;
    if (input != NULL) {
        if (input == m->host_code) {
            input_addr = EXT_CODE_ADDR;
        } else if (code == 3 || code == 6) {
            /*
             * Some start.mr files pass non-pointer scalar/table markers for
             * these bootstrap messages.  The ARM helper does not dereference
             * them, so keep the raw 32-bit value instead of memcpy'ing from an
             * invalid host address such as 0x1.
             */
            input_addr = (uint32_t)(uintptr_t)input;
        } else {
            input_addr = arm_alloc(m, input_len ? input_len : 1);
            if (!input_addr) return MR_FAILED;
            if (input_len) memcpy(arm_ptr(m, input_addr), input, input_len);
        }
    }
    uint32_t outp_addr = arm_alloc(m, 4);
    uint32_t outl_addr = arm_alloc(m, 4);
    /* 真机上事件(code=1)始终先经 wrapper 的 mrc_extHelper（反汇编
     * docs/反汇编研究.c:428-476），wrapper 通过 mrc_event 的内部回调链
     * 分发给 game。code=0/3-5 为生命周期调用，路由到 primary game helper。
     * code=2 路由到当前定时器所有者（wrapper 或 game）。 */
    int has_separate_wrapper = m->helper_addr && m->primary_helper_addr &&
                               m->helper_addr != m->primary_helper_addr;
    uint32_t call_p_addr, call_helper_addr;
    if (code == 1 && has_separate_wrapper) {
        call_p_addr = m->p_addr;
        call_helper_addr = m->helper_addr;
    } else if (code >= 0 && code <= 5 && m->primary_p_addr) {
        call_p_addr = m->primary_p_addr;
        call_helper_addr = m->primary_helper_addr;
    } else {
        call_p_addr = m->active_p_addr ? m->active_p_addr : m->p_addr;
        call_helper_addr = m->active_helper_addr ? m->active_helper_addr : m->helper_addr;
    }
    if (code == 2 && m->timer_p_addr && m->timer_helper_addr) {
        call_p_addr = m->timer_p_addr;
        call_helper_addr = m->timer_helper_addr;
    }
    int wrapper_raw_event_routed = 0;
    int reopen_set_this_call = 0;
    uint32_t modal_ext_chunk = 0;
    uint32_t modal_suspend_depth_pre = 0;
    if (m->primary_p_addr && arm_ptr(m, m->primary_p_addr + 12))
        memcpy(&modal_ext_chunk, arm_ptr(m, m->primary_p_addr + 12), 4);
    if (modal_ext_chunk && arm_ptr(m, modal_ext_chunk + 0x34))
        memcpy(&modal_suspend_depth_pre, arm_ptr(m, modal_ext_chunk + 0x34), 4);
    int suspended_foreground_child =
        arm_ext_has_suspended_foreground_child(m, modal_ext_chunk);
    int foreground_child_has_private_timer =
        arm_ext_has_foreground_child(m) &&
        (arm_ext_foreground_child_has_frame_timer_queue(m) ||
         arm_ext_foreground_child_has_compact_timer_queue(m));
    int foreground_child_owns_timer =
        m->active_p_addr && m->active_helper_addr &&
        m->timer_p_addr == m->active_p_addr &&
        m->timer_helper_addr == m->active_helper_addr;
    if (code == 2 && foreground_child_has_private_timer &&
        (foreground_child_owns_timer ||
         !m->timer_p_addr || !m->timer_helper_addr)) {
        /*
         * A painted foreground child is not enough timer-liveness evidence:
         * optwar's compact downloader walker returns immediately when
         * R9+0x250 is null, while the wrapper still owns the host timer.
         * Likewise advbar can keep a private cleanup node while wrapper-owned
         * modal/download transitions are pending.  Route direct child ticks
         * only when the child owns the timer, or no owner has been recorded
         * yet.
         */
        call_p_addr = m->active_p_addr;
        call_helper_addr = m->active_helper_addr;
    }
    int foreground_child_active =
        m->active_p_addr &&
        m->active_helper_addr &&
        m->active_p_addr != m->primary_p_addr &&
        m->active_helper_addr != m->primary_helper_addr &&
        m->active_p_addr != m->p_addr &&
        m->active_helper_addr != m->helper_addr;
    /* 真机上 code=1 事件始终先经 wrapper 的 mrc_extHelper（反汇编
     * docs/反汇编研究.c:428-476），wrapper 通过内部回调链分发给 game
     * 或前台子模块。不绕过 wrapper，否则 wrapper 的 suspend/resume
     * 机制无法正确管理模态框生命周期。
     *
     * 预处理：
     * - 修复 extChunk[8]（game helper 指针），确保 wrapper 的 resume
     *   (0xE83220) 能通过 blx extChunk[8] 正确调用 game 的 resumeApp。
     *   cfunction.ext ARM 代码运行中会将该槽位清零（gxdzc 反汇编证实）。 */
    if (code == 1 && has_separate_wrapper) {
        uint32_t _ec = modal_ext_chunk;
        if (_ec && m->primary_helper_addr && arm_ptr(m, _ec + 8)) {
            memcpy(arm_ptr(m, _ec + 8), &m->primary_helper_addr, 4);
        }
        /*
         * Host-originated code=1 packets carry five int32 fields.  The wrapper
         * sees those raw events before Lua dealevent; return success for them
         * so a key press is not replayed by Lua's later 12-byte _strCom(801)
         * forwarding path.  Keep 12-byte internal forwards on the helper's real
         * return value, which lets non-modal overlays decline events after
         * their visible state has already been cleared.
         */
        if (input_len >= 20 && input_addr) {
            wrapper_raw_event_routed = 1;
        }
    }

    /* chain walker dispatch 的 supplementary_init_done 计数在
     * arm_ext_call_dispatch 内部管理，这里不需要额外门控。 */

    uint32_t rw_base = 0;
    memcpy(&rw_base, arm_ptr(m, call_p_addr), 4);
    if (getenv("VMRP_ARM_EXT_DIAG")) {
        uint32_t ev[5] = {0, 0, 0, 0, 0};
        if (code == 1 && input_addr && input_len >= sizeof(ev) &&
            arm_ptr(m, input_addr + sizeof(ev) - 1)) {
            memcpy(ev, arm_ptr(m, input_addr), sizeof(ev));
        }
        printf("DIAG arm_ext_call code=%d input_len=%u callP=0x%X callH=0x%X rw=0x%X activeP=0x%X activeH=0x%X primaryP=0x%X primaryH=0x%X timerP=0x%X timerH=0x%X modalDepth=%u ev=%u,%u,%u,%u,%u\n",
               (int)code, input_len, call_p_addr, call_helper_addr, rw_base,
               m->active_p_addr, m->active_helper_addr, m->primary_p_addr,
               m->primary_helper_addr, m->timer_p_addr, m->timer_helper_addr,
               modal_suspend_depth_pre, ev[0], ev[1], ev[2], ev[3], ev[4]);
        if (code == 2 && suspended_foreground_child) {
            /*
             * Browser private children keep their timer queue under child
             * R9+0x94 (frame.ext 0x2C96A0), not the primary game timer head.
             * Dump that window around each routed tick so stalled foreground
             * progress can be tied to the child ABI instead of broad tracing.
             */
            arm_ext_diag_dump_wrapper_timer_state(m, "call-pre");
            arm_ext_diag_dump_rw_timer_state(m, "call-pre-active", rw_base);
        }
    }
    arm_ext_diag_dump_layer_state(m, "call-pre");
    if (rw_base) reg_write32(m->uc, UC_ARM_REG_R9, rw_base);
    reg_write32(m->uc, UC_ARM_REG_R0, call_p_addr);
    reg_write32(m->uc, UC_ARM_REG_R1, (uint32_t)code);
    reg_write32(m->uc, UC_ARM_REG_R2, input_addr);
    reg_write32(m->uc, UC_ARM_REG_R3, input_len);
    uint32_t sp = EXT_STACK_ADDR + EXT_STACK_SIZE - 16;
    uint32_t helper_args[4] = {outp_addr, outl_addr, 0, 0};
    uc_mem_write(m->uc, sp, helper_args, sizeof(helper_args));
    reg_write32(m->uc, UC_ARM_REG_SP, sp);

    /* 保存 arm_ext_call 前的 game state[8] */
    uint32_t _ac_grw = 0, _ac_s8_pre = 0;
    if (m->primary_p_addr && arm_ptr(m, m->primary_p_addr))
        memcpy(&_ac_grw, arm_ptr(m, m->primary_p_addr), 4);
    _ac_s8_pre = read_game_timer_head(m, _ac_grw);
    int call_modal_snapshot_candidate = 0;
    /* 非模态状态下的 code=1 事件可能触发子模块加载，需要事先保存 game
     * 的快照以便子模块关闭后恢复。事件经 wrapper 路由时 call_p_addr
     * 指向 wrapper 而非 primary，但语义不变：game 仍然会收到该事件。 */
    /* gxdzc 的下载提示可能在普通事件回调或 primary timer(code=2) 中
     * 直接触发 wrapper suspend，而不是等到 wrapper dispatch；回调前
     * 保存，回调后确认 [0x34] 0->1 才允许 cancel 使用这张底层页面截图。 */
    if ((code == 1 || code == 2) && modal_suspend_depth_pre == 0) {
        if (_ac_s8_pre != 0)
            call_modal_snapshot_candidate = arm_ext_save_modal_screen_snapshot(m);
        /* 同步快照 wrapper 前台分发区，供模态子模块关闭后还原事件路由。 */
        arm_ext_save_modal_fg_snapshot(m);
    }

    /* extChunk[8]（game helper 指针）在 wrapper ARM 代码执行期间会被清零，
     * code=2 定时器回调中 wrapper 通过 blx extChunk[8] 转发给 game helper，
     * 若该槽已被先前回调清零，后续定时器将无法到达 game，导致 game 的图像
     * 加载初始化仅执行一次。对所有经 wrapper 转发的事件统一修复。 */
    if (has_separate_wrapper && modal_ext_chunk &&
        m->primary_helper_addr && arm_ptr(m, modal_ext_chunk + 8)) {
        uint32_t cur_helper = 0;
        memcpy(&cur_helper, arm_ptr(m, modal_ext_chunk + 8), 4);
        if (!cur_helper) {
            memcpy(arm_ptr(m, modal_ext_chunk + 8), &m->primary_helper_addr, 4);
        }
    }

    uint16 *saved_screenBuf = NULL;
    uint32_t present_depth_before = 0;
    enter_screen_context(m, &saved_screenBuf, &present_depth_before);
    sync_internal_state_to_arm(m);
    int was_host_timer_pending = m->host_timer_pending;
    if (code == 2) {
        m->host_timer_pending = 0;
    }
    /* wrapper 的 code=2 回调中可能通过 table[25]（mr_load_c_function）加载
     * 新的前台子模块（如下载完成后启动 promote.mrp），此时 active_p_addr
     * 会在 run_arm_with_sp 内部被更新。记录回调前的值，回调后比对以检测
     * 前台子模块替换。 */
    uint32_t active_p_addr_pre = m->active_p_addr;
    m->current_p_addr = call_p_addr;
    m->current_helper_addr = call_helper_addr;
    int prev_in_dispatch = m->in_dispatch;
    /*
     * Runtime event/timer callbacks may query the ARM EXT device band while
     * building child graphics dispatch tables.  Lifecycle calls such as code=0
     * still run during startVmrp(), before the E2E control socket exists, and
     * must keep native startup behavior.
     */
    if (code == 1 || code == 2) m->in_dispatch = 1;
    int call_ret = run_arm_with_sp(m, call_helper_addr, sp);
    m->in_dispatch = prev_in_dispatch;
    m->current_p_addr = 0;
    m->current_helper_addr = 0;
    sync_timer_state_from_arm(m);
    if (code == 2 && was_host_timer_pending && m->host_timer_pending) {
        internal_slot_write(m, m->mr_timer_state_slot, (uint32_t)mr_timer_state);
    }
    leave_screen_context(m, saved_screenBuf, present_depth_before);
    capture_timer_dispatches(m);
    arm_ext_diag_dump_layer_state(m, "call-post");
    if (getenv("VMRP_ARM_EXT_DIAG") && code == 2 &&
        suspended_foreground_child) {
        uint32_t active_rw_post = 0;
        if (m->active_p_addr && arm_ptr(m, m->active_p_addr)) {
            memcpy(&active_rw_post, arm_ptr(m, m->active_p_addr), 4);
        }
        arm_ext_diag_dump_wrapper_timer_state(m, "call-post");
        arm_ext_diag_dump_rw_timer_state(m, "call-post-active",
                                         active_rw_post);
    }

    /* wrapper 无 primary module 时（如"请稍后"加载阶段），wrapper 的
     * code=2 处理完后不会通过 table[31] 重启宿主 timer。宿主需要持续
     * 给 wrapper 发送 timer tick，直到 wrapper 完成 game 模块的加载并
     * 注册 primary module。 */
    if (code == 2 && !m->host_timer_pending &&
        !m->primary_p_addr && m->wrapper_timer_dispatch_addr) {
        mr_timerStart(50);
        mr_timer_state = 1;
        m->host_timer_pending = 1;
        internal_slot_write(m, m->mr_timer_state_slot, 1);
    }
    /* arm_ext_call 后检查 game timer head (state[8]) 变化：wrapper 的
     * suspend 会清零 game_rw[0x8C]，保存旧值以便 resume 时恢复。 */
    int primary_game_timer_live_post = 0;
    if (_ac_grw && _ac_s8_pre) {
        uint32_t _ac_s8_post = read_game_timer_head(m, _ac_grw);
        primary_game_timer_live_post = _ac_s8_post != 0;
        if (getenv("VMRP_ARM_EXT_DIAG")) {
            printf("DIAG game_timer_head grw=0x%X pre=0x%X post=0x%X code=%d\n",
                   _ac_grw, _ac_s8_pre, _ac_s8_post, code);
        }
        if (_ac_s8_post == 0) {
            m->saved_game_timer_head = _ac_s8_pre;
        }
    } else if (_ac_grw && !_ac_s8_pre && getenv("VMRP_ARM_EXT_DIAG")) {
        printf("DIAG game_timer_head_zero grw=0x%X code=%d\n", _ac_grw, code);
    }
    int wrapper_timer_live_post = arm_ext_wrapper_has_timer_queue(m);
    int foreground_child_live_post = arm_ext_has_foreground_child(m);
    int foreground_child_timer_live_post =
        foreground_child_live_post &&
        (arm_ext_foreground_child_has_frame_timer_queue(m) ||
         arm_ext_foreground_child_has_compact_timer_queue(m));
    int wrapper_dispatch_busy_post = arm_ext_wrapper_dispatch_is_busy(m);
    if (getenv("VMRP_ARM_EXT_TIMER_LIVENESS_DIAG") && code == 2) {
        printf("DIAG timer_liveness suspended=%d fgChildPre=%d fgChildPost=%d childTimerPre=%d childTimerPost=%d wrapperBusy=%d wrapperTimer=%d primaryTimer=%d openSockets=%d hostTimer=%d mrTimer=%d activeP=0x%X activeH=0x%X timerP=0x%X timerH=0x%X callP=0x%X callH=0x%X\n",
               suspended_foreground_child, foreground_child_active,
               foreground_child_live_post, foreground_child_has_private_timer,
               foreground_child_timer_live_post, wrapper_dispatch_busy_post,
               wrapper_timer_live_post,
               primary_game_timer_live_post, my_openSocketCount(),
               m->host_timer_pending, mr_timer_state,
               m->active_p_addr, m->active_helper_addr,
               m->timer_p_addr, m->timer_helper_addr,
               call_p_addr, call_helper_addr);
        arm_ext_diag_dump_wrapper_compact_timer_nodes(m, "liveness");
    }
    if (code == 2 &&
        m->timer_p_addr == m->p_addr &&
        m->timer_helper_addr == m->helper_addr &&
        suspended_foreground_child &&
        !foreground_child_has_private_timer &&
        !wrapper_timer_live_post &&
        !wrapper_dispatch_busy_post &&
        primary_game_timer_live_post) {
        /*
         * A wrapper-owned due-list tick can legitimately invoke a foreground
         * child and drain the wrapper queue without installing a new wrapper
         * timer.  At that point keeping timer_p_addr pinned to the wrapper
         * spins empty wrapper code=2 calls and starves the still-live primary
         * game timer head.  Drop the stale owner only when all wrapper/child
         * private liveness checks are empty so the next host tick routes
         * through the normal primary helper path.
         */
        m->timer_p_addr = 0;
        m->timer_helper_addr = 0;
        if (getenv("VMRP_ARM_EXT_DIAG")) {
            printf("DIAG timer_owner_clear staleWrapper primaryTimer=0x%X activeP=0x%X activeH=0x%X\n",
                   read_game_timer_head(m, _ac_grw),
                   m->active_p_addr, m->active_helper_addr);
        }
    }
    /* timer_p_addr 指向一个已失活的子模块（无私有定时器队列），wrapper
     * 仍有待处理的定时器节点。子模块在下载期间通过 wrapper veneer 调用
     * table[31] 获得 timer 所有权，下载完成后清空了自己的队列但
     * timer_p_addr 未被重置。宿主 tick 被路由到空的子模块 helper，
     * wrapper 的定时器队列永远得不到消费。将 timer 所有权还给 wrapper，
     * 让后续 tick 到达 wrapper 的 code=2 处理，推进定时器队列。 */
    if (code == 2 &&
        m->timer_p_addr &&
        m->timer_p_addr != m->p_addr &&
        m->timer_p_addr != m->primary_p_addr &&
        suspended_foreground_child &&
        !foreground_child_has_private_timer &&
        wrapper_timer_live_post) {
        m->timer_p_addr = m->p_addr;
        m->timer_helper_addr = m->helper_addr;
        if (getenv("VMRP_ARM_EXT_DIAG")) {
            printf("DIAG timer_owner_transfer staleChild->wrapper wrapperTimer=%d activeP=0x%X activeH=0x%X\n",
                   wrapper_timer_live_post,
                   m->active_p_addr, m->active_helper_addr);
        }
    }
    if (code == 2 && !m->host_timer_pending &&
        suspended_foreground_child &&
        (foreground_child_timer_live_post ||
         wrapper_dispatch_busy_post ||
         wrapper_timer_live_post ||
         primary_game_timer_live_post)) {
        /*
         * Wrapper suspend/resume keeps the primary extChunk depth at +0x34
         * while a child module owns the foreground.  Direct child ticks are
         * useful only after the child's disassembled scheduler queue contains
         * runnable nodes; before that, a still-linked wrapper/game timer queue
         * is the generic liveness signal for wrapper/game-side progress.
         * Keep the host tick alive without faking wrapper flags or calling
         * code=5 directly.
         */
        mr_timerStart(50);
        mr_timer_state = 1;
        m->host_timer_pending = 1;
        internal_slot_write(m, m->mr_timer_state_slot, 1);
    }
    if (code == 2 && !m->host_timer_pending &&
        foreground_child_live_post &&
        !foreground_child_timer_live_post &&
        (wrapper_timer_live_post || wrapper_dispatch_busy_post)) {
        /*
         * Private-loader foreground children do not always use the primary
         * extChunk suspend-depth word.  After such a child drains its compact
         * queue, a real wrapper timer queue can still own the transition; keep
         * one host tick alive and route it through wrapper code=2 so the
         * wrapper can finish replacing/resuming the foreground.  The broad
         * wrapper busy bit is only a fallback for older wrappers without a
         * discovered scheduler layout.
         */
        m->timer_p_addr = m->p_addr;
        m->timer_helper_addr = m->helper_addr;
        mr_timerStart(50);
        mr_timer_state = 1;
        m->host_timer_pending = 1;
        internal_slot_write(m, m->mr_timer_state_slot, 1);
    }
    if (code == 2 && !m->host_timer_pending &&
        foreground_child_timer_live_post) {
        /*
         * A proven foreground child timer queue is runnable work by itself.
         * Socket-driven children also use this path while polling network
         * responses, but the error/close transition after an HTTP failure can
         * leave no sockets open while the child still has a cleanup timer node.
         */
        mr_timerStart(50);
        mr_timer_state = 1;
        m->host_timer_pending = 1;
        internal_slot_write(m, m->mr_timer_state_slot, 1);
    }

    /* wrapper 的定时器回调中，前台子模块可能已经完成（如下载器
     * verdload.ext 下载完毕后清空了自己的定时器队列）。此时子模块虽然
     * 仍占据 active_p_addr，但已不再需要前台绘制权——它的所有定时器节点
     * 已耗尽，不再有后续回调会推进其 UI 状态。
     *
     * 真机上 wrapper 的 resume() 会递减 extChunk[0x34] 并归还前台，但
     * 某些 wrapper 路径（如下载完成后直接启动新应用 promote.mrp）不走
     * resume，导致 extChunk[0x34] 一直 > 0。模拟器需要在此检测前台子模块
     * 已失活（无私有定时器），并将前台控制权交还给 wrapper/primary，
     * 使后续 wrapper 的画面输出（或新加载的子模块的画面输出）能被宿主
     * 接受并呈现。
     *
     * 条件：
     *   - code=2 定时器回调
     *   - 存在已暂停的前台子模块
     *   - 子模块无私有定时器队列（compact/frame timer 均空）
     *   - wrapper 已接管定时器（或回调刚路由给 wrapper） */
    if (code == 2 &&
        suspended_foreground_child &&
        !foreground_child_has_private_timer &&
        m->active_p_addr != active_p_addr_pre) {
        /* active_p_addr 在回调中被 table[25] 更新：wrapper 加载了新的
         * 前台子模块（如从 verdload 过渡到 promote），旧子模块的前台
         * 覆盖区域需要清除，让新模块能正常绘制。 */
        arm_ext_clear_foreground_screen_owner(m);
        m->modal_screen_snapshot_valid = 0;
        m->modal_fg_snapshot_valid = 0;
    } else if (code == 2 &&
               suspended_foreground_child &&
               !foreground_child_has_private_timer &&
               (call_p_addr == m->p_addr ||
                m->timer_p_addr == m->p_addr ||
                wrapper_timer_live_post)) {
        /* wrapper 已接管定时器但 active_p_addr 未在本次回调中变化：
         * 旧的前台子模块已失活（定时器队列空），但 wrapper 尚未通过
         * table[25] 加载新模块。清除前台状态让 wrapper 后续的画面
         * 输出能通过宿主的 screen_write/present 接受路径。 */
        arm_ext_clear_foreground_screen_owner(m);
        m->modal_screen_snapshot_valid = 0;
        m->modal_fg_snapshot_valid = 0;
    }

    /* wrapper helper code=2 自己消费 timer delta chain。宿主侧只负责在
     * timer tick 到达时调用 helper，不伪造 wrapper busy 状态或 code=5。 */
    uint32_t modal_suspend_depth_post = modal_suspend_depth_pre;
    if (modal_ext_chunk && arm_ptr(m, modal_ext_chunk + 0x34))
        memcpy(&modal_suspend_depth_post, arm_ptr(m, modal_ext_chunk + 0x34), 4);
    int wrapper_entered_modal =
        code == 1 && has_separate_wrapper && input_len >= 12 && input_addr &&
        modal_suspend_depth_pre == 0 && modal_suspend_depth_post > 0;
    if (modal_suspend_depth_pre == 0 && modal_suspend_depth_post > 0 &&
        call_modal_snapshot_candidate) {
        m->modal_screen_snapshot_valid = 1;
    }
    /* 进入模态（0->1）：标记前台分发区快照有效，供关闭时还原事件路由。 */
    if (modal_suspend_depth_pre == 0 && modal_suspend_depth_post > 0 &&
        m->modal_fg_snapshot) {
        m->modal_fg_snapshot_valid = 1;
    }
    /* 通用模态框关闭处理（suspend depth >0 → 0）。
     * 真机上 wrapper 的 resume(0xE83220) 恢复 game 状态（定时器链表头、
     * pauseApp/resumeApp 回调），模拟器只需做模块级清理：
     *  - 将前台归还给 primary game
     *  - 清除子模块残留的定时器归属
     *  - 恢复 game timer head（wrapper 的 suspend 清零了它）
     *  - 恢复模态前屏幕快照
     *  - 补启宿主定时器让 wrapper 完成 resume 队列 */
    if (modal_suspend_depth_pre > 0 && modal_suspend_depth_post == 0) {
        m->active_helper_addr = m->primary_helper_addr;
        m->active_p_addr = m->primary_p_addr;
        arm_ext_clear_foreground_screen_owner(m);
        /* 还原 wrapper 前台分发区，使事件路由回到下层页面（修复返回后
         * 无法二次进入子模块界面）。 */
        arm_ext_restore_modal_fg_snapshot(m);
        if (foreground_child_active) {
            arm_ext_reset_child_modules(m);
            if (m->timer_p_addr &&
                m->timer_p_addr != m->primary_p_addr &&
                m->timer_p_addr != m->p_addr) {
                m->timer_p_addr = 0;
                m->timer_helper_addr = 0;
            }
        }
        if (m->saved_game_timer_head) {
            uint32_t grw = 0;
            if (m->primary_p_addr && arm_ptr(m, m->primary_p_addr))
                memcpy(&grw, arm_ptr(m, m->primary_p_addr), 4);
            /* wrapper suspend 会临时清空 game timer head；仅当 resume/close
             * 结束后仍为空时恢复旧 head。若 game 在关闭回调里已经安装了
             * 新 timer，覆盖它会丢失后续异步请求处理。 */
            if (grw && read_game_timer_head(m, grw) == 0)
                write_game_timer_head(m, grw, m->saved_game_timer_head);
            m->saved_game_timer_head = 0;
        }
        if (m->modal_screen_snapshot_valid &&
            m->modal_screen_snapshot &&
            m->modal_screen_snapshot_len == m->screen_len &&
            m->screen_addr && arm_ptr(m, m->screen_addr)) {
            memcpy(arm_ptr(m, m->screen_addr),
                   m->modal_screen_snapshot, m->screen_len);
            extern uint16 *mr_screenBuf;
            extern void mr_drawBitmap(uint16 *bmp, int16 x, int16 y,
                                      uint16 w, uint16 h);
            if (mr_screenBuf) {
                int32_t stride = arm_ext_screen_stride(m);
                uint16_t *snapshot =
                    (uint16_t *)m->modal_screen_snapshot;
                for (int32_t y = 0; y < m->screen_h; ++y) {
                    memcpy(mr_screenBuf + (size_t)y * (size_t)m->screen_w,
                           snapshot + (size_t)y * (size_t)stride,
                           (size_t)m->screen_w * sizeof(uint16_t));
                }
                mr_drawBitmap(mr_screenBuf, 0, 0,
                              (uint16)m->screen_w, (uint16)m->screen_h);
            }
            m->modal_screen_snapshot_valid = 0;
        }
        m->primary_child_reopen_timer_needed = 1;
        reopen_set_this_call = 1;
    }

    if (call_ret != MR_SUCCESS) return MR_FAILED;

    if (arm_ext_finish_callback_state(m, modal_ext_chunk)) {
        /* mr_exit() 在存在 old app 时会重启上层并返回；当前 ARM 回调的
         * 控制权已经交给平台层，不能继续读取旧 helper 的 output/R0。 */
        return MR_SUCCESS;
    }
    /* 模态框关闭后的定时器恢复。
     * 真机上 game 的 table[31/32] 经 mrc_extTimerStart/Stop 路由到
     * wrapper 的定时器队列，不影响宿主定时器。模拟器里 game 的
     * table[32] 直接停掉宿主定时器，需要反复补启直到 game 完成
     * 内部状态转换。仅在新模态进入时清除标志。 */
    if (m->primary_child_reopen_timer_needed &&
        !m->host_timer_pending && mr_timer_state == 0) {
        mr_timerStart(100);
        mr_timer_state = 1;
        m->host_timer_pending = 1;
        m->timer_p_addr = m->p_addr;
        m->timer_helper_addr = m->helper_addr;
    }
    if (m->primary_child_reopen_timer_needed && !reopen_set_this_call) {
        /* 新模态进入：child 加载成功并进入前台层 */
        if (modal_suspend_depth_post > 0 && modal_suspend_depth_pre == 0)
            m->primary_child_reopen_timer_needed = 0;
    }

    uint32_t arm_output = 0;
    uint32_t arm_output_len = 0;
    uc_mem_read(m->uc, outp_addr, &arm_output, 4);
    uc_mem_read(m->uc, outl_addr, &arm_output_len, 4);
    if (output_len) *output_len = (int32)arm_output_len;
    if (output) *output = arm_output ? (uint8 *)arm_ptr(m, arm_output) : NULL;
    int32_t call_result = (int32_t)reg_read32(m->uc, UC_ARM_REG_R0);
    if (getenv("VMRP_ARM_EXT_DIAG")) {
        printf("DIAG arm_ext_return code=%d input_len=%u callR0=%d wrapperRaw=%d enteredModal=%d modalPre=%u modalPost=%u activeP=0x%X activeH=0x%X timerP=0x%X timerH=0x%X hostTimer=%d mrTimer=%d\n",
               (int)code, input_len, call_result, wrapper_raw_event_routed,
               wrapper_entered_modal, modal_suspend_depth_pre,
               modal_suspend_depth_post, m->active_p_addr,
               m->active_helper_addr, m->timer_p_addr, m->timer_helper_addr,
               m->host_timer_pending, mr_timer_state);
    }
    arm_ext_dispatch_pending_sms_result(m);

    /* 20-byte 原始事件已经先经过 wrapper；这里向宿主返回 MR_SUCCESS，
     * 避免同一个输入随后又经 Lua 的 12-byte _strCom(801) 转发重复处理。
     * 事件导致 0->1 进入模态时也必须消费；12-byte 内部转发仍返回
     * helper 的真实 R0，供非模态前台模块在清掉可见层后继续把事件交给 game/Lua。 */
    if (wrapper_raw_event_routed || wrapper_entered_modal) return MR_SUCCESS;
    return call_result;
}

uint32 arm_ext_helper_addr(ArmExtModule *m) {
    return m ? m->helper_addr : 0;
}

uint32 arm_ext_primary_helper(ArmExtModule *m) {
    if (!m) return 0;
    uint32_t ret = 0;
    uint32_t ext_chunk = 0, dispatch = 0;
    if (m->timer_p_addr && m->timer_helper_addr &&
        !(m->timer_p_addr == m->p_addr && m->timer_helper_addr == m->helper_addr)) {
        /* gxdzc/netpay 的“请稍后...”路径由 wrapper 自己在 0xE83A69
         * 启动 10ms timer；该 owner 不能屏蔽宿主侧 wrapper timer 路由。
         * 只有 timer 属于后加载子插件时，才跳过 wrapper timer，避免把
         * 独立子插件 timer 当成 wrapper tick。 */
        goto out;
    }
    if (!(m->primary_helper_addr && m->primary_helper_addr != m->helper_addr)) {
        goto out;
    }
    if (m->primary_child_reopen_timer_needed) {
        /*
         * After a modal child closes, the wrapper still owns one or more
         * resume/reopen timer ticks.  The normal helper code=2 path preserves
         * the wrapper's ARM-side argument setup for that transition; direct
         * 0xE83A81 dispatch fabricates only the primary extChunk ABI.  DOTA's
         * browser launch hits this boundary immediately after the download
         * result modal closes, before brw.ext/frame.ext are registered.
         */
        goto out;
    }
    if (arm_ext_has_foreground_child(m)) {
        int child_has_private_timer =
            arm_ext_foreground_child_has_frame_timer_queue(m) ||
            arm_ext_foreground_child_has_compact_timer_queue(m);
        int wrapper_compact_timer_live =
            m->wrapper_compact_timer_scheduler_off &&
            arm_ext_wrapper_has_timer_queue(m);
        uint32_t suspend_depth = 0;
        int active_modal =
            arm_ext_suspend_depth_for_p(m, m->active_p_addr,
                                        &suspend_depth) &&
            suspend_depth > 0;
        uint32_t active_chunk = 0;
        uint32_t active_dispatch = 0;
        if (m->active_p_addr && arm_ptr(m, m->active_p_addr + 12)) {
            memcpy(&active_chunk, arm_ptr(m, m->active_p_addr + 12), 4);
        }
        if (active_chunk && arm_ptr(m, active_chunk + 0x28)) {
            memcpy(&active_dispatch, arm_ptr(m, active_chunk + 0x28), 4);
        }
        uint32_t active_dispatch_addr = active_dispatch & ~1u;
        /*
         * Old direct-dispatch wrappers require the normal helper code=2 path
         * while a non-modal foreground child is active; the synthesized ABI
         * otherwise reaches child helpers with wrapper arguments. Compact
         * wrapper walkers are different: disassembly proves they consume a
         * real R9-relative node queue, and runtime diagnostics show live nodes
         * shaped as cb=wrapperThunk/data=extChunk. Let only that proven compact
         * queue bypass the foreground-child guard so the wrapper can dispatch
         * the node under its own ARM ABI.
         */
        if (!wrapper_compact_timer_live &&
            (child_has_private_timer || !active_modal ||
             !active_dispatch ||
             active_dispatch_addr < EXT_CODE_ADDR ||
             active_dispatch_addr >= EXT_CODE_ADDR + m->code_len)) {
            goto out;
        }
    }
    if (!m->primary_p_addr || !arm_ptr(m, m->primary_p_addr + 12)) {
        goto out;
    }
    memcpy(&ext_chunk, arm_ptr(m, m->primary_p_addr + 12), 4);
    if (ext_chunk && arm_ptr(m, ext_chunk + 0x28)) {
        memcpy(&dispatch, arm_ptr(m, ext_chunk + 0x28), 4);
    }
    uint32_t dispatch_addr = dispatch & ~1u;
    /* 只有指向外层 wrapper 代码段、且已定位到 wrapper timer queue consumer
     * 的 extChunk dispatch 才需要宿主从 arm_ext_call_dispatch() 直接驱动。
     * 另一些 wrapper（gghjt cfunction.ext 反汇编：helper code=2 分支进入
     * 0xE83E6C）把 timer consumer 收在 helper switch 里；这类应走普通
     * code=2 路由，由当前 timer owner 选择 wrapper helper。mpc/game.ext 的
     * extChunk[0x28] 还可指向后续嵌套 EXT 的 code dispatcher，它的 timer
     * 已通过 table[31]/[32] veneer 接入，不能按 wrapper tick 调用。 */
    /* chain_walker_thunk 通过运行时 ARM 代码 patch 集成到 wrapper 的
     * code=2 分支中，不依赖外部时序门控。这是通用的 wrapper timer dispatch
     * 机制修复。仅对有 wrapper_timer_dispatch_addr 的 wrapper 启用 dispatch。 */
    ret = (dispatch && (dispatch & 1u) &&
           dispatch_addr >= EXT_CODE_ADDR &&
           dispatch_addr < EXT_CODE_ADDR + m->code_len &&
           m->wrapper_timer_dispatch_addr)
          ? m->primary_helper_addr : 0;
out:
    if (getenv("VMRP_ARM_EXT_DIAG")) {
        printf("DIAG primary_helper ret=0x%X timerP=0x%X timerH=0x%X wrapperP=0x%X wrapperH=0x%X primaryP=0x%X primaryH=0x%X chunk=0x%X dispatch=0x%X wrapperTimer=0x%X\n",
               ret, m->timer_p_addr, m->timer_helper_addr, m->p_addr, m->helper_addr,
               m->primary_p_addr, m->primary_helper_addr, ext_chunk, dispatch,
               m->wrapper_timer_dispatch_addr);
    }
    return ret;
}

int arm_ext_consume_primary_host_init(ArmExtModule *m) {
    if (!m || !m->primary_host_init_pending) return 0;
    m->primary_host_init_pending = 0;
    return 1;
}

void arm_ext_reset_child_modules(ArmExtModule *m) {
    if (!m) return;
    /* 保留 primary 模块，清除所有其他嵌套模块注册。
     * child 流程关闭后 nested_modules 里残留的子插件条目会干扰
     * hook_restore_r9 的 R9 同步和 arm_ext_p_for_code_addr 的
     * 代码归属判定，导致 wrapper 内部分发走错路径。 */
    int out = 0;
    for (int i = 0; i < m->nested_module_count; ++i) {
        ArmExtNestedModule *mod = &m->nested_modules[i];
        int is_primary = m->primary_file_addr &&
                         mod->file_addr == m->primary_file_addr &&
                         mod->file_len == m->primary_file_len;
        if (is_primary) {
            m->nested_modules[out++] = *mod;
        } else {
            /* 临时诊断:记录 child 模块清除(修复后删除) */
            arm_ext_watch_module_event(m, "reset_drop", mod->file_addr,
                                       mod->file_len, mod->p_addr);
        }
    }
    m->nested_module_count = out;
    m->active_p_addr = m->primary_p_addr;
    m->active_helper_addr = m->primary_helper_addr;
    arm_ext_clear_foreground_screen_owner(m);
}

uint32 arm_ext_read_timer_queue(ArmExtModule *m) {
    if (!m || !m->primary_p_addr) return 0;
    uint32_t rw = 0;
    memcpy(&rw, arm_ptr(m, m->primary_p_addr), 4);
    return read_game_timer_head(m, rw);
}


int arm_ext_call_dispatch(ArmExtModule *m, int is_stop, uint32_t timer_interval) {
    if (!m || !m->p_addr) return MR_FAILED;
    uint32_t nested_p = m->primary_p_addr ? m->primary_p_addr : 0;
    if (!nested_p && m->last_file_addr) {
        void *p4 = arm_ptr(m, m->last_file_addr + 4);
        if (p4) memcpy(&nested_p, p4, 4);
    }
    if (!nested_p || !arm_ptr(m, nested_p + 12)) return MR_FAILED;
    /* 读取 P->mrc_extChunk (offset 12) */
    uint32_t ext_chunk = 0;
    memcpy(&ext_chunk, arm_ptr(m, nested_p + 12), 4);
    if (!ext_chunk || !arm_ptr(m, ext_chunk + 0x28)) return MR_FAILED;
    /* 读取目标函数 extChunk[0x28] 和参数 extChunk[0x24] */
    uint32_t func = 0, param = 0;
    memcpy(&func, arm_ptr(m, ext_chunk + 0x28), 4);
    memcpy(&param, arm_ptr(m, ext_chunk + 0x24), 4);
    int wrapper_timer_owner =
        m->timer_p_addr == m->p_addr &&
        m->timer_helper_addr == m->helper_addr &&
        m->wrapper_timer_dispatch_addr != 0;
    int compact_wrapper_timer_owner =
        wrapper_timer_owner && m->wrapper_compact_timer_scheduler_off != 0;
    /* 19KB cfunction.ext 的 chain walker thunk：wrapper 不使用宿主直接
     * 驱动（wrapper_timer_dispatch_addr==0），但有 chain_walker_thunk_addr。
     * 宿主 timer 每次触发时让 chain walker 遍历 timer chain，通过
     * 0xE83590 在 wrapper 栈上下文中安全调用 game helper(code=5)。 */
    int chain_walker_owner = !wrapper_timer_owner && m->chain_walker_thunk_addr != 0;
    if (wrapper_timer_owner) {
        /*
         * wrapper 自己的 table[31] timerStart 来自 timer queue 管理函数
         * 0xE83A34，下一次宿主 timer 必须进入 0xE83A80 的队列消费逻辑；
         * extChunk[0x28] 是事件/控件分发入口 0xE830BD，会把节点重新挂回
         * 队列而不是执行到 0xE83B46 的回调。验证：gxdzc 付费点击后
         * 0x66FFBD 回调被消费并显示 netpay 付费界面。
         */
        func = m->wrapper_timer_dispatch_addr;
    } else if (chain_walker_owner) {
        func = m->chain_walker_thunk_addr;
    }
    if (getenv("VMRP_ARM_EXT_DIAG")) {
        printf("DIAG call_dispatch nestedP=0x%X chunk=0x%X func=0x%X param=0x%X wrapperOwner=%d timerP=0x%X timerH=0x%X\n",
               nested_p, ext_chunk, func, param, wrapper_timer_owner,
               m->timer_p_addr, m->timer_helper_addr);
    }
    if (!func) return MR_FAILED;
    /* 以 wrapper 的 R9 调用目标函数:
     * R0=extChunk, R1=is_stop, R2=param, R3=timer_interval */
    uint32_t wrapper_rw = 0;
    memcpy(&wrapper_rw, arm_ptr(m, m->p_addr), 4);
    /* wrapper 的 RW 段基址由 ARM ext 在 mr_c_function_load 期间设置。
     * 部分 wrapper（如 mrpinfo 的 cfunction.ext）在 Unicorn 环境下
     * 不初始化 R9，导致 P[0]=0。此时 dispatch 无法正确访问 wrapper 数据，
     * 跳过即可——primary ext 的定时器仍可通过 arm_ext_call(code=2) 驱动。 */
    if (!wrapper_rw) return MR_FAILED;

    reg_write32(m->uc, UC_ARM_REG_R9, wrapper_rw);
    reg_write32(m->uc, UC_ARM_REG_R0, ext_chunk);
    reg_write32(m->uc, UC_ARM_REG_R1, (uint32_t)is_stop);
    reg_write32(m->uc, UC_ARM_REG_R2, param);
    reg_write32(m->uc, UC_ARM_REG_R3, timer_interval);
    /* dispatch 的栈参数全为0 */
    uint32_t sp = EXT_STACK_ADDR + EXT_STACK_SIZE - 32;
    uint32_t stack_args[8] = {0};
    uc_mem_write(m->uc, sp, stack_args, sizeof(stack_args));
    reg_write32(m->uc, UC_ARM_REG_SP, sp);

    /* 保存 dispatch 前的 game timer head，用于模态框取消时恢复 */
    uint32_t _game_rw = 0, _s8_pre = 0;
    if (m->primary_p_addr && arm_ptr(m, m->primary_p_addr))
        memcpy(&_game_rw, arm_ptr(m, m->primary_p_addr), 4);
    if (_game_rw)
        _s8_pre = read_game_timer_head(m, _game_rw);
    uint32_t suspend_depth_pre = 0;
    if (arm_ptr(m, ext_chunk + 0x34))
        memcpy(&suspend_depth_pre, arm_ptr(m, ext_chunk + 0x34), 4);
    int modal_snapshot_candidate = 0;
    /* cfunction.ext 的 suspend(0xE831A4) 在同一次 dispatch 内把
     * extChunk[0x34] 从 0 加到 1 并绘制模态层。这里只在尚未 suspend
     * 时抓取 framebuffer，dispatch 后确认 0->1 再标记为可用于 cancel 恢复。 */
    if (suspend_depth_pre == 0) {
        if (_s8_pre != 0)
            modal_snapshot_candidate = arm_ext_save_modal_screen_snapshot(m);
        /* 同步快照 wrapper 前台分发区（timer 内进入模态子模块的路径）。 */
        arm_ext_save_modal_fg_snapshot(m);
    }

    uint16 *saved_screenBuf = NULL;
    uint32_t present_depth_before = 0;
    enter_screen_context(m, &saved_screenBuf, &present_depth_before);
    sync_internal_state_to_arm(m);
    if (compact_wrapper_timer_owner) {
        arm_ext_diag_dump_wrapper_compact_timer_nodes(m, "dispatch_pre");
    }
    /* 19KB cfunction.ext 的 timer dispatcher 是 chain walker；它从
     * wrapper_rw+0x190 取节点并调用节点回调 0xE83590。该回调会读取
     * extChunk[8] 调 game helper(code=5)，所以这里只修复可能被 wrapper
     * 清零的 helper 槽，不篡改 suspend depth。 */
    uint32_t depth_patched = 0;
    if ((wrapper_timer_owner || chain_walker_owner) && suspend_depth_pre == 0 &&
        arm_ptr(m, ext_chunk + 0x34)) {
        /* 确保 extChunk[8] = game helper (wrapper 代码可能已清零) */
        if (m->primary_helper_addr && arm_ptr(m, ext_chunk + 8)) {
            memcpy(arm_ptr(m, ext_chunk + 8), &m->primary_helper_addr, 4);
        }
        /*
         * Chain walkers and compact wrapper queues both dispatch through node
         * data. The compact callback at 0xE83BD0 loads helper from data+8
         * before blx, so repair every registered extChunk helper slot instead
         * of only the primary module.
         */
        if (chain_walker_owner || compact_wrapper_timer_owner) {
            for (int ci = 0; ci < m->nested_module_count; ++ci) {
                ArmExtNestedModule *mod = &m->nested_modules[ci];
                if (!mod->p_addr || !mod->helper_addr) continue;
                uint32_t child_chunk = 0;
                if (arm_ptr(m, mod->p_addr + 12))
                    memcpy(&child_chunk, arm_ptr(m, mod->p_addr + 12), 4);
                if (child_chunk && arm_ptr(m, child_chunk + 8)) {
                    uint32_t cur = 0;
                    memcpy(&cur, arm_ptr(m, child_chunk + 8), 4);
                    if (!cur) {
                        memcpy(arm_ptr(m, child_chunk + 8), &mod->helper_addr, 4);
                    }
                }
            }
        }
    }
    m->current_p_addr = m->p_addr;
    m->current_helper_addr = m->helper_addr;
    int prev_in_dispatch = m->in_dispatch;
    m->in_dispatch = 1;
    int ret = run_arm_with_sp(m, func, sp);
    m->in_dispatch = prev_in_dispatch;
    m->current_p_addr = 0;
    m->current_helper_addr = 0;
    if (compact_wrapper_timer_owner) {
        arm_ext_diag_dump_wrapper_compact_timer_nodes(m, "dispatch_post");
    }
    /* 旧版 queue consumer 兼容路径可能会临时修改 suspend depth；
     * 当前 19KB chain walker 不需要该 patch，正常保持 no-op。 */
    if (depth_patched && arm_ptr(m, ext_chunk + 0x34)) {
        memcpy(arm_ptr(m, ext_chunk + 0x34), &suspend_depth_pre, 4);
    }
    sync_timer_state_from_arm(m);
    leave_screen_context(m, saved_screenBuf, present_depth_before);
    capture_timer_dispatches(m);
    if (wrapper_timer_owner) {
        int queue_live = 0;
        uint32_t queue_base_ms = 0;
        uint32_t q = 0;
        if (m->wrapper_compact_timer_scheduler_off) {
            /*
             * The wrapper walker scan proved the scheduler offset.  Keep the
             * host timer alive only when that real compact queue still has
             * nodes; unrelated wrapper state must not pin ownership here.
             */
            q = wrapper_rw + m->wrapper_compact_timer_scheduler_off;
            queue_live = arm_ext_wrapper_has_timer_queue(m);
            if (arm_ptr(m, q + 20))
                memcpy(&queue_base_ms, arm_ptr(m, q + 20), 4);
        } else {
            uint32_t queue_head = 0;
            q = wrapper_rw + 0x1FCu;
            if (arm_ptr(m, q + 20)) {
                memcpy(&queue_head, arm_ptr(m, q + 12), 4);
                memcpy(&queue_base_ms, arm_ptr(m, q + 20), 4);
            }
            /* 19KB wrapper timer walker 使用 wrapper_rw+0x190 的桶数组；旧
             * 0x274 位置只是早期误判的诊断偏移，不能用来决定宿主 timer 生死。 */
            uint32_t active_index = arm_ext_read_u32_or_zero_(m, wrapper_rw + 0x18u);
            uint32_t tc_after = arm_ext_read_u32_or_zero_(m, wrapper_rw + 0x190u + active_index * 8u);
            queue_live = queue_head || tc_after;
        }
        if (queue_live) {
            mr_timer_state = 1;
            m->host_timer_pending = 1;
            internal_slot_write(m, m->mr_timer_state_slot, 1);
            /* ARM 侧可能未调用 table[31] timerStart 重新注册 SDL 定时器，
             * 需要宿主主动启动一次以保证下一个 tick 到达 */
            mr_timerStart((uint16)timer_interval);
            /*
             * 0xE83A80 可能只消费到期节点并留下后续节点；若 ARM 侧没有
             * 再次调用 table[31]，宿主仍要保持 timer running，下一 tick
             * 才能继续推进 wrapper 队列。验证：gxdzc pay.sh 不再停在
             * 标题/“请稍后”，同时 gghjt pay-normal-back.sh 仍能返回。
             */
            if (q && queue_base_ms == 0 && arm_ptr(m, q + 20)) {
                uint32_t now = mr_getTime();
                memcpy(arm_ptr(m, q + 20), &now, 4);
            }
        }
    }
    /* chain walker owner: 保持宿主 timer 运行并恢复 suspend depth */
    if (chain_walker_owner) {
        uint32_t active_index = arm_ext_read_u32_or_zero_(m, wrapper_rw + 0x18u);
        uint32_t tc_after = arm_ext_read_u32_or_zero_(m, wrapper_rw + 0x190u + active_index * 8u);
        if (tc_after) {
            mr_timer_state = 1;
            m->host_timer_pending = 1;
            internal_slot_write(m, m->mr_timer_state_slot, 1);
            mr_timerStart((uint16)timer_interval);
        }
        /* 0xE83590 递减了 extChunk[0x34]，恢复所有嵌套模块的 suspend depth */
        if (arm_ptr(m, ext_chunk + 0x34))
            memcpy(arm_ptr(m, ext_chunk + 0x34), &suspend_depth_pre, 4);
        for (int ci = 0; ci < m->nested_module_count; ++ci) {
            ArmExtNestedModule *mod = &m->nested_modules[ci];
            if (!mod->p_addr) continue;
            uint32_t child_chunk = 0;
            if (arm_ptr(m, mod->p_addr + 12))
                memcpy(&child_chunk, arm_ptr(m, mod->p_addr + 12), 4);
            if (child_chunk && child_chunk != ext_chunk &&
                arm_ptr(m, child_chunk + 0x34)) {
                uint32_t zero = 0;
                memcpy(arm_ptr(m, child_chunk + 0x34), &zero, 4);
            }
        }
    }

    /* wrapper dispatch 可能清零 game timer head（模态框进入时），保存旧值 */
    /* dispatch 后重新读 game_rw（ARM 代码可能修改了 primary_p_addr[0]） */
    {
        uint32_t _grw2 = 0;
        if (m->primary_p_addr && arm_ptr(m, m->primary_p_addr))
            memcpy(&_grw2, arm_ptr(m, m->primary_p_addr), 4);
        uint32_t _s8_post = read_game_timer_head(m, _grw2);
        if (_s8_post == 0 && _s8_pre != 0) {
            m->saved_game_timer_head = _s8_pre;
        }
        uint32_t suspend_depth_post = 0;
        if (arm_ptr(m, ext_chunk + 0x34))
            memcpy(&suspend_depth_post, arm_ptr(m, ext_chunk + 0x34), 4);
        if (suspend_depth_pre == 0 && suspend_depth_post > 0 && modal_snapshot_candidate) {
            m->modal_screen_snapshot_valid = 1;
        }
        if (suspend_depth_pre == 0 && suspend_depth_post > 0 && m->modal_fg_snapshot) {
            m->modal_fg_snapshot_valid = 1;
        }
        /* 通用模态框关闭：与 arm_ext_call 中相同的通用清理 */
        if (suspend_depth_pre > 0 && suspend_depth_post == 0) {
            m->active_helper_addr = m->primary_helper_addr;
            m->active_p_addr = m->primary_p_addr;
            arm_ext_clear_foreground_screen_owner(m);
            /* 还原 wrapper 前台分发区，使事件路由回到下层页面。 */
            arm_ext_restore_modal_fg_snapshot(m);
            arm_ext_reset_child_modules(m);
            if (m->saved_game_timer_head) {
                /* 与 arm_ext_call 的模态关闭路径保持一致：只填补仍为空的
                 * game timer head，不覆盖关闭回调中新挂上的 timer。 */
                if (_grw2 && read_game_timer_head(m, _grw2) == 0)
                    write_game_timer_head(m, _grw2, m->saved_game_timer_head);
                m->saved_game_timer_head = 0;
            }
            if (m->modal_screen_snapshot_valid &&
                m->modal_screen_snapshot &&
                m->modal_screen_snapshot_len == m->screen_len &&
                m->screen_addr && arm_ptr(m, m->screen_addr)) {
                memcpy(arm_ptr(m, m->screen_addr),
                       m->modal_screen_snapshot, m->screen_len);
                extern uint16 *mr_screenBuf;
                extern void mr_drawBitmap(uint16 *bmp, int16 x, int16 y,
                                          uint16 w, uint16 h);
                if (mr_screenBuf) {
                    int32_t stride = arm_ext_screen_stride(m);
                    uint16_t *snapshot =
                        (uint16_t *)m->modal_screen_snapshot;
                    for (int32_t y = 0; y < m->screen_h; ++y) {
                        memcpy(mr_screenBuf + (size_t)y * (size_t)m->screen_w,
                               snapshot + (size_t)y * (size_t)stride,
                               (size_t)m->screen_w * sizeof(uint16_t));
                    }
                    mr_drawBitmap(mr_screenBuf, 0, 0,
                                  (uint16)m->screen_w, (uint16)m->screen_h);
                }
                m->modal_screen_snapshot_valid = 0;
            }
            m->primary_child_reopen_timer_needed = 1;
        }
    }

    if (arm_ext_finish_callback_state(m, ext_chunk)) {
        /* dispatch 已经触发平台退出/重启语义；返回成功让外层停止继续解释
         * 本次 wrapper timer 的旧返回值。 */
        return MR_SUCCESS;
    }
    arm_ext_dispatch_pending_sms_result(m);
    return ret;
}

int arm_ext_invoke0(ArmExtModule *m, uint32 func, int32 *ret_out) {
    if (ret_out) *ret_out = MR_FAILED;
    if (!m || !func) return MR_FAILED;

    m->busy_wait_count = 0;
    m->busy_wait_start_ms = 0;
    restore_ext_r9(m);
    reg_write32(m->uc, UC_ARM_REG_R0, 0);
    reg_write32(m->uc, UC_ARM_REG_R1, 0);
    reg_write32(m->uc, UC_ARM_REG_R2, 0);
    reg_write32(m->uc, UC_ARM_REG_R3, 0);

    uint16 *saved_screenBuf = NULL;
    uint32_t present_depth_before = 0;
    enter_screen_context(m, &saved_screenBuf, &present_depth_before);
    sync_internal_state_to_arm(m);
    m->current_p_addr = m->active_p_addr ? m->active_p_addr : m->p_addr;
    m->current_helper_addr = func;
    int prev_in_dispatch = m->in_dispatch;
    m->in_dispatch = 1;
    int call_ret = run_arm(m, func);
    m->in_dispatch = prev_in_dispatch;
    m->current_p_addr = 0;
    m->current_helper_addr = 0;
    sync_timer_state_from_arm(m);
    leave_screen_context(m, saved_screenBuf, present_depth_before);
    if (call_ret != MR_SUCCESS) return MR_FAILED;

    if (ret_out) *ret_out = (int32)reg_read32(m->uc, UC_ARM_REG_R0);
    arm_ext_dispatch_pending_sms_result(m);
    return MR_SUCCESS;
}

int arm_ext_invoke3(ArmExtModule *m, uint32 func, uint32 arg0, uint32 arg1,
                    uint32 arg2, int32 *ret_out) {
    if (ret_out) *ret_out = MR_FAILED;
    if (!m || !func) return MR_FAILED;

    m->busy_wait_count = 0;
    m->busy_wait_start_ms = 0;
    restore_ext_r9(m);
    reg_write32(m->uc, UC_ARM_REG_R0, arg0);
    reg_write32(m->uc, UC_ARM_REG_R1, arg1);
    reg_write32(m->uc, UC_ARM_REG_R2, arg2);

    uint16 *saved_screenBuf = NULL;
    uint32_t present_depth_before = 0;
    enter_screen_context(m, &saved_screenBuf, &present_depth_before);
    sync_internal_state_to_arm(m);
    m->current_p_addr = m->active_p_addr ? m->active_p_addr : m->p_addr;
    m->current_helper_addr = func;
    int prev_in_dispatch = m->in_dispatch;
    m->in_dispatch = 1;
    int call_ret = run_arm(m, func);
    m->in_dispatch = prev_in_dispatch;
    m->current_p_addr = 0;
    m->current_helper_addr = 0;
    sync_timer_state_from_arm(m);
    leave_screen_context(m, saved_screenBuf, present_depth_before);
    if (call_ret != MR_SUCCESS) return MR_FAILED;

    if (ret_out) *ret_out = (int32)reg_read32(m->uc, UC_ARM_REG_R0);
    arm_ext_dispatch_pending_sms_result(m);
    return MR_SUCCESS;
}

void arm_ext_host_cache_sync(ArmExtModule *m, const void *host_data, uint32 len) {
    if (!m || !host_data || !len) return;
    if (!m->pending_internal_file_addr || !m->pending_internal_file_len) return;
    uint32_t addr = m->pending_internal_file_addr;
    uint32_t flen = m->pending_internal_file_len;
    uint32_t copy_len = len < flen ? len : flen;
    void *dst = arm_ptr(m, addr);
    if (!dst) return;
    memcpy(dst, host_data, copy_len);
    uc_ctl_remove_cache(m->uc, addr, addr + copy_len);
    arm_ext_sync_internal_nested_module(m, addr, flen);
}

void arm_ext_unload(ArmExtModule *m) {
    if (!m) return;
    if (m->profile && m->profile->cleanup)
        m->profile->cleanup(m->app_state);
    m->app_state = NULL;
    mrp_cache_free(m);
    if (m->uc) uc_close(m->uc);
    free(m->last_file_copy);
    free(m->modal_screen_snapshot);
    free(m->modal_fg_snapshot);
    free(m->short_pack_aliases);
    free(m->bump_live);
    free(m->bump_free_blocks);
    arm_ext_free_row_spans(&m->screen_damage);
    arm_ext_free_row_spans(&m->screen_present);
    arm_ext_free_row_spans(&m->foreground_cover);
    free(m->low_table);
    free(m->platform_mem);
    free(m->platform_io_mem);
    free(m->platform_alt_mem);
    free(m->executor_meta_mem);
    if (m->mem_is_mmap) {
#ifdef _MSC_VER
        VirtualFree(m->mem, 0, MEM_RELEASE);
#else
        munmap(m->mem, EXT_MEM_SIZE);
#endif
    } else {
        free(m->mem);
    }
    free(m);
}
