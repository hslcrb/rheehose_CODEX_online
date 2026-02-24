#include "binary_tracker.h"
#include "hash.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int binary_chunk_signature(const char *path, char **signature) {
    unsigned char *buf = NULL;
    size_t len = 0;
    if (util_read_file(path, &buf, &len) != 0) return -1;

    size_t chunks = (len + BINARY_CHUNK - 1) / BINARY_CHUNK;
    size_t cap = chunks * 80 + 64;
    char *sig = malloc(cap);
    if (!sig) {
        free(buf);
        return -1;
    }
    sig[0] = '\0';

    for (size_t i = 0; i < chunks; ++i) {
        size_t off = i * BINARY_CHUNK;
        size_t n = (off + BINARY_CHUNK <= len) ? BINARY_CHUNK : (len - off);
        unsigned char digest[32];
        char hex[65];
        hash_sha256(buf + off, n, digest);
        hash_hex(digest, 32, hex);

        char line[96];
        snprintf(line, sizeof(line), "%zu:%s\n", i, hex);
        strncat(sig, line, cap - strlen(sig) - 1);
    }

    free(buf);
    *signature = sig;
    return 0;
}

int binary_diff_signatures(const char *old_sig, const char *new_sig, char **report) {
    size_t cap = strlen(old_sig) + strlen(new_sig) + 256;
    char *out = malloc(cap);
    if (!out) return -1;

    snprintf(out, cap,
             "[Binary chunk diff]\n"
             "- old chunk-signature bytes: %zu\n"
             "- new chunk-signature bytes: %zu\n"
             "- note: chunk-level insert/delete detection extension point.\n",
             strlen(old_sig), strlen(new_sig));
    *report = out;
    return 0;
}
