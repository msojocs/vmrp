/*
 * aex_pack.c —— MRP 包缓存与虚拟文件描述符(Phase 2 拆分第 2 步)。
 *
 * 宿主侧 MRP 解析(gzip 解压缓存全部条目)与只读虚拟 fd,供 table[40/44/46]
 * 等文件桥在磁盘查不到时使用。函数体自 arm_ext_executor.c 原样迁移。
 */
#include "../include/arm_ext_priv.h"
#include "../include/fileLib.h"

#include <zlib.h>

static uint32_t mrp_rd32le(const uint8_t *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

void mrp_cache_free(ArmExtModule *m);

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

/* 解析 MRP 并缓存所有条目（解压 gzip） */
void parse_mrp_cache(ArmExtModule *m, const char *mrp_path) {
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
MrpCacheEntry *mrp_cache_find(ArmExtModule *m, const char *filename) {
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
uint32_t mrp_vfd_open(ArmExtModule *m, const uint8_t *data, uint32_t len) {
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

MrpVirtualFd *mrp_vfd_get(ArmExtModule *m, uint32_t fd) {
    if (fd <= MRP_VFD_BASE || fd > MRP_VFD_BASE + MRP_VFD_MAX) return NULL;
    MrpVirtualFd *v = &m->mrp_vfds[fd - MRP_VFD_BASE - 1];
    return v->in_use ? v : NULL;
}

void mrp_cache_free(ArmExtModule *m) {
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
