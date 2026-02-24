#ifndef GITEX_INDEX_H
#define GITEX_INDEX_H

#include "common.h"

void index_init(gitex_index *index);
void index_free(gitex_index *index);
int index_load(gitex_index *index);
int index_save(const gitex_index *index);
gitex_index_entry *index_find(gitex_index *index, const char *path);
int index_upsert(gitex_index *index, const gitex_index_entry *entry);
int index_set_state(gitex_index *index, const char *path, gitex_track_state state);

#endif
