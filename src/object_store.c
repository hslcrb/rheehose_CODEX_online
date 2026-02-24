#include "gitex.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int ensure_obj_dir(const char *hash) {
    char dir[GITEX_PATH_MAX];
    snprintf(dir, sizeof(dir), "%s/%.2s", GITEX_OBJECTS, hash);
    return util_mkdir_p(dir);
}

static void obj_path(const char *hash, char *out, size_t out_sz) {
    snprintf(out, out_sz, "%s/%.2s/%s", GITEX_OBJECTS, hash, hash + 2);
}

int object_store_blob_from_memory(const uint8_t *data, size_t len, char out_hash[GITEX_HASH_HEX + 1]) {
    if (hash_buffer_sha256(data, len, out_hash) != 0) return -1;
    if (ensure_obj_dir(out_hash) != 0) return -1;
    char path[GITEX_PATH_MAX];
    obj_path(out_hash, path, sizeof(path));
    if (!util_file_exists(path)) {
        if (util_write_file(path, data, len) != 0) return -1;
    }
    return 0;
}

int object_store_blob_from_file(const char *path, char out_hash[GITEX_HASH_HEX + 1]) {
    uint8_t *buf;
    size_t len;
    if (util_read_file(path, &buf, &len) != 0) return -1;
    int rc = object_store_blob_from_memory(buf, len, out_hash);
    free(buf);
    return rc;
}

int object_read_to_memory(const char *hash, uint8_t **data, size_t *len) {
    char path[GITEX_PATH_MAX];
    obj_path(hash, path, sizeof(path));
    return util_read_file(path, data, len);
}
