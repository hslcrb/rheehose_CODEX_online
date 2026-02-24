#ifndef GITEX_UTIL_H
#define GITEX_UTIL_H

#include <stddef.h>

int util_ensure_dir(const char *path);
int util_read_file(const char *path, unsigned char **buf, size_t *len);
int util_write_file(const char *path, const unsigned char *buf, size_t len);
int util_append_file(const char *path, const char *text);
int util_file_exists(const char *path);
int util_is_dir(const char *path);
int util_copy_file(const char *src, const char *dst);
int util_walk_files(const char *root, int (*cb)(const char *, void *), void *ctx);
char *util_trim(char *s);

#endif
