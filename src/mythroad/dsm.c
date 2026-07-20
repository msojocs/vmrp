#include "./include/dsm.h"

#include <stdio.h>
#include "./include/encode.h"
#include "./include/fixR9.h"
#include "./include/mem.h"
#include "./include/printf.h"
#include "./include/string.h"
#include <stdlib.h>

#define DSM_MAX_FILE_LEN 256
#define DSM_MRP_TRACKER_MAX 32
#define DSM_SEEK_SET 0
#define DSM_SEEK_CUR 1
#define DSM_SEEK_END 2

/* skyengine advertises a real MTK platform version that common ARM EXT libraries
 * accept for graphics setup; actual SCRRAM availability is implemented by
 * mr_platEx(MR_MALLOC_SCRRAM), not inferred from this handset bucket. */
#define MT6227

/*请不要修改这些值*/
#if (defined(MT6223P) || defined(MT6223) || defined(MT6223P_S00))
#define DSM_PLAT_VERSION (2) /*手机平台区分(1~99)*/
#elif (defined(MT6226) || defined(MT6226M) || defined(MT6226D))
#define DSM_PLAT_VERSION (4) /*手机平台区分(1~99)*/
#elif (defined(MT6228))
#define DSM_PLAT_VERSION (5) /*手机平台区分(1~99)*/
#elif (defined(MT6225))
#define DSM_PLAT_VERSION (3) /*手机平台区分(1~99)*/
#elif (defined(MT6230))
#define DSM_PLAT_VERSION (6) /*手机平台区分(1~99)*/
#elif (defined(MT6227) || defined(MT6227D))
#define DSM_PLAT_VERSION (7)
#elif (defined(MT6219))
#define DSM_PLAT_VERSION (1)
#elif (defined(MT6235) || defined(MT6235B))
#define DSM_PLAT_VERSION (8)
#elif (defined(MT6229))
#define DSM_PLAT_VERSION (9)
#elif (defined(MT6253) || defined(MT6253T))
#define DSM_PLAT_VERSION (10)
#elif (defined(MT6238))
#define DSM_PLAT_VERSION (11)
#elif (defined(MT6239))
#define DSM_PLAT_VERSION (12)
#else
#error PLATFORM NOT IN LIST PLEASE CALL SKY TO ADD THE PLATFORM
#endif

#ifdef DSM_IDLE_APP
#define DSM_FAE_VERSION (180) /*由平台组统一分配版本号，有需求请联系平台组*/
#else
#define DSM_FAE_VERSION (182) /*由平台组统一分配版本号，有需求请联系平台组*/
#endif

static DSM_REQUIRE_FUNCS *dsmInFuncs;
static uint32 dsmStartTime;  //虚拟机初始化时间，用来计算系统运行时间

/* 当前 LCD 旋转(MR_LCD_ROTATE_*: 0=正常,1=90°,2=180°,3=270°)。guest 经
 * mr_plat(101,param) 设置;真机上这是 LCD 驱动状态,故由 DSM 层持有(本层
 * 作为独立子项目编译,不能访问 skyengine_config)。宿主展示层经
 * dsm_get_lcd_rotation() 读取以计算旋转后的显示尺寸。 */
static int32 dsmLcdRotation = 0;

int32 dsm_get_lcd_rotation(void) {
    return dsmLcdRotation;
}

void dsm_set_lcd_rotation(int32 rotation) {
    dsmLcdRotation = rotation;
}

/* 动感芯片(加速度传感器)状态,guest 经 mr_plat(4001~4006) 控制(接口语义
 * 见 dsm.h 声明处)。真机上这是传感器驱动状态,由 DSM 层持有;宿主注入层
 * (mr_motion_input)经 dsm_motion_listening_mode() 判断是否需要上送样本。 */
static int32 dsmMotionPowered = 0;
static int32 dsmMotionListening = 0;
static int32 dsmMotionMode = MR_MOTION_EVENT_SHAKE;

static void dsm_motion_reset(void) {
    dsmMotionPowered = 0;
    dsmMotionListening = 0;
    dsmMotionMode = MR_MOTION_EVENT_SHAKE;
}

int32 dsm_motion_listening_mode(void) {
    if (!dsmMotionPowered || !dsmMotionListening) return -1;
    return dsmMotionMode;
}

typedef struct DsmMrpWriteTracker {
    int32 handle;
    char path[DSM_MAX_FILE_LEN];
    uint8 prefix[4];
    uint32 prefix_len;
    int tracking;
    uint8 *copy;
    uint32 len;
    uint32 cap;
    uint32 pos;
} DsmMrpWriteTracker;

static DsmMrpWriteTracker dsm_mrp_write_trackers[DSM_MRP_TRACKER_MAX];
static char dsm_last_written_mrp_path[DSM_MAX_FILE_LEN];
static uint8 *dsm_last_written_mrp_copy;
static uint32 dsm_last_written_mrp_len;

static uint32 dsm_read_le32(const uint8 *p) {
    return (uint32)p[0] |
           ((uint32)p[1] << 8) |
           ((uint32)p[2] << 16) |
           ((uint32)p[3] << 24);
}

static int dsm_file_mode_writes(uint32 mode) {
    return (mode & (MR_FILE_WRONLY | MR_FILE_RDWR | MR_FILE_CREATE |
                    MR_FILE_RECREATE | MR_FILE_COMMITTED)) != 0;
}

static void dsm_mrp_tracker_reset(DsmMrpWriteTracker *tracker) {
    if (!tracker) return;
    free(tracker->copy);
    memset2(tracker, 0, sizeof(*tracker));
}

static DsmMrpWriteTracker *dsm_mrp_tracker_for_handle(int32 handle) {
    int i;
    if (handle <= 0) return NULL;
    for (i = 0; i < DSM_MRP_TRACKER_MAX; ++i) {
        if (dsm_mrp_write_trackers[i].handle == handle)
            return &dsm_mrp_write_trackers[i];
    }
    return NULL;
}

static void dsm_mrp_tracker_reset_stream(DsmMrpWriteTracker *tracker,
                                         uint32 pos) {
    if (!tracker) return;
    free(tracker->copy);
    tracker->copy = NULL;
    tracker->prefix_len = 0;
    tracker->tracking = 0;
    tracker->len = 0;
    tracker->cap = 0;
    tracker->pos = pos;
}

static void dsm_mrp_track_open(int32 handle, const char *path, uint32 mode) {
    int i;
    DsmMrpWriteTracker *slot = NULL;
    if (handle <= 0 || !path || !path[0] || !dsm_file_mode_writes(mode))
        return;
    for (i = 0; i < DSM_MRP_TRACKER_MAX; ++i) {
        if (dsm_mrp_write_trackers[i].handle == handle) {
            slot = &dsm_mrp_write_trackers[i];
            break;
        }
    }
    if (!slot) {
        for (i = 0; i < DSM_MRP_TRACKER_MAX; ++i) {
            if (!dsm_mrp_write_trackers[i].handle) {
                slot = &dsm_mrp_write_trackers[i];
                break;
            }
        }
    }
    if (!slot) return;
    dsm_mrp_tracker_reset(slot);
    slot->handle = handle;
    snprintf_(slot->path, sizeof(slot->path), "%s", path);
}

static void dsm_mrp_publish(DsmMrpWriteTracker *tracker) {
    uint32 total_len;
    uint8 *copy;
    if (!tracker || !tracker->tracking || tracker->len < 12 ||
        !tracker->path[0] || memcmp2(tracker->copy, "MRPG", 4) != 0)
        return;
    total_len = dsm_read_le32(tracker->copy + 8);
    if (total_len <= 240 || total_len > tracker->len)
        return;

    copy = (uint8 *)malloc(total_len);
    if (!copy) return;
    memcpy2(copy, tracker->copy, total_len);
    free(dsm_last_written_mrp_copy);
    dsm_last_written_mrp_copy = copy;
    dsm_last_written_mrp_len = total_len;
    snprintf_(dsm_last_written_mrp_path, sizeof(dsm_last_written_mrp_path),
              "%s", tracker->path);
}

static void dsm_mrp_track_write(int32 handle, const void *src, uint32 len) {
    DsmMrpWriteTracker *tracker = dsm_mrp_tracker_for_handle(handle);
    uint32 consumed = 0;
    uint32 expected_pos;
    uint32 next_pos;
    if (!tracker || !src || !len) return;
    expected_pos = tracker->tracking ? tracker->len : tracker->prefix_len;
    if (tracker->pos > 0xFFFFFFFFu - len) {
        dsm_mrp_tracker_reset_stream(tracker, 0);
        return;
    }
    next_pos = tracker->pos + len;
    if (tracker->pos != expected_pos) {
        dsm_mrp_tracker_reset_stream(tracker, next_pos);
        return;
    }

    if (!tracker->tracking && tracker->prefix_len < 4) {
        uint32 need = 4 - tracker->prefix_len;
        uint32 take = len < need ? len : need;
        memcpy2(tracker->prefix + tracker->prefix_len, src, take);
        tracker->prefix_len += take;
        consumed = take;
        if (tracker->prefix_len == 4) {
            if (memcmp2(tracker->prefix, "MRPG", 4) != 0) {
                dsm_mrp_tracker_reset_stream(tracker, next_pos);
                return;
            }
            tracker->tracking = 1;
            tracker->cap = 4096;
            tracker->copy = (uint8 *)malloc(tracker->cap);
            if (!tracker->copy) {
                dsm_mrp_tracker_reset_stream(tracker, next_pos);
                return;
            }
            memcpy2(tracker->copy, tracker->prefix, 4);
            tracker->len = 4;
        } else {
            tracker->pos = next_pos;
            return;
        }
    }
    if (!tracker->tracking) {
        dsm_mrp_tracker_reset_stream(tracker, next_pos);
        return;
    }
    if (consumed < len) {
        const uint8 *bytes = (const uint8 *)src + consumed;
        uint32 append_len = len - consumed;
        if (tracker->len + append_len > tracker->cap) {
            uint32 new_cap = tracker->cap ? tracker->cap : 4096;
            while (new_cap < tracker->len + append_len) {
                if (new_cap > 0x7FFFFFFFu) {
                    dsm_mrp_tracker_reset_stream(tracker, next_pos);
                    return;
                }
                new_cap *= 2;
            }
            uint8 *new_copy = (uint8 *)realloc(tracker->copy, new_cap);
            if (!new_copy) {
                dsm_mrp_tracker_reset_stream(tracker, next_pos);
                return;
            }
            tracker->copy = new_copy;
            tracker->cap = new_cap;
        }
        memcpy2(tracker->copy + tracker->len, bytes, append_len);
        tracker->len += append_len;
    }
    tracker->pos = next_pos;
    /*
     * Downloads can stream the final package through repeated mr_write calls.
     * Publish only after the MRPG header's total file length has arrived, so
     * ARM child ownership never binds to a partial network artifact.
     */
    dsm_mrp_publish(tracker);
}

