#include "gitex/commands.h"
#include "gitex/common.h"
#include "gitex/repo.h"

#include <stdio.h>

int cmd_branch(int argc, char **argv) {
    if (!repo_is_initialized()) return -1;
    if (argc < 2) {
        gitex_log_info("usage: gitex branch <name>");
        return 0;
    }

    char head_ref[256] = {0}, head_hash[65] = {0};
    repo_head_ref(head_ref, sizeof(head_ref));
    repo_read_ref(head_ref, head_hash, sizeof(head_hash));

    char ref[256];
    snprintf(ref, sizeof(ref), "refs/heads/%s", argv[1]);
    if (repo_write_ref(ref, head_hash) != 0) return -1;
    gitex_log_info("created branch %s at %s", argv[1], head_hash);
    return 0;
}
