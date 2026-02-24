#include "gitex.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>

void hash_bytes(const uint8_t *data, size_t len, char out_hex[32]) {
    uint64_t h = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        h ^= data[i];
        h *= 1099511628211ULL;
    }
    snprintf(out_hex, 32, "%016llx", (unsigned long long)h);
}

int file_read_all(const char *path, uint8_t **buf, size_t *len) {
    *buf = NULL;
    *len = 0;
    FILE *f = fopen(path, "rb");
    if (!f) return -1;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return -1; }
    long sz = ftell(f);
    if (sz < 0) { fclose(f); return -1; }
    rewind(f);
    uint8_t *tmp = malloc((size_t)sz + 1);
    if (!tmp) { fclose(f); return -1; }
    size_t n = fread(tmp, 1, (size_t)sz, f);
    fclose(f);
    if (n != (size_t)sz) { free(tmp); return -1; }
    *buf = tmp;
    *len = n;
    return 0;
}

int file_write_all(const char *path, const uint8_t *buf, size_t len) {
    if (repo_ensure_parent_dirs(path) != 0) return -1;
    FILE *f = fopen(path, "wb");
    if (!f) return -1;
    size_t n = fwrite(buf, 1, len, f);
    fclose(f);
    return n == len ? 0 : -1;
}

int hash_file(const char *path, char out_hex[32]) {
    uint8_t *buf = NULL;
    size_t len = 0;
    if (file_read_all(path, &buf, &len) != 0) return -1;
    hash_bytes(buf, len, out_hex);
    free(buf);
    return 0;
}

int path_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0;
}

static int has_ext(const char *p, const char *ext) {
    size_t lp = strlen(p), le = strlen(ext);
    if (lp < le) return 0;
    return strcasecmp(p + lp - le, ext) == 0;
}

int is_xml_like(const char *path) {
    return has_ext(path, ".xml") || has_ext(path, ".html") || has_ext(path, ".xhtml");
}

int is_zip_xml_container(const char *path) {
    return has_ext(path, ".docx") || has_ext(path, ".xlsx") || has_ext(path, ".hwpx");
}
