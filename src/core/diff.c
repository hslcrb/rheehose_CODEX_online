#include "gitex/diff.h"
#include "gitex/fs.h"
#include "gitex/objects.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int diff_file(const char *path, const char *old_hash, gitex_file_type type) {
    if (type == GITEX_FILE_BINARY) return binary_chunk_diff(path, old_hash);
    if (type == GITEX_FILE_ZIPXML) return zipxml_diff(path, old_hash);

    char *new_data = NULL;
    size_t new_len = 0;
    if (fs_read_file(path, &new_data, &new_len) != 0) return -1;

    char *old_payload = NULL;
    size_t old_len = 0;
    if (old_hash[0] && objects_load(old_hash, &old_payload, &old_len) != 0) old_payload = NULL;

    char *old_data = "";
    if (old_payload) {
        old_data = strchr(old_payload, '\n');
        if (old_data) old_data++;
        else old_data = "";
    }

    if (type == GITEX_FILE_XML) {
        xml_structural_diff(old_data, new_data);
    } else {
        printf("Text diff not implemented yet, file=%s\n", path);
    }

    free(new_data);
    free(old_payload);
    return 0;
}
