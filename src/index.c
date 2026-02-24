#include "index.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void index_init(gitex_index *idx) {
    idx->entries = NULL;
    idx->count = 0;
    idx->cap = 0;
}

void index_free(gitex_index *idx) {
    free(idx->entries);
    idx->entries = NULL;
    idx->count = idx->cap = 0;
}

static int index_reserve(gitex_index *idx, size_t need) {
    if (need <= idx->cap) return 0;
    size_t ncap = idx->cap == 0 ? 16 : idx->cap * 2;
    while (ncap < need) ncap *= 2;
    gitex_index_entry *tmp = realloc(idx->entries, sizeof(*tmp) * ncap);
    if (!tmp) return -1;
    idx->entries = tmp;
    idx->cap = ncap;
    return 0;
}

int index_find(const gitex_index *idx, const char *path) {
    for (size_t i = 0; i < idx->count; ++i) {
        if (strcmp(idx->entries[i].path, path) == 0) return (int)i;
    }
    return -1;
}

int index_upsert(gitex_index *idx, const gitex_index_entry *entry) {
    int pos = index_find(idx, entry->path);
    if (pos >= 0) {
        idx->entries[pos] = *entry;
        return 0;
    }
    if (index_reserve(idx, idx->count + 1) != 0) return -1;
    idx->entries[idx->count++] = *entry;
    return 0;
}

int index_load(gitex_index *idx) {
    index_init(idx);
    if (!util_file_exists(GITEX_INDEX_FILE)) return 0;

    unsigned char *buf = NULL;
    size_t len = 0;
    if (util_read_file(GITEX_INDEX_FILE, &buf, &len) != 0) return -1;

    char *line = strtok((char *)buf, "\n");
    while (line) {
        gitex_index_entry e = {0};
        int t = 0, tr = 0;
        if (sscanf(line, "%4095[^\t]\t%64[^\t]\t%d\t%d", e.path, e.hash, &t, &tr) == 4) {
            e.type = (gitex_file_type)t;
            e.tracked = tr ? true : false;
            if (index_upsert(idx, &e) != 0) {
                free(buf);
                return -1;
            }
        }
        line = strtok(NULL, "\n");
    }
    free(buf);
    return 0;
}

int index_save(const gitex_index *idx) {
    FILE *fp = fopen(GITEX_INDEX_FILE, "wb");
    if (!fp) return -1;
    for (size_t i = 0; i < idx->count; ++i) {
        const gitex_index_entry *e = &idx->entries[i];
        if (fprintf(fp, "%s\t%s\t%d\t%d\n", e->path, e->hash, (int)e->type, e->tracked ? 1 : 0) < 0) {
            fclose(fp);
            return -1;
        }
    }
    fclose(fp);
    return 0;
}
