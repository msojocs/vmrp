#include "./include/app_compat.h"

#include <string.h>

static const AppCompatProfile *profiles[] = {
    &app_compat_gghjt,
    &app_compat_gxdzc,
};

const AppCompatProfile *app_compat_select(const char *pack_filename) {
    if (!pack_filename) return NULL;
    const char *slash = strrchr(pack_filename, '/');
    const char *name = slash ? slash + 1 : pack_filename;

    for (int i = 0; i < (int)(sizeof(profiles) / sizeof(profiles[0])); i++) {
        if (strcmp(name, profiles[i]->name) == 0)
            return profiles[i];
    }
    return NULL;
}
