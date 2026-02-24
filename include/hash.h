#ifndef GITEX_HASH_H
#define GITEX_HASH_H

#include <stddef.h>

void hash_sha256(const unsigned char *data, size_t len, unsigned char out[32]);
void hash_hex(const unsigned char *in, size_t len, char *out_hex);
int hash_file_sha256(const char *path, char out_hex[65]);

#endif
