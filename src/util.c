#include "gitex.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int util_read_file(const char *path, uint8_t **out, size_t *out_len) {
    FILE *fp = fopen(path, "rb");
    if (!fp) return -1;
    if (fseek(fp, 0, SEEK_END) != 0) { fclose(fp); return -1; }
    long n = ftell(fp);
    if (n < 0) { fclose(fp); return -1; }
    rewind(fp);
    uint8_t *buf = malloc((size_t)n + 1);
    if (!buf) { fclose(fp); return -1; }
    size_t rd = fread(buf, 1, (size_t)n, fp);
    fclose(fp);
    if (rd != (size_t)n) { free(buf); return -1; }
    buf[n] = 0;
    *out = buf;
    *out_len = (size_t)n;
    return 0;
}

int util_write_file(const char *path, const uint8_t *data, size_t len) {
    FILE *fp = fopen(path, "wb");
    if (!fp) return -1;
    size_t wr = fwrite(data, 1, len, fp);
    fclose(fp);
    return wr == len ? 0 : -1;
}

bool util_file_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0;
}

bool util_is_dir(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) return false;
    return S_ISDIR(st.st_mode);
}

int util_mkdir_p(const char *path) {
    char tmp[GITEX_PATH_MAX];
    size_t len = strlen(path);
    if (len == 0 || len >= sizeof(tmp)) return -1;
    memcpy(tmp, path, len + 1);
    for (size_t i = 1; i < len; i++) {
        if (tmp[i] == '/') {
            tmp[i] = '\0';
            if (mkdir(tmp, 0755) != 0 && errno != EEXIST) return -1;
            tmp[i] = '/';
        }
    }
    if (mkdir(tmp, 0755) != 0 && errno != EEXIST) return -1;
    return 0;
}

static int walk_inner(const char *root, int (*cb)(const char *, void *), void *ctx) {
    DIR *d = opendir(root);
    if (!d) return -1;
    struct dirent *ent;
    while ((ent = readdir(d))) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
        if (strcmp(ent->d_name, GITEX_DIR) == 0) continue;
        char path[GITEX_PATH_MAX];
        snprintf(path, sizeof(path), "%s/%s", root, ent->d_name);
        struct stat st;
        if (stat(path, &st) != 0) continue;
        if (S_ISDIR(st.st_mode)) {
            walk_inner(path, cb, ctx);
        } else if (S_ISREG(st.st_mode)) {
            cb(path, ctx);
        }
    }
    closedir(d);
    return 0;
}

int util_walk_files(const char *root, int (*cb)(const char *, void *), void *ctx) {
    return walk_inner(root, cb, ctx);
}

int util_relpath(const char *base, const char *target, char *out, size_t out_sz) {
    size_t b = strlen(base);
    if (strncmp(base, target, b) == 0 && target[b] == '/') {
        snprintf(out, out_sz, "%s", target + b + 1);
        return 0;
    }
    snprintf(out, out_sz, "%s", target);
    return 0;
}
