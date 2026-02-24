#include "gitex/commands.h"
#include "gitex/common.h"
#include "gitex/fs.h"
#include "gitex/index.h"
#include "gitex/objects.h"
#include "gitex/repo.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int working_copy_matches_object(const char *path, const char *obj_hash) {
    char *file_data = NULL, *obj_data = NULL;
    size_t file_len = 0, obj_len = 0;
    if (fs_read_file(path, &file_data, &file_len) != 0) return 0;
    if (objects_load(obj_hash, &obj_data, &obj_len) != 0) { free(file_data); return 0; }

    char *payload = strchr(obj_data, '\n');
    if (!payload) { free(file_data); free(obj_data); return 0; }
    payload++;
    size_t payload_len = obj_len - (size_t)(payload - obj_data);

    int same = (file_len == payload_len && memcmp(file_data, payload, file_len) == 0);
    free(file_data);
    free(obj_data);
    return same;
}

int cmd_status(int argc, char **argv) {
    (void)argc; (void)argv;
    if (!repo_is_initialized()) return -1;

    gitex_index idx;
    index_init(&idx);
    if (index_load(&idx) != 0) return -1;

    printf("Tracking matrix (file exists != tracked):\n");
    for (size_t i = 0; i < idx.count; ++i) {
        gitex_index_entry *e = &idx.entries[i];
        int exists = fs_exists(e->path);
        int dirty = 0;
        if (exists && e->state != EXCLUDED) dirty = !working_copy_matches_object(e->path, e->content_hash);

        const char *state = (e->state == TRACKED) ? "TRACKED" : (e->state == STAGED ? "STAGED" : "EXCLUDED");
        printf("%-8s %s %s%s\n", state, exists ? "present" : "missing", e->path, dirty ? " [modified]" : "");
    }

    index_free(&idx);
    return 0;
}
