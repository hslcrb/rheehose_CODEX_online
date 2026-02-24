#include "gitex.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static int ensure_repo(void) {
    return util_is_dir(GITEX_DIR) ? 0 : -1;
}

int cmd_init(void) {
    if (repo_init_layout() != 0) {
        fprintf(stderr, "gitex: init failed\n");
        return 1;
    }
    printf("Initialized empty gitex repository in %s\n", GITEX_DIR);
    return 0;
}

static int add_one(Index *idx, const char *path) {
    if (!util_file_exists(path)) return -1;
    IndexEntry e = {0};
    snprintf(e.path, sizeof(e.path), "%s", path);
    e.kind = detect_track_kind(path);
    e.state = IDX_TRACKED;

    if (e.kind == TRACK_ZIP_XML) {
        uint8_t *sem = NULL;
        size_t sem_len = 0;
        if (zipxml_collect_semantic(path, &sem, &sem_len) == 0) {
            object_store_blob_from_memory(sem, sem_len, e.hash);
            free(sem);
        } else {
            object_store_blob_from_file(path, e.hash);
        }
    } else if (e.kind == TRACK_XML) {
        uint8_t *raw = NULL, *norm = NULL;
        size_t rlen = 0, nlen = 0;
        if (util_read_file(path, &raw, &rlen) == 0 && xml_semantic_normalize(raw, rlen, &norm, &nlen) == 0) {
            object_store_blob_from_memory(norm, nlen, e.hash);
            free(norm);
        } else {
            object_store_blob_from_file(path, e.hash);
        }
        free(raw);
    } else {
        object_store_blob_from_file(path, e.hash);
    }
    return index_upsert(idx, &e);
}

struct AddCtx { Index *idx; char root[GITEX_PATH_MAX]; };

static int add_walk_cb(const char *full, void *ctx_v) {
    struct AddCtx *ctx = (struct AddCtx *)ctx_v;
    char rel[GITEX_PATH_MAX];
    util_relpath(ctx->root, full, rel, sizeof(rel));
    return add_one(ctx->idx, rel);
}

int cmd_add(int argc, char **argv) {
    if (ensure_repo() != 0 || argc < 1) return 1;
    Index idx;
    if (index_load(&idx) != 0) return 1;
    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], ".") == 0) {
            struct AddCtx ctx = {.idx = &idx};
            if (!getcwd(ctx.root, sizeof(ctx.root))) { index_free(&idx); return 1; }
            util_walk_files(".", add_walk_cb, &ctx);
        } else {
            add_one(&idx, argv[i]);
        }
    }
    index_save(&idx);
    index_free(&idx);
    return 0;
}

static int read_branch(char *out, size_t sz) {
    uint8_t *buf = NULL;
    size_t len = 0;
    if (util_read_file(GITEX_HEAD, &buf, &len) != 0) return -1;
    snprintf(out, sz, "%s", (char *)buf);
    char *nl = strchr(out, '\n');
    if (nl) *nl = '\0';
    free(buf);
    return 0;
}

static int branch_head_commit(const char *branch, char *out, size_t sz) {
    char path[GITEX_PATH_MAX];
    snprintf(path, sizeof(path), "%s/%s", GITEX_REFS, branch);
    if (!util_file_exists(path)) { out[0] = 0; return 0; }
    uint8_t *buf; size_t len;
    if (util_read_file(path, &buf, &len) != 0) return -1;
    snprintf(out, sz, "%s", (char *)buf);
    char *nl = strchr(out, '\n');
    if (nl) *nl = '\0';
    free(buf);
    return 0;
}

int cmd_status(void) {
    if (ensure_repo() != 0) return 1;
    Index idx;
    index_load(&idx);
    printf("Tracked files:\n");
    for (size_t i = 0; i < idx.count; ++i) {
        IndexEntry *e = &idx.entries[i];
        printf("  %s [%s] (%s)\n", e->path,
               e->state == IDX_TRACKED ? "tracked" : "excluded",
               track_kind_name(e->kind));
    }
    index_free(&idx);
    return 0;
}

int cmd_commit(int argc, char **argv) {
    if (ensure_repo() != 0) return 1;
    const char *msg = "no message";
    for (int i = 0; i < argc; ++i) if (strcmp(argv[i], "-m") == 0 && i + 1 < argc) msg = argv[i + 1];

    Index idx;
    index_load(&idx);
    size_t cap = 1024 + idx.count * 512;
    char *manifest = malloc(cap);
    if (!manifest) return 1;
    size_t w = 0;
    for (size_t i = 0; i < idx.count; ++i) {
        IndexEntry *e = &idx.entries[i];
        if (e->state == IDX_EXCLUDED) continue;
        w += snprintf(manifest + w, cap - w, "%s|%s|%d\n", e->path, e->hash, e->kind);
    }

    char tree_hash[GITEX_HASH_HEX + 1];
    object_store_blob_from_memory((uint8_t *)manifest, w, tree_hash);

    char branch[128] = {0};
    read_branch(branch, sizeof(branch));
    char parent[GITEX_HASH_HEX + 1] = {0};
    branch_head_commit(branch, parent, sizeof(parent));

    char commit_body[8192];
    int64_t ts = (int64_t)time(NULL);
    snprintf(commit_body, sizeof(commit_body), "tree=%s\nparent=%s\nbranch=%s\ntime=%lld\nmessage=%s\n", tree_hash, parent, branch, (long long)ts, msg);
    char commit_id[GITEX_HASH_HEX + 1];
    object_store_blob_from_memory((uint8_t *)commit_body, strlen(commit_body), commit_id);

    char cpath[GITEX_PATH_MAX];
    snprintf(cpath, sizeof(cpath), "%s/%s.meta", GITEX_COMMITS, commit_id);
    util_write_file(cpath, (uint8_t *)commit_body, strlen(commit_body));

    char ref_path[GITEX_PATH_MAX];
    snprintf(ref_path, sizeof(ref_path), "%s/%s", GITEX_REFS, branch);
    char buf[GITEX_HASH_HEX + 2];
    snprintf(buf, sizeof(buf), "%s\n", commit_id);
    util_write_file(ref_path, (uint8_t *)buf, strlen(buf));

    printf("[%s %s] %s\n", branch, commit_id, msg);
    free(manifest);
    index_free(&idx);
    return 0;
}

