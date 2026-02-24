#include "gitex.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int idx_grow(Index *idx) {
    if (idx->count < idx->cap) return 0;
    size_t ncap = idx->cap ? idx->cap * 2 : 32;
    IndexEntry *n = realloc(idx->entries, ncap * sizeof(IndexEntry));
    if (!n) return -1;
    idx->entries = n;
    idx->cap = ncap;
    return 0;
}

IndexEntry *index_find(Index *idx, const char *path) {
    for (size_t i = 0; i < idx->count; ++i) {
        if (strcmp(idx->entries[i].path, path) == 0) return &idx->entries[i];
    }
    return NULL;
}

int index_upsert(Index *idx, const IndexEntry *entry) {
    IndexEntry *e = index_find(idx, entry->path);
    if (e) {
        *e = *entry;
        return 0;
    }
    if (idx_grow(idx) != 0) return -1;
    idx->entries[idx->count++] = *entry;
    return 0;
}

int index_load(Index *idx) {
    memset(idx, 0, sizeof(*idx));
    uint8_t *buf;
    size_t len;
    if (!util_file_exists(GITEX_INDEX)) return 0;
    if (util_read_file(GITEX_INDEX, &buf, &len) != 0) return -1;
    char *line = strtok((char *)buf, "\n");
    while (line) {
        IndexEntry e = {0};
        int kind = 0, state = 0;
        if (sscanf(line, "%4095[^|]|%64[^|]|%d|%d", e.path, e.hash, &kind, &state) == 4) {
            e.kind = (TrackKind)kind;
            e.state = (IndexState)state;
            if (index_upsert(idx, &e) != 0) { free(buf); return -1; }
        }
        line = strtok(NULL, "\n");
    }
    free(buf);
    return 0;
}

int index_save(const Index *idx) {
    FILE *fp = fopen(GITEX_INDEX, "wb");
    if (!fp) return -1;
    for (size_t i = 0; i < idx->count; ++i) {
        const IndexEntry *e = &idx->entries[i];
        fprintf(fp, "%s|%s|%d|%d\n", e->path, e->hash, e->kind, e->state);
    }
    fclose(fp);
    return 0;
}

void index_free(Index *idx) {
    free(idx->entries);
    memset(idx, 0, sizeof(*idx));
}
