#include "gitex.h"

#include <ncurses.h>

int cmd_tui(void) {
    Index idx;
    if (index_load(&idx) != 0) return 1;
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    mvprintw(0, 0, "gitex TUI - tracked entries (press q to quit)");
    for (size_t i = 0; i < idx.count; ++i) {
        mvprintw((int)i + 2, 0, "%s [%s]", idx.entries[i].path,
                 idx.entries[i].state == IDX_TRACKED ? "tracked" : "excluded");
    }
    int ch;
    while ((ch = getch()) != 'q') {}
    endwin();
    index_free(&idx);
    return 0;
}