static void dsm_mrp_track_close(int32 handle) {
    DsmMrpWriteTracker *tracker = dsm_mrp_tracker_for_handle(handle);
    if (!tracker) return;
    dsm_mrp_publish(tracker);
    dsm_mrp_tracker_reset(tracker);
}

static void dsm_mrp_track_seek(int32 handle, int32 pos, int method) {
    DsmMrpWriteTracker *tracker = dsm_mrp_tracker_for_handle(handle);
    uint32 expected_pos;
    uint32 new_pos;
    long long computed_pos;
    if (!tracker) return;
    /*
     * Only a sequential write stream starting at offset zero can publish MRP
     * provenance.  Keep harmless seeks to the current offset; reset the stream
     * on jumps so later writes must restart proof from offset zero.
     */
    if (method == DSM_SEEK_SET) {
        if (pos < 0) {
            dsm_mrp_tracker_reset_stream(tracker, 0);
            return;
        }
        new_pos = (uint32)pos;
    } else if (method == DSM_SEEK_CUR) {
        computed_pos = (long long)tracker->pos + (long long)pos;
        if (computed_pos < 0 || computed_pos > 0xFFFFFFFFll) {
            dsm_mrp_tracker_reset_stream(tracker, 0);
            return;
        }
        new_pos = (uint32)computed_pos;
    } else if (method == DSM_SEEK_END && pos == 0) {
        int32 file_len = dsmInFuncs->getLen(tracker->path);
        if (file_len < 0) {
            dsm_mrp_tracker_reset_stream(tracker, 0);
            return;
        }
        new_pos = (uint32)file_len;
    } else {
        dsm_mrp_tracker_reset_stream(tracker, 0);
        return;
    }

    expected_pos = tracker->tracking ? tracker->len : tracker->prefix_len;
    if (new_pos == expected_pos) {
        tracker->pos = new_pos;
    } else {
        dsm_mrp_tracker_reset_stream(tracker, new_pos);
    }
}

static void dsm_mrp_forget_path(const char *path) {
    int i;
    if (!path || !path[0]) return;
    for (i = 0; i < DSM_MRP_TRACKER_MAX; ++i) {
        DsmMrpWriteTracker *tracker = &dsm_mrp_write_trackers[i];
        if (tracker->handle && strcmp2(tracker->path, path) == 0)
            dsm_mrp_tracker_reset(tracker);
    }
    if (dsm_last_written_mrp_path[0] &&
        strcmp2(dsm_last_written_mrp_path, path) == 0) {
        free(dsm_last_written_mrp_copy);
        dsm_last_written_mrp_copy = NULL;
        dsm_last_written_mrp_len = 0;
        dsm_last_written_mrp_path[0] = '\0';
    }
}

static void dsm_mrp_track_rename(const char *old_path, const char *new_path) {
    int i;
    if (!old_path || !old_path[0] || !new_path || !new_path[0]) return;
    for (i = 0; i < DSM_MRP_TRACKER_MAX; ++i) {
        DsmMrpWriteTracker *tracker = &dsm_mrp_write_trackers[i];
        if (tracker->handle && strcmp2(tracker->path, old_path) == 0)
            snprintf_(tracker->path, sizeof(tracker->path), "%s", new_path);
    }
    if (dsm_last_written_mrp_path[0] &&
        strcmp2(dsm_last_written_mrp_path, old_path) == 0) {
        snprintf_(dsm_last_written_mrp_path,
                  sizeof(dsm_last_written_mrp_path), "%s", new_path);
    }
}

static void dsm_mrp_reset_all(void) {
    int i;
    for (i = 0; i < DSM_MRP_TRACKER_MAX; ++i)
        dsm_mrp_tracker_reset(&dsm_mrp_write_trackers[i]);
    free(dsm_last_written_mrp_copy);
    dsm_last_written_mrp_copy = NULL;
    dsm_last_written_mrp_len = 0;
    dsm_last_written_mrp_path[0] = '\0';
}

const char *mr_get_last_written_mrp_path(void) {
    return dsm_last_written_mrp_path;
}

const uint8 *mr_get_last_written_mrp_data(uint32 *len) {
    if (len) *len = dsm_last_written_mrp_len;
    return dsm_last_written_mrp_copy;
}

//////////////////////////////////////////////////////////////////

void mr_printf(const char *format, ...) {
    static int log_checked = 0;
    static int log_enabled = 0;
    char printfBuf[512];
    va_list params;

    if (!log_checked) {
        log_enabled = getenv("SKYENGINE_LOG") != NULL;
        log_checked = 1;
    }
    if (!log_enabled) {
        return;
    }

    va_start(params, format);
    vsnprintf_(printfBuf, sizeof(printfBuf), format, params);
    va_end(params);
    dsmInFuncs->log(printfBuf);
}

#define LOGI(fmt, ...) mr_printf("[INFO]" fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...) mr_printf("[WARN]" fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) mr_printf("[ERROR]" fmt, ##__VA_ARGS__)
#define LOGD(fmt, ...) mr_printf("[DEBUG]" fmt, ##__VA_ARGS__)

static void panic(char *msg) {
    LOGE("panic: %s", msg);
    while (1) {
    }
}

///////////////////////////////////////////////////////////////////
// gb16字体参数：每行2字节，高16像素，每字符32字节
#define CHAR_H_16 16
#define EN_CHAR_W_16 8
#define CN_CHAR_W_16 16
#define BYTES_PER_CHAR_16 32

// gb12字体参数：英文每行1字节(宽8)，中文每行12位紧凑排列(3字节存2行)，每字符18字节
#define CHAR_H_12 12
#define EN_CHAR_W_12 8
#define CN_CHAR_W_12 12
#define BYTES_PER_CHAR_12 18

// todo "上有名不"这四个字必定显示为错别字(是编码转换的问题，不是字体的问题)
static unsigned char font_sky16_bitbuf[BYTES_PER_CHAR_16];
static int font_sky16_f;

static unsigned char font_sky12_bitbuf[BYTES_PER_CHAR_12];
static int font_sky12_f;

// 应用打开gb12_uc2.adl时置1，表示MR_FONT_MEDIUM也使用gb12
static int font_medium_use_12 = 0;

// mr_platDrawChar没有fontSize参数，通过此变量传递当前字体大小
static uint16 current_font_size = MR_FONT_MEDIUM;

static void xl_font_sky16_charWidthHeight(uint16 id, int32 *width, int32 *height);
static void xl_font_sky12_charWidthHeight(uint16 id, int32 *width, int32 *height);

static int xl_font_sky16_init() {  //字体初始化，打开字体文件
    font_sky16_f = mr_open("system/gb16.uc2", MR_FILE_RDONLY);
    if (font_sky16_f == 0) {
        LOGW("%s", "font16 load fail");
        return -1;
    }
    LOGI("font16 load suc fd=%d", font_sky16_f);
    return 0;
}

static int xl_font_sky12_init() {
    font_sky12_f = mr_open("system/gb12.uc2", MR_FILE_RDONLY);
    if (font_sky12_f == 0) {
        LOGW("%s", "font12 load fail");
        return -1;
    }
    LOGI("font12 load suc fd=%d", font_sky12_f);
    return 0;
}

static int xl_font_sky16_close() {  //关闭字体
    return mr_close(font_sky16_f);
}

static int xl_font_sky12_close() {
    if (font_sky12_f > 0) {
        return mr_close(font_sky12_f);
    }
    return 0;
}

// MR_FONT_SMALL始终使用gb12；MR_FONT_MEDIUM在应用激活gb12后也使用gb12
static int xl_font_should_use_12(uint16 fontSize) {
    if (font_sky12_f <= 0) {
        return 0;
    }
    if (fontSize == MR_FONT_SMALL) {
        return 1;
    }
    if (fontSize == MR_FONT_MEDIUM && font_medium_use_12) {
        return 1;
    }
    return 0;
}

//获得字符的位图
static char *xl_font_sky16_getChar(uint16 id) {
    mr_seek(font_sky16_f, id * BYTES_PER_CHAR_16, 0);
    mr_read(font_sky16_f, font_sky16_bitbuf, BYTES_PER_CHAR_16);
    return (char *)font_sky16_bitbuf;
}

static char *xl_font_sky12_getChar(uint16 id) {
    mr_seek(font_sky12_f, id * BYTES_PER_CHAR_12, 0);
    mr_read(font_sky12_f, font_sky12_bitbuf, BYTES_PER_CHAR_12);
    return (char *)font_sky12_bitbuf;
}

static void xl_font_sky16_drawChar(uint16 ch, int x, int y, uint16 color) {
    extern void _DrawPoint(int16 x, int16 y, uint16 nativecolor);
    int width, height;
    int ix, iy;
    uint16 data;

    mr_seek(font_sky16_f, ch * BYTES_PER_CHAR_16, 0);  // 一行两字节，高度16，所以2*16=32字节
    mr_read(font_sky16_f, font_sky16_bitbuf, BYTES_PER_CHAR_16);
    xl_font_sky16_charWidthHeight(ch, &width, &height);
    for (iy = 0; iy < height; iy++) {
        data = ((uint16)font_sky16_bitbuf[iy * 2] << 8) | font_sky16_bitbuf[iy * 2 + 1];
        for (ix = 0; ix < width; ix++) {
            if (data & (1 << 15)) {
                _DrawPoint(ix + x, iy + y, color);
            }
            data = data << 1;
        }
    }
}

