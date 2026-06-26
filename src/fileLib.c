#include "./include/fileLib.h"
#include "./include/utils.h"
#include "./include/compat_msvc.h"

#include <string.h>
#ifdef _MSC_VER
#include "./include/dirent_win.h"
#else
#include <dirent.h>
#endif
#include <fcntl.h>
#include <malloc.h>
#include <sys/stat.h>
#include <zlib.h>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#define VMRP_OPEN_PERMS (_S_IREAD | _S_IWRITE)
#ifndef O_RAW
#define O_RAW _O_BINARY
#endif
#else
#include <unistd.h>
#define VMRP_OPEN_PERMS (S_IRWXU | S_IRWXG | S_IRWXO)
#endif

// mrc_open需要返回0表示失败
static struct rb_root filef_map = RB_ROOT;
static uint32_t filef_count = 0;

// mrc_findStart需要返回-1表示失败
static struct rb_root dirf_map = RB_ROOT;
static uint32_t dirf_count = 0;

#ifdef _WIN32
static wchar_t *utf8_to_wide(const char *text) {
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

static int utf8_to_wide_mode(const char *mode, wchar_t *out, size_t out_len) {
    size_t len;
    if (!mode || !out || out_len == 0) return MR_FAILED;
    len = strlen(mode);
    if (len >= out_len) return MR_FAILED;
    for (size_t i = 0; i <= len; ++i) {
        out[i] = (wchar_t)(unsigned char)mode[i];
    }
    return MR_SUCCESS;
}
#endif

FILE *vmrp_host_fopen(const char *filename, const char *mode) {
#ifdef _WIN32
    wchar_t *wfilename = utf8_to_wide(filename);
    wchar_t wmode[16];
    FILE *fp;
    if (!wfilename) return NULL;
    if (utf8_to_wide_mode(mode, wmode, sizeof(wmode) / sizeof(wmode[0])) != MR_SUCCESS) {
        free(wfilename);
        return NULL;
    }
    fp = _wfopen(wfilename, wmode);
    free(wfilename);
    return fp;
#else
    return fopen(filename, mode);
#endif
}

int32_t vmrp_host_chdir(const char *name) {
#ifdef _WIN32
    wchar_t *wname = utf8_to_wide(name);
    int ret;
    if (!wname) return MR_FAILED;
    ret = _wchdir(wname);
    free(wname);
    return ret == 0 ? MR_SUCCESS : MR_FAILED;
#else
    return chdir(name) == 0 ? MR_SUCCESS : MR_FAILED;
#endif
}

int32_t my_open(const char *filename, uint32_t mode) {
    int f;
    int new_mode = 0;

    if (mode & MR_FILE_RDONLY) new_mode = O_RDONLY;
    if (mode & MR_FILE_WRONLY) new_mode = O_WRONLY;
    if (mode & MR_FILE_RDWR) new_mode = O_RDWR;
    if (mode & MR_FILE_CREATE) new_mode |= O_CREAT;

#ifdef _WIN32
    new_mode |= O_RAW;
    wchar_t *wfilename = utf8_to_wide(filename);
    if (!wfilename) {
        return 0;
    }
    f = _wopen(wfilename, new_mode, VMRP_OPEN_PERMS);
    free(wfilename);
#else
    f = open((char *)filename, new_mode, VMRP_OPEN_PERMS);
#endif

    if (f == -1) {
        return 0;
    }

    filef_count++;
    uIntMap *obj = malloc(sizeof(uIntMap));
    obj->key = filef_count;
    obj->data = (void *)(intptr_t)f;
    uIntMap_insert(&filef_map, obj);
    return filef_count;
}

int32_t my_close(int32_t f) {
    uIntMap *obj = uIntMap_delete(&filef_map, f);
    if (obj == NULL) {
        return MR_FAILED;
    }
    if (f == filef_count) {
        filef_count--;
    }
    int fh = (int)(intptr_t)obj->data;
    free(obj);
    if (close(fh) != 0) {
        return MR_FAILED;
    }
    return MR_SUCCESS;
}

int32_t my_seek(int32_t f, int32_t pos, int method) {
    uIntMap *obj = uIntMap_search(&filef_map, f);
    if (obj == NULL) {
        return MR_FAILED;
    }
    off_t ret = lseek((int)(intptr_t)obj->data, (off_t)pos, method);
    if (ret == -1) {
        return MR_FAILED;
    }
    return MR_SUCCESS;
}

int32_t my_read(int32_t f, void *p, uint32_t l) {
    uIntMap *obj = uIntMap_search(&filef_map, f);
    if (obj == NULL) {
        return MR_FAILED;
    }
    int32_t readnum = read((int)(intptr_t)obj->data, p, (size_t)l);
    if (readnum == -1) {
        return MR_FAILED;
    }
    return readnum;
}

int32_t my_write(int32_t f, void *p, uint32_t l) {
    uIntMap *obj = uIntMap_search(&filef_map, f);
    if (obj == NULL) {
        return MR_FAILED;
    }
    int32_t writenum = write((int)(intptr_t)obj->data, p, (size_t)l);
    if (writenum == -1) {
        return MR_FAILED;
    }
    return writenum;
}

int32_t my_rename(const char *oldname, const char *newname) {
#ifdef _WIN32
    wchar_t *woldname = utf8_to_wide(oldname);
    wchar_t *wnewname = utf8_to_wide(newname);
    int ret;
    if (!woldname || !wnewname) {
        free(woldname);
        free(wnewname);
        return MR_FAILED;
    }
    ret = _wrename(woldname, wnewname);
    free(woldname);
    free(wnewname);
#else
    int ret = rename(oldname, newname);
#endif
    if (ret != 0) {
        return MR_FAILED;
    }
    return MR_SUCCESS;
}

int32_t my_remove(const char *filename) {
#ifdef _WIN32
    wchar_t *wfilename = utf8_to_wide(filename);
    int ret;
    if (!wfilename) return MR_FAILED;
    ret = _wremove(wfilename);
    if (ret != 0) {
        DWORD attrs = GetFileAttributesW(wfilename);
        if (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_READONLY) &&
            !(attrs & FILE_ATTRIBUTE_DIRECTORY)) {
            SetFileAttributesW(wfilename, attrs & ~FILE_ATTRIBUTE_READONLY);
            ret = _wremove(wfilename);
        }
    }
    free(wfilename);
#else
    int ret = remove(filename);
#endif
    if (ret != 0) {
        return MR_FAILED;
    }
    return MR_SUCCESS;
}

