#include "util.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int util_file_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0;
}

int util_is_dir(const char *path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

int util_ensure_dir(const char *path) {
    if (util_is_dir(path)) return 0;
    if (mkdir(path, 0755) == 0) return 0;
    if (errno == EEXIST) return 0;
    return -1;
}

int util_read_file(const char *path, unsigned char **buf, size_t *len) {
    FILE *fp = fopen(path, "rb");
    if (!fp) return -1;
    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return -1;
    }
    long n = ftell(fp);
    if (n < 0) {
        fclose(fp);
        return -1;
    }
    if (fseek(fp, 0, SEEK_SET) != 0) {
        fclose(fp);
        return -1;
    }
    unsigned char *tmp = malloc((size_t)n + 1);
    if (!tmp) {
        fclose(fp);
        return -1;
    }
    size_t r = fread(tmp, 1, (size_t)n, fp);
    fclose(fp);
    if (r != (size_t)n) {
        free(tmp);
        return -1;
    }
    tmp[n] = '\0';
    *buf = tmp;
    *len = (size_t)n;
    return 0;
}

int util_write_file(const char *path, const unsigned char *buf, size_t len) {
    FILE *fp = fopen(path, "wb");
    if (!fp) return -1;
    if (len > 0 && fwrite(buf, 1, len, fp) != len) {
        fclose(fp);
        return -1;
    }
    fclose(fp);
    return 0;
}

int util_append_file(const char *path, const char *text) {
    FILE *fp = fopen(path, "ab");
    if (!fp) return -1;
    size_t n = strlen(text);
    if (n > 0 && fwrite(text, 1, n, fp) != n) {
        fclose(fp);
        return -1;
    }
    fclose(fp);
    return 0;
}

int util_copy_file(const char *src, const char *dst) {
    unsigned char *buf = NULL;
    size_t len = 0;
    if (util_read_file(src, &buf, &len) != 0) return -1;
    int rc = util_write_file(dst, buf, len);
    free(buf);
    return rc;
}

static int walk_recursive(const char *root, int (*cb)(const char *, void *), void *ctx) {
    DIR *d = opendir(root);
    if (!d) return -1;

    struct dirent *ent;
    while ((ent = readdir(d)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
        if (strcmp(ent->d_name, ".git") == 0 || strcmp(ent->d_name, ".gitex") == 0) continue;

        char path[4096];
        snprintf(path, sizeof(path), "%s/%s", root, ent->d_name);

        struct stat st;
        if (stat(path, &st) != 0) continue;

        if (S_ISDIR(st.st_mode)) {
            walk_recursive(path, cb, ctx);
        } else if (S_ISREG(st.st_mode)) {
            if (cb(path, ctx) != 0) {
                closedir(d);
                return -1;
            }
        }
    }
    closedir(d);
    return 0;
}

int util_walk_files(const char *root, int (*cb)(const char *, void *), void *ctx) {
    return walk_recursive(root, cb, ctx);
}

char *util_trim(char *s) {
    if (!s) return s;
    while (*s && isspace((unsigned char)*s)) s++;
    if (*s == '\0') return s;
    char *end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }
    return s;
}