static void xl_font_sky12_drawChar(uint16 ch, int x, int y, uint16 color) {
    extern void _DrawPoint(int16 x, int16 y, uint16 nativecolor);
    int width, height;
    int ix, iy;

    mr_seek(font_sky12_f, ch * BYTES_PER_CHAR_12, 0);
    mr_read(font_sky12_f, font_sky12_bitbuf, BYTES_PER_CHAR_12);
    xl_font_sky12_charWidthHeight(ch, &width, &height);

    if (ch < 128) {
        // 英文：每行1字节，共12行，高8位在前（MSB为最左像素）
        for (iy = 0; iy < height; iy++) {
            uint8 data = font_sky12_bitbuf[iy];
            for (ix = 0; ix < width; ix++) {
                if (data & 0x80) {
                    _DrawPoint(ix + x, iy + y, color);
                }
                data <<= 1;
            }
        }
    } else {
        // 中文：每行12位，每3字节存2行（高12位=第一行，低12位=第二行）
        for (iy = 0; iy < height; iy++) {
            int group = iy / 2;
            uint32 val = ((uint32)font_sky12_bitbuf[group * 3] << 16) |
                         ((uint32)font_sky12_bitbuf[group * 3 + 1] << 8) |
                         font_sky12_bitbuf[group * 3 + 2];
            uint16 data;
            if (iy % 2 == 0) {
                data = (val >> 12) & 0xFFF;
            } else {
                data = val & 0xFFF;
            }
            for (ix = 0; ix < width; ix++) {
                if (data & (1 << 11)) {
                    _DrawPoint(ix + x, iy + y, color);
                }
                data <<= 1;
            }
        }
    }
}

//获取一个文字的宽高
static void xl_font_sky16_charWidthHeight(uint16 id, int32 *width, int32 *height) {
    if (id < 128) {
        if (width) *width = EN_CHAR_W_16;
        if (height) *height = CHAR_H_16;
        return;
    }
    if (width) *width = CN_CHAR_W_16;
    if (height) *height = CHAR_H_16;
}

static void xl_font_sky12_charWidthHeight(uint16 id, int32 *width, int32 *height) {
    if (id < 128) {
        if (width) *width = EN_CHAR_W_12;
        if (height) *height = CHAR_H_12;
        return;
    }
    if (width) *width = CN_CHAR_W_12;
    if (height) *height = CHAR_H_12;
}

int32 mr_exit(void) {
    LOGD("%s", "mr_exit() called by mythroad!");
    if (mr_restart_old_app() == MR_SUCCESS) {
        return MR_SUCCESS;
    }
    xl_font_sky16_close();
    xl_font_sky12_close();
    dsmInFuncs->exit();
    return MR_SUCCESS;
}

#define MAKE_PLAT_VERSION(plat, ver, card, impl, brun) \
    (100000000 + (plat)*1000000 + (ver)*10000 + (card)*1000 + (impl)*10 + (brun))

int32 mr_getUserInfo(mr_userinfo *info) {
    if (info == NULL) {
        return MR_FAILED;
    }

    memset2(info, 0, sizeof(mr_userinfo));
    strcpy2((char *)info->IMEI, "864086040622841");
    strcpy2((char *)info->IMSI, "460019707327302");
    strncpy2(info->manufactory, "opense", 7);
    strncpy2(info->type, "opense", 7);

    info->ver = 101000000 + DSM_PLAT_VERSION * 10000 + DSM_FAE_VERSION;
    //	info->ver = 116000000 + DSM_PLAT_VERSION * 10000 + DSM_FAE_VERSION; //SPLE
    //	info->ver = MAKE_PLAT_VERSION(1, 3, 0, 18, 0);

    memset2(info->spare, 0, sizeof(info->spare));

    return MR_SUCCESS;
}

int32 mr_cacheSync(void *addr, int32 len) {
    LOGW("mr_cacheSync(0x%p, %d)", addr, len);
#if defined(__arm__)
    // cacheflush((long)addr, (long)(addr + len), 0);
#endif
    return MR_SUCCESS;
}

int32 mr_mem_get(char **mem_base, uint32 *mem_len) {
    return dsmInFuncs->mem_get(mem_base, mem_len);
}

int32 mr_mem_free(char *mem, uint32 mem_len) {
    return dsmInFuncs->mem_free(mem, mem_len);
}

int32 mr_timerStart(uint16 t) {
    return dsmInFuncs->timerStart(t);
}

int32 mr_timerStop(void) {
    return dsmInFuncs->timerStop();
}

uint32 mr_getTime(void) {
    return dsmInFuncs->get_uptime_ms() - dsmStartTime;
}

int32 mr_getDatetime(mr_datetime *datetime) {
    return dsmInFuncs->getDatetime(datetime);
}

int32 mr_sleep(uint32 ms) {
    return dsmInFuncs->sleep(ms);
}

///////////////////////// 文件操作接口 //////////////////////////////////////
#define MYTHROAD_PATH "mythroad/"
#define DSM_HIDE_DRIVE "mythroad/disk/"
#define DSM_DRIVE_A "mythroad/disk/a/"
#define DSM_DRIVE_B "mythroad/disk/b/"
#define DSM_DRIVE_X "mythroad/disk/x/"

static char dsmWorkPath[DSM_MAX_FILE_LEN] = MYTHROAD_PATH; /*当前工作路径 gb 编码*/

/*
 * 整理路径，将分隔符统一为sep，并清除连续的多个
 * 参数：路径(必须可读写)
 */
static char *formatPathString(char *path, char sep) {
    char *p, *q;
    int flag = 0;

    for (p = q = path; *p; p++) {
        if ('\\' == *p || '/' == *p) {
            if (0 == flag) {
                *q = sep;
                q++;
            }
            flag = 1;
        } else {
            *q = *p;
            q++;
            flag = 0;
        }
    }
    *q = '\0';
    return path;
}

static void SetDsmWorkPath(const char *path) {
    int l;
    strncpy2(dsmWorkPath, path, sizeof(dsmWorkPath) - 1);
    formatPathString(dsmWorkPath, '/');
    /* dump/restore 后 ARM 内存中的路径可能被 netpay 破坏，
     * 检测并修正为正确的 mythroad/ 前缀 */
    if (strncmp2(dsmWorkPath, "mythroad/", 9) != 0 &&
        strncmp2(dsmWorkPath, "mythr", 5) == 0) {
        char fixed[DSM_MAX_FILE_LEN];
        snprintf_(fixed, sizeof(fixed), "%s%s", MYTHROAD_PATH, dsmWorkPath + strlen2("mythroad/"));
        strncpy2(dsmWorkPath, fixed, sizeof(dsmWorkPath) - 1);
    }

    l = strlen2(dsmWorkPath);
    if (dsmWorkPath[l - 1] != '/') {
        dsmWorkPath[l] = '/';
        dsmWorkPath[l + 1] = '\0';
    }
    LOGW("SetDsmWorkPath():'%s'", dsmWorkPath);
}

const char *mr_get_dsm_work_path(void) {
    return dsmWorkPath;
}

void mr_set_dsm_work_path(const char *path) {
    SetDsmWorkPath((path && path[0]) ? path : MYTHROAD_PATH);
}

void mr_reset_dsm_work_path(void) {
    /* The DSM working directory is process-global platform state, while a
     * RESTART creates a fresh VM inside the same process.  Real app starts
     * begin from the Mythroad root, so handoff restarts must reset it too. */
    SetDsmWorkPath(MYTHROAD_PATH);
}

static char dsmSwitchPathBuf[DSM_MAX_FILE_LEN + 10];
static int32 dsmSwitchPath(uint8 *input, int32 input_len, uint8 **output, int32 *output_len) {
    LOGI("dsmSwitchPath '%s', %d, %p, %p", input, input_len, output, output_len);
    /*
        功能：将SkyEngine的根目录切换至新目录。目录字符串如：”C:/App/”，第一个字符表示切换至的存储设备：（盘符不区分大小写，GB编码）
        第二、第三字符为“:/”，第四字符起为该存储设备上的目录名。
    */
    switch (input[0]) {
        case 'Z':  // 返回刚启动时路径
        case 'z':
            strcpy2(dsmWorkPath, MYTHROAD_PATH);
            break;

        case 'Y':
        case 'y': {  // 获取当前的路径设置，返回型如："C:/App/"（即必须符合上述输入标准），gb编码；
            char *p;
            if ((p = strstr2(dsmWorkPath, DSM_HIDE_DRIVE)) != NULL) {  //在A盘下
                p += strlen2(DSM_HIDE_DRIVE);                          // a/...
                if (p) {
                    if (*(p + 2))
                        snprintf_(dsmSwitchPathBuf, sizeof(dsmSwitchPathBuf), "%c:/%s", *p, (p + 2));
                    else
                        snprintf_(dsmSwitchPathBuf, sizeof(dsmSwitchPathBuf), "%c:/", *p);
                } else {
                    panic("dsmWorkPath y ERROR!");
                }
            } else {
                snprintf_(dsmSwitchPathBuf, sizeof(dsmSwitchPathBuf), "c:/%s", dsmWorkPath);
            }
            LOGW("dsmSwitchPathBuf: y '%s'", dsmSwitchPathBuf);
            *output = (uint8 *)dsmSwitchPathBuf;
            *output_len = strlen2(dsmSwitchPathBuf);
            break;
        }

        case 'x':
        case 'X':  // 进入vm的根目录（后继子串参数无意义）。这个根目录必须放在用户不可见的，不能卸载的盘上。在这个根目录下可以保存一些设置信息，及收费信息等；
            strcpy2(dsmWorkPath, DSM_DRIVE_X);
            break;

        case 'a':
        case 'A': {  // A：  普通用户不可见（不可操作）存储盘；
            if (input_len > 3) {
                sprintf_(dsmSwitchPathBuf, "%s%s", DSM_DRIVE_A, input + 3);
            } else {
                sprintf_(dsmSwitchPathBuf, "%s", DSM_DRIVE_A);
            }
            SetDsmWorkPath(dsmSwitchPathBuf);
            break;
        }
        case 'b':
        case 'B': {  // B：  普通用户可操作存储盘（即可usb连接在PC上操作）；
            if (input_len > 3) {
                sprintf_(dsmSwitchPathBuf, "%s%s", DSM_DRIVE_B, input + 3);
            } else {
                sprintf_(dsmSwitchPathBuf, "%s", DSM_DRIVE_B);
            }
            SetDsmWorkPath(dsmSwitchPathBuf);
            break;
        }
        case 'c':
        case 'C':  // 外插存储设备，如mmc，sd，t-flash等；
            if (input_len > 3) {
                SetDsmWorkPath((char *)(input + 3));
            } else {
                SetDsmWorkPath("./");
            }
            break;

        default:
            LOGE("%s", "dsmSwitchPath() default");
            return MR_IGNORE;
    }

    return MR_SUCCESS;
}

