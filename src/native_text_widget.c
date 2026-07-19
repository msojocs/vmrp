#include "./include/native_text_widget.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./include/bridge.h"
#include "./include/fileLib.h"
#include "./include/types.h"
#include "./include/vmrp.h"

/*
 * 平台文本框(黑底绿字全屏文本页)的宿主实现,语义与布局说明:
 *
 * - SKYENGINE 手册(mr_textCreate.md):title/text 为 UCS2 大端;type 取
 *   MR_DIALOG_OK(0)/MR_DIALOG_OK_CANCEL(1)/MR_DIALOG_CANCEL(2),决定底部
 *   出现"确定"(左软键)/"取消"(右软键)哪些按钮;用户选择按钮时平台通过
 *   mr_event(MR_DIALOG_EVENT, MR_DIALOG_KEY_OK/CANCEL) 通知应用。
 * - 渲染直接读取 Mythroad 平台字库 system/gb16.uc2(UCS2 码点索引,每字
 *   32 字节,16 行 x 2 字节,MSB 在左;ASCII 宽 8,汉字宽 16),与 DSM 层
 *   xl_font_sky16_drawChar 的解码方式一致,避免依赖 DSM 内部 static 函数。
 * - 文本框显示期间 guest 的上屏帧被 capture_frame 截留进"显示镜像",
 *   关闭时把镜像整帧重推上屏:恢复的是应用最后一帧画面本身,不要求应用
 *   收到取消事件后主动重绘(部分应用只弹出模态状态,不重绘背景)。
 */

#define TW_COLOR_TEXT 0x07E0 /* RGB565 纯绿,经典平台文本页配色 */

/* 布局常量(随显示尺寸自适应,数值在 240x320 下与经典平台文本页一致):
 * 标题(8,8);正文自标题下方 8px 起,行距 18;底部软键栏高 26px,顶部 1px
 * 分隔线,标签垂直居中(栏顶+5)。 */
#define TW_MARGIN_X 8
#define TW_TITLE_Y 8
#define TW_CHAR_H 16
#define TW_LINE_H 18
#define TW_BODY_Y (TW_TITLE_Y + TW_CHAR_H + 8)
#define TW_SOFTBAR_H 26
#define TW_LABEL_MARGIN_X 4

/* gb16.uc2 字库:每字符 32 字节 */
#define TW_FONT_BYTES_PER_CHAR 32

typedef struct {
    int active;
    int32_t handle;      /* 递增句柄,>0(SKYENGINE 契约:>0 有效) */
    int32_t type;        /* MR_DIALOG_OK / MR_DIALOG_OK_CANCEL / MR_DIALOG_CANCEL */
    uint16_t *title;     /* UCS2 码点数组(已转主机序),NUL 结尾 */
    uint16_t *text;
    int scroll;          /* 正文首个可见行号 */
    int line_count;      /* 正文按当前宽度折行后的总行数 */
} TextWidget;

static TextWidget tw;
static int32_t tw_handle_gen = 0;

/* guest 显示镜像:累积每次上屏的矩形,始终保存"应用画面当前应有的样子" */
static uint16_t *tw_mirror = NULL;
static int tw_mirror_w = 0;
static int tw_mirror_h = 0;
static int tw_mirror_valid = 0;

/* 文本框自身上屏时置位,防止把文本页写进镜像/被自己截留 */
static int tw_presenting = 0;

/* ---------------- 字库 ---------------- */

static int32_t tw_font_fd = 0;

/* 打开平台 16px 字库。失败返回 0:调用方按 MR_FAILED 处理,不做降级绘制。
 * 路径说明:进程已 chdir 到 --work-dir(vmrp.c),guest 文件系统根对应宿主
 * "mythroad/"(dsm.c MYTHROAD_PATH),故 guest 的 system/gb16.uc2 在宿主侧
 * 是 mythroad/system/gb16.uc2;my_open 接受宿主相对路径。 */
static int tw_font_open(void) {
    if (tw_font_fd > 0) return 1;
    tw_font_fd = my_open("mythroad/system/gb16.uc2", MR_FILE_RDONLY);
    return tw_font_fd > 0;
}

static int tw_char_width(uint16_t ch) {
    return ch < 128 ? 8 : 16;
}

