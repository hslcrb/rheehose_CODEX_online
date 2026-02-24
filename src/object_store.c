#include "gitex.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int object_write(const char *type, const uint8_t *data, size_t len, char oid_out[32]) {
    size_t hdr_len = strlen(type) + 32;
    uint8_t *full = malloc(hdr_len + len);
    if (!full) return -1;
    int n = snprintf((char *)full, hdr_len, "%s %zu\n", type, len);
    memcpy(full + n, data, len);
    hash_bytes(full, n + len, oid_out);
    char dir[64], path[128];
    snprintf(dir, sizeof(dir), GITEX_OBJECTS "/%.2s", oid_out);
    if (repo_ensure_parent_dirs(dir) != 0) { free(full); return -1; }
    mkdir(dir, 0755);
    snprintf(path, sizeof(path), "%s/%s", dir, oid_out + 2);
    int rc = file_write_all(path, full, n + len);
    free(full);
    return rc;
}

int object_read(const char *oid, char *type_out, size_t type_sz, uint8_t **data_out, size_t *len_out) {
    (void)type_sz;
    char path[128];
    snprintf(path, sizeof(path), GITEX_OBJECTS "/%.2s/%s", oid, oid + 2);
    uint8_t *buf = NULL; size_t len = 0;
    if (file_read_all(path, &buf, &len) != 0) return -1;
    char *nl = memchr(buf, '\n', len);
    if (!nl) { free(buf); return -1; }
    size_t hdr = (size_t)(nl - (char *)buf);
    sscanf((char *)buf, "%31s", type_out);
    size_t data_start = hdr + 1;
    if (data_start > len) { free(buf); return -1; }
    *len_out = len - data_start;
    *data_out = malloc(*len_out + 1);
    if (!*data_out) { free(buf); return -1; }
    memcpy(*data_out, buf + data_start, *len_out);
    free(buf);
    return 0;
}