int cmd_log(void) {
    if (ensure_repo() != 0) return 1;
    char branch[128] = {0}, cur[GITEX_HASH_HEX + 1] = {0};
    read_branch(branch, sizeof(branch));
    branch_head_commit(branch, cur, sizeof(cur));

    while (cur[0]) {
        char cpath[GITEX_PATH_MAX];
        snprintf(cpath, sizeof(cpath), "%s/%s.meta", GITEX_COMMITS, cur);
        uint8_t *buf; size_t len;
        if (util_read_file(cpath, &buf, &len) != 0) break;
        printf("commit %s\n%s\n", cur, buf);
        char *parent = strstr((char *)buf, "parent=");
        if (!parent) { free(buf); break; }
        parent += 7;
        char *nl = strchr(parent, '\n');
        if (nl) *nl = '\0';
        snprintf(cur, sizeof(cur), "%s", parent);
        free(buf);
    }
    return 0;
}

int cmd_diff(int argc, char **argv) {
    if (ensure_repo() != 0) return 1;
    if (argc < 1) return 1;
    if (argc >= 2) {
        char *rep = NULL;
        if (binary_chunk_diff_report(argv[0], argv[1], &rep) == 0) {
            printf("%s\n", rep);
            free(rep);
            return 0;
        }
        return 1;
    }
    Index idx; index_load(&idx);
    IndexEntry *e = index_find(&idx, argv[0]);
    if (!e) { index_free(&idx); return 1; }
    char cur_hash[GITEX_HASH_HEX + 1];
    object_store_blob_from_file(argv[0], cur_hash);
    printf("%s\n", strcmp(cur_hash, e->hash) == 0 ? "clean" : "modified");
    index_free(&idx);
    return 0;
}

int cmd_branch(int argc, char **argv) {
    if (ensure_repo() != 0) return 1;
    if (argc == 0) {
        char cur[128] = {0}; read_branch(cur, sizeof(cur));
        printf("* %s\n", cur);
        return 0;
    }
    char cur[128] = {0}, head[GITEX_HASH_HEX + 1] = {0};
    read_branch(cur, sizeof(cur));
    branch_head_commit(cur, head, sizeof(head));
    char p[GITEX_PATH_MAX]; snprintf(p, sizeof(p), "%s/%s", GITEX_REFS, argv[0]);
    char line[GITEX_HASH_HEX + 2]; snprintf(line, sizeof(line), "%s\n", head);
    util_write_file(p, (uint8_t *)line, strlen(line));
    return 0;
}

int cmd_checkout(int argc, char **argv) {
    if (ensure_repo() != 0 || argc < 1) return 1;
    char p[GITEX_PATH_MAX]; snprintf(p, sizeof(p), "%s/%s", GITEX_REFS, argv[0]);
    if (!util_file_exists(p)) return 1;
    char line[256]; snprintf(line, sizeof(line), "%s\n", argv[0]);
    util_write_file(GITEX_HEAD, (uint8_t *)line, strlen(line));
    return 0;
}

int cmd_merge(int argc, char **argv) { (void)argc; (void)argv; puts("gitex merge: strategy planned"); return 0; }
int cmd_reset(int argc, char **argv) {
    if (ensure_repo() != 0) return 1;
    Index idx; index_load(&idx);
    if (argc > 1 && strcmp(argv[0], "--discard") == 0) {
        IndexEntry *e = index_find(&idx, argv[1]);
        if (e) e->state = IDX_EXCLUDED;
        index_save(&idx);
    }
    index_free(&idx);
    return 0;
}
int cmd_stash(int argc, char **argv) { (void)argc; (void)argv; puts("gitex stash: prototype"); return 0; }
int cmd_tag(int argc, char **argv) {
    if (ensure_repo() != 0 || argc < 1) return 1;
    char branch[128] = {0}, head[GITEX_HASH_HEX + 1] = {0};
    read_branch(branch, sizeof(branch));
    branch_head_commit(branch, head, sizeof(head));
    char p[GITEX_PATH_MAX]; snprintf(p, sizeof(p), "%s/%s", GITEX_TAGS, argv[0]);
    char line[GITEX_HASH_HEX + 2]; snprintf(line, sizeof(line), "%s\n", head);
    util_write_file(p, (uint8_t *)line, strlen(line));
    return 0;
}
int cmd_revert(int argc, char **argv) { (void)argc; (void)argv; puts("gitex revert: prototype"); return 0; }
