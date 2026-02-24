#ifndef GITEX_H
#define GITEX_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define GITEX_DIR ".gitex"
#define GITEX_INDEX ".gitex/index"
#define GITEX_HEAD ".gitex/HEAD"
#define GITEX_REFS ".gitex/refs"
#define GITEX_OBJECTS ".gitex/objects"
#define GITEX_COMMITS ".gitex/commits"
#define GITEX_STASH ".gitex/stash"
#define GITEX_TAGS ".gitex/tags"
#define GITEX_DEFAULT_BRANCH "main"

#define GITEX_HASH_HEX 64
#define GITEX_PATH_MAX 4096
#define GITEX_CHUNK_SIZE 4096

typedef enum {
    TRACK_TEXT,
    TRACK_XML,
    TRACK_ZIP_XML,
    TRACK_BINARY
} TrackKind;

typedef enum {
    IDX_TRACKED,
    IDX_EXCLUDED
} IndexState;

typedef struct {
    char path[GITEX_PATH_MAX];
    char hash[GITEX_HASH_HEX + 1];
    TrackKind kind;
    IndexState state;
} IndexEntry;

typedef struct {
    IndexEntry *entries;
    size_t count;
    size_t cap;
} Index;

typedef struct {
    char id[GITEX_HASH_HEX + 1];
    char parent[GITEX_HASH_HEX + 1];
    char branch[128];
    char message[256];
    int64_t timestamp;
} CommitMeta;

int cmd_init(void);
int cmd_add(int argc, char **argv);
int cmd_status(void);
int cmd_commit(int argc, char **argv);
int cmd_log(void);
int cmd_diff(int argc, char **argv);
int cmd_branch(int argc, char **argv);
int cmd_checkout(int argc, char **argv);
int cmd_merge(int argc, char **argv);
int cmd_reset(int argc, char **argv);
int cmd_stash(int argc, char **argv);
int cmd_tag(int argc, char **argv);
int cmd_revert(int argc, char **argv);
int cmd_tui(void);

int repo_find_root(char *out, size_t out_sz);
int repo_init_layout(void);

int index_load(Index *idx);
int index_save(const Index *idx);
void index_free(Index *idx);
int index_upsert(Index *idx, const IndexEntry *entry);
IndexEntry *index_find(Index *idx, const char *path);

TrackKind detect_track_kind(const char *path);
const char *track_kind_name(TrackKind kind);

int hash_file_sha256(const char *path, char out_hex[GITEX_HASH_HEX + 1]);
int hash_buffer_sha256(const uint8_t *buf, size_t len, char out_hex[GITEX_HASH_HEX + 1]);

int object_store_blob_from_file(const char *path, char out_hash[GITEX_HASH_HEX + 1]);
int object_store_blob_from_memory(const uint8_t *data, size_t len, char out_hash[GITEX_HASH_HEX + 1]);
int object_read_to_memory(const char *hash, uint8_t **data, size_t *len);

int util_read_file(const char *path, uint8_t **out, size_t *out_len);
int util_write_file(const char *path, const uint8_t *data, size_t len);
int util_mkdir_p(const char *path);
bool util_file_exists(const char *path);
bool util_is_dir(const char *path);
int util_walk_files(const char *root, int (*cb)(const char *, void *), void *ctx);
int util_relpath(const char *base, const char *target, char *out, size_t out_sz);

int zipxml_collect_semantic(const char *zip_path, uint8_t **out, size_t *out_len);
int xml_semantic_normalize(const uint8_t *xml, size_t xml_len, uint8_t **out, size_t *out_len);
int binary_chunk_diff_report(const char *old_path, const char *new_path, char **report);

#endif
