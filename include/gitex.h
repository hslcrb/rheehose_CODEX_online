#ifndef GITEX_H
#define GITEX_H

#include <stdbool.h>
#include <stddef.h>

#define GITEX_DIR ".gitex"
#define GITEX_OBJECTS_DIR ".gitex/objects"
#define GITEX_REFS_DIR ".gitex/refs"
#define GITEX_HEAD_FILE ".gitex/HEAD"
#define GITEX_INDEX_FILE ".gitex/index"
#define GITEX_COMMITS_FILE ".gitex/commits.log"
#define GITEX_TRACK_FILE ".gitex/tracked"

#define GITEX_PATH_MAX 4096
#define GITEX_HASH_HEX_LEN 64

typedef enum {
    GITEX_FT_UNKNOWN = 0,
    GITEX_FT_XML_CONTAINER,
    GITEX_FT_XML_TEXT,
    GITEX_FT_BINARY
} gitex_file_type;

typedef struct {
    char path[GITEX_PATH_MAX];
    char hash[GITEX_HASH_HEX_LEN + 1];
    gitex_file_type type;
    bool tracked;
} gitex_index_entry;

typedef struct {
    gitex_index_entry *entries;
    size_t count;
    size_t cap;
} gitex_index;

int gitex_repo_init(void);
int gitex_repo_exists(void);

int gitex_add_path(const char *path);
int gitex_status(void);
int gitex_commit(const char *message);
int gitex_log(void);
int gitex_diff(const char *path);
int gitex_discard(const char *path);

int gitex_branch(const char *name);
int gitex_checkout(const char *target);
int gitex_merge(const char *target);
int gitex_reset(const char *target);
int gitex_stash(void);
int gitex_tag(const char *name);
int gitex_revert(const char *target);

int gitex_tui_run(void);

#endif
