#include "gitex.h"

#include <stdio.h>
#include <string.h>

static void usage(void) {
    puts("gitex - content-aware VCS for XML containers and binaries");
    puts("usage: gitex <command> [args]");
    puts("commands:");
    puts("  init | add <path> | status | commit <msg> | log | diff <path>");
    puts("  branch <name> | checkout <target> | merge <target> | reset <target>");
    puts("  stash | tag <name> | revert <target> | discard <path> | tui");
}

int main(int argc, char **argv) {
    if (argc < 2) {
        usage();
        return 1;
    }

    const char *cmd = argv[1];
    if (strcmp(cmd, "init") == 0) return gitex_repo_init();
    if (strcmp(cmd, "add") == 0 && argc >= 3) return gitex_add_path(argv[2]);
    if (strcmp(cmd, "status") == 0) return gitex_status();
    if (strcmp(cmd, "commit") == 0 && argc >= 3) return gitex_commit(argv[2]);
    if (strcmp(cmd, "log") == 0) return gitex_log();
    if (strcmp(cmd, "diff") == 0 && argc >= 3) return gitex_diff(argv[2]);
    if (strcmp(cmd, "branch") == 0 && argc >= 3) return gitex_branch(argv[2]);
    if (strcmp(cmd, "checkout") == 0 && argc >= 3) return gitex_checkout(argv[2]);
    if (strcmp(cmd, "merge") == 0 && argc >= 3) return gitex_merge(argv[2]);
    if (strcmp(cmd, "reset") == 0 && argc >= 3) return gitex_reset(argv[2]);
    if (strcmp(cmd, "stash") == 0) return gitex_stash();
    if (strcmp(cmd, "tag") == 0 && argc >= 3) return gitex_tag(argv[2]);
    if (strcmp(cmd, "revert") == 0 && argc >= 3) return gitex_revert(argv[2]);
    if (strcmp(cmd, "discard") == 0 && argc >= 3) return gitex_discard(argv[2]);
    if (strcmp(cmd, "tui") == 0) return gitex_tui_run();

    usage();
    return 1;
}
