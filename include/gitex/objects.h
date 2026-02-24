#ifndef GITEX_OBJECTS_H
#define GITEX_OBJECTS_H

#include <stddef.h>

int objects_store(const char *type, const void *data, size_t len, char out_hash[65]);
int objects_load(const char *hash, char **data, size_t *len);
int objects_store_file(const char *type, const char *path, char out_hash[65]);

#endif
