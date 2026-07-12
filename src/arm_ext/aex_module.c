/*
 * aex_module.c —— 嵌套模块注册与生命周期、包归属、私有 loader 桥(Phase 2 收尾)。
 *
 * MRP payload 匹配、pack 名/alias、nested 模块注册/查找/retire/drop/restore、
 * 内部 loader tuple 与 sync、私有 child RW 桥修复、find_wrapper_ 与
 * child_has_ 系列模式扫描(治理分级见 docs/arm-ext-fingerprint-classification.md)。
 * 函数体原样迁移。
 */
#include "../include/arm_ext_priv.h"
#include "../include/fileLib.h"

#include <unistd.h>
#include <zlib.h>

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

void arm_ext_init_pack_names(ArmExtModule *m) {
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

const char *arm_ext_pack_to_host_path(ArmExtModule *m, const char *pack) {
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


static int arm_ext_current_pack_matches_staged_file(ArmExtModule *m,
                                                    uint32_t file_addr,
                                                    uint32_t file_len,
                                                    const char **pack_out);


static inline void app_on_child_confirmed(ArmExtModule *m, uint32_t p, uint32_t h) {
    if (m && m->profile && m->profile->on_child_confirmed)
        m->profile->on_child_confirmed(m, m->app_state, p, h);
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

int arm_ext_nested_exec_range_for_lr(ArmExtModule *m, uint32_t lr,
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

ArmExtNestedModule *arm_ext_find_nested_module(ArmExtModule *m, uint32_t addr) {
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

ArmExtNestedModule *arm_ext_find_nested_module_by_p(ArmExtModule *m,
                                                           uint32_t p_addr) {
    if (!m || !p_addr) return NULL;
    for (int i = m->nested_module_count - 1; i >= 0; --i) {
        ArmExtNestedModule *mod = &m->nested_modules[i];
        if (mod->p_addr == p_addr) return mod;
    }
    return NULL;
}

ArmExtNestedModule *arm_ext_find_nested_module_by_rw(ArmExtModule *m,
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
    /* P5.2:正向结果缓存。键 = staged 内容 FNV + 包路径/长度/包头 FNV;
     * 包被重装(下载流程)时长度或包头必变,staged 变化时 FNV 变——
     * 不会拿旧结论冒充。未命中才走整包读入+逐条目解压比对的慢路径。 */
    uint32_t staged_fnv = arm_ext_fnv1a(staged, file_len);
    uint8_t pack_head[64];
    uint32_t pack_head_fnv = 0;
    {
        int32_t hfd = mr_open(host_pack, MR_FILE_RDONLY);
        if (hfd == 0) return 0;
        int32_t hgot = mr_read(hfd, pack_head, sizeof(pack_head));
        mr_close(hfd);
        if (hgot <= 0) return 0;
        pack_head_fnv = arm_ext_fnv1a(pack_head, (uint32_t)hgot);
    }
    for (uint32_t ci = 0; ci < ARM_EXT_PACK_MATCH_CACHE_MAX; ++ci) {
        ArmExtPackMatchCache *e = &m->pack_match_cache[ci];
        if (e->file_len == file_len && e->file_addr == file_addr &&
            e->staged_fnv == staged_fnv && e->pack_len == (uint32_t)fl &&
            e->pack_head_fnv == pack_head_fnv &&
            mrp_path_equal(e->pack, host_pack)) {
            if (pack_out) *pack_out = host_pack;
            return 1;
        }
    }
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
    /* 记入正向缓存(环形覆盖) */
    {
        ArmExtPackMatchCache *e =
            &m->pack_match_cache[m->pack_match_cache_next %
                                 ARM_EXT_PACK_MATCH_CACHE_MAX];
        m->pack_match_cache_next++;
        e->file_addr = file_addr;
        e->file_len = file_len;
        e->staged_fnv = staged_fnv;
        e->pack_len = (uint32_t)fl;
        e->pack_head_fnv = pack_head_fnv;
        snprintf(e->pack, sizeof(e->pack), "%s", host_pack);
    }
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

void arm_ext_record_nested_module(ArmExtModule *m, uint32_t file_addr,
                                         uint32_t file_len, uint32_t p_addr,
                                         uint32_t helper_addr) {
    if (!m || !file_addr || !file_len || !p_addr || !helper_addr) return;
    uint32_t package_ram_addr = 0;
    uint32_t package_ram_len = 0;
    const char *package_host_path = arm_ext_child_package_context(
        m, file_addr, file_len, &package_ram_addr, &package_ram_len);
    uint32_t compact_timer_scheduler_off = 0;
    uint32_t compact_timer_walker_file_off = 0;
    int has_compact_timer_walker = 0;
    const uint8_t *code = (const uint8_t *)arm_ptr(m, file_addr);
    if (code) {
        has_compact_timer_walker = arm_ext_find_compact_timer_scheduler(
            code, file_len, &compact_timer_scheduler_off,
            &compact_timer_walker_file_off);
    }

    for (int i = 0; i < m->nested_module_count; ++i) {
        ArmExtNestedModule *mod = &m->nested_modules[i];
        if (mod->file_addr == file_addr && mod->file_len == file_len) {
            mod->p_addr = p_addr;
            mod->helper_addr = helper_addr;
            mod->compact_timer_scheduler_off = compact_timer_scheduler_off;
            mod->compact_timer_walker_addr = has_compact_timer_walker
                ? file_addr + compact_timer_walker_file_off + 1u : 0;
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
        /* B6:注册表满意味着该模块此后在 PC→owner 归属查找中不可见,事件会
         * 派发给错误 owner——这是功能已损坏的状态,必须无条件告警而不是只在
         * TRACE 下打印 */
        printf("arm_ext_executor: nested module registry full, file=0x%X len=%u (events may route to wrong owner)\n",
               file_addr, file_len);
        return;
    }

    ArmExtNestedModule *slot = &m->nested_modules[m->nested_module_count++];
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
    slot->compact_timer_scheduler_off = compact_timer_scheduler_off;
    slot->compact_timer_walker_addr = has_compact_timer_walker
        ? file_addr + compact_timer_walker_file_off + 1u : 0;
    arm_ext_protect_registered_module_storage(m, file_addr, file_len);
    // 新嵌套模块注册后安装 PC 观察点(VMRP_ARM_EXT_WATCH_PC,诊断用)
    arm_ext_install_pc_watches(m, file_addr, file_len);
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

void arm_ext_repair_private_child_record_bridges(ArmExtModule *m,
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

uint32_t arm_ext_read_file_table_context(ArmExtModule *m,
                                         ArmExtNestedModule *owner) {
    if (!owner) return EXT_TABLE_ADDR;
    if (!m || !arm_ptr_span(m, owner->file_addr, 4u)) return 0;

    uint32_t record = arm_ext_read_u32_or_zero_(m, owner->file_addr);
    /*
     * Private loaders store the child's module-record pointer in file_base[0].
     * That record is the table context used by the child's BLX calls, so all
     * package slots through table[105] must be present as one validated span.
     * A malformed nested record is an ABI error; do not read the outer table.
     */
    return record && arm_ptr_span(m, record, 106u * 4u)
               ? record
               : 0;
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

int arm_ext_should_skip_got_snapshot_restore(ArmExtModule *m,
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
    if (arm_ext_diag_on()) {
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

int arm_ext_find_compact_timer_scheduler(const uint8_t *code,
                                         uint32_t file_len,
                                         uint32_t *scheduler_off,
                                         uint32_t *walker_file_off) {
    if (scheduler_off) *scheduler_off = 0;
    if (walker_file_off) *walker_file_off = 0;
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
        if (match) {
            uint32_t off_value = 0;
            /* The second literal is added to R9 immediately before the walker
             * reads queued/current/last-tick at +8/+12/+16.  Decode it instead
             * of assuming one SDK's fixed RW layout. */
            if (arm_ext_thumb_ldr_literal_u32(code, file_len, off + 16u,
                                              &off_value) &&
                off_value && (off_value & 3u) == 0) {
                if (scheduler_off) *scheduler_off = off_value;
                /* The matched instruction is the Thumb function entry, not
                 * merely a signature near it, so callers can run the walker
                 * with the owning module's R9 when helper routing is indirect. */
                if (walker_file_off) *walker_file_off = off;
                return 1;
            }
        }
    }
    return 0;
}

int arm_ext_child_has_compact_timer_walker(const uint8_t *code,
                                           uint32_t file_len) {
    return arm_ext_find_compact_timer_scheduler(code, file_len, NULL, NULL);
}

uint32_t find_wrapper_compact_heap_free_return(const uint8_t *code,
                                                      uint32_t len,
                                                      uint32_t *ctrl_off_out) {
    if (ctrl_off_out) *ctrl_off_out = 0;
    if (!code || len < 0x90u) return 0;

    for (uint32_t off = 0; off + 0x90u <= len; off += 2u) {
        uint16_t ldr = arm_ext_code_u16(code, off);
        uint32_t ctrl_off = 0;
        if ((ldr & 0xF800u) != 0x4800u || ((ldr >> 8) & 0x7u) != 2u)
            continue;
        if (!arm_ext_thumb_ldr_literal_u32(code, len, off, &ctrl_off))
            continue;
        if (ctrl_off < 0x80u || ctrl_off >= 0x1000u ||
            (ctrl_off & 3u) != 0) {
            continue;
        }

        /*
         * Compact SDK mr_free() starts by loading the heap-control pointer
         * from [R9+literal], then walks a {next,len} free-list rooted at
         * ctrl+0x18.  The final block updates ctrl+0x0c before returning;
         * a hook on the pop instruction lets the executor remove live timer
         * nodes from the free-list after the allocator's own merge is done.
         */
        if (arm_ext_code_u16(code, off + 0x02u) != 0xB4F0u ||
            arm_ext_code_u16(code, off + 0x04u) != 0x444Au ||
            arm_ext_code_u16(code, off + 0x06u) != 0x6813u ||
            arm_ext_code_u16(code, off + 0x08u) != 0x1C02u ||
            arm_ext_code_u16(code, off + 0x0Au) != 0x3107u ||
            arm_ext_code_u16(code, off + 0x0Cu) != 0x08CCu ||
            arm_ext_code_u16(code, off + 0x0Eu) != 0x00E4u ||
            arm_ext_code_u16(code, off + 0x16u) != 0x689Du ||
            arm_ext_code_u16(code, off + 0x1Cu) != 0x691Eu ||
            arm_ext_code_u16(code, off + 0x2Eu) != 0x3118u ||
            arm_ext_code_u16(code, off + 0x30u) != 0x6998u ||
            arm_ext_code_u16(code, off + 0x64u) != 0x600Du ||
            arm_ext_code_u16(code, off + 0x6Cu) != 0xC114u ||
            arm_ext_code_u16(code, off + 0x86u) != 0x68D8u ||
            arm_ext_code_u16(code, off + 0x88u) != 0x1900u ||
            arm_ext_code_u16(code, off + 0x8Au) != 0x60D8u ||
            arm_ext_code_u16(code, off + 0x8Cu) != 0xBCF0u ||
            arm_ext_code_u16(code, off + 0x8Eu) != 0x4770u) {
            continue;
        }

        if (ctrl_off_out) *ctrl_off_out = ctrl_off;
        return EXT_CODE_ADDR + off + 0x8Cu;
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

void arm_ext_apply_short_pack_alias_for_private_child(ArmExtModule *m,
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
                if (arm_ext_diag_on()) {
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
    memcpy(&init_func, arm_ptr(m, ext_chunk + AEX_CHUNK_INIT_OFF), 4);
    memcpy(&helper_addr, arm_ptr(m, ext_chunk + AEX_CHUNK_HELPER_OFF), 4);
    memcpy(&chunk_file, arm_ptr(m, ext_chunk + AEX_CHUNK_FILE_BASE_OFF), 4);
    memcpy(&chunk_len, arm_ptr(m, ext_chunk + AEX_CHUNK_FILE_LEN_OFF), 4);
    memcpy(&chunk_rw, arm_ptr(m, ext_chunk + AEX_CHUNK_RW_BASE_OFF), 4);
    memcpy(&chunk_rw_len, arm_ptr(m, ext_chunk + AEX_CHUNK_RW_LEN_OFF), 4);
    memcpy(&p_addr, arm_ptr(m, ext_chunk + AEX_CHUNK_P_ADDR_OFF), 4);
    memcpy(&p_len, arm_ptr(m, ext_chunk + AEX_CHUNK_P_LEN_OFF), 4);
    memcpy(&chunk_record, arm_ptr(m, ext_chunk + AEX_CHUNK_RECORD_OFF), 4);
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
    memcpy(&p_ext_chunk, arm_ptr(m, p_addr + AEX_P_EXT_CHUNK_OFF), 4);
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
        memcpy(&rw_len, arm_ptr(m, p_addr + AEX_P_ER_RW_LEN_OFF), 4);
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

int arm_ext_has_internal_loader_chunk(ArmExtModule *m,
                                             uint32_t file_addr,
                                             uint32_t file_len) {
    if (!m || !file_addr || !file_len) return 0;

    for (uint32_t ext_chunk = EXT_HEAP_ADDR;
         ext_chunk + AEX_CHUNK_HEAP_TOP_OFF <= m->heap_top;
         ext_chunk += 4) {
        if (arm_ext_read_internal_loader_tuple(m, ext_chunk,
                                               file_addr, file_len, 0, NULL)) {
            return 1;
        }
    }

    return 0;
}

int arm_ext_sync_internal_nested_module(ArmExtModule *m,
                                               uint32_t file_addr,
                                               uint32_t file_len) {
    if (!m || !file_addr || !file_len) return 0;

    for (uint32_t ext_chunk = EXT_HEAP_ADDR;
         ext_chunk + AEX_CHUNK_HEAP_TOP_OFF <= m->heap_top;
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
        if (arm_ext_diag_on()) {
            uint32_t ext_type = 0, is_pause = 0;
            uint32_t p_rw = 0, p_rw_len = 0;
            uint32_t chunk_rw = 0, chunk_rw_len = 0;
            memcpy(&ext_type, arm_ptr(m, p_addr + AEX_P_EXT_TYPE_OFF), 4);
            memcpy(&is_pause, arm_ptr(m, ext_chunk + AEX_CHUNK_SUSPEND_OFF), 4);
            memcpy(&p_rw, arm_ptr(m, p_addr), 4);
            memcpy(&p_rw_len, arm_ptr(m, p_addr + AEX_P_ER_RW_LEN_OFF), 4);
            memcpy(&chunk_rw, arm_ptr(m, ext_chunk + AEX_CHUNK_RW_BASE_OFF), 4);
            memcpy(&chunk_rw_len, arm_ptr(m, ext_chunk + AEX_CHUNK_RW_LEN_OFF), 4);
            printf("DIAG synced file=0x%X len=%u chunk=0x%X P=0x%X H=0x%X p_rw=0x%X p_rw_len=%u chunk_rw=0x%X chunk_rw_len=%u ext_type=%u pause=%u activeP=0x%X activeH=0x%X primaryP=0x%X primaryH=0x%X timerP=0x%X timerH=0x%X\n",
                   file_addr, file_len, ext_chunk, p_addr, helper_addr,
                   p_rw, p_rw_len, chunk_rw, chunk_rw_len, ext_type, is_pause,
                   m->active_p_addr, m->active_helper_addr, m->primary_p_addr,
                   m->primary_helper_addr, m->timer_p_addr, m->timer_helper_addr);
            arm_ext_diag_dump_rw_timer_state(m, "sync-child", rw_base);
        }
        if (arm_ext_trace_on()) {
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

int arm_ext_range_contains(uint32_t outer, uint32_t outer_len,
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

int arm_ext_find_first_registered_code_overlap(ArmExtModule *m,
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

int arm_ext_first_unprotected_subrange(ArmExtModule *m,
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


int arm_ext_mirror_read_file_to_adjacent_slot(ArmExtModule *m,
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

void arm_ext_drop_overlapping_stale_nested_modules(ArmExtModule *m,
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
            if (arm_ext_diag_on()) {
                printf("DIAG stale_child_drop oldFile=0x%X/%u oldP=0x%X oldH=0x%X newFile=0x%X/%u\n",
                       mod.file_addr, mod.file_len, mod.p_addr,
                       mod.helper_addr, file_addr, file_len);
            }
            continue;
        }
        m->nested_modules[out++] = mod;
    }
    m->nested_module_count = out;
    /* P4.3:模块状态迁移后的结构一致性断言(VMRP_ARM_EXT_INVARIANTS 门控) */
    arm_ext_verify_invariants(m, "drop-stale");
}

void arm_ext_retire_modules_overwritten_by_data_read(ArmExtModule *m,
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
    /* P4.3:模块状态迁移后的结构一致性断言(VMRP_ARM_EXT_INVARIANTS 门控) */
    arm_ext_verify_invariants(m, "retire-data-read");
}

void arm_ext_restore_primary_mapping_after_dump0(ArmExtModule *m,
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
    /* P4.3:模块状态迁移后的结构一致性断言(VMRP_ARM_EXT_INVARIANTS 门控) */
    arm_ext_verify_invariants(m, "restore-dump0");
}

uint32_t arm_ext_p_for_code_addr(ArmExtModule *m, uint32_t addr,
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

int arm_ext_has_foreground_child(ArmExtModule *m) {
    return m &&
           m->primary_p_addr && m->primary_helper_addr &&
           m->active_p_addr && m->active_helper_addr &&
           m->active_p_addr != m->primary_p_addr &&
           m->active_helper_addr != m->primary_helper_addr &&
           m->active_p_addr != m->p_addr &&
           m->active_helper_addr != m->helper_addr;
}

int arm_ext_has_suspended_foreground_child(ArmExtModule *m,
                                                  uint32_t primary_ext_chunk) {
    if (!arm_ext_has_foreground_child(m) ||
        !primary_ext_chunk ||
        !arm_ptr(m, primary_ext_chunk + AEX_CHUNK_SUSPEND_OFF)) {
        return 0;
    }
    uint32_t suspend_depth = 0;
    memcpy(&suspend_depth, arm_ptr(m, primary_ext_chunk + AEX_CHUNK_SUSPEND_OFF), 4);
    return suspend_depth > 0;
}

uint32_t arm_ext_active_rw_base(ArmExtModule *m) {
    uint32_t rw_base = 0;
    if (m && m->active_p_addr && arm_ptr(m, m->active_p_addr)) {
        memcpy(&rw_base, arm_ptr(m, m->active_p_addr), 4);
    }
    return rw_base;
}


uint32_t find_wrapper_timer_dispatch(const uint8_t *code, uint32_t len,
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