static int isHostAbsolutePath(const char *filename) {
    if (!filename || !*filename) {
        return 0;
    }
    if (filename[0] == '/') {
        return 1;
    }
#ifdef _WIN32
    if (filename[0] == '\\') {
        return 1;
    }
    if (((filename[0] >= 'A' && filename[0] <= 'Z') || (filename[0] >= 'a' && filename[0] <= 'z')) &&
        filename[1] == ':' && (filename[2] == '/' || filename[2] == '\\')) {
        return 1;
    }
#endif
    return 0;
}

static int isDsmRootPath(const char *filename) {
    return filename && strncmp2(filename, MYTHROAD_PATH, strlen2(MYTHROAD_PATH)) == 0;
}

#ifndef USE_FINDDIR
/* ASCII 大小写不敏感比较:定长路径段 vs NUL 结尾目录项 */
static int dsm_segment_ieq(const char *seg, uint32 seg_len, const char *name) {
    uint32 i;
    for (i = 0; i < seg_len; ++i) {
        char a = seg[i];
        char b = name[i];
        if (!b) return 0;
        if (a >= 'A' && a <= 'Z') a = (char)(a + ('a' - 'A'));
        if (b >= 'A' && b <= 'Z') b = (char)(b + ('a' - 'A'));
        if (a != b) return 0;
    }
    return name[seg_len] == '\0';
}

/* 大小写不敏感路径回退。MRP 应用产自 FAT(大小写不敏感)存储的真机环境,
 * 可能用与落盘时不同的大小写重新访问同一文件:gtcm 安装场景数据时写
 * gtcm\1004\uid.scene,"开始游戏"二次加载却用 UID.scene 打开——真机能
 * 命中,Linux 区分大小写的文件系统打开失败,场景名读不出导致空文件名
 * 拼接与未初始化指针崩溃。这里在原样查找失败(MR_IS_INVALID)时逐段
 * 扫描父目录做不区分大小写匹配,命中则回填磁盘上的真实大小写,忠实
 * 模拟 FAT 语义;找不到的段保留原样(可能是即将创建的新文件)。
 * 兼容性:仅在原样查找失败时触发,对已能命中的路径零改动。 */
static void dsm_fix_path_case(char *pathbuf) {
    char fixed[DSM_MAX_FILE_LEN];
    uint32 fi = 0;
    const char *p = pathbuf;
    int changed = 0;

    if (*p == '/') {
        fixed[fi++] = '/';
        p++;
    }
    while (*p && fi < DSM_MAX_FILE_LEN - 1) {
        const char *seg = p;
        uint32 seg_len = 0;
        uint32 base = fi;
        int trailing;
        while (p[seg_len] && p[seg_len] != '/') seg_len++;
        p += seg_len;
        trailing = (*p == '/');
        if (trailing) p++;

        if (base + seg_len + 2 >= DSM_MAX_FILE_LEN) return; /* 超长,放弃 */
        memcpy2(fixed + base, (void *)seg, seg_len);
        fixed[base + seg_len] = '\0';
        if (dsmInFuncs->info(fixed) == MR_IS_INVALID) {
            /* 原样不存在:扫描父目录寻找仅大小写不同的目录项 */
            char parent[DSM_MAX_FILE_LEN];
            int32 dh;
            if (base == 0) {
                strcpy2(parent, ".");
            } else if (base == 1 && fixed[0] == '/') {
                strcpy2(parent, "/");
            } else {
                memcpy2(parent, fixed, base - 1); /* 去掉结尾的 '/' */
                parent[base - 1] = '\0';
            }
            dh = dsmInFuncs->opendir(parent);
            if (dh > 0) {
                char *dn;
                while ((dn = dsmInFuncs->readdir(dh)) != NULL) {
                    if (dsm_segment_ieq(seg, seg_len, dn)) {
                        uint32 nl = strlen2(dn);
                        if (base + nl + 2 >= DSM_MAX_FILE_LEN) break;
                        memcpy2(fixed + base, dn, nl);
                        fixed[base + nl] = '\0';
                        seg_len = nl;
                        changed = 1;
                        break;
                    }
                }
                dsmInFuncs->closedir(dh);
            }
        }
        fi = base + seg_len;
        if (trailing && fi < DSM_MAX_FILE_LEN - 1) fixed[fi++] = '/';
        fixed[fi] = '\0';
    }
    if (changed) {
        strcpy2(pathbuf, fixed);
    }
}
#endif /* !USE_FINDDIR */

char *get_filename(char *outputbuf, const char *filename) {
    if (isHostAbsolutePath(filename) || isDsmRootPath(filename)) {
        sprintf_(outputbuf, "%s", filename);
    } else {
        sprintf_(outputbuf, "%s%s", dsmWorkPath, filename);
    }
    formatPathString(outputbuf, '/');
    if ((dsmInFuncs->flags & FLAG_USE_UTF8_FS) &&
        dsmInFuncs->info(outputbuf) == MR_IS_INVALID) {
        /* Startup paths imported by Flutter/Android are already host UTF-8
         * (for example mythroad/中文.mrp).  Existing host files must not be
         * converted as GB again; MRP-internal GB paths still miss this raw
         * lookup and are converted for the UTF-8 filesystem below. */
        char *us = (char *)GBStrToUCS2BEStr((uint8 *)outputbuf, NULL);
        char *utf8s = UCS2BEStrToUTF8Str((uint8 *)us, NULL);
        strcpy2(outputbuf, utf8s);
        mr_freeExt(us);
        mr_freeExt(utf8s);
    }
#ifndef USE_FINDDIR
    if (dsmInFuncs->info(outputbuf) == MR_IS_INVALID) {
        dsm_fix_path_case(outputbuf);
    }
#endif
    return outputbuf;
}

/*
 * get_filename() 编码规则的逆操作：guest 世界（VM/EXT ARM 代码）固定使用
 * GBK 编码，宿主文件系统使用 UTF-8 时（FLAG_USE_UTF8_FS），暴露给 guest 的
 * 文件名/包名必须先转成 GBK —— guest 会把它拼进 cache/<包名> 之类的路径再
 * 交回来，由 get_filename() 按 GBK→UTF-8 还原。若缺少本转换，UTF-8 字节会
 * 被当作 GBK 二次转换生成乱码路径，且 UTF-8 中文比 GBK 长，还会撑爆 guest
 * 侧固定长度的包名缓冲区。非 UTF-8 文件系统上宿主路径本身就是本地编码，
 * 原样拷贝。
 */
void dsm_host_path_to_guest(char *buf, uint32 bufsize, const char *host_path) {
    if (!buf || !bufsize) return;
    if (!host_path) host_path = "";
    if (dsmInFuncs->flags & FLAG_USE_UTF8_FS) {
        char *gb = UTF8StrToGBStr((uint8 *)host_path, NULL);
        if (gb) {
            snprintf_(buf, bufsize, "%s", gb);
            mr_freeExt(gb);
            return;
        }
    }
    snprintf_(buf, bufsize, "%s", host_path);
}

typedef struct {
    int device;
    int status;
    uint8 *data;
    int32 len;
    int owns_data;
} DsmMediaDevice;

static DsmMediaDevice dsm_media_devices[ACI_AMR_WB_DEVICE + 1];

/* MR_MEDIA_*_MUTICHANNEL is a handle API: OPEN owns a compressed-data copy,
 * while PLAY transfers a decoded copy to the native mixer.  Generation bits
 * keep a delayed PLAY/STOP/CLOSE from targeting a slot reused after CLOSE. */
#define DSM_MEDIA_CHANNEL_COUNT 16
#define DSM_MEDIA_CHANNEL_TOKEN_MASK 0xffu
#define DSM_MEDIA_CHANNEL_GENERATION_MAX 0x7fffffu

typedef struct {
    uint8 *data;
    uint32 len;
    uint32 generation;
    int32 device;
    int32 loop;
    int in_use;
    int playing;
} DsmMediaChannel;

static DsmMediaChannel dsm_media_channels[DSM_MEDIA_CHANNEL_COUNT];

static int32 dsm_media_to_sound_type(int device) {
    switch (device) {
        case ACI_MIDI_DEVICE: return MR_SOUND_MIDI;
        case ACI_WAVE_DEVICE: return MR_SOUND_WAV;
        case ACI_MP3_DEVICE: return MR_SOUND_MP3;
        case ACI_AMR_DEVICE: return MR_SOUND_AMR;
        case ACI_PCM_DEVICE: return MR_SOUND_PCM;
        case ACI_M4A_DEVICE: return MR_SOUND_M4A;
        case ACI_AMR_WB_DEVICE: return MR_SOUND_AMR_WB;
        default: return MR_FAILED;
    }
}

static void dsm_media_release(DsmMediaDevice *media) {
    if (media->owns_data && media->data) {
        mr_free(media->data, (uint32)media->len);
    }
    media->data = NULL;
    media->len = 0;
    media->owns_data = FALSE;
    media->status = MR_MEDIA_IDLE;
}

static void dsm_media_reset_all(void) {
    /* dsm_init 在 Mythroad 堆重新初始化前调用；上一轮应用的 media/channel
     * 缓冲跟随整块堆内存释放，这里只清状态，不能再对旧指针调用 mr_free。 */
    memset2(dsm_media_devices, 0, sizeof(dsm_media_devices));
    memset2(dsm_media_channels, 0, sizeof(dsm_media_channels));
}

static int32 dsm_media_channel_handle(int slot, uint32 generation) {
    return (int32)((generation << 8) | (uint32)(slot + 1));
}