/* 读取一个字符的 16 行点阵(每行 uint16,MSB 为最左像素) */
static int tw_font_glyph(uint16_t ch, uint16_t rows[TW_CHAR_H]) {
    uint8_t buf[TW_FONT_BYTES_PER_CHAR];
    if (my_seek(tw_font_fd, (int32_t)ch * TW_FONT_BYTES_PER_CHAR, 0) < 0) return 0;
    if (my_read(tw_font_fd, buf, TW_FONT_BYTES_PER_CHAR) != TW_FONT_BYTES_PER_CHAR) return 0;
    for (int iy = 0; iy < TW_CHAR_H; iy++) {
        rows[iy] = (uint16_t)((buf[iy * 2] << 8) | buf[iy * 2 + 1]);
    }
    return 1;
}

/* ---------------- UCS2 字符串工具 ---------------- */

/* UCS2-BE 字节流复制成主机序码点数组(guest 指针仅在调用期间有效,必须复制) */
static uint16_t *tw_ucs2be_dup(const char *s) {
    const uint8_t *p = (const uint8_t *)s;
    size_t n = 0;
    if (p != NULL) {
        while (((uint16_t)(p[n * 2] << 8) | p[n * 2 + 1]) != 0) n++;
    }
    uint16_t *out = (uint16_t *)malloc((n + 1) * sizeof(uint16_t));
    if (out == NULL) return NULL;
    for (size_t i = 0; i < n; i++) {
        out[i] = (uint16_t)((p[i * 2] << 8) | p[i * 2 + 1]);
    }
    out[n] = 0;
    return out;
}

/* ---------------- 渲染 ---------------- */

static void tw_draw_char(uint16_t *page, int pw, int ph, uint16_t ch, int x, int y) {
    uint16_t rows[TW_CHAR_H];
    if (!tw_font_glyph(ch, rows)) return;
    int w = tw_char_width(ch);
    for (int iy = 0; iy < TW_CHAR_H; iy++) {
        int py = y + iy;
        if (py < 0 || py >= ph) continue;
        for (int ix = 0; ix < w; ix++) {
            int px = x + ix;
            if (px < 0 || px >= pw) continue;
            if (rows[iy] & (uint16_t)(1u << (15 - ix))) {
                page[(size_t)py * (size_t)pw + (size_t)px] = TW_COLOR_TEXT;
            }
        }
    }
}

static int tw_draw_string(uint16_t *page, int pw, int ph, const uint16_t *s, int x, int y) {
    for (; *s != 0; s++) {
        tw_draw_char(page, pw, ph, *s, x, y);
        x += tw_char_width(*s);
    }
    return x;
}

static int tw_string_width(const uint16_t *s) {
    int w = 0;
    for (; *s != 0; s++) w += tw_char_width(*s);
    return w;
}

/*
 * 正文折行遍历:按显示宽度折行('\n' 强制换行,'\r' 忽略),对每个可见字符
 * 回调行号/坐标。render 与行数统计共用,保证滚动边界与实际绘制一致。
 */
typedef void (*tw_line_char_fn)(void *ctx, uint16_t ch, int line, int x);

static int tw_layout_body(const uint16_t *s, int pw, tw_line_char_fn fn, void *ctx) {
    int line = 0;
    int x = TW_MARGIN_X;
    for (; *s != 0; s++) {
        uint16_t ch = *s;
        if (ch == '\r') continue;
        if (ch == '\n') {
            line++;
            x = TW_MARGIN_X;
            continue;
        }
        int w = tw_char_width(ch);
        if (x + w > pw - TW_MARGIN_X) {
            line++;
            x = TW_MARGIN_X;
        }
        if (fn != NULL) fn(ctx, ch, line, x);
        x += w;
    }
    return line + 1;
}

typedef struct {
    uint16_t *page;
    int pw, ph;
    int first_line;   /* 首个可见正文行 */
    int max_lines;    /* 可见行数 */
} TwRenderCtx;

static void tw_render_body_char(void *vctx, uint16_t ch, int line, int x) {
    TwRenderCtx *ctx = (TwRenderCtx *)vctx;
    int vis = line - ctx->first_line;
    if (vis < 0 || vis >= ctx->max_lines) return;
    tw_draw_char(ctx->page, ctx->pw, ctx->ph, ch, x, TW_BODY_Y + vis * TW_LINE_H);
}

