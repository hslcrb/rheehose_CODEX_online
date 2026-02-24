#include "gitex.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int xml_semantic_normalize(const uint8_t *xml, size_t xml_len, uint8_t **out, size_t *out_len) {
    size_t cap = xml_len * 2 + 64;
    char *buf = malloc(cap);
    if (!buf) return -1;
    size_t w = 0;
    int in_tag = 0;
    int space_run = 0;

    for (size_t i = 0; i < xml_len; ++i) {
        char c = (char)xml[i];
        if (c == '<') {
            in_tag = 1;
            if (w && buf[w-1] != '\n') buf[w++] = '\n';
            buf[w++] = c;
            space_run = 0;
            continue;
        }
        if (c == '>') {
            buf[w++] = c;
            buf[w++] = '\n';
            in_tag = 0;
            space_run = 0;
            continue;
        }
        if (isspace((unsigned char)c)) {
            if (!space_run && in_tag) buf[w++] = ' ';
            space_run = 1;
            continue;
        }
        space_run = 0;
        if (!in_tag && isspace((unsigned char)c)) continue;
        if (w + 2 >= cap) {
            cap *= 2;
            char *n = realloc(buf, cap);
            if (!n) { free(buf); return -1; }
            buf = n;
        }
        buf[w++] = c;
    }

    *out = (uint8_t *)buf;
    *out_len = w;
    return 0;
}
