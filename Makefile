CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O2 -Iinclude
LDFLAGS ?=

SRC := $(shell find src -name '*.c' | sort)
OBJ := $(SRC:.c=.o)

ifeq ($(USE_NCURSES),1)
  CFLAGS += -DGITEX_HAVE_NCURSES
  LDFLAGS += -lncurses
endif

all: gitex

gitex: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) gitex

.PHONY: all clean