static DsmMediaChannel *dsm_media_channel_get(int32 handle, int device) {
    if (handle <= 0) return NULL;
    uint32 raw = (uint32)handle;
    uint32 token = raw & DSM_MEDIA_CHANNEL_TOKEN_MASK;
    uint32 generation = raw >> 8;
    if (token == 0 || token > DSM_MEDIA_CHANNEL_COUNT || generation == 0) {
        return NULL;
    }
    DsmMediaChannel *channel = &dsm_media_channels[token - 1];
    if (!channel->in_use || channel->generation != generation ||
        channel->device != device) {
        return NULL;
    }
    return channel;
}

static int32 dsm_media_channel_release(DsmMediaChannel *channel, int slot) {
    int32 stop_ret = MR_SUCCESS;
    if (channel->playing) {
        int32 handle = dsm_media_channel_handle(slot, channel->generation);
        stop_ret = dsmInFuncs->mr_stopSoundChannel(handle);
    }
    uint32 generation = channel->generation;
    mr_free(channel->data, channel->len);
    memset2(channel, 0, sizeof(*channel));
    channel->generation = generation;
    return stop_ret;
}

void dsm_media_channels_release_all(void) {
    /* App restart frees the whole Mythroad heap without calling dsm_init().
     * Stop host-owned decoded voices and release channel copies first so no
     * slot can retain a dangling pointer into the next application's heap. */
    for (int slot = 0; slot < DSM_MEDIA_CHANNEL_COUNT; slot++) {
        DsmMediaChannel *channel = &dsm_media_channels[slot];
        if (channel->in_use) {
            dsm_media_channel_release(channel, slot);
        }
    }
}

/* ARM table[38] calls this after translating the nested 32-bit guest address.
 * Keeping the host pointer outside the 12-byte guest payload avoids truncating
 * a 64-bit pointer while DSM retains ownership of the compressed-data copy. */
int32 dsm_media_open_channel_host(int device, const void *data, uint32 data_len,
                                  int32 loop) {
    if (!data || data_len == 0 || dsm_media_to_sound_type(device) == MR_FAILED ||
        !dsmInFuncs->mr_playSoundChannel || !dsmInFuncs->mr_stopSoundChannel) {
        return MR_FAILED;
    }

    int slot = -1;
    for (int i = 0; i < DSM_MEDIA_CHANNEL_COUNT; i++) {
        if (!dsm_media_channels[i].in_use) {
            slot = i;
            break;
        }
    }
    if (slot < 0) return MR_FAILED;

    uint8 *copy = mr_malloc(data_len);
    if (!copy) return MR_FAILED;
    memcpy2(copy, data, data_len);

    DsmMediaChannel *channel = &dsm_media_channels[slot];
    uint32 generation = channel->generation + 1u;
    if (generation == 0 || generation > DSM_MEDIA_CHANNEL_GENERATION_MAX) {
        generation = 1;
    }
    channel->data = copy;
    channel->len = data_len;
    channel->generation = generation;
    channel->device = device;
    channel->loop = loop ? TRUE : FALSE;
    channel->in_use = TRUE;
    channel->playing = FALSE;
    return dsm_media_channel_handle(slot, generation);
}

static int32 dsm_media_channel_platEx(int32 cmd, int device, uint8 *input,
                                      int32 input_len) {
    /* OPEN's first word is a guest address, so only table[38], which owns the
     * guest mapping, may translate it and call dsm_media_open_channel_host(). */
    if (cmd == MR_MEDIA_OPEN_MUTICHANNEL) return MR_FAILED;
    if (!input || input_len != (int32)sizeof(int32)) return MR_FAILED;

    int32 handle = MR_FAILED;
    memcpy2(&handle, input, sizeof(handle));
    DsmMediaChannel *channel = dsm_media_channel_get(handle, device);
    if (!channel) return MR_FAILED;

    switch (cmd) {
        case MR_MEDIA_PLAY_MUTICHANNEL: {
            int32 type = dsm_media_to_sound_type(device);
            int32 ret = dsmInFuncs->mr_playSoundChannel(
                handle, type, channel->data, channel->len, channel->loop);
            if (ret == MR_SUCCESS) channel->playing = TRUE;
            return ret;
        }
        case MR_MEDIA_STOP_MUTICHANNEL:
            if (!channel->playing) return MR_SUCCESS;
            if (dsmInFuncs->mr_stopSoundChannel(handle) == MR_SUCCESS) {
                channel->playing = FALSE;
                return MR_SUCCESS;
            }
            return MR_FAILED;
        case MR_MEDIA_CLOSE_MUTICHANNEL: {
            int slot = (int)(((uint32)handle & DSM_MEDIA_CHANNEL_TOKEN_MASK) - 1u);
            return dsm_media_channel_release(channel, slot);
        }
        default:
            return MR_FAILED;
    }
}

static DsmMediaDevice *dsm_media_get(int device, int create) {
    if (device < ACI_MIDI_DEVICE || device > ACI_AMR_WB_DEVICE) {
        return NULL;
    }
    DsmMediaDevice *media = &dsm_media_devices[device];
    if (create && media->device == 0) {
        media->device = device;
        media->status = MR_MEDIA_IDLE;
    }
    return media;
}

static int32 dsm_media_file_load(DsmMediaDevice *media, uint8 *input, int32 input_len) {
    if (!media || !input || input_len <= 0) {
        return MR_FAILED;
    }

    char fullpathname[DSM_MAX_FILE_LEN];
    char *path = get_filename(fullpathname, (const char *)input);
    int32 len = dsmInFuncs->getLen(path);
    if (len <= 0) {
        LOGW("MR_MEDIA_FILE_LOAD device=%d path='%s' len=%d", media->device, path, len);
        return MR_FAILED;
    }

    uint8 *data = mr_malloc((uint32)len);
    if (!data) {
        return MR_FAILED;
    }
    int32 f = dsmInFuncs->open(path, MR_FILE_RDONLY);
    if (f == 0) {
        mr_free(data, (uint32)len);
        return MR_FAILED;
    }
    int32 got = dsmInFuncs->read(f, data, (uint32)len);
    dsmInFuncs->close(f);
    if (got != len) {
        mr_free(data, (uint32)len);
        return MR_FAILED;
    }

    dsm_media_release(media);
    media->data = data;
    media->len = len;
    media->owns_data = TRUE;
    media->status = MR_MEDIA_LOADED;
    return MR_SUCCESS;
}

static int32 dsm_media_buf_load(DsmMediaDevice *media, uint8 *input, int32 input_len) {
    if (!media || !input || input_len <= 0) {
        return MR_FAILED;
    }
    dsm_media_release(media);
    media->data = input;
    media->len = input_len;
    media->owns_data = FALSE;
    media->status = MR_MEDIA_LOADED;
    return MR_SUCCESS;
}

static int32 dsm_media_play(DsmMediaDevice *media, uint8 *input, int32 input_len) {
    if (!media || !media->data || media->len <= 0) {
        return MR_FAILED;
    }
    T_DSM_MEDIA_PLAY play;
    memset2(&play, 0, sizeof(play));
    if (input && input_len >= (int32)sizeof(play)) {
        memcpy2(&play, input, sizeof(play));
    }

    int32 type = dsm_media_to_sound_type(media->device);
    if (type == MR_FAILED) {
        return MR_IGNORE;
    }
    int32 ret = mr_playSound(type, media->data, (uint32)media->len, play.loop);
    if (ret == MR_SUCCESS) {
        media->status = MR_MEDIA_PLAYING;
    }
    return ret;
}

static int32 dsm_media_platEx(int32 cmd, int device, uint8 *input, int32 input_len,
                              uint8 **output, int32 *output_len) {
    /* mrc_sound.h 设备接口把命令编码为 cmd * 10 + device。
     * 这里统一转接到 mr_playSound/mr_stopSound，避免 C SDK 游戏绕过
     * Lua SoundSet/BgMusicStart 后在平台层静默返回未实现。 */
    DsmMediaDevice *media = dsm_media_get(device, TRUE);
    if (!media) {
        return MR_IGNORE;
    }

    switch (cmd) {
        case MR_MEDIA_INIT:
            dsm_media_release(media);
            media->device = device;
            media->status = MR_MEDIA_INITED;
            return MR_SUCCESS;
        case MR_MEDIA_FILE_LOAD:
            return dsm_media_file_load(media, input, input_len);
        case MR_MEDIA_BUF_LOAD:
            return dsm_media_buf_load(media, input, input_len);
        case MR_MEDIA_PLAY_CUR_REQ:
            return dsm_media_play(media, input, input_len);
        case MR_MEDIA_PAUSE_REQ:
        case MR_MEDIA_STOP_REQ:
            mr_stopSound(dsm_media_to_sound_type(device));
            media->status = MR_MEDIA_LOADED;
            return MR_SUCCESS;
        case MR_MEDIA_RESUME_REQ:
            return dsm_media_play(media, input, input_len);
        case MR_MEDIA_CLOSE:
        case MR_MEDIA_FREE:
            mr_stopSound(dsm_media_to_sound_type(device));
            dsm_media_release(media);
            media->device = device;
            return MR_SUCCESS;
        case MR_MEDIA_GET_STATUS:
            if (output && output_len) {
                *output = (uint8 *)&media->status;
                *output_len = sizeof(media->status);
            }
            return media->status;
        case MR_MEDIA_SETPOS:
        case MR_MEDIA_GETTIME:
        case MR_MEDIA_GET_TOTAL_TIME:
        case MR_MEDIA_GET_CURTIME:
        case MR_MEDIA_GET_CURTIME_MSEC: {
            static T_MEDIA_TIME media_time;
            media_time.pos = 0;
            if (output && output_len) {
                *output = (uint8 *)&media_time;
                *output_len = sizeof(media_time);
            }
            return MR_SUCCESS;
        }
        default:
            LOGW("mr_platEx(media cmd=%d device=%d) not impl!", cmd, device);
            return MR_IGNORE;
    }
}

int32 mr_open(const char *filename, uint32 mode) {
    char fullpathname[DSM_MAX_FILE_LEN];
    int32 ret = dsmInFuncs->open(get_filename(fullpathname, filename), mode);
    LOGI("mr_open(%s,%d) fd is: %d\n", fullpathname, mode, ret);
    if (ret > 0) {
        dsm_mrp_track_open(ret, fullpathname, mode);
    }
    // 应用打开gb12_uc2.adl表示激活gb12字体用于MR_FONT_MEDIUM
    if (ret > 0 && strstr2(filename, "gb12_uc2.adl")) {
        font_medium_use_12 = 1;
        LOGI("gb12 font activated for MR_FONT_MEDIUM");
    }
    return ret;
}

