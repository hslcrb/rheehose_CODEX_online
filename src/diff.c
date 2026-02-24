#include "gitex.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_xml_tokens(const char *label, const uint8_t *buf, size_t len) {
    printf("[%s]\n", label);
    size_t i = 0;
    while (i < len) {
        if (buf[i] == '<') {
            size_t j = i;
            while (j < len && buf[j] != '>') j++;
            if (j < len) j++;
            printf("TAG: %.*s\n", (int)(j - i), buf + i);
            i = j;
        } else {
            size_t j = i;
            while (j < len && buf[j] != '<') j++;
            if (j > i) printf("TEXT: %.*s\n", (int)(j - i), buf + i);
            i = j;
        }
    }
}

int diff_xml_files(const char *a, const char *b) {
    uint8_t *ba = NULL, *bb = NULL;
    size_t la = 0, lb = 0;
    if (file_read_all(a, &ba, &la) != 0 || file_read_all(b, &bb, &lb) != 0) {
        free(ba); free(bb);
        return -1;
    }
    if (la == lb && memcmp(ba, bb, la) == 0) {
        printf("XML identical\n");
    } else {
        print_xml_tokens("A", ba, la);
        print_xml_tokens("B", bb, lb);
    }
    free(ba); free(bb);
    return 0;
}

int diff_binary_chunks(const char *a, const char *b) {
    uint8_t *ba = NULL, *bb = NULL;
    size_t la = 0, lb = 0;
    const size_t chunk = 4096;
    if (file_read_all(a, &ba, &la) != 0 || file_read_all(b, &bb, &lb) != 0) {
        free(ba); free(bb);
        return -1;
    }
    size_t max = la > lb ? la : lb;
    size_t changed = 0;
    for (size_t off = 0; off < max; off += chunk) {
        size_t ra = off < la ? (la - off < chunk ? la - off : chunk) : 0;
        size_t rb = off < lb ? (lb - off < chunk ? lb - off : chunk) : 0;
        char ha[32] = {0}, hb[32] = {0};
        hash_bytes(ra ? ba + off : (const uint8_t *)"", ra, ha);
        hash_bytes(rb ? bb + off : (const uint8_t *)"", rb, hb);
        if (strcmp(ha, hb) != 0) {
            changed++;
            printf("chunk %zu changed (%zu vs %zu bytes)\n", off / chunk, ra, rb);
        }
    }
    printf("changed chunks: %zu\n", changed);
    free(ba); free(bb);
    return 0;
}
