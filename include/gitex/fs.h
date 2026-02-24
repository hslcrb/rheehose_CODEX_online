#ifndef GITEX_FS_H
#define GITEX_FS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool fs_exists(const char *path);
bool fs_is_file(const char *path);
bool fs_is_dir(const char *path);
int fs_mkdir_p(const char *path);
int fs_read_file(const char *path, char **data, size_t *len);
int fs_write_file(const char *path, const void *data, size_t len);
int fs_copy_file(const char *src, const char *dst);
int fs_remove_file(const char *path);
int fs_file_stat(const char *path, uint64_t *size, uint64_t *mtime);
int fs_walk_files(const char *root, int (*callback)(const char*, void*), void *ctx);

#endif
