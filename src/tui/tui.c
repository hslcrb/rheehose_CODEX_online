#include "gitex/commands.h"
#include "gitex/common.h"
#include "gitex/index.h"
#include "gitex/repo.h"

#ifdef GITEX_HAVE_NCURSES
#include <ncurses.h>
#endif

#include <stdio.h>

int cmd_tui(int argc, char **argv) {
    (void)argc; (void)argv;
    if (!repo_is_initialized()) return -1;

#ifdef GITEX_HAVE_NCURSES
    initscr();
    cbreak();
    noecho();
    mvprintw(0, 0, "gitex TUI - content aware version control");
    mvprintw(2, 0, "q: quit");
    refresh();
    while (getch() != 'q') {}
    endwin();
#else
    gitex_log_warn("ncurses not enabled. rebuild with USE_NCURSES=1");
#endif
    return 0;
}
