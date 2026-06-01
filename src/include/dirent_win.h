#ifndef DIRENT_WIN_H
#define DIRENT_WIN_H

#ifdef _MSC_VER

#include <windows.h>
#include <string.h>

struct dirent {
    char d_name[MAX_PATH];
};

typedef struct {
    HANDLE hFind;
    WIN32_FIND_DATAA fdata;
    struct dirent entry;
    int first;
} DIR;

static inline DIR *opendir(const char *name) {
    char pattern[MAX_PATH];
    if (strlen(name) + 3 > MAX_PATH) return NULL;
    snprintf(pattern, MAX_PATH, "%s\\*", name);

    DIR *d = (DIR *)malloc(sizeof(DIR));
    if (!d) return NULL;

    d->hFind = FindFirstFileA(pattern, &d->fdata);
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
        if (!FindNextFileA(d->hFind, &d->fdata)) return NULL;
    }
    strncpy(d->entry.d_name, d->fdata.cFileName, MAX_PATH - 1);
    d->entry.d_name[MAX_PATH - 1] = '\0';
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
