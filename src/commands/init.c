#include "gitex/commands.h"
#include "gitex/repo.h"

int cmd_init(int argc, char **argv) {
    (void)argc; (void)argv;
    return repo_init();
}
