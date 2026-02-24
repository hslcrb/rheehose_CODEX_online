#ifndef GITEX_DIFF_H
#define GITEX_DIFF_H

#include "common.h"

int diff_file(const char *path, const char *old_hash, gitex_file_type type);
int binary_chunk_diff(const char *path, const char *old_hash);
int xml_structural_diff(const char *old_data, const char *new_data);
int zipxml_diff(const char *path, const char *old_hash);

#endif
