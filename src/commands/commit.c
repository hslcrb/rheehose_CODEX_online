#include "gitex/commands.h"
#include "gitex/common.h"
#include "gitex/index.h"
#include "gitex/objects.h"
#include "gitex/repo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static const char *arg_value(int argc, char **argv, const char *flag, const char *def) {
    for (int i = 1; i + 1 < argc; ++i) if (strcmp(argv[i], flag) == 0) return argv[i + 1];
    return def;
}

int cmd_commit(int argc, char **argv) {
    if (!repo_is_initialized()) return -1;

    const char *msg = arg_value(argc, argv, "-m", "no message");
    gitex_index idx;
    index_init(&idx);
    if (index_load(&idx) != 0) return -1;

    char tree[65536] = {0};
    size_t off = 0;
    for (size_t i = 0; i < idx.count; ++i) {
        gitex_index_entry *e = &idx.entries[i];
        if (e->state == EXCLUDED) continue;
        if (e->state == STAGED) {
            if (objects_store_file("blob", e->path, e->content_hash) != 0) {
                index_free(&idx);
                return -1;
            }
            e->state = TRACKED;
        }
        off += snprintf(tree + off, sizeof(tree) - off, "%s\t%s\t%d\n", e->path, e->content_hash, e->type);
    }

    char tree_hash[65] = {0};
    if (objects_store("tree", tree, strlen(tree), tree_hash) != 0) { index_free(&idx); return -1; }

    char ref[256] = {0}, parent[65] = {0};
    repo_head_ref(ref, sizeof(ref));
    repo_read_ref(ref, parent, sizeof(parent));

    char commit_body[8192];
    snprintf(commit_body, sizeof(commit_body),
             "tree %s\nparent %s\ntime %lld\nmessage %s\n",
             tree_hash,
             parent[0] ? parent : "-",
             (long long)time(NULL),
             msg);

    char commit_hash[65] = {0};
    if (objects_store("commit", commit_body, strlen(commit_body), commit_hash) != 0) { index_free(&idx); return -1; }
    if (repo_write_ref(ref, commit_hash) != 0) { index_free(&idx); return -1; }

    if (index_save(&idx) != 0) { index_free(&idx); return -1; }
    index_free(&idx);

    gitex_log_info("commit %s", commit_hash);
    return 0;
}
