#ifndef GITEX_H
#define GITEX_H

#include <stddef.h>
#include <stdint.h>

#define GITEX_DIR ".gitex"
#define GITEX_INDEX ".gitex/index"
#define GITEX_HEAD ".gitex/HEAD"
#define GITEX_OBJECTS ".gitex/objects"
#define GITEX_REFS_HEADS ".gitex/refs/heads"
#define GITEX_REFS_TAGS ".gitex/refs/tags"
#define GITEX_STASH ".gitex/refs/stash"
#define GITEX_MAX_LINE 4096

typedef struct {
    char path[1024];
    char oid[32];
    char kind[32];
} IndexEntry;

typedef struct {
    IndexEntry *items;
    size_t len;
    size_t cap;
} IndexList;

int repo_init(void);
int repo_exists(void);
int repo_head_ref(char *out, size_t out_sz);
int repo_read_head_commit(char *out, size_t out_sz);
int repo_update_ref(const char *ref, const char *oid);
int repo_ensure_parent_dirs(const char *path);

int index_load(IndexList *list);
void index_free(IndexList *list);
int index_save(const IndexList *list);
int index_add_or_update(IndexList *list, const char *path, const char *oid, const char *kind);
int index_remove(IndexList *list, const char *path);
IndexEntry *index_find(IndexList *list, const char *path);

void hash_bytes(const uint8_t *data, size_t len, char out_hex[32]);
int hash_file(const char *path, char out_hex[32]);

int object_write(const char *type, const uint8_t *data, size_t len, char oid_out[32]);
int object_read(const char *oid, char *type_out, size_t type_sz, uint8_t **data_out, size_t *len_out);

int cmd_init(int argc, char **argv);
int cmd_add(int argc, char **argv);
int cmd_status(int argc, char **argv);
int cmd_commit(int argc, char **argv);
int cmd_log(int argc, char **argv);
int cmd_diff(int argc, char **argv);
int cmd_branch(int argc, char **argv);
int cmd_checkout(int argc, char **argv);
int cmd_merge(int argc, char **argv);
int cmd_reset(int argc, char **argv);
int cmd_stash(int argc, char **argv);
int cmd_tag(int argc, char **argv);
int cmd_revert(int argc, char **argv);
int cmd_tui(int argc, char **argv);

int file_read_all(const char *path, uint8_t **buf, size_t *len);
int file_write_all(const char *path, const uint8_t *buf, size_t len);
int path_exists(const char *path);
int is_xml_like(const char *path);
int is_zip_xml_container(const char *path);
int diff_xml_files(const char *a, const char *b);
int diff_binary_chunks(const char *a, const char *b);

#endif
