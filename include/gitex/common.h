#ifndef GITEX_COMMON_H
#define GITEX_COMMON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define GITEX_DIR ".gitex"
#define GITEX_OBJECTS_DIR ".gitex/objects"
#define GITEX_REFS_DIR ".gitex/refs"
#define GITEX_HEADS_DIR ".gitex/refs/heads"
#define GITEX_TAGS_DIR ".gitex/refs/tags"
#define GITEX_INDEX_FILE ".gitex/index"
#define GITEX_HEAD_FILE ".gitex/HEAD"
#define GITEX_LOG_DIR ".gitex/logs"
#define GITEX_STASH_FILE ".gitex/stash"

#define GITEX_HASH_HEX_LEN 64
#define GITEX_MAX_PATH 1024
#define GITEX_CHUNK_SIZE (64 * 1024)

typedef enum {
    GITEX_FILE_TEXT = 0,
    GITEX_FILE_XML,
    GITEX_FILE_ZIPXML,
    GITEX_FILE_BINARY
} gitex_file_type;

typedef enum {
    TRACKED = 0,
    EXCLUDED,
    STAGED
} gitex_track_state;

typedef struct {
    char path[GITEX_MAX_PATH];
    char content_hash[GITEX_HASH_HEX_LEN + 1];
    gitex_file_type type;
    gitex_track_state state;
    uint64_t size;
    uint64_t mtime;
} gitex_index_entry;

typedef struct {
    gitex_index_entry *entries;
    size_t count;
    size_t capacity;
} gitex_index;

void gitex_log_info(const char *fmt, ...);
void gitex_log_warn(const char *fmt, ...);
void gitex_log_error(const char *fmt, ...);

#endif
