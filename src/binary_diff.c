#include "gitex.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char hash[GITEX_HASH_HEX + 1];
} Chunk;

typedef struct {
    Chunk *items;
    size_t n;
    size_t cap;
} ChunkVec;

static int push_chunk(ChunkVec *v, const char *hash) {
    if (v->n == v->cap) {
        size_t nc = v->cap ? v->cap * 2 : 64;
        Chunk *ni = realloc(v->items, nc * sizeof(Chunk));
        if (!ni) return -1;
        v->items = ni;
        v->cap = nc;
    }
    snprintf(v->items[v->n++].hash, sizeof(v->items[0].hash), "%s", hash);
    return 0;
}

static int chunk_file(const char *path, ChunkVec *out) {
    memset(out, 0, sizeof(*out));
    FILE *fp = fopen(path, "rb");
    if (!fp) return -1;
    uint8_t buf[GITEX_CHUNK_SIZE];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), fp)) > 0) {
        char hash[GITEX_HASH_HEX + 1];
        hash_buffer_sha256(buf, n, hash);
        if (push_chunk(out, hash) != 0) { fclose(fp); return -1; }
    }
    fclose(fp);
    return 0;
}

int binary_chunk_diff_report(const char *old_path, const char *new_path, char **report) {
    ChunkVec a, b;
    if (chunk_file(old_path, &a) != 0 || chunk_file(new_path, &b) != 0) return -1;

    size_t changed = 0;
    size_t max = a.n > b.n ? a.n : b.n;
    for (size_t i = 0; i < max; ++i) {
        if (i >= a.n || i >= b.n || strcmp(a.items[i].hash, b.items[i].hash) != 0) changed++;
    }

    char *buf = malloc(256);
    if (!buf) return -1;
    snprintf(buf, 256, "binary-chunk-diff: old_chunks=%zu new_chunks=%zu changed_chunks=%zu", a.n, b.n, changed);
    *report = buf;

    free(a.items);
    free(b.items);
    return 0;
}
