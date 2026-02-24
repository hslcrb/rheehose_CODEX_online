#include "gitex/repo.h"
#include "gitex/common.h"
#include "gitex/fs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool repo_is_initialized(void) {
    return fs_is_dir(GITEX_DIR) && fs_is_file(GITEX_HEAD_FILE);
}

int repo_init(void) {
    if (repo_is_initialized()) {
        gitex_log_warn("repository already initialized");
        return 0;
    }

    if (fs_mkdir_p(GITEX_OBJECTS_DIR) != 0) return -1;
    if (fs_mkdir_p(GITEX_HEADS_DIR) != 0) return -1;
    if (fs_mkdir_p(GITEX_TAGS_DIR) != 0) return -1;
    if (fs_mkdir_p(GITEX_LOG_DIR) != 0) return -1;

    if (fs_write_file(GITEX_HEAD_FILE, "refs/heads/main\n", 16) != 0) return -1;
    if (fs_write_file(".gitex/refs/heads/main", "", 0) != 0) return -1;
    if (fs_write_file(GITEX_INDEX_FILE, "", 0) != 0) return -1;
    if (fs_write_file(GITEX_STASH_FILE, "", 0) != 0) return -1;

    gitex_log_info("initialized empty gitex repository");
    return 0;
}

int repo_head_ref(char *out_ref, size_t len) {
    char *data = NULL;
    size_t n = 0;
    if (fs_read_file(GITEX_HEAD_FILE, &data, &n) != 0) return -1;
    if (n > 0 && data[n - 1] == '\n') data[n - 1] = '\0';
    snprintf(out_ref, len, "%s", data);
    free(data);
    return 0;
}

int repo_set_head_ref(const char *ref) {
    char buf[256];
    snprintf(buf, sizeof(buf), "%s\n", ref);
    return fs_write_file(GITEX_HEAD_FILE, buf, strlen(buf));
}

int repo_read_ref(const char *ref, char *out_hash, size_t len) {
    char path[512];
    snprintf(path, sizeof(path), ".gitex/%s", ref);
    if (!fs_exists(path)) {
        if (len) out_hash[0] = '\0';
        return 0;
    }

    char *data = NULL;
    size_t n = 0;
    if (fs_read_file(path, &data, &n) != 0) return -1;
    if (n > 0 && data[n - 1] == '\n') data[n - 1] = '\0';
    snprintf(out_hash, len, "%s", data);
    free(data);
    return 0;
}

int repo_write_ref(const char *ref, const char *hash) {
    char path[512];
    snprintf(path, sizeof(path), ".gitex/%s", ref);

    char dir[512];
    snprintf(dir, sizeof(dir), "%s", path);
    char *slash = strrchr(dir, '/');
    if (slash) {
        *slash = '\0';
        if (fs_mkdir_p(dir) != 0) return -1;
    }

    char buf[96];
    snprintf(buf, sizeof(buf), "%s\n", hash);
    return fs_write_file(path, buf, strlen(buf));
}
