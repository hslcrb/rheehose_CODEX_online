#include "gitex/diff.h"
#include "gitex/fs.h"
#include "gitex/common.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint16_t rd16(const uint8_t *p) { return (uint16_t)(p[0] | (p[1] << 8)); }
static uint32_t rd32(const uint8_t *p) { return (uint32_t)(p[0] | (p[1]<<8) | (p[2]<<16) | (p[3]<<24)); }

static int list_zip_xml_entries(const uint8_t *buf, size_t len, int *xml_count) {
    size_t i = 0;
    *xml_count = 0;
    while (i + 30 < len) {
        if (rd32(buf + i) != 0x04034b50U) {
            i++;
            continue;
        }
        uint16_t method = rd16(buf + i + 8);
        uint32_t csize = rd32(buf + i + 18);
        uint16_t nlen = rd16(buf + i + 26);
        uint16_t xlen = rd16(buf + i + 28);
        if (i + 30 + nlen + xlen > len) break;

        const char *name = (const char*)(buf + i + 30);
        if (nlen > 4 && strstr(name, ".xml")) {
            (*xml_count)++;
            printf("ZIP-XML entry: %.*s (method=%u, compressed=%u)\n", nlen, name, method, csize);
        }

        size_t next = i + 30 + nlen + xlen + csize;
        if (next <= i) break;
        i = next;
    }
    return 0;
}

int zipxml_diff(const char *path, const char *old_hash) {
    (void)old_hash;
    char *data = NULL;
    size_t len = 0;
    if (fs_read_file(path, &data, &len) != 0) return -1;

    int xml_count = 0;
    list_zip_xml_entries((uint8_t*)data, len, &xml_count);
    printf("ZIP container XML parts discovered: %d\n", xml_count);

    free(data);
    return 0;
}