/* 可见正文行数(正文区 = 正文起点到软键栏分隔线上方) */
static int tw_visible_lines(int ph) {
    int body_h = (ph - TW_SOFTBAR_H) - TW_BODY_Y;
    return body_h > 0 ? body_h / TW_LINE_H : 0;
}

/* 整页渲染并上屏(黑底绿字;上屏经 guiDrawBitmap 走统一 present 计数) */
static void tw_render_and_present(void) {
    int pw = vmrp_display_width();
    int ph = vmrp_display_height();
    uint16_t *page = (uint16_t *)calloc((size_t)pw * (size_t)ph, sizeof(uint16_t));
    if (page == NULL) return;

    /* 标题 */
    tw_draw_string(page, pw, ph, tw.title, TW_MARGIN_X, TW_TITLE_Y);

    /* 正文(带滚动窗口) */
    TwRenderCtx ctx = {page, pw, ph, tw.scroll, tw_visible_lines(ph)};
    tw.line_count = tw_layout_body(tw.text, pw, tw_render_body_char, &ctx);

    /* 底部软键栏:顶部 1px 分隔线 + 按 type 显示按钮标签 */
    int bar_y = ph - TW_SOFTBAR_H;
    if (bar_y >= 0 && bar_y < ph) {
        for (int x = 0; x < pw; x++) page[(size_t)bar_y * (size_t)pw + (size_t)x] = TW_COLOR_TEXT;
    }
    static const uint16_t label_ok[] = {0x786E, 0x5B9A, 0};     /* 确定 */
    static const uint16_t label_cancel[] = {0x53D6, 0x6D88, 0}; /* 取消 */
    int label_y = bar_y + 5;
    if (tw.type == MR_DIALOG_OK || tw.type == MR_DIALOG_OK_CANCEL) {
        tw_draw_string(page, pw, ph, label_ok, TW_LABEL_MARGIN_X, label_y);
    }
    if (tw.type == MR_DIALOG_OK_CANCEL || tw.type == MR_DIALOG_CANCEL) {
        tw_draw_string(page, pw, ph, label_cancel,
                       pw - tw_string_width(label_cancel) - TW_LABEL_MARGIN_X, label_y);
    }

    /* 上屏。tw_presenting 防止本帧被 capture_frame 写入镜像或截留 */
    tw_presenting = 1;
    guiDrawBitmap(page, 0, 0, pw, ph);
    tw_presenting = 0;
    free(page);
}

/* ---------------- 镜像 ---------------- */

int native_text_widget_capture_frame(const uint16_t *bmp, int32_t x, int32_t y,
                                     int32_t w, int32_t h, int32_t stride,
                                     int32_t sx, int32_t sy) {
    if (tw_presenting) return 0; /* 文本框自身的上屏帧:直通,不入镜像 */
    if (bmp == NULL || stride <= 0 || w <= 0 || h <= 0) return 0;

    int dw = vmrp_display_width();
    int dh = vmrp_display_height();
    if (tw_mirror == NULL || tw_mirror_w != dw || tw_mirror_h != dh) {
        /* 显示尺寸变化(plat(101) 旋转):镜像重建,旧内容对新尺寸无意义 */
        free(tw_mirror);
        tw_mirror = (uint16_t *)calloc((size_t)dw * (size_t)dh, sizeof(uint16_t));
        tw_mirror_w = dw;
        tw_mirror_h = dh;
        tw_mirror_valid = 0;
        if (tw_mirror == NULL) return 0;
    }
    /* 与 guiDrawBitmapWithStride 相同的裁剪规则,保证镜像与真实上屏一致 */
    for (int32_t j = 0; j < h; j++) {
        int32_t yy = y + j;
        int32_t syy = sy + j;
        if (yy < 0 || yy >= dh || syy < 0) continue;
        for (int32_t i = 0; i < w; i++) {
            int32_t xx = x + i;
            int32_t sxx = sx + i;
            if (xx < 0 || xx >= dw || sxx < 0 || sxx >= stride) continue;
            tw_mirror[(size_t)yy * (size_t)dw + (size_t)xx] =
                bmp[(size_t)syy * (size_t)stride + (size_t)sxx];
        }
    }
    tw_mirror_valid = 1;

    /* 显示期间截留 guest 帧:平台窗口在应用画面之上(真机语义) */
    return tw.active;
}

