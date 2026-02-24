#include "gitex/commands.h"
#include "gitex/common.h"
#include "gitex/filetype.h"
#include "gitex/fs.h"
#include "gitex/hash.h"
#include "gitex/index.h"
#include "gitex/repo.h"

#include <stdio.h>
#include <string.h>

static int stage_path(gitex_index *idx, const char *path) {
    if (!fs_is_file(path)) {
        gitex_log_warn("skip non-file: %s", path);
        return 0;
    }

    gitex_index_entry e = {0};
    snprintf(e.path, sizeof(e.path), "%s", path);
    if (hash_file(path, e.content_hash) != 0) return -1;
    e.type = detect_file_type(path);
    e.state = STAGED;
    if (fs_file_stat(path, &e.size, &e.mtime) != 0) return -1;

    gitex_log_info("staged %s (%s)", path, file_type_name(e.type));
    return index_upsert(idx, &e);
}

int cmd_add(int argc, char **argv) {
    if (!repo_is_initialized()) return -1;

    gitex_index idx;
    index_init(&idx);
    if (index_load(&idx) != 0) return -1;

    if (argc >= 3 && strcmp(argv[1], "--discard") == 0) {
        if (index_set_state(&idx, argv[2], EXCLUDED) != 0) {
            gitex_log_warn("path not tracked yet, marking metadata-only exclude: %s", argv[2]);
            gitex_index_entry e = {0};
            snprintf(e.path, sizeof(e.path), "%s", argv[2]);
            e.state = EXCLUDED;
            snprintf(e.content_hash, sizeof(e.content_hash), "-");
            e.type = detect_file_type(argv[2]);
            index_upsert(&idx, &e);
        }
        gitex_log_info("discarded from tracking (file kept): %s", argv[2]);
        int rc = index_save(&idx);
        index_free(&idx);
        return rc;
    }

    for (int i = 1; i < argc; ++i) {
        if (stage_path(&idx, argv[i]) != 0) {
            index_free(&idx);
            return -1;
        }
    }

    int rc = index_save(&idx);
    index_free(&idx);
    return rc;
}
