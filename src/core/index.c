#include "gitex/index.h"
#include "gitex/fs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void index_init(gitex_index *index) {
    index->entries = NULL;
    index->count = 0;
    index->capacity = 0;
}

void index_free(gitex_index *index) {
    free(index->entries);
    index_init(index);
}

static int index_reserve(gitex_index *index, size_t capacity) {
    if (capacity <= index->capacity) return 0;
    gitex_index_entry *next = realloc(index->entries, capacity * sizeof(gitex_index_entry));
    if (!next) return -1;
    index->entries = next;
    index->capacity = capacity;
    return 0;
}

gitex_index_entry *index_find(gitex_index *index, const char *path) {
    for (size_t i = 0; i < index->count; ++i) {
        if (strcmp(index->entries[i].path, path) == 0) return &index->entries[i];
    }
    return NULL;
}

int index_upsert(gitex_index *index, const gitex_index_entry *entry) {
    gitex_index_entry *existing = index_find(index, entry->path);
    if (existing) {
        *existing = *entry;
        return 0;
    }
    if (index_reserve(index, index->count + 16) != 0) return -1;
    index->entries[index->count++] = *entry;
    return 0;
}

int index_set_state(gitex_index *index, const char *path, gitex_track_state state) {
    gitex_index_entry *entry = index_find(index, path);
    if (!entry) return -1;
    entry->state = state;
    return 0;
}

int index_load(gitex_index *index) {
    index_free(index);
    index_init(index);

    if (!fs_exists(GITEX_INDEX_FILE)) return 0;

    char *data = NULL;
    size_t len = 0;
    if (fs_read_file(GITEX_INDEX_FILE, &data, &len) != 0) return -1;

    char *line = strtok(data, "\n");
    while (line) {
        gitex_index_entry e = {0};
        int st = 0;
        int tp = 0;
        if (sscanf(line, "%1023[^\t]\t%64[^\t]\t%d\t%d\t%llu\t%llu",
                e.path, e.content_hash, &st, &tp,
                (unsigned long long*)&e.size,
                (unsigned long long*)&e.mtime) == 6) {
            e.state = (gitex_track_state)st;
            e.type = (gitex_file_type)tp;
            if (index_upsert(index, &e) != 0) { free(data); return -1; }
        }
        line = strtok(NULL, "\n");
    }

    free(data);
    return 0;
}

int index_save(const gitex_index *index) {
    FILE *f = fopen(GITEX_INDEX_FILE, "wb");
    if (!f) return -1;
    for (size_t i = 0; i < index->count; ++i) {
        const gitex_index_entry *e = &index->entries[i];
        fprintf(f, "%s\t%s\t%d\t%d\t%llu\t%llu\n",
                e->path, e->content_hash,
                (int)e->state, (int)e->type,
                (unsigned long long)e->size,
                (unsigned long long)e->mtime);
    }
    fclose(f);
    return 0;
}
