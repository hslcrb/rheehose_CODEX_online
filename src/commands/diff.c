#include "gitex/commands.h"
#include "gitex/diff.h"
#include "gitex/index.h"
#include "gitex/repo.h"

#include <stdio.h>

int cmd_diff(int argc, char **argv) {
    if (!repo_is_initialized()) return -1;

    gitex_index idx;
    index_init(&idx);
    if (index_load(&idx) != 0) return -1;

    if (argc >= 2) {
        gitex_index_entry *e = index_find(&idx, argv[1]);
        if (e) diff_file(e->path, e->content_hash, e->type);
    } else {
        for (size_t i = 0; i < idx.count; ++i) {
            gitex_index_entry *e = &idx.entries[i];
            if (e->state != EXCLUDED) diff_file(e->path, e->content_hash, e->type);
        }
    }

    index_free(&idx);
    return 0;
}
