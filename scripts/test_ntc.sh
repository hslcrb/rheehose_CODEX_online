#!/usr/bin/env bash
set -euo pipefail

cat > hello.c <<'SRC'
#include <stdio.h>
int main(void) { puts("ntc"); return 0; }
SRC

cat > util.c <<'SRC'
int util(void) { return 7; }
SRC

# compile-only with cache pipeline
./ntc -c hello.c -O2 -o hello.o
test -f hello.o

# link with C source
./ntc hello.c -O2 -o hello_ntc
./hello_ntc | grep -q ntc

# second build should show cache hit
./ntc hello.c -O2 -o hello_ntc >/tmp/ntc_test.log
grep -q "cache hits:" /tmp/ntc_test.log

# passthrough compatibility: object-only link should work (not cache pipeline)
cc -c util.c -o util.o
if ./ntc util.o -o util_bin; then
  echo "expected object-only link without main to fail" >&2
  exit 1
fi

# preprocess mode passthrough
./ntc -E hello.c >/tmp/hello.i
grep -q "int main" /tmp/hello.i

rm -f hello.c util.c hello.o util.o hello_ntc util_bin /tmp/ntc_test.log /tmp/hello.i
