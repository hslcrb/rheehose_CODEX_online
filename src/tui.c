#include "gitex.h"

#include <stdio.h>

#ifdef USE_NCURSES
#include <ncurses.h>
#endif

int cmd_tui(int argc, char **argv) {
    (void)argc; (void)argv;
#ifdef USE_NCURSES
    initscr();
    cbreak(); noecho();
    mvprintw(0, 0, "gitex TUI - q to quit");
    mvprintw(2, 0, "Commands: status, log are available in CLI.");
    mvprintw(4, 0, "Tracking metadata is independent from file existence.");
    refresh();
    while (getch() != 'q') {}
    endwin();
#else
    printf("gitex built without ncurses; rebuild with USE_NCURSES=1\n");
#endif
    return 0;
}
