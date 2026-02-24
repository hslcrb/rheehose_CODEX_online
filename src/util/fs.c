#include "gitex/fs.h"
#include "gitex/common.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

bool fs_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0;
}

bool fs_is_file(const char *path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

bool fs_is_dir(const char *path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

int fs_mkdir_p(const char *path) {
    char tmp[1024];
    size_t len = strlen(path);
    if (len >= sizeof(tmp)) return -1;
    strcpy(tmp, path);
    if (tmp[len - 1] == '/') tmp[len - 1] = '\0';

    for (char *p = tmp + 1; *p; ++p) {
        if (*p == '/') {
            *p = '\0';
            if (mkdir(tmp, 0755) != 0 && errno != EEXIST) return -1;
            *p = '/';
        }
    }

    if (mkdir(tmp, 0755) != 0 && errno != EEXIST) return -1;
    return 0;
}

int fs_read_file(const char *path, char **data, size_t *len) {
    FILE *f = fopen(path, "rb");
    if (!f) return -1;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return -1; }
    long sz = ftell(f);
    if (sz < 0) { fclose(f); return -1; }
    rewind(f);

    char *buf = (char*)malloc((size_t)sz + 1);
    if (!buf) { fclose(f); return -1; }
    if (fread(buf, 1, (size_t)sz, f) != (size_t)sz) { free(buf); fclose(f); return -1; }
    fclose(f);

    buf[sz] = '\0';
    *data = buf;
    if (len) *len = (size_t)sz;
    return 0;
}

int fs_write_file(const char *path, const void *data, size_t len) {
    FILE *f = fopen(path, "wb");
    if (!f) return -1;
    if (fwrite(data, 1, len, f) != len) { fclose(f); return -1; }
    fclose(f);
    return 0;
}

int fs_copy_file(const char *src, const char *dst) {
    char *data = NULL;
    size_t len = 0;
    if (fs_read_file(src, &data, &len) != 0) return -1;
    int rc = fs_write_file(dst, data, len);
    free(data);
    return rc;
}

int fs_remove_file(const char *path) {
    return unlink(path);
}

int fs_file_stat(const char *path, uint64_t *size, uint64_t *mtime) {
    struct stat st;
    if (stat(path, &st) != 0) return -1;
    if (size) *size = (uint64_t)st.st_size;
    if (mtime) *mtime = (uint64_t)st.st_mtime;
    return 0;
}

static int walk_inner(const char *root, int (*callback)(const char*, void*), void *ctx) {
    DIR *dir = opendir(root);
    if (!dir) return -1;

    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
        if (strcmp(ent->d_name, GITEX_DIR) == 0) continue;

        char path[2048];
        snprintf(path, sizeof(path), "%s/%s", root, ent->d_name);
        if (fs_is_dir(path)) {
            if (walk_inner(path, callback, ctx) != 0) { closedir(dir); return -1; }
        } else if (fs_is_file(path)) {
            if (callback(path, ctx) != 0) { closedir(dir); return -1; }
        }
    }

    closedir(dir);
    return 0;
}

int fs_walk_files(const char *root, int (*callback)(const char*, void*), void *ctx) {
    return walk_inner(root, callback, ctx);
}