int32 mr_close(int32 f) {
    int32 ret;
    ret = dsmInFuncs->close(f);
    LOGI("mr_close(%d): ret:%d", f, ret);
    if (ret == MR_SUCCESS) {
        dsm_mrp_track_close(f);
    }
    return ret;
}

int32 mr_read(int32 f, void *p, uint32 l) {
    return dsmInFuncs->read(f, p, l);
}

int32 mr_write(int32 f, void *p, uint32 l) {
    // LOGI("mr_write %d,%p,%d", f, p, l);
    int32 ret = dsmInFuncs->write(f, p, l);
    if (ret > 0) {
        dsm_mrp_track_write(f, p, (uint32)ret);
    }
    return ret;
}

int32 mr_seek(int32 f, int32 pos, int method) {
    int32 ret = dsmInFuncs->seek(f, pos, method);
    if (ret == MR_SUCCESS) {
        dsm_mrp_track_seek(f, pos, method);
    }
    return ret;
}

int32 mr_info(const char *filename) {
    char fullpathname[DSM_MAX_FILE_LEN];
    return dsmInFuncs->info(get_filename(fullpathname, filename));
}

int32 mr_remove(const char *filename) {
    char fullpathname[DSM_MAX_FILE_LEN];
    int32 ret;
    ret = dsmInFuncs->remove(get_filename(fullpathname, filename));
    LOGI("mr_remove(%s) ret:%d", fullpathname, ret);
    if (ret == MR_SUCCESS) {
        dsm_mrp_forget_path(fullpathname);
    }
    return ret;
}

int32 mr_rename(const char *oldname, const char *newname) {
    char fullpathname_1[DSM_MAX_FILE_LEN];
    char fullpathname_2[DSM_MAX_FILE_LEN];
    int32 ret;
    get_filename(fullpathname_1, oldname);
    get_filename(fullpathname_2, newname);
    LOGI("mr_rename(%s to %s)", fullpathname_1, fullpathname_2);
    ret = dsmInFuncs->rename(fullpathname_1, fullpathname_2);
    if (ret == MR_SUCCESS) {
        dsm_mrp_track_rename(fullpathname_1, fullpathname_2);
    }
    return ret;
}

int32 mr_mkDir(const char *name) {
    char fullpathname[DSM_MAX_FILE_LEN];
    get_filename(fullpathname, name);
    LOGI("mr_mkDir(%s)", fullpathname);
    return dsmInFuncs->mkDir(fullpathname);
}

int32 mr_rmDir(const char *name) {
    char fullpathname[DSM_MAX_FILE_LEN];
    get_filename(fullpathname, name);
    LOGI("mr_rmDir(%s)", fullpathname);
    return dsmInFuncs->rmDir(fullpathname);
}

int32 mr_findGetNext(int32 search_handle, char *buffer, uint32 len) {
#ifdef USE_FINDDIR
    return dsmInFuncs->mrc_findGetNext(search_handle, buffer, len);
#else
    char *d_name = dsmInFuncs->readdir(search_handle);
    if (d_name != NULL) {
        if (dsmInFuncs->flags & FLAG_USE_UTF8_FS) {
            char *gb = UTF8StrToGBStr((uint8 *)d_name, NULL);
            strncpy2(buffer, gb, len);
            mr_freeExt(gb);
        } else {
            strncpy2(buffer, d_name, len);
        }
        LOGI("mr_findGetNext %d %s", search_handle, d_name);
        return MR_SUCCESS;
    }
    LOGI("mr_findGetNext %d (NULL)", search_handle);
    return MR_FAILED;
#endif
}

int32 mr_findStop(int32 search_handle) {
#ifdef USE_FINDDIR
    return dsmInFuncs->mrc_findStop(search_handle);
#else
    return dsmInFuncs->closedir(search_handle);
#endif
}

int32 mr_findStart(const char *name, char *buffer, uint32 len) {
    int32 ret = MR_FAILED;
    char fullpathname[DSM_MAX_FILE_LEN];

    get_filename(fullpathname, name);
    LOGI("mr_findStart(%s)", fullpathname);

#ifdef USE_FINDDIR
    ret = dsmInFuncs->mrc_findStart(fullpathname, buffer, len);
#else
    ret = dsmInFuncs->opendir(fullpathname);
    if (ret != MR_FAILED) {
        mr_findGetNext(ret, buffer, len);
    } else {
        LOGE("mr_findStart %s: opendir FAIL!", fullpathname);
    }
#endif
    return ret;
}

int32 mr_ferrno(void) {
    return MR_FAILED;
}

int32 mr_getLen(const char *filename) {
    char fullpathname[DSM_MAX_FILE_LEN];
    return dsmInFuncs->getLen(get_filename(fullpathname, filename));
}

int32 mr_getScreenInfo(mr_screeninfo *s) {
    if (s) {
        int32 w = dsmInFuncs->screen_width;
        int32 h = dsmInFuncs->screen_height;
        s->width = (w > 0) ? w : SCREEN_WIDTH;
        s->height = (h > 0) ? h : SCREEN_HEIGHT;
        s->bit = 16;
    }
    return MR_SUCCESS;
}

void mr_drawBitmap(uint16 *bmp, int16 x, int16 y, uint16 w, uint16 h) {
    dsmInFuncs->drawBitmap(bmp, x, y, w, h);
}

const char *mr_getCharBitmap(uint16 ch, uint16 fontSize, int *width, int *height) {
    // 记录当前字体大小，供mr_platDrawChar使用
    current_font_size = fontSize;
    if (xl_font_should_use_12(fontSize)) {
        xl_font_sky12_charWidthHeight(ch, width, height);
        return xl_font_sky12_getChar(ch);
    }
    xl_font_sky16_charWidthHeight(ch, width, height);
    return xl_font_sky16_getChar(ch);
}

void mr_platDrawChar(uint16 ch, int32 x, int32 y, uint32 color) {
    // 根据mr_getCharBitmap设置的current_font_size选择字体
    if (xl_font_should_use_12(current_font_size)) {
        xl_font_sky12_drawChar(ch, x, y, (uint16)color);
    } else {
        xl_font_sky16_drawChar(ch, x, y, (uint16)color);
    }
}

int32 mr_startShake(int32 ms) {
    return dsmInFuncs->mr_startShake(ms);
}

int32 mr_stopShake() {
    return dsmInFuncs->mr_stopShake();
}

int32 mr_playSound(int type, const void *data, uint32 dataLen, int32 loop) {
    return dsmInFuncs->mr_playSound(type, data, dataLen, loop);
}

int32 mr_stopSound(int type) {
    return dsmInFuncs->mr_stopSound(type);
}


int32 mr_sendSms(char *pNumber, char *pContent, int32 encode) {
    LOGI("mr_sendSms(%s)", pNumber);
    return MR_SUCCESS;
}

void mr_call(char *number) {
    LOGI("mr_call(%s)", number);
}

int32 mr_getNetworkID(void) {
    return MR_NET_ID_MOBILE;
}

void mr_connectWAP(char *wap) {
    LOGI("mr_connectWAP(%s)", wap);
}

int32 mr_menuCreate(const char *title, int16 num) {
    return MR_FAILED;
}

int32 mr_menuSetItem(int32 hd, const char *text, int32 index) {
    return MR_FAILED;
}

int32 mr_menuShow(int32 menu) {
    return MR_IGNORE;
}

int32 mr_menuSetFocus(int32 menu, int32 index) {
    return MR_IGNORE;
}

int32 mr_menuRelease(int32 menu) {
    return MR_IGNORE;
}

int32 mr_menuRefresh(int32 menu) {
    return MR_IGNORE;
}

int32 mr_dialogCreate(const char *title, const char *text, int32 type) {
    return dsmInFuncs->mr_dialogCreate(title, text, type);
}

int32 mr_dialogRelease(int32 dialog) {
    return dsmInFuncs->mr_dialogRelease(dialog);
}

int32 mr_dialogRefresh(int32 dialog, const char *title, const char *text, int32 type) {
    return dsmInFuncs->mr_dialogRefresh(dialog, title, text, type);
}

int32 mr_textCreate(const char *title, const char *text, int32 type) {
    return dsmInFuncs->mr_textCreate(title, text, type);
}

int32 mr_textRelease(int32 text) {
    return dsmInFuncs->mr_textRelease(text);
}

int32 mr_textRefresh(int32 handle, const char *title, const char *text) {
    return dsmInFuncs->mr_textRefresh(handle, title, text);
}

static uint8 *holdTextMem;

static void freeHoldTextMem() {
    if (holdTextMem != NULL) {
        mr_freeExt(holdTextMem);
        holdTextMem = NULL;
    }
}

int32 mr_editCreate(const char *title, const char *text, int32 type, int32 max_size) {
    if (dsmInFuncs->flags & FLAG_USE_UTF8_EDIT) {
        char *u8_title = UCS2BEStrToUTF8Str((uint8 *)title, NULL);
        char *u8_text = UCS2BEStrToUTF8Str((uint8 *)text, NULL);
        int32 ret = dsmInFuncs->mr_editCreate(u8_title, u8_text, type, max_size);
        mr_freeExt(u8_title);
        mr_freeExt(u8_text);
        return ret;
    }
    return dsmInFuncs->mr_editCreate(title, text, type, max_size);
}

int32 mr_editRelease(int32 edit) {
    if (dsmInFuncs->flags & FLAG_USE_UTF8_EDIT) {
        freeHoldTextMem();
    }
    return dsmInFuncs->mr_editRelease(edit);
}

const char *mr_editGetText(int32 edit) {
    if (dsmInFuncs->flags & FLAG_USE_UTF8_EDIT) {
        char *gbStr = UTF8StrToGBStr((uint8 *)dsmInFuncs->mr_editGetText(edit), NULL);
        freeHoldTextMem();
        holdTextMem = (uint8 *)GBStrToUCS2BEStr((uint8 *)gbStr, NULL);
        mr_freeExt(gbStr);
        return (const char *)holdTextMem;
    }
    return dsmInFuncs->mr_editGetText(edit);
}

