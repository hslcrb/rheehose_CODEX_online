#include "gitex/diff.h"
#include "gitex/common.h"

#include <stdio.h>
#include <string.h>

static int count_token(const char *s, const char *token) {
    int c = 0;
    const char *p = s;
    size_t n = strlen(token);
    while ((p = strstr(p, token)) != NULL) {
        ++c;
        p += n;
    }
    return c;
}

int xml_structural_diff(const char *old_data, const char *new_data) {
    int old_nodes = count_token(old_data, "<");
    int new_nodes = count_token(new_data, "<");
    int old_attrs = count_token(old_data, "=");
    int new_attrs = count_token(new_data, "=");

    printf("XML structural delta: nodes %+d, attrs %+d\n",
           new_nodes - old_nodes,
           new_attrs - old_attrs);
    return 0;
}
