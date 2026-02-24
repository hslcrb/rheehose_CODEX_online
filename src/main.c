#include "gitex.h"

#include <stdio.h>
#include <string.h>

static void usage(void) {
    printf("gitex commands:\n");
    printf("  init add commit status diff log branch checkout merge reset stash tag revert tui\n");
}

int main(int argc, char **argv) {
    if (argc < 2) { usage(); return 1; }
    const char *cmd = argv[1];
    int cargc = argc - 2;
    char **cargv = argv + 2;

    if (strcmp(cmd, "init") == 0) return cmd_init();
    if (strcmp(cmd, "add") == 0) return cmd_add(cargc, cargv);
    if (strcmp(cmd, "status") == 0) return cmd_status();
    if (strcmp(cmd, "commit") == 0) return cmd_commit(cargc, cargv);
    if (strcmp(cmd, "log") == 0) return cmd_log();
    if (strcmp(cmd, "diff") == 0) return cmd_diff(cargc, cargv);
    if (strcmp(cmd, "branch") == 0) return cmd_branch(cargc, cargv);
    if (strcmp(cmd, "checkout") == 0) return cmd_checkout(cargc, cargv);
    if (strcmp(cmd, "merge") == 0) return cmd_merge(cargc, cargv);
    if (strcmp(cmd, "reset") == 0) return cmd_reset(cargc, cargv);
    if (strcmp(cmd, "stash") == 0) return cmd_stash(cargc, cargv);
    if (strcmp(cmd, "tag") == 0) return cmd_tag(cargc, cargv);
    if (strcmp(cmd, "revert") == 0) return cmd_revert(cargc, cargv);
    if (strcmp(cmd, "tui") == 0) return cmd_tui();
    usage();
    return 1;
}
