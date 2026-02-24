#ifndef GITEX_REPO_H
#define GITEX_REPO_H

#include <stdbool.h>
#include <stddef.h>

bool repo_is_initialized(void);
int repo_init(void);
int repo_head_ref(char *out_ref, size_t len);
int repo_set_head_ref(const char *ref);
int repo_read_ref(const char *ref, char *out_hash, size_t len);
int repo_write_ref(const char *ref, const char *hash);

#endif
