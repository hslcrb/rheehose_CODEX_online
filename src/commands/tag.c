#include "gitex/commands.h"
#include "gitex/common.h"
#include "gitex/repo.h"

#include <stdio.h>

int cmd_tag(int argc, char **argv) {
    if (!repo_is_initialized()) return -1;
    if (argc < 2) {
        gitex_log_info("usage: gitex tag <name>");
        return 0;
    }

    char ref[256] = {0}, head[65] = {0}, tag_ref[256] = {0};
    repo_head_ref(ref, sizeof(ref));
    repo_read_ref(ref, head, sizeof(head));
    snprintf(tag_ref, sizeof(tag_ref), "refs/tags/%s", argv[1]);
    return repo_write_ref(tag_ref, head);
}
