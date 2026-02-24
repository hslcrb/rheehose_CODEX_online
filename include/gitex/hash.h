#ifndef GITEX_HASH_H
#define GITEX_HASH_H

#include <stddef.h>
#include <stdint.h>

void hash_bytes(const uint8_t *data, size_t len, char out_hex[65]);
int hash_file(const char *path, char out_hex[65]);

#endif
