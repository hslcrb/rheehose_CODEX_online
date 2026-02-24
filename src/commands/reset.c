#include "gitex/commands.h"
#include "gitex/common.h"
#include "gitex/index.h"
#include "gitex/repo.h"

#include <string.h>

int cmd_reset(int argc, char **argv) {
    if (!repo_is_initialized()) return -1;
    if (argc >= 2 && strcmp(argv[1], "--hard") == 0) {
        gitex_log_warn("hard reset (worktree rewrite) not implemented to preserve defensive safety");
        return 0;
    }

    gitex_index idx;
    index_init(&idx);
    if (index_load(&idx) != 0) return -1;
    for (size_t i = 0; i < idx.count; ++i) {
        if (idx.entries[i].state == STAGED) idx.entries[i].state = TRACKED;
    }
    int rc = index_save(&idx);
    index_free(&idx);
    return rc;
}
