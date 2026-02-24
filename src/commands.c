#include "gitex.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static int ensure_repo(void) {
    if (!repo_exists()) {
        fprintf(stderr, "not a gitex repo. run gitex init\n");
        return -1;
    }
    return 0;
}

int cmd_init(int argc, char **argv) {
    (void)argc; (void)argv;
    if (repo_exists()) { printf("gitex repo already exists\n"); return 0; }
    if (repo_init() != 0) return 1;
    printf("initialized empty gitex repository in %s\n", GITEX_DIR);
    return 0;
}

int cmd_add(int argc, char **argv) {
    if (ensure_repo() != 0) return 1;
    if (argc < 1) return 1;
    IndexList list; if (index_load(&list) != 0) return 1;
    for (int i = 0; i < argc; ++i) {
        char oid[32] = {0};
        if (hash_file(argv[i], oid) != 0) { fprintf(stderr, "cannot read %s\n", argv[i]); continue; }
        const char *kind = is_zip_xml_container(argv[i]) ? "zipxml" : (is_xml_like(argv[i]) ? "xml" : "binary");
        if (index_add_or_update(&list, argv[i], oid, kind) != 0) { index_free(&list); return 1; }
        printf("tracked %s (%s)\n", argv[i], kind);
    }
    int rc = index_save(&list);
    index_free(&list);
    return rc != 0;
}

int cmd_status(int argc, char **argv) {
    (void)argc; (void)argv;
    if (ensure_repo() != 0) return 1;
    IndexList list; if (index_load(&list) != 0) return 1;
    char head[64] = {0}; repo_read_head_commit(head, sizeof(head));
    printf("HEAD: %s\n", strlen(head) ? head : "(none)");
    for (size_t i = 0; i < list.len; ++i) {
        char oid[32] = {0};
        int exists = path_exists(list.items[i].path);
        if (!exists) {
            printf("missing: %s (still tracked, not deleted)\n", list.items[i].path);
            continue;
        }
        hash_file(list.items[i].path, oid);
        if (strcmp(oid, list.items[i].oid) == 0) printf("clean: %s\n", list.items[i].path);
        else printf("modified: %s\n", list.items[i].path);
    }
    index_free(&list);
    return 0;
}

static int write_tree_from_index(IndexList *list, char tree_oid[32]) {
    size_t cap = list->len * 120 + 1;
    char *buf = malloc(cap);
    if (!buf) return -1;
    buf[0] = 0;
    for (size_t i = 0; i < list->len; ++i) {
        char line[1300];
        snprintf(line, sizeof(line), "%s\t%s\t%s\n", list->items[i].path, list->items[i].kind, list->items[i].oid);
        strncat(buf, line, cap - strlen(buf) - 1);
    }
    int rc = object_write("tree", (const uint8_t *)buf, strlen(buf), tree_oid);
    free(buf);
    return rc;
}

int cmd_commit(int argc, char **argv) {
    if (ensure_repo() != 0) return 1;
    const char *msg = "no message";
    for (int i = 0; i + 1 < argc; ++i) if (strcmp(argv[i], "-m") == 0) msg = argv[i + 1];
    IndexList list; if (index_load(&list) != 0) return 1;
    for (size_t i = 0; i < list.len; ++i) {
        if (!path_exists(list.items[i].path)) continue;
        hash_file(list.items[i].path, list.items[i].oid);
    }
    if (index_save(&list) != 0) { index_free(&list); return 1; }
    char tree[32] = {0};
    if (write_tree_from_index(&list, tree) != 0) { index_free(&list); return 1; }
    index_free(&list);
    char parent[64] = {0}; repo_read_head_commit(parent, sizeof(parent));
    time_t now = time(NULL);
    char body[4096];
    snprintf(body, sizeof(body), "tree %s\nparent %s\ntime %lld\nmessage %s\n", tree, strlen(parent) ? parent : "", (long long)now, msg);
    char commit_oid[32] = {0};
    if (object_write("commit", (const uint8_t *)body, strlen(body), commit_oid) != 0) return 1;
    char ref[256]; if (repo_head_ref(ref, sizeof(ref)) != 0) return 1;
    if (repo_update_ref(ref, commit_oid) != 0) return 1;
    printf("[%s] %s\n", commit_oid, msg);
    return 0;
}

