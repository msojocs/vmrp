#include "./include/app_compat.h"

#include <string.h>

static const AppCompatProfile *profiles[] = {
    &app_compat_gghjt,
    &app_compat_gxdzc,
};

const AppCompatProfile *app_compat_select(const char *pack_filename) {
    if (!pack_filename) return NULL;
    const char *slash = strrchr(pack_filename, '/');
    const char *backslash = strrchr(pack_filename, '\\');
    if (!slash || (backslash && backslash > slash))
        slash = backslash;
    /* Windows CLI paths keep backslashes in pack_filename.  This only changes
     * profile selection (no ABI impact) so hooks like gghjt resource unpacking
     * run on both Windows and Linux.  Verified by vmrp_app_compat_tests and a
     * cold Windows gghjt.mrp boot after deleting the test save directory. */
    const char *name = slash ? slash + 1 : pack_filename;

    for (int i = 0; i < (int)(sizeof(profiles) / sizeof(profiles[0])); i++) {
        if (strcmp(name, profiles[i]->name) == 0)
            return profiles[i];
    }
    return NULL;
}
