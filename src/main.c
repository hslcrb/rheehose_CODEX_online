#include "gitex.h"

#include <stdio.h>
#include <string.h>

typedef int (*cmd_fn)(int, char **);

typedef struct { const char *name; cmd_fn fn; } Command;

static Command commands[] = {
    {"init", cmd_init}, {"add", cmd_add}, {"status", cmd_status}, {"commit", cmd_commit},
    {"log", cmd_log}, {"diff", cmd_diff}, {"branch", cmd_branch}, {"checkout", cmd_checkout},
    {"merge", cmd_merge}, {"reset", cmd_reset}, {"stash", cmd_stash}, {"tag", cmd_tag},
    {"revert", cmd_revert}, {"tui", cmd_tui},
};

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: gitex <command> [args]\n");
        return 1;
    }
    for (size_t i = 0; i < sizeof(commands)/sizeof(commands[0]); ++i) {
        if (strcmp(argv[1], commands[i].name) == 0) {
            return commands[i].fn(argc - 2, argv + 2);
        }
    }
    fprintf(stderr, "unknown command: %s\n", argv[1]);
    return 1;
}
