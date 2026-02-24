#include "gitex/commands.h"
#include "gitex/common.h"

#include <stdio.h>
#include <string.h>

typedef int (*cmd_fn)(int, char**);

typedef struct {
    const char *name;
    cmd_fn fn;
} cmd_entry;

static cmd_entry COMMANDS[] = {
    {"init", cmd_init},
    {"add", cmd_add},
    {"commit", cmd_commit},
    {"status", cmd_status},
    {"diff", cmd_diff},
    {"log", cmd_log},
    {"branch", cmd_branch},
    {"checkout", cmd_checkout},
    {"merge", cmd_merge},
    {"reset", cmd_reset},
    {"stash", cmd_stash},
    {"tag", cmd_tag},
    {"revert", cmd_revert},
    {"tui", cmd_tui},
};

static void usage(void) {
    puts("gitex <command> [args]");
}

int main(int argc, char **argv) {
    if (argc < 2) {
        usage();
        return 1;
    }

    size_t count = sizeof(COMMANDS) / sizeof(COMMANDS[0]);
    for (size_t i = 0; i < count; ++i) {
        if (strcmp(argv[1], COMMANDS[i].name) == 0) {
            return COMMANDS[i].fn(argc - 1, argv + 1);
        }
    }

    gitex_log_error("unknown command: %s", argv[1]);
    usage();
    return 1;
}
