#include "xml_tracker.h"
#include "util.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int build_xml_signature(const unsigned char *buf, size_t len, char **out) {
    size_t cap = 4096;
    size_t used = 0;
    char *sig = malloc(cap);
    if (!sig) return -1;
    sig[0] = '\0';

    for (size_t i = 0; i < len; ++i) {
        if (buf[i] == '<' && i + 2 < len && isalpha((unsigned char)buf[i+1])) {
            size_t j = i + 1;
            char tag[128] = {0};
            size_t t = 0;
            while (j < len && t < sizeof(tag)-1 && (isalnum((unsigned char)buf[j]) || buf[j]=='_' || buf[j]==':' || buf[j]=='-')) {
                tag[t++] = (char)tolower((unsigned char)buf[j]);
                j++;
            }
            if (t > 0) {
                size_t need = strlen(tag) + 2;
                if (used + need + 1 > cap) {
                    cap *= 2;
                    char *tmp = realloc(sig, cap);
                    if (!tmp) { free(sig); return -1; }
                    sig = tmp;
                }
                strcpy(sig + used, tag);
                used += strlen(tag);
                sig[used++] = '\n';
                sig[used] = '\0';
            }
        }
    }
    *out = sig;
    return 0;
}

int xml_extract_semantic_signature(const char *path, char **signature) {
    unsigned char *buf = NULL;
    size_t len = 0;

    if (util_read_file(path, &buf, &len) == 0) {
        int rc = build_xml_signature(buf, len, signature);
        free(buf);
        return rc;
    }

    return -1;
}

int xml_diff_signatures(const char *old_sig, const char *new_sig, char **report) {
    size_t cap = strlen(old_sig) + strlen(new_sig) + 256;
    char *out = malloc(cap);
    if (!out) return -1;

    snprintf(out, cap,
             "[XML semantic diff]\n"
             "- previous signature length: %zu\n"
             "- current signature length: %zu\n"
             "- note: enhanced node-level diff engine hook point.\n",
             strlen(old_sig), strlen(new_sig));
    *report = out;
    return 0;
}
