CC ?= cc
CFLAGS ?= -Iinclude -std=c11 -Wall -Wextra -pedantic -O2
LDFLAGS ?=
LDLIBS ?= -lz -lncurses

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)

all: gitex

gitex: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(OBJ) gitex

.PHONY: all clean
