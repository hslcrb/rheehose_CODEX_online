#include "gitex.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

static uint16_t rd16(const uint8_t *p) { return (uint16_t)p[0] | ((uint16_t)p[1] << 8); }
static uint32_t rd32(const uint8_t *p) { return (uint32_t)rd16(p) | ((uint32_t)rd16(p + 2) << 16); }

static int append_buf(uint8_t **buf, size_t *len, size_t *cap, const uint8_t *src, size_t add) {
    if (*len + add + 1 > *cap) {
        size_t nc = *cap ? *cap * 2 : 4096;
        while (nc < *len + add + 1) nc *= 2;
        uint8_t *n = realloc(*buf, nc);
        if (!n) return -1;
        *buf = n;
        *cap = nc;
    }
    memcpy(*buf + *len, src, add);
    *len += add;
    (*buf)[*len] = '\0';
    return 0;
}

int zipxml_collect_semantic(const char *zip_path, uint8_t **out, size_t *out_len) {
    uint8_t *raw = NULL;
    size_t raw_len = 0;
    if (util_read_file(zip_path, &raw, &raw_len) != 0) return -1;

    size_t off = 0;
    uint8_t *combined = NULL;
    size_t len = 0, cap = 0;
    while (off + 30 <= raw_len) {
        if (rd32(raw + off) != 0x04034b50) break;
        uint16_t method = rd16(raw + off + 8);
        uint32_t csize = rd32(raw + off + 18);
        uint32_t usize = rd32(raw + off + 22);
        uint16_t nlen = rd16(raw + off + 26);
        uint16_t xlen = rd16(raw + off + 28);
        if (off + 30 + nlen + xlen > raw_len) break;

        const uint8_t *name = raw + off + 30;
        const uint8_t *cdata = raw + off + 30 + nlen + xlen;
        if ((size_t)(cdata - raw) + csize > raw_len) break;

        if (nlen > 4 && memcmp(name + nlen - 4, ".xml", 4) == 0) {
            uint8_t *xml = NULL;
            size_t xml_len = 0;
            if (method == 0) {
                xml = malloc(csize + 1);
                if (!xml) { free(raw); free(combined); return -1; }
                memcpy(xml, cdata, csize);
                xml_len = csize;
            } else if (method == 8) {
                xml = malloc(usize + 1);
                if (!xml) { free(raw); free(combined); return -1; }
                z_stream zs = {0};
                zs.next_in = (Bytef *)cdata;
                zs.avail_in = csize;
                zs.next_out = xml;
                zs.avail_out = usize;
                if (inflateInit2(&zs, -MAX_WBITS) != Z_OK) { free(xml); free(raw); free(combined); return -1; }
                int irc = inflate(&zs, Z_FINISH);
                inflateEnd(&zs);
                if (irc != Z_STREAM_END) { free(xml); free(raw); free(combined); return -1; }
                xml_len = usize - zs.avail_out;
            }
            if (xml) {
                uint8_t *norm = NULL;
                size_t norm_len = 0;
                if (xml_semantic_normalize(xml, xml_len, &norm, &norm_len) == 0) {
                    append_buf(&combined, &len, &cap, name, nlen);
                    append_buf(&combined, &len, &cap, (const uint8_t *)"\n", 1);
                    append_buf(&combined, &len, &cap, norm, norm_len);
                    append_buf(&combined, &len, &cap, (const uint8_t *)"\n", 1);
                    free(norm);
                }
                free(xml);
            }
        }
        off = (size_t)(cdata - raw) + csize;
    }

    free(raw);
    if (!combined) {
        combined = malloc(1);
        if (!combined) return -1;
        combined[0] = '\0';
        len = 0;
    }
    *out = combined;
    *out_len = len;
    return 0;
}