/* 关闭时把镜像整帧重推上屏,露出被文本框遮住的应用画面 */
static void tw_present_mirror(void) {
    if (!tw_mirror_valid || tw_mirror == NULL) return;
    if (tw_mirror_w != vmrp_display_width() || tw_mirror_h != vmrp_display_height()) return;
    tw_presenting = 1;
    guiDrawBitmap(tw_mirror, 0, 0, tw_mirror_w, tw_mirror_h);
    tw_presenting = 0;
}

/* ---------------- 生命周期 ---------------- */

static void tw_free_content(void) {
    free(tw.title);
    free(tw.text);
    tw.title = NULL;
    tw.text = NULL;
}

int native_text_widget_active(void) {
    return tw.active;
}

int32_t native_text_widget_create(const char *title_ucs2be, const char *text_ucs2be, int32_t type) {
    if (type != MR_DIALOG_OK && type != MR_DIALOG_OK_CANCEL && type != MR_DIALOG_CANCEL) {
        return MR_FAILED;
    }
    if (!tw_font_open()) {
        /* 平台字库缺失时无法按契约渲染,保持原有的失败语义 */
        return MR_FAILED;
    }
    uint16_t *title = tw_ucs2be_dup(title_ucs2be);
    uint16_t *text = tw_ucs2be_dup(text_ucs2be);
    if (title == NULL || text == NULL) {
        free(title);
        free(text);
        return MR_FAILED;
    }
    if (tw.active) {
        /* 重复创建视为替换:旧文本框直接被新窗口覆盖,旧句柄作废 */
        tw_free_content();
    }
    tw.title = title;
    tw.text = text;
    tw.type = type;
    tw.scroll = 0;
    tw.active = 1;
    tw.handle = ++tw_handle_gen > 0 ? tw_handle_gen : (tw_handle_gen = 1);
    tw_render_and_present();
    return tw.handle;
}

int32_t native_text_widget_release(int32_t handle) {
    if (!tw.active || handle != tw.handle) return MR_FAILED;
    tw.active = 0;
    tw_free_content();
    /* 恢复应用画面(镜像中保存的 guest 最后一帧) */
    tw_present_mirror();
    return MR_SUCCESS;
}

int32_t native_text_widget_refresh(int32_t handle, const char *title_ucs2be, const char *text_ucs2be) {
    if (!tw.active || handle != tw.handle) return MR_FAILED;
    uint16_t *title = tw_ucs2be_dup(title_ucs2be);
    uint16_t *text = tw_ucs2be_dup(text_ucs2be);
    if (title == NULL || text == NULL) {
        free(title);
        free(text);
        return MR_FAILED;
    }
    tw_free_content();
    tw.title = title;
    tw.text = text;
    tw.scroll = 0;
    tw_render_and_present();
    return MR_SUCCESS;
}

/* ---------------- 事件 ---------------- */

int native_text_widget_filter_event(int32_t code, int32_t p0, int32_t *dialog_param) {
    if (!tw.active) return 0;
    /* 显示期间平台窗口拥有全部输入:按键与触屏都不再到达应用 */
    switch (code) {
        case MR_KEY_PRESS:
            break;
        case MR_KEY_RELEASE:
        case MR_MOUSE_DOWN:
        case MR_MOUSE_UP:
        case MR_MOUSE_MOVE:
            return 1;
        default:
            return 0; /* 定时器/网络等非输入事件照常投递 */
    }
    switch (p0) {
        case MR_KEY_SOFTLEFT:
            if (tw.type == MR_DIALOG_OK || tw.type == MR_DIALOG_OK_CANCEL) {
                *dialog_param = MR_DIALOG_KEY_OK;
                return 2;
            }
            return 1;
        case MR_KEY_SOFTRIGHT:
            if (tw.type == MR_DIALOG_OK_CANCEL || tw.type == MR_DIALOG_CANCEL) {
                *dialog_param = MR_DIALOG_KEY_CANCEL;
                return 2;
            }
            return 1;
        case MR_KEY_UP:
            if (tw.scroll > 0) {
                tw.scroll--;
                tw_render_and_present();
            }
            return 1;
        case MR_KEY_DOWN: {
            int vis = tw_visible_lines(vmrp_display_height());
            if (tw.scroll + vis < tw.line_count) {
                tw.scroll++;
                tw_render_and_present();
            }
            return 1;
        }
        default:
            return 1;
    }
}
