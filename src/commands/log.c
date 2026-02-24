#include "gitex/commands.h"
#include "gitex/fs.h"
#include "gitex/objects.h"
#include "gitex/repo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_commit(const char *hash, const char *data) {
    const char *payload = strchr(data, '\n');
    if (!payload) return;
    payload++;
    printf("commit %s\n%s\n", hash, payload);
}

int cmd_log(int argc, char **argv) {
    (void)argc; (void)argv;
    if (!repo_is_initialized()) return -1;

    char ref[256] = {0}, head[65] = {0};
    repo_head_ref(ref, sizeof(ref));
    repo_read_ref(ref, head, sizeof(head));

    char current[65];
    snprintf(current, sizeof(current), "%s", head);

    while (current[0]) {
        char *obj = NULL;
        size_t len = 0;
        if (objects_load(current, &obj, &len) != 0) break;
        print_commit(current, obj);

        char *parent = strstr(obj, "\nparent ");
        if (!parent) { free(obj); break; }
        parent += 8;
        char next[65] = {0};
        sscanf(parent, "%64s", next);
        if (strcmp(next, "-") == 0) next[0] = '\0';
        snprintf(current, sizeof(current), "%s", next);
        free(obj);
    }

    return 0;
}
