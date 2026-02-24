#include "gitex/hash.h"
#include "gitex/fs.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static uint64_t fnv1a64(const uint8_t *data, size_t len, uint64_t seed) {
    uint64_t h = 1469598103934665603ULL ^ seed;
    for (size_t i = 0; i < len; ++i) {
        h ^= data[i];
        h *= 1099511628211ULL;
    }
    return h;
}

void hash_bytes(const uint8_t *data, size_t len, char out_hex[65]) {
    uint64_t h1 = fnv1a64(data, len, 0xA5A5A5A5ULL);
    uint64_t h2 = fnv1a64(data, len, 0x5A5A5A5AULL);
    uint64_t h3 = fnv1a64(data, len, 0xDEADBEEFULL);
    uint64_t h4 = fnv1a64(data, len, 0x12345678ULL);
    snprintf(out_hex, 65, "%016llx%016llx%016llx%016llx",
             (unsigned long long)h1,
             (unsigned long long)h2,
             (unsigned long long)h3,
             (unsigned long long)h4);
}

int hash_file(const char *path, char out_hex[65]) {
    char *buf = NULL;
    size_t len = 0;
    if (fs_read_file(path, &buf, &len) != 0) return -1;
    hash_bytes((uint8_t*)buf, len, out_hex);
    free(buf);
    return 0;
}
