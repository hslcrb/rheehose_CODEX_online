#include "gitex.h"

#include <stdio.h>
#include <string.h>

static void print_menu(void) {
    printf("\n=== gitex TUI ===\n");
    printf("1) status\n");
    printf("2) log\n");
    printf("3) commit\n");
    printf("4) exit\n");
    printf("select> ");
}

int gitex_tui_run(void) {
    char line[1024];
    while (1) {
        print_menu();
        if (!fgets(line, sizeof(line), stdin)) return -1;
        if (line[0] == '1') {
            gitex_status();
        } else if (line[0] == '2') {
            gitex_log();
        } else if (line[0] == '3') {
            printf("message> ");
            if (!fgets(line, sizeof(line), stdin)) return -1;
            line[strcspn(line, "\n")] = 0;
            gitex_commit(line);
        } else if (line[0] == '4') {
            return 0;
        } else {
            printf("unknown selection\n");
        }
    }
}
