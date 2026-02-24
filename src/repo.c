#include "gitex.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

int repo_find_root(char *out, size_t out_sz) {
    char cwd[GITEX_PATH_MAX];
    if (!getcwd(cwd, sizeof(cwd))) return -1;
    char cur[GITEX_PATH_MAX];
    snprintf(cur, sizeof(cur), "%s", cwd);

    while (1) {
        char probe[GITEX_PATH_MAX];
        snprintf(probe, sizeof(probe), "%s/%s", cur, GITEX_DIR);
        if (util_is_dir(probe)) {
            snprintf(out, out_sz, "%s", cur);
            return 0;
        }
        char *slash = strrchr(cur, '/');
        if (!slash || slash == cur) break;
        *slash = '\0';
    }
    return -1;
}

int repo_init_layout(void) {
    if (util_mkdir_p(GITEX_DIR) != 0) return -1;
    if (util_mkdir_p(GITEX_OBJECTS) != 0) return -1;
    if (util_mkdir_p(GITEX_COMMITS) != 0) return -1;
    if (util_mkdir_p(GITEX_REFS) != 0) return -1;
    if (util_mkdir_p(GITEX_STASH) != 0) return -1;
    if (util_mkdir_p(GITEX_TAGS) != 0) return -1;

    if (!util_file_exists(GITEX_INDEX)) {
        if (util_write_file(GITEX_INDEX, (const uint8_t *)"", 0) != 0) return -1;
    }
    if (!util_file_exists(GITEX_HEAD)) {
        if (util_write_file(GITEX_HEAD, (const uint8_t *)GITEX_DEFAULT_BRANCH "\n", strlen(GITEX_DEFAULT_BRANCH) + 1) != 0) return -1;
    }

    char ref_path[GITEX_PATH_MAX];
    snprintf(ref_path, sizeof(ref_path), "%s/%s", GITEX_REFS, GITEX_DEFAULT_BRANCH);
    if (!util_file_exists(ref_path)) {
        util_write_file(ref_path, (const uint8_t *)"\n", 1);
    }
    return 0;
}
