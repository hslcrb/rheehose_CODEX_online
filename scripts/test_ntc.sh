#!/usr/bin/env bash
set -euo pipefail

cat > hello.c <<'SRC'
#include <stdio.h>
int main(void) { puts("ntc"); return 0; }
SRC

./ntc -c hello.c -O2 -o hello.o
test -f hello.o

./ntc hello.c -O2 -o hello_ntc
./hello_ntc | grep -q ntc

./ntc hello.c -O2 -o hello_ntc >/tmp/ntc_test.log
grep -q "cache hits:" /tmp/ntc_test.log

rm -f hello.c hello.o hello_ntc /tmp/ntc_test.log
