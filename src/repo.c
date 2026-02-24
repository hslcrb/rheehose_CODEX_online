#include "gitex.h"
#include "binary_tracker.h"
#include "filetype.h"
#include "hash.h"
#include "index.h"
#include "util.h"
#include "xml_tracker.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void normalize_path(const char *in, char *out, size_t out_len) {
    if (!in || !out || out_len == 0) return;
    while (in[0] == '.' && in[1] == '/') in += 2;
    snprintf(out, out_len, "%s", in);
}

int gitex_repo_exists(void) {
    return util_is_dir(GITEX_DIR);
}

int gitex_repo_init(void) {
    if (util_ensure_dir(GITEX_DIR) != 0) return -1;
    if (util_ensure_dir(GITEX_OBJECTS_DIR) != 0) return -1;
    if (util_ensure_dir(GITEX_REFS_DIR) != 0) return -1;

    if (!util_file_exists(GITEX_HEAD_FILE)) {
        const unsigned char *head = (const unsigned char *)"ref: refs/main\n";
        if (util_write_file(GITEX_HEAD_FILE, head, strlen((const char *)head)) != 0) return -1;
    }
    if (!util_file_exists(GITEX_INDEX_FILE)) {
        if (util_write_file(GITEX_INDEX_FILE, (const unsigned char *)"", 0) != 0) return -1;
    }
    if (!util_file_exists(GITEX_COMMITS_FILE)) {
        if (util_write_file(GITEX_COMMITS_FILE, (const unsigned char *)"", 0) != 0) return -1;
    }
    printf("Initialized empty gitex repository in %s\n", GITEX_DIR);
    return 0;
}

static int object_store(const char *hash, const char *src_path) {
    char obj[4096];
    snprintf(obj, sizeof(obj), "%s/%s", GITEX_OBJECTS_DIR, hash);
    if (util_file_exists(obj)) return 0;
    return util_copy_file(src_path, obj);
}

int gitex_add_path(const char *path) {
    if (!gitex_repo_exists()) return -1;
    if (!util_file_exists(path)) return -1;

    gitex_index idx;
    if (index_load(&idx) != 0) return -1;

    gitex_index_entry e = {0};
    normalize_path(path, e.path, sizeof(e.path));
    e.type = filetype_detect(e.path);
    e.tracked = true;

    if (hash_file_sha256(path, e.hash) != 0) {
        index_free(&idx);
        return -1;
    }
    if (object_store(e.hash, path) != 0) {
        index_free(&idx);
        return -1;
    }
    if (index_upsert(&idx, &e) != 0) {
        index_free(&idx);
        return -1;
    }
    int rc = index_save(&idx);
    index_free(&idx);
    printf("tracked: %s (%s)\n", e.path, filetype_name(e.type));
    return rc;
}

static int status_cb(const char *path, void *ctx) {
    (void)ctx;
    gitex_index idx;
    if (index_load(&idx) != 0) return -1;

    char norm[4096];
    normalize_path(path, norm, sizeof(norm));

    int pos = index_find(&idx, norm);
    if (pos < 0 || !idx.entries[pos].tracked) {
        printf("untracked: %s\n", norm);
    } else {
        char now[65];
        if (hash_file_sha256(norm, now) == 0 && strcmp(now, idx.entries[pos].hash) != 0) {
            printf("modified : %s\n", norm);
        }
    }
    index_free(&idx);
    return 0;
}

int gitex_status(void) {
    if (!gitex_repo_exists()) return -1;
    printf("=== gitex status ===\n");
    return util_walk_files(".", status_cb, NULL);
}

