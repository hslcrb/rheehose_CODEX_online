#include "gitex.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int ensure_cap(IndexList *l, size_t n) {
    if (l->cap >= n) return 0;
    size_t nc = l->cap ? l->cap * 2 : 16;
    while (nc < n) nc *= 2;
    IndexEntry *ni = realloc(l->items, nc * sizeof(IndexEntry));
    if (!ni) return -1;
    l->items = ni;
    l->cap = nc;
    return 0;
}

int index_load(IndexList *list) {
    memset(list, 0, sizeof(*list));
    uint8_t *buf = NULL; size_t len = 0;
    if (file_read_all(GITEX_INDEX, &buf, &len) != 0) return 0;
        char *line = strtok((char *)buf, "\n");
    while (line) {
        if (*line) {
            if (ensure_cap(list, list->len + 1) != 0) { free(buf); return -1; }
            IndexEntry *e = &list->items[list->len++];
            memset(e, 0, sizeof(*e));
            sscanf(line, "%1023[^\t]\t%31[^\t]\t%31s", e->path, e->kind, e->oid);
        }
        line = strtok(NULL, "\n");
    }
    free(buf);
    return 0;
}

void index_free(IndexList *list) {
    free(list->items);
    memset(list, 0, sizeof(*list));
}

int index_save(const IndexList *list) {
    FILE *f = fopen(GITEX_INDEX, "wb");
    if (!f) return -1;
    for (size_t i = 0; i < list->len; ++i) {
        const IndexEntry *e = &list->items[i];
        fprintf(f, "%s\t%s\t%s\n", e->path, e->kind, e->oid);
    }
    fclose(f);
    return 0;
}

IndexEntry *index_find(IndexList *list, const char *path) {
    for (size_t i = 0; i < list->len; ++i) if (strcmp(list->items[i].path, path) == 0) return &list->items[i];
    return NULL;
}

int index_add_or_update(IndexList *list, const char *path, const char *oid, const char *kind) {
    IndexEntry *e = index_find(list, path);
    if (!e) {
        if (ensure_cap(list, list->len + 1) != 0) return -1;
        e = &list->items[list->len++];
        memset(e, 0, sizeof(*e));
        snprintf(e->path, sizeof(e->path), "%s", path);
    }
    snprintf(e->oid, sizeof(e->oid), "%s", oid);
    snprintf(e->kind, sizeof(e->kind), "%s", kind);
    return 0;
}

int index_remove(IndexList *list, const char *path) {
    for (size_t i = 0; i < list->len; ++i) {
        if (strcmp(list->items[i].path, path) == 0) {
            memmove(&list->items[i], &list->items[i+1], (list->len - i - 1) * sizeof(IndexEntry));
            list->len--;
            return 0;
        }
    }
    return -1;
}
