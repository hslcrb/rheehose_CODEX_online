#ifndef GITEX_INDEX_H
#define GITEX_INDEX_H

#include "gitex.h"

void index_init(gitex_index *idx);
void index_free(gitex_index *idx);
int index_load(gitex_index *idx);
int index_save(const gitex_index *idx);
int index_upsert(gitex_index *idx, const gitex_index_entry *entry);
int index_find(const gitex_index *idx, const char *path);

#endif
