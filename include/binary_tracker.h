#ifndef GITEX_BINARY_TRACKER_H
#define GITEX_BINARY_TRACKER_H

#include <stddef.h>

#define BINARY_CHUNK 4096

int binary_chunk_signature(const char *path, char **signature);
int binary_diff_signatures(const char *old_sig, const char *new_sig, char **report);

#endif
