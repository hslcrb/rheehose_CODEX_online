#include "gitex.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static int mkdir_p(const char *path) {
    char tmp[1024];
    snprintf(tmp, sizeof(tmp), "%s", path);
    for (char *p = tmp + 1; *p; ++p) {
        if (*p == '/') {
            *p = 0;
            if (mkdir(tmp, 0755) != 0 && errno != EEXIST) return -1;
            *p = '/';
        }
    }
    if (mkdir(tmp, 0755) != 0 && errno != EEXIST) return -1;
    return 0;
}

int repo_ensure_parent_dirs(const char *path) {
    char tmp[1024];
    snprintf(tmp, sizeof(tmp), "%s", path);
    char *slash = strrchr(tmp, '/');
    if (!slash) return 0;
    *slash = 0;
    return mkdir_p(tmp);
}

int repo_exists(void) { return path_exists(GITEX_DIR); }

int repo_init(void) {
    if (mkdir_p(GITEX_OBJECTS) != 0) return -1;
    if (mkdir_p(GITEX_REFS_HEADS) != 0) return -1;
    if (mkdir_p(GITEX_REFS_TAGS) != 0) return -1;
    if (mkdir_p(".gitex/refs") != 0) return -1;
    const char *head = "ref: refs/heads/main\n";
    if (file_write_all(GITEX_HEAD, (const uint8_t *)head, strlen(head)) != 0) return -1;
    if (!path_exists(".gitex/refs/heads/main")) {
        if (file_write_all(".gitex/refs/heads/main", (const uint8_t *)"", 0) != 0) return -1;
    }
    if (!path_exists(GITEX_INDEX)) {
        if (file_write_all(GITEX_INDEX, (const uint8_t *)"", 0) != 0) return -1;
    }
    return 0;
}

int repo_head_ref(char *out, size_t out_sz) {
    uint8_t *buf = NULL; size_t len = 0;
    if (file_read_all(GITEX_HEAD, &buf, &len) != 0) return -1;
    buf[len] = 0;
    if (strncmp((char *)buf, "ref: ", 5) == 0) {
        snprintf(out, out_sz, "%s", (char *)buf + 5);
        char *nl = strchr(out, '\n');
        if (nl) *nl = 0;
        free(buf);
        return 0;
    }
    free(buf);
    return -1;
}

int repo_read_head_commit(char *out, size_t out_sz) {
    char ref[256];
    if (repo_head_ref(ref, sizeof(ref)) != 0) return -1;
    char path[512];
    snprintf(path, sizeof(path), ".gitex/%s", ref);
    uint8_t *buf = NULL; size_t len = 0;
    if (file_read_all(path, &buf, &len) != 0) return -1;
    snprintf(out, out_sz, "%.*s", (int)len, (char *)buf);
    free(buf);
    return 0;
}

int repo_update_ref(const char *ref, const char *oid) {
    char path[512];
    snprintf(path, sizeof(path), ".gitex/%s", ref);
    return file_write_all(path, (const uint8_t *)oid, strlen(oid));
}