int gitex_commit(const char *message) {
    if (!gitex_repo_exists()) return -1;
    gitex_index idx;
    if (index_load(&idx) != 0) return -1;

    time_t now = time(NULL);
    char commit_body[65536];
    size_t off = 0;
    off += snprintf(commit_body + off, sizeof(commit_body) - off,
                    "time=%ld\nmessage=%s\n", (long)now, message);

    for (size_t i = 0; i < idx.count; ++i) {
        gitex_index_entry *e = &idx.entries[i];
        if (!e->tracked) continue;
        char current[65];
        if (hash_file_sha256(e->path, current) == 0) {
            strncpy(e->hash, current, sizeof(e->hash)-1);
            object_store(e->hash, e->path);
        }
        off += snprintf(commit_body + off, sizeof(commit_body) - off,
                        "file=%s\thash=%s\ttype=%d\n", e->path, e->hash, (int)e->type);
    }

    unsigned char digest[32];
    char commit_hash[65];
    hash_sha256((const unsigned char *)commit_body, strlen(commit_body), digest);
    hash_hex(digest, 32, commit_hash);

    char obj_path[4096];
    snprintf(obj_path, sizeof(obj_path), "%s/%s.commit", GITEX_OBJECTS_DIR, commit_hash);
    if (util_write_file(obj_path, (const unsigned char *)commit_body, strlen(commit_body)) != 0) {
        index_free(&idx);
        return -1;
    }

    char line[70000];
    snprintf(line, sizeof(line), "%s\t%ld\t%s\n", commit_hash, (long)now, message);
    if (util_append_file(GITEX_COMMITS_FILE, line) != 0) {
        index_free(&idx);
        return -1;
    }

    int rc = index_save(&idx);
    index_free(&idx);
    printf("[gitex commit] %s\n", commit_hash);
    return rc;
}

int gitex_log(void) {
    if (!gitex_repo_exists()) return -1;
    unsigned char *buf = NULL;
    size_t len = 0;
    if (util_read_file(GITEX_COMMITS_FILE, &buf, &len) != 0) return -1;
    printf("%s", (char *)buf);
    free(buf);
    return 0;
}

int gitex_diff(const char *path) {
    if (!gitex_repo_exists()) return -1;
    char norm[4096];
    normalize_path(path, norm, sizeof(norm));

    gitex_index idx;
    if (index_load(&idx) != 0) return -1;
    int pos = index_find(&idx, norm);
    if (pos < 0) {
        index_free(&idx);
        return -1;
    }

    gitex_index_entry *e = &idx.entries[pos];
    char obj[4096];
    snprintf(obj, sizeof(obj), "%s/%s", GITEX_OBJECTS_DIR, e->hash);

    char *old_sig = NULL;
    char *new_sig = NULL;
    char *report = NULL;
    int rc = -1;

    if (e->type == GITEX_FT_XML_CONTAINER || e->type == GITEX_FT_XML_TEXT) {
        if (xml_extract_semantic_signature(obj, &old_sig) != 0) goto done;
        if (xml_extract_semantic_signature(norm, &new_sig) != 0) goto done;
        if (xml_diff_signatures(old_sig, new_sig, &report) != 0) goto done;
    } else {
        if (binary_chunk_signature(obj, &old_sig) != 0) goto done;
        if (binary_chunk_signature(norm, &new_sig) != 0) goto done;
        if (binary_diff_signatures(old_sig, new_sig, &report) != 0) goto done;
    }

    printf("diff for %s\n%s", norm, report ? report : "");
    rc = 0;

done:
    free(old_sig);
    free(new_sig);
    free(report);
    index_free(&idx);
    return rc;
}

int gitex_discard(const char *path) {
    if (!gitex_repo_exists()) return -1;
    char norm[4096];
    normalize_path(path, norm, sizeof(norm));

    gitex_index idx;
    if (index_load(&idx) != 0) return -1;
    int pos = index_find(&idx, norm);
    if (pos < 0) { index_free(&idx); return -1; }
    idx.entries[pos].tracked = false;
    int rc = index_save(&idx);
    index_free(&idx);
    printf("discard(untrack only): %s\n", norm);
    return rc;
}

#define STUB_CMD(name) int name(const char *arg) { printf("%s: planned (content-graph backend hook). target=%s\n", #name, arg ? arg : "-"); return 0; }
STUB_CMD(gitex_branch)
STUB_CMD(gitex_checkout)
STUB_CMD(gitex_merge)
STUB_CMD(gitex_reset)
int gitex_stash(void) { printf("gitex_stash: planned (workspace snapshot stack).\n"); return 0; }
STUB_CMD(gitex_tag)
STUB_CMD(gitex_revert)
