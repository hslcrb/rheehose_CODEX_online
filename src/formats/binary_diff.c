#include "gitex/diff.h"
#include "gitex/common.h"
#include "gitex/fs.h"
#include "gitex/hash.h"
#include "gitex/objects.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void chunk_hashes(const uint8_t *data, size_t len, size_t *out_chunks, char ***out_hashes) {
    size_t chunks = (len + GITEX_CHUNK_SIZE - 1) / GITEX_CHUNK_SIZE;
    char **hashes = calloc(chunks ? chunks : 1, sizeof(char*));
    for (size_t i = 0; i < chunks; ++i) {
        size_t off = i * GITEX_CHUNK_SIZE;
        size_t part = len - off;
        if (part > GITEX_CHUNK_SIZE) part = GITEX_CHUNK_SIZE;
        hashes[i] = calloc(65, 1);
        hash_bytes(data + off, part, hashes[i]);
    }
    *out_chunks = chunks;
    *out_hashes = hashes;
}

int binary_chunk_diff(const char *path, const char *old_hash) {
    char *new_data = NULL, *old_data = NULL;
    size_t new_len = 0, old_len = 0;
    if (fs_read_file(path, &new_data, &new_len) != 0) return -1;
    if (old_hash[0] && objects_load(old_hash, &old_data, &old_len) != 0) old_data = NULL;

    size_t n_chunks = 0, o_chunks = 0;
    char **nh = NULL, **oh = NULL;
    chunk_hashes((uint8_t*)new_data, new_len, &n_chunks, &nh);
    if (old_data) {
        char *payload = strchr(old_data, '\n');
        if (payload) {
            payload++;
            size_t plen = old_len - (size_t)(payload - old_data);
            chunk_hashes((uint8_t*)payload, plen, &o_chunks, &oh);
        }
    }

    size_t max = n_chunks > o_chunks ? n_chunks : o_chunks;
    size_t changed = 0;
    for (size_t i = 0; i < max; ++i) {
        const char *a = (i < n_chunks) ? nh[i] : "";
        const char *b = (i < o_chunks) ? oh[i] : "";
        if (strcmp(a, b) != 0) changed++;
    }

    printf("Binary chunk diff: %zu/%zu chunks changed\n", changed, max);

    for (size_t i = 0; i < n_chunks; ++i) free(nh[i]);
    for (size_t i = 0; i < o_chunks; ++i) free(oh[i]);
    free(nh); free(oh); free(new_data); free(old_data);
    return 0;
}
