#ifndef DIRENT_WIN_H
#define DIRENT_WIN_H

#ifdef _MSC_VER

#include <windows.h>
#include <string.h>
#include <stdlib.h>

struct dirent {
    char d_name[MAX_PATH];
};

typedef struct {
    HANDLE hFind;
    WIN32_FIND_DATAW fdata;
    struct dirent entry;
    int first;
} DIR;

static inline wchar_t *dirent_utf8_to_wide(const char *text) {
    int len;
    wchar_t *wide;
    if (!text) return NULL;
    len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text, -1, NULL, 0);
    if (len <= 0) return NULL;
    wide = (wchar_t *)malloc((size_t)len * sizeof(wchar_t));
    if (!wide) return NULL;
    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text, -1, wide, len) <= 0) {
        free(wide);
        return NULL;
    }
    return wide;
}

static inline int dirent_wide_to_utf8(const wchar_t *wide, char *out, int out_len) {
    int len;
    if (!wide || !out || out_len <= 0) return 0;
    len = WideCharToMultiByte(CP_UTF8, 0, wide, -1, out, out_len, NULL, NULL);
    if (len <= 0) {
        out[0] = '\0';
        return 0;
    }
    out[out_len - 1] = '\0';
    return 1;
}

static inline DIR *opendir(const char *name) {
    wchar_t *wname = dirent_utf8_to_wide(name);
    wchar_t pattern[MAX_PATH];
    size_t name_len;
    if (!wname) return NULL;
    name_len = wcslen(wname);
    if (name_len + 3 > MAX_PATH) {
        free(wname);
        return NULL;
    }
    swprintf(pattern, MAX_PATH, L"%ls\\*", wname);
    free(wname);

    DIR *d = (DIR *)malloc(sizeof(DIR));
    if (!d) return NULL;

    d->hFind = FindFirstFileW(pattern, &d->fdata);
    if (d->hFind == INVALID_HANDLE_VALUE) {
        free(d);
        return NULL;
    }
    d->first = 1;
    return d;
}

static inline struct dirent *readdir(DIR *d) {
    if (!d) return NULL;
    if (d->first) {
        d->first = 0;
    } else {
        if (!FindNextFileW(d->hFind, &d->fdata)) return NULL;
    }
    if (!dirent_wide_to_utf8(d->fdata.cFileName, d->entry.d_name, MAX_PATH)) {
        return NULL;
    }
    return &d->entry;
}

static inline int closedir(DIR *d) {
    if (!d) return -1;
    FindClose(d->hFind);
    free(d);
    return 0;
}

#endif /* _MSC_VER */
#endif /* DIRENT_WIN_H */