int32 mr_winCreate(void) {
    return MR_IGNORE;
}

int32 mr_winRelease(int32 win) {
    return MR_IGNORE;
}

int32 mr_rand(void) {
    return dsmInFuncs->rand();
}
//----------------------------------------------------
/*平台扩展接口*/
int32 mr_plat(int32 code, int32 param) {
    switch (code) {
        case MR_CONNECT:  // 1001
            return mr_getSocketState(param);
        case MR_GET_RAND:  // 1211
            dsmInFuncs->srand(mr_getTime());
            return (MR_PLAT_VALUE_BASE + dsmInFuncs->rand() % param);
        case MR_GET_FILE_POS:  // 1231
            /*
             * Platform values are biased by MR_PLAT_VALUE_BASE.  TalkCat's ARM
             * loader subtracts that base before deriving the mr_read() length;
             * returning the raw offset would truncate every file by 1000 bytes,
             * while returning MR_IGNORE makes the subtraction negative.
             */
            {
                int32 pos = dsmInFuncs->tell(param);
                if (pos < 0 || pos > 0x7fffffff - MR_PLAT_VALUE_BASE) {
                    return MR_FAILED;
                }
                return MR_PLAT_VALUE_BASE + pos;
            }
        case MR_CHECK_TOUCH:  // 1205是否支持触屏
            return MR_TOUCH_SCREEN;
        case MR_GET_HANDSET_LG:  // 1206获取语言
            return MR_CHINESE;
        case 1106:  // 获取短信中心
            return MR_WAITING;
        case 1218:  // 查询存储卡的状态
            return MR_MSDC_OK;
        case 1101:  // 兼容性查询 (e.g. gghjt netpay 在 adisk.sys 重命名后调用)
            return MR_IGNORE;
        case 1011:  // netpay SMS 查询/触发，返回 MR_IGNORE 让正常流程走完
            return MR_IGNORE;
        case 1016:  // 获取SIM卡状态/网络类型
            return MR_SUCCESS;
        case 1100:  // 浏览器引擎初始化查询（wbrw在加载主页前调用）
            return MR_SUCCESS;
        case 101:
            /* 设置 LCD 旋转,param 取 MR_LCD_ROTATE_*(0=正常,3=270°)。
             * gtcm(SphinxJoy 引擎)启动时调 plat(101,3) 请求横屏;反汇编
             * (game.ext 0x2370AC: cmp r0,#0 / beq 正常路径)证明返回非 0
             * 会进入"不支持横竖转换请退出"错误分支并黑屏,必须返回
             * MR_SUCCESS。
             * 旋转状态记入 skyengine_config.screen_rotation,消费方:
             * - 展示层(main.c/skyengine_api.c)按 skyengine_display_width/height()
             *   自动翻转窗口/裁剪/行宽;
             * - ARM EXT 桥(aex_t037)随后调 arm_ext_apply_lcd_rotation()
             *   把模块画布基准与 ARM 可见 mr_screen_w/h 更新为显示尺寸,
             *   等价于真机 LCD 驱动旋转后更新平台屏幕全局。 */
            /* 0..3 即 MR_LCD_ROTATE_NORMAL/90/180/270(mrporting.h:796) */
            if (param >= 0 && param <= 3) {
                dsm_set_lcd_rotation(param);
                return MR_SUCCESS;
            }
            return MR_IGNORE;
        case MR_SET_KEY_END:  // 1214 启用/禁用按键结束事件
            return MR_SUCCESS;
        /* 动感芯片接口(SKYENGINE 文档 mr_plat(4001~4006)),状态机在本文件
         * 顶部 dsmMotion*;样本注入入口是宿主侧 mr_motion_input()。 */
        case 4001:  // 停止动感芯片监听
            dsmMotionListening = 0;
            return MR_SUCCESS;
        case 4002:  // 给动感芯片上电
            dsmMotionPowered = 1;
            return MR_SUCCESS;
        case 4003:  // 给动感芯片断电(监听随之失效)
            dsmMotionPowered = 0;
            dsmMotionListening = 0;
            return MR_SUCCESS;
        case 4004:  // 监听晃动模式,值经 mr_event(18, SHAKE, T_MOTION_ACC*) 上送
            dsmMotionListening = 1;
            dsmMotionMode = MR_MOTION_EVENT_SHAKE;
            return MR_SUCCESS;
        case 4005:  // 监听倾斜模式,值经 mr_event(18, TILT, T_MOTION_ACC*) 上送
            dsmMotionListening = 1;
            dsmMotionMode = MR_MOTION_EVENT_TILT;
            return MR_SUCCESS;
        case 4006:  // 量程查询:返回 A(>1000),上送分量范围 ±(A-1000)
            return 1000 + DSM_MOTION_ACC_MAX;
        default:
            LOGW("mr_plat(code:%d, param:%d) not impl!", code, param);
            break;
    }
    return MR_IGNORE;
}

static T_DSM_FREE_SAPCE dsm_free_sapce;

