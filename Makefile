CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O2 -Iinclude
LDFLAGS ?=
SRC = src/main.c src/repo.c src/util.c src/index.c src/object_store.c src/commands.c src/diff.c src/tui.c

ifeq ($(USE_NCURSES),1)
CFLAGS += -DUSE_NCURSES
LDFLAGS += -lncurses
endif

all: gitex

gitex: $(SRC)
	$(CC) $(CFLAGS) -o $@ $(SRC) $(LDFLAGS)

clean:
	rm -f gitex

.PHONY: all clean
