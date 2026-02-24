#include "gitex/commands.h"
#include "gitex/common.h"

int cmd_revert(int argc, char **argv) {
    (void)argc; (void)argv;
    gitex_log_warn("revert commit generation is not yet implemented");
    return 0;
}
