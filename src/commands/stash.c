#include "gitex/commands.h"
#include "gitex/common.h"
#include "gitex/fs.h"
#include "gitex/index.h"
#include "gitex/repo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int cmd_stash(int argc, char **argv) {
    if (!repo_is_initialized()) return -1;
    if (argc >= 2 && strcmp(argv[1], "list") == 0) {
        char *data = NULL; size_t len = 0;
        if (fs_read_file(GITEX_STASH_FILE, &data, &len) == 0) {
            printf("%s", data); free(data);
        }
        return 0;
    }

    gitex_index idx;
    index_init(&idx);
    if (index_load(&idx) != 0) return -1;

    FILE *f = fopen(GITEX_STASH_FILE, "ab");
    if (!f) return -1;
    fprintf(f, "stash snapshot\n");
    for (size_t i = 0; i < idx.count; ++i) {
        if (idx.entries[i].state == STAGED) {
            fprintf(f, "%s\t%s\n", idx.entries[i].path, idx.entries[i].content_hash);
            idx.entries[i].state = TRACKED;
        }
    }
    fclose(f);
    int rc = index_save(&idx);
    index_free(&idx);
    return rc;
}
