CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra -Werror -std=c11
LDFLAGS ?= -pthread

BIN := ntc
SRC := src/ntc.c

.PHONY: all clean test

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(BIN) $(LDFLAGS)

test: $(BIN)
	./scripts/test_ntc.sh

clean:
	$(RM) $(BIN) hello_ntc hello.c hello.o
	$(RM) -r .ntc-cache
