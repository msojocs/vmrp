#include "./include/app_compat.h"
#include "./include/file_lib.h"

#include <stdio.h>
#include <string.h>

// TODO: 后期将改成通用代码，废弃掉这种指定APP硬编码的逻辑
static const AppCompatProfile *profiles[] = {
    &app_compat_gghjt,
};

static int app_compat_name_matches(const char *name, const char *profile_name) {
    size_t name_len;
    size_t profile_len;

    if (strcmp(name, profile_name) == 0)
        return 1;

    name_len = strlen(name);
    profile_len = strlen(profile_name);
    if (name_len <= profile_len + 1)
        return 0;

    /* Some handset/app-store dumps prefix the original MRP name with an id,
     * e.g. 018982845369-gghjt.mrp.  The app still extracts resources and
     * saves under its original directory, so the existing profile must follow
     * this stable suffix.  ABI and save paths are unchanged; verified by
     * cold gghjt Vitest boot coverage after deleting only the test save
     * directory. */
    return name[name_len - profile_len - 1] == '-' &&
           strcmp(name + name_len - profile_len, profile_name) == 0;
}

static void app_compat_read_mrp_header_name(const char *pack_filename,
                                            char *out, size_t out_size) {
    unsigned char header[28];
    FILE *fp;
    size_t n;

    if (!out || out_size == 0)
        return;
    out[0] = '\0';
    if (!pack_filename || !*pack_filename)
        return;

    fp = skyengine_host_fopen(pack_filename, "rb");
    if (!fp)
        return;
    n = fread(header, 1, sizeof(header), fp);
    fclose(fp);
    if (n != sizeof(header) || memcmp(header, "MRPG", 4) != 0)
        return;

    /* MRP app info keeps the original 12-byte filename at offset 16.  App
     * compat hooks are tied to the packaged EXT behavior rather than the host
     * filename, so renamed dumps such as test.mrp must still select the same
     * profile.  Save/resource paths are not changed; verified by cold Vitest
     * boots of renamed gghjt packages. */
    for (size_t i = 0; i < 12 && i + 1 < out_size; ++i) {
        if (header[16 + i] == '\0')
            break;
        out[i] = (char)header[16 + i];
        out[i + 1] = '\0';
    }
}

const AppCompatProfile *app_compat_select(const char *pack_filename) {
    if (!pack_filename) return NULL;
    const char *slash = strrchr(pack_filename, '/');
    const char *backslash = strrchr(pack_filename, '\\');
    if (!slash || (backslash && backslash > slash))
        slash = backslash;
    /* Windows CLI paths keep backslashes in pack_filename.  This only changes
     * profile selection (no ABI impact) so hooks like gghjt resource unpacking
     * run on both Windows and Linux.  Verified by cold gghjt.mrp boot coverage
     * after deleting the test save directory. */
    const char *name = slash ? slash + 1 : pack_filename;

    for (int i = 0; i < (int)(sizeof(profiles) / sizeof(profiles[0])); i++) {
        if (app_compat_name_matches(name, profiles[i]->name))
            return profiles[i];
    }

    char header_name[13];
    app_compat_read_mrp_header_name(pack_filename, header_name,
                                    sizeof(header_name));
    if (header_name[0]) {
        for (int i = 0; i < (int)(sizeof(profiles) / sizeof(profiles[0])); i++) {
            if (app_compat_name_matches(header_name, profiles[i]->name))
                return profiles[i];
        }
    }
    return NULL;
}
