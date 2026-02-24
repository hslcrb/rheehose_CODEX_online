CC := cc
CFLAGS := -std=c11 -Wall -Wextra -Werror -pedantic -Iinclude

SRC := $(wildcard src/*.c)
OBJ := $(SRC:.c=.o)

all: gitex

gitex: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

clean:
	rm -f $(OBJ) gitex

.PHONY: all clean
