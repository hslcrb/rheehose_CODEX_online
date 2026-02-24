#include "gitex/commands.h"
#include "gitex/common.h"
#include "gitex/repo.h"

#include <stdio.h>

int cmd_checkout(int argc, char **argv) {
    if (!repo_is_initialized()) return -1;
    if (argc < 2) {
        gitex_log_info("usage: gitex checkout <branch>");
        return 0;
    }
    char ref[256];
    snprintf(ref, sizeof(ref), "refs/heads/%s", argv[1]);
    if (repo_set_head_ref(ref) != 0) return -1;
    gitex_log_info("switched HEAD to %s", ref);
    return 0;
}
