#include "gitex/objects.h"
#include "gitex/hash.h"
#include "gitex/fs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int objects_store(const char *type, const void *data, size_t len, char out_hash[65]) {
    size_t header_len = strlen(type) + 32;
    char *payload = malloc(header_len + len);
    if (!payload) return -1;

    int n = snprintf(payload, header_len, "%s %zu\n", type, len);
    memcpy(payload + n, data, len);

    hash_bytes((const uint8_t*)payload, n + len, out_hash);

    char path[256];
    snprintf(path, sizeof(path), ".gitex/objects/%.2s/%s", out_hash, out_hash + 2);

    char dir[64];
    snprintf(dir, sizeof(dir), ".gitex/objects/%.2s", out_hash);
    if (fs_mkdir_p(dir) != 0) { free(payload); return -1; }

    int rc = 0;
    if (!fs_exists(path)) rc = fs_write_file(path, payload, n + len);
    free(payload);
    return rc;
}

int objects_load(const char *hash, char **data, size_t *len) {
    char path[256];
    snprintf(path, sizeof(path), ".gitex/objects/%.2s/%s", hash, hash + 2);
    return fs_read_file(path, data, len);
}

int objects_store_file(const char *type, const char *path, char out_hash[65]) {
    char *data = NULL;
    size_t len = 0;
    if (fs_read_file(path, &data, &len) != 0) return -1;
    int rc = objects_store(type, data, len, out_hash);
    free(data);
    return rc;
}