int cmd_log(int argc, char **argv) {
    (void)argc; (void)argv;
    if (ensure_repo() != 0) return 1;
    char oid[64] = {0};
    if (repo_read_head_commit(oid, sizeof(oid)) != 0 || !strlen(oid)) return 0;
    for (int depth = 0; depth < 200 && strlen(oid); ++depth) {
        uint8_t *data = NULL; size_t len = 0; char type[32] = {0};
        if (object_read(oid, type, sizeof(type), &data, &len) != 0) break;
        data[len] = 0;
        printf("commit %s\n%s\n", oid, data);
        char parent[64] = {0};
        sscanf((char *)data, "tree %*s\nparent %63s", parent);
        free(data);
        if (!strlen(parent)) break;
        snprintf(oid, sizeof(oid), "%s", parent);
    }
    return 0;
}

int cmd_diff(int argc, char **argv) {
    if (ensure_repo() != 0) return 1;
    if (argc == 2) {
        if (is_xml_like(argv[0])) return diff_xml_files(argv[0], argv[1]) != 0;
        return diff_binary_chunks(argv[0], argv[1]) != 0;
    }
    IndexList list; if (index_load(&list) != 0) return 1;
    for (size_t i = 0; i < list.len; ++i) {
        if (!path_exists(list.items[i].path)) continue;
        char oid[32] = {0}; hash_file(list.items[i].path, oid);
        if (strcmp(oid, list.items[i].oid) != 0) {
            printf("diff: %s\n", list.items[i].path);
            if (strcmp(list.items[i].kind, "xml") == 0) diff_xml_files(list.items[i].path, list.items[i].path);
            else printf("binary changed (chunk strategy)\n");
        }
    }
    index_free(&list);
    return 0;
}

int cmd_branch(int argc, char **argv) {
    if (ensure_repo() != 0) return 1;
    if (argc == 0) {
        DIR *d = opendir(GITEX_REFS_HEADS);
        struct dirent *de;
        while (d && (de = readdir(d))) if (de->d_name[0] != '.') printf("%s\n", de->d_name);
        if (d) closedir(d);
        return 0;
    }
    char head[64] = {0}; repo_read_head_commit(head, sizeof(head));
    char ref[256]; snprintf(ref, sizeof(ref), "refs/heads/%s", argv[0]);
    return repo_update_ref(ref, head) != 0;
}

static int restore_tree(const char *tree_oid) {
    uint8_t *data = NULL; size_t len = 0; char type[32] = {0};
    if (object_read(tree_oid, type, sizeof(type), &data, &len) != 0) return -1;
    data[len] = 0;
    IndexList list = {0};
        char *line = strtok((char *)data, "\n");
    while (line) {
        char path[1024], kind[32], oid[32];
        if (sscanf(line, "%1023[^\t]\t%31[^\t]\t%31s", path, kind, oid) == 3) {
            uint8_t *blob = NULL; size_t blen = 0; char btype[32] = {0};
            if (object_read(oid, btype, sizeof(btype), &blob, &blen) == 0) {
                file_write_all(path, blob, blen);
                free(blob);
            }
            index_add_or_update(&list, path, oid, kind);
        }
        line = strtok(NULL, "\n");
    }
    index_save(&list);
    index_free(&list);
    free(data);
    return 0;
}