/*增强的平台扩展接口*/
int32 mr_platEx(int32 code, uint8 *input, int32 input_len, uint8 **output, int32 *output_len, MR_PLAT_EX_CB *cb) {
    LOGI("mr_platEx code=%d in=@%p inlen=%d out=@%p outlen=@%p cb=@%p", code, input, input_len, output, output_len, cb);

    int media_cmd = code / 10;
    int media_device = code % 10;
    if (media_device != 0 &&
        (media_cmd == MR_MEDIA_OPEN_MUTICHANNEL ||
         media_cmd == MR_MEDIA_PLAY_MUTICHANNEL ||
         media_cmd == MR_MEDIA_STOP_MUTICHANNEL ||
         media_cmd == MR_MEDIA_CLOSE_MUTICHANNEL)) {
        return dsm_media_channel_platEx(media_cmd, media_device, input, input_len);
    }
    if (media_device != 0 &&
        (media_cmd == MR_MEDIA_INIT ||
         media_cmd == MR_MEDIA_FILE_LOAD ||
         media_cmd == MR_MEDIA_BUF_LOAD ||
         media_cmd == MR_MEDIA_PLAY_CUR_REQ ||
         media_cmd == MR_MEDIA_PAUSE_REQ ||
         media_cmd == MR_MEDIA_RESUME_REQ ||
         media_cmd == MR_MEDIA_STOP_REQ ||
         media_cmd == MR_MEDIA_CLOSE ||
         media_cmd == MR_MEDIA_GET_STATUS ||
         media_cmd == MR_MEDIA_SETPOS ||
         media_cmd == MR_MEDIA_GETTIME ||
         media_cmd == MR_MEDIA_GET_TOTAL_TIME ||
         media_cmd == MR_MEDIA_GET_CURTIME ||
         media_cmd == MR_MEDIA_GET_CURTIME_MSEC ||
         media_cmd == MR_MEDIA_FREE)) {
        return dsm_media_platEx(media_cmd, media_device, input, input_len, output, output_len);
    }

    switch (code) {
        case 1012:  //申请内部cache
        case 1013:  //释放内部cache
            return MR_IGNORE;
        case 1014: {  //申请拓展内存
            // *output_len = SCRW * SCRH * 4;
            // *output = malloc(*output_len);
            // LOGI("malloc exRam addr=%p len=%d", output, output_len);
            // ret= MR_SUCCESS;
            return MR_IGNORE;
        }
        case 1015: {  //释放拓展内存
            // LOGI("free exRam");
            // free(input);
            // ret= MR_SUCCESS;
            return MR_IGNORE;
        }
        case MR_TUROFFBACKLIGHT:  //关闭背光常亮
        case MR_TURONBACKLIGHT:   //开启背光常亮
            return MR_SUCCESS;
        case MR_SWITCHPATH:  //切换跟目录 1204
            return dsmSwitchPath(input, input_len, output, output_len);
            // case MR_GET_FREE_SPACE:

        case MR_CHARACTER_HEIGHT: {  // 1201，返回默认（中号/gb16）字体尺寸
            static int32 wordInfo = (CHAR_H_16 << 24) | (EN_CHAR_W_16 << 16) | (CHAR_H_16 << 8) | (CN_CHAR_W_16);
            *output = (unsigned char *)&wordInfo;
            *output_len = 4;
            return MR_SUCCESS;
        }

        case 1116: {  //获取编译时间
            static char buf[32];
            int l = snprintf_(buf, sizeof(buf), "%s %s", __TIME__, __DATE__);
            *output = (uint8 *)buf;  //"2013/3/21 21:36";
            *output_len = l + 1;
            LOGI("build time %s", buf);
            return MR_SUCCESS;
        }

        case 1224:  //小区信息ID
        case 1307:  //获取SIM卡个数，非多卡多待直接返回 MR_INGORE
            return MR_IGNORE;
        case 4033:  // 浏览器渲染引擎初始化（wbrw在加载主页前调用）
            return MR_SUCCESS;

        case 0x90004:
            /*
             * gghjt 的 netpay 超时返回路径在收到返回键后先调用 platEx(0x90004)
             * 查询/提交一段 48 字节的计费状态，然后才会回到 dump0 -> gghjt.pak 的
             * 恢复流程。反汇编定位到调用点在 netpay 恢复后的 0x64D551；default 的
             * MR_IGNORE 会让 ARM 代码把请求当成“平台未接管”，继续走
             * mr_getNetworkID/mr_initNetwork 轮询，主事件循环被占住，后续返回点击到不了。
             * 桌面宿主不提供真实网络计费通道；该私有 code 只用于查询平台是否
             * 接管计费状态。返回 MR_IGNORE 与未实现分支的语义一致，但避免超时
             * 路径反复刷 warning。真正让 UI 保持可返回的是 table[81] 的异步网络
             * 语义，见 arm_ext_executor.c。
             */
            return MR_IGNORE;

        case MR_GET_FREE_SPACE: {  // 1305 获得指定盘符的剩余空间大小
            // 真机数据, 可以看出内存地址是一样的，因此返回的内存不需要释放
            // mrc_sprintf(buf, "%s %p %d Info:totalSpace=%d/%d, freeSpace=%d/%d\n", disk, getInfo, len, getInfo->total, getInfo->tunit, getInfo->account, getInfo->unit);
            // A 0834b690 16 Info:totalSpace=1722/1024, freeSpace=1271/1024
            // B 0834b690 16 Info:totalSpace=95/1024, freeSpace=77/1024
            // C 0834b690 16 Info:totalSpace=1874/1048576, freeSpace=1873/1048576
            switch (*input) {
                case 'A':
                case 'a':
                    *output_len = sizeof(T_DSM_FREE_SAPCE);
                    *output = (void *)&dsm_free_sapce;
                    dsm_free_sapce.total = 1722;
                    dsm_free_sapce.tunit = 1024;
                    dsm_free_sapce.account = 1271;
                    dsm_free_sapce.unit = 1024;
                    return MR_SUCCESS;
                case 'B':
                case 'b':
                    *output_len = sizeof(T_DSM_FREE_SAPCE);
                    *output = (void *)&dsm_free_sapce;
                    dsm_free_sapce.total = 95;
                    dsm_free_sapce.tunit = 1024;
                    dsm_free_sapce.account = 77;
                    dsm_free_sapce.unit = 1024;
                    return MR_SUCCESS;
                case 'C':
                case 'c':
                    *output_len = sizeof(T_DSM_FREE_SAPCE);
                    *output = (void *)&dsm_free_sapce;
                    dsm_free_sapce.total = 1874;
                    dsm_free_sapce.tunit = 1024 * 1024;
                    dsm_free_sapce.account = 1873;
                    dsm_free_sapce.unit = 1024 * 1024;
                    return MR_SUCCESS;
            }
            return MR_IGNORE;
        }

        case 1017: {  //获得信号强度。
            static T_RX rx = {3, 5, 5, 1};
            *output = (uint8 *)&rx;
            *output_len = sizeof(T_RX);
            return MR_SUCCESS;
        }
        case MR_UCS2GB: {  // 1207
            if (*output) {
                char *gbstr = UCS2BEStrToGBStr((uint16 *)input, NULL);
                strcpy2((char *)*output, gbstr);
                // strncpy2((char *)*output, gbstr, *output_len); // qq浏览器*output_len传的是0导致无法保存设置
                mr_freeExt(gbstr);
            } else {
                *output = (uint8 *)UCS2BEStrToGBStr((uint16 *)input, (uint32 *)output_len);
            }
            // LOGI("gbstr:%s %d", (char *)*output, *output_len);
            return MR_SUCCESS;
        }

        default: {
            LOGW("mr_platEx(code=%d, in=%#p, inlen=%d) not impl!", code, input, input_len);
            break;
        }
    }

    return MR_IGNORE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef int32 (*MR_NETWORK_CB)(int32 result);
typedef struct networkData_st {
    MR_NETWORK_CB cb;  // MR_GET_HOST_CB和MR_INIT_NETWORK_CB其实是一样的，因此可以用同一个函数签名
    int isExtCB;
} networkData_st;

static int32 network_cb(int32 result, void *userData) {
    networkData_st *data = (networkData_st *)userData;

    register int32 ret = MR_FAILED;
    register MR_NETWORK_CB cb = data->cb;
    if (data->isExtCB) {  // ext的回调，需要设置r9
        extern mr_c_function_st *mr_c_function_P;
        register void *oldR9 = getR9();
        fixR9_saveMythroad();
        setR9(mr_c_function_P->start_of_ER_RW);
        ret = cb(result);
        setR9(oldR9);
    } else {
        ret = cb(result);
    }
    mr_freeExt(data);
    return ret;
}

static int32 initNetwork(MR_INIT_NETWORK_CB cb, const char *mode, int isExtCB) {
    int32 ret;
    networkData_st *data = mr_mallocExt(sizeof(networkData_st));
    data->cb = (MR_NETWORK_CB)cb;
    data->isExtCB = isExtCB;
    LOGI("mr_initNetwork(mod:%s)", mode);
    // MR_SUCCESS 同步模式，初始化成功，不再调用cb
    // MR_FAILED （立即感知的）失败，不再调用cb
    // MR_WAITING 使用回调函数通知引擎初始化结果
    ret = dsmInFuncs->initNetwork(network_cb, mode, data);
    if (ret != MR_WAITING) {
        mr_freeExt(data);
    }
    return ret;
}

static int32 getHostByName(const char *ptr, MR_GET_HOST_CB cb, int isExtCB) {
    int32 ret;
    networkData_st *data = mr_mallocExt(sizeof(networkData_st));
    data->cb = (MR_NETWORK_CB)cb;
    data->isExtCB = isExtCB;
    // MR_FAILED （立即感知的）失败，不再调用cb
    // MR_WAITING 使用回调函数通知引擎获取IP的结果
    // 其他值 同步模式，立即返回的IP地址，不再调用cb
    ret = dsmInFuncs->getHostByName(ptr, network_cb, data);
    if (ret != MR_WAITING) {
        mr_freeExt(data);
    }
    return ret;
}

// 此函数只能由mythroad层自身调用
int32 mythroad_initNetwork(MR_INIT_NETWORK_CB cb, const char *mode) {
    return initNetwork(cb, mode, 0);
}

// 此函数由ext调用，需要注意回调函数执行时r9寄存器的问题
int32 mr_initNetwork(MR_INIT_NETWORK_CB cb, const char *mode) {
    return initNetwork(cb, mode, 1);
}

// 此函数只能由mythroad层自身调用
int32 mythroad_getHostByName(const char *ptr, MR_GET_HOST_CB cb) {
    return getHostByName(ptr, cb, 0);
}

// 此函数由ext调用，需要注意回调函数执行时r9寄存器的问题
int32 mr_getHostByName(const char *ptr, MR_GET_HOST_CB cb) {
    return getHostByName(ptr, cb, 1);
}

int32 mr_closeNetwork() {
    LOGI("%s", "mr_closeNetwork");
    return dsmInFuncs->mr_closeNetwork();
}

int32 mr_socket(int32 type, int32 protocol) {
    LOGI("mr_socket(type:%d, protocol:%d)", type, protocol);
    return dsmInFuncs->mr_socket(type, protocol);
}

int32 mr_connect(int32 s, int32 ip, uint16 port, int32 type) {
    LOGI("mr_connect(s:%d, ip:%d, port:%d, type:%d)", s, ip, port, type);
    return dsmInFuncs->mr_connect(s, ip, port, type);
}

int32 mr_getSocketState(int32 s) {
    LOGI("getSocketState(%d)", s);
    return dsmInFuncs->mr_getSocketState(s);
}

int32 mr_closeSocket(int32 s) {
    LOGI("mr_closeSocket(%d)", s);
    return dsmInFuncs->mr_closeSocket(s);
}

int32 mr_recv(int32 s, char *buf, int len) {
    LOGI("mr_recv(%d)", s);
    return dsmInFuncs->mr_recv(s, buf, len);
}

int32 mr_send(int32 s, const char *buf, int len) {
    LOGI("mr_send %d %s", s, buf);
    return dsmInFuncs->mr_send(s, buf, len);
}

int32 mr_recvfrom(int32 s, char *buf, int len, int32 *ip, uint16 *port) {
    LOGI("mr_recvfrom(%d,%s,%d,%d,%d)", s, buf, len, *ip, *port);
    return dsmInFuncs->mr_recvfrom(s, buf, len, ip, port);
}

int32 mr_sendto(int32 s, const char *buf, int len, int32 ip, uint16 port) {
    LOGI("mr_sendto(%d,%s,%d,%d,%d)", s, buf, len, ip, port);
    return dsmInFuncs->mr_sendto(s, buf, len, ip, port);
}

#ifdef USE_GET_SCREEN_BUFFER
uint16 *mr_getScreenBuffer(void) {
    return dsmInFuncs->getScreenBuffer();
}
#endif

// Anti-Apple
#if defined(__APPLE__) || defined(__OSX__)
#error "我不喜欢苹果公司及其产品，禁止在任何苹果公司的产品中使用此代码"
#error "I don't like Apple and its products, and I forbid using this code in any Apple product"
#endif

void dsm_prepare(void) {
    if (getenv("SKYENGINE_LOG")) {
        fprintf(stderr, "[dsm_prepare] mkDir...\n");
        fflush(stderr);
    }
    dsmInFuncs->mkDir(MYTHROAD_PATH);
    dsmInFuncs->mkDir(DSM_HIDE_DRIVE);
    dsmInFuncs->mkDir(DSM_DRIVE_A);
    dsmInFuncs->mkDir(DSM_DRIVE_B);
    dsmInFuncs->mkDir(DSM_DRIVE_X);
    if (getenv("SKYENGINE_LOG")) {
        fprintf(stderr, "[dsm_prepare] xl_font_sky16_init...\n");
        fflush(stderr);
    }
    xl_font_sky16_init();
    if (getenv("SKYENGINE_LOG")) {
        fprintf(stderr, "[dsm_prepare] xl_font_sky12_init...\n");
        fflush(stderr);
    }
    xl_font_sky12_init();
    if (getenv("SKYENGINE_LOG")) {
        fprintf(stderr, "[dsm_prepare] encode_init...\n");
        fflush(stderr);
    }
    if (encode_init() == MR_FAILED) {
        LOGW("%s", "encode load fail");
    }
    if (getenv("SKYENGINE_LOG")) {
        fprintf(stderr, "[dsm_prepare] done\n");
        fflush(stderr);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////

int32 dsm_init(DSM_REQUIRE_FUNCS *inFuncs) {
    // 注意！这里面只能做一些不涉及malloc()的操作
    dsmInFuncs = inFuncs;
    dsmStartTime = dsmInFuncs->get_uptime_ms();
    holdTextMem = NULL;
    /* LCD 旋转/动感芯片是 guest 运行期经 plat 请求的状态,随 DSM 初始化归零 */
    dsm_set_lcd_rotation(0);
    dsm_motion_reset();
    dsm_mrp_reset_all();
    dsm_media_reset_all();

#ifdef DSM_FULL
    mr_tm_init();
    mr_baselib_init();
    mr_tablib_init();
    mr_socket_target_init();
    mr_tcp_target_init();
    mr_iolib_target_init();
    mr_strlib_init();
    mr_pluto_init();
#endif
    mythroad_init();
    return VMRP_VER;
}