int32_t my_getLen(const char *filename) {
#ifdef _WIN32
    struct _stat s1;
    wchar_t *wfilename = utf8_to_wide(filename);
    int ret;
    if (!wfilename) return -1;
    ret = _wstat(wfilename, &s1);
    free(wfilename);
#else
    struct stat s1;
    int ret = stat(filename, &s1);
#endif
    if (ret != 0)
        return -1;
    return s1.st_size;
}

int32_t my_mkDir(const char *name) {
    int ret;
#ifdef _WIN32
    wchar_t *wname = utf8_to_wide(name);
    if (!wname) return MR_FAILED;
    if (_waccess(wname, F_OK) == 0) {  //检测是否已存在
        goto ok;
    }
    ret = _wmkdir(wname);
#else
    if (access(name, F_OK) == 0) {  //检测是否已存在
        goto ok;
    }
    ret = mkdir(name, S_IRWXU | S_IRWXG | S_IRWXO);
#endif
    if (ret != 0) {
#ifdef _WIN32
        free(wname);
#endif
        return MR_FAILED;
    }
ok:
#ifdef _WIN32
    free(wname);
#endif
    return MR_SUCCESS;
}

int32_t my_rmDir(const char *name) {
#ifdef _WIN32
    wchar_t *wname = utf8_to_wide(name);
    int ret;
    if (!wname) return MR_FAILED;
    ret = _wrmdir(wname);
    free(wname);
#else
    int ret = rmdir(name);
#endif
    if (ret != 0) {
        return MR_FAILED;
    }
    return MR_SUCCESS;
}

int32_t my_info(const char *filename) {
#ifdef _WIN32
    struct _stat s1;
    wchar_t *wfilename = utf8_to_wide(filename);
    int ret;
    if (!wfilename) return MR_IS_INVALID;
    ret = _wstat(wfilename, &s1);
    free(wfilename);
#else
    struct stat s1;
    int ret = stat(filename, &s1);
#endif

    if (ret != 0) {
        return MR_IS_INVALID;
    }
    if (s1.st_mode & S_IFDIR) {
        return MR_IS_DIR;
    } else if (s1.st_mode & S_IFREG) {
        return MR_IS_FILE;
    }
    return MR_IS_INVALID;
}

int32_t my_opendir(const char *name) {
    DIR *pDir = opendir(name);
    if (pDir != NULL) {
        dirf_count++;
        uIntMap *obj = malloc(sizeof(uIntMap));
        obj->key = dirf_count;
        obj->data = (void *)pDir;
        uIntMap_insert(&dirf_map, obj);
        return dirf_count;
    }
    return MR_FAILED;
}

char *my_readdir(int32_t f) {
    uIntMap *obj = uIntMap_search(&dirf_map, f);
    if (obj == NULL) {
        return NULL;
    }
    struct dirent *pDt = readdir((DIR *)obj->data); // 手册说返回的内存可能是静态分配的，不要尝试free()
    if (pDt != NULL) {
        return pDt->d_name;
    }
    return NULL;
}

int32_t my_closedir(int32_t f) {
    uIntMap *obj = uIntMap_delete(&dirf_map, f);
    if (obj == NULL) {
        return MR_FAILED;
    }
    if (f == dirf_count) {
        dirf_count--;
    }
    DIR *pDir = (DIR *)obj->data;
    free(obj);
    if (closedir(pDir) != 0) {
        return MR_FAILED;
    }
    return MR_SUCCESS;
}

void vmrp_writeFile(const char *filename, void *data, uint32 length) {
    int fh = my_open(filename, MR_FILE_CREATE | MR_FILE_RDWR);
    int32_t wLen = 0;
    char *ptr = (char *)data;
    do {
        ptr += wLen;
        wLen = my_write(fh, ptr, length < 1000 ? length : 1000);
        if (wLen == MR_FAILED) {
            break;
        }
        length -= wLen;
    } while (length > 0);
    my_close(fh);
}

char *readFile(const char *filename) {
    int32_t len = my_getLen(filename);
    char *p = malloc(len);
    if (p == NULL) {
        return NULL;
    }
    int32_t fh = my_open(filename, MR_FILE_RDONLY);
    if (fh) {
        int32_t rLen = 0;
        char *ptr = p;
        do {
            ptr += rLen;
            rLen = my_read(fh, ptr, len);
            if (rLen == MR_FAILED) {
                free(p);
                return NULL;
            }
            len -= rLen;
        } while (len > 0);
        my_close(fh);
        return p;
    }
    return NULL;
}