int cmd_checkout(int argc, char **argv) {
    if (ensure_repo() != 0) return 1;
    if (argc < 1) return 1;
    char refpath[512]; snprintf(refpath, sizeof(refpath), ".gitex/refs/heads/%s", argv[0]);
    uint8_t *buf = NULL; size_t len = 0;
    if (file_read_all(refpath, &buf, &len) != 0) return 1;
    char commit[64]; snprintf(commit, sizeof(commit), "%.*s", (int)len, (char *)buf); free(buf);
    char head[256]; snprintf(head, sizeof(head), "ref: refs/heads/%s\n", argv[0]);
    file_write_all(GITEX_HEAD, (const uint8_t *)head, strlen(head));
    if (!strlen(commit)) return 0;
    uint8_t *cdata = NULL; size_t clen = 0; char type[32] = {0};
    if (object_read(commit, type, sizeof(type), &cdata, &clen) != 0) return 1;
    cdata[clen] = 0;
    char tree[32] = {0}; sscanf((char *)cdata, "tree %31s", tree); free(cdata);
    return restore_tree(tree) != 0;
}

int cmd_merge(int argc, char **argv) {
    if (ensure_repo() != 0) return 1;
    if (argc < 1) return 1;
    char refpath[512]; snprintf(refpath, sizeof(refpath), ".gitex/refs/heads/%s", argv[0]);
    uint8_t *buf = NULL; size_t len = 0;
    if (file_read_all(refpath, &buf, &len) != 0) return 1;
    char target[64]; snprintf(target, sizeof(target), "%.*s", (int)len, (char *)buf); free(buf);
    if (!strlen(target)) return 1;
    // simplistic: fast-forward by checking out target tree then creating merge commit marker
    uint8_t *cdata = NULL; size_t clen = 0; char type[32] = {0};
    if (object_read(target, type, sizeof(type), &cdata, &clen) != 0) return 1;
    cdata[clen] = 0;
    char tree[32] = {0}; sscanf((char *)cdata, "tree %31s", tree); free(cdata);
    if (restore_tree(tree) != 0) return 1;
    char *args[] = {"-m", "merge commit"};
    return cmd_commit(2, args);
}

int cmd_reset(int argc, char **argv) {
    if (ensure_repo() != 0) return 1;
    if (argc >= 2 && strcmp(argv[0], "--discard") == 0) {
        IndexList list; if (index_load(&list) != 0) return 1;
        index_remove(&list, argv[1]);
        index_save(&list);
        index_free(&list);
        printf("untracked %s (file kept on disk)\n", argv[1]);
        return 0;
    }
    if (argc >= 1) {
        char oid[64]; snprintf(oid, sizeof(oid), "%s", argv[0]);
        uint8_t *cdata = NULL; size_t clen = 0; char type[32] = {0};
        if (object_read(oid, type, sizeof(type), &cdata, &clen) != 0) return 1;
        cdata[clen] = 0;
        char tree[32] = {0}; sscanf((char *)cdata, "tree %31s", tree); free(cdata);
        return restore_tree(tree) != 0;
    }
    return 0;
}

int cmd_stash(int argc, char **argv) {
    if (ensure_repo() != 0) return 1;
    if (argc < 1 || strcmp(argv[0], "push") != 0) return 1;
    IndexList list; if (index_load(&list) != 0) return 1;
    char tree[32] = {0}; if (write_tree_from_index(&list, tree) != 0) { index_free(&list); return 1; }
    index_free(&list);
    char line[128]; snprintf(line, sizeof(line), "%lld %s\n", (long long)time(NULL), tree);
    FILE *f = fopen(GITEX_STASH, "ab"); if (!f) return 1; fwrite(line, 1, strlen(line), f); fclose(f);
    printf("stashed tree %s\n", tree);
    return 0;
}

int cmd_tag(int argc, char **argv) {
    if (ensure_repo() != 0) return 1;
    if (argc < 1) return 1;
    char head[64] = {0}; if (repo_read_head_commit(head, sizeof(head)) != 0) return 1;
    char ref[256]; snprintf(ref, sizeof(ref), "refs/tags/%s", argv[0]);
    return repo_update_ref(ref, head) != 0;
}

int cmd_revert(int argc, char **argv) {
    if (ensure_repo() != 0 || argc < 1) return 1;
    char *args[] = {argv[0]};
    if (cmd_reset(1, args) != 0) return 1;
    char *cargs[] = {"-m", "revert commit"};
    return cmd_commit(2, cargs);
}
