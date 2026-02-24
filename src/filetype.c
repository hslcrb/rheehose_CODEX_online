#include "filetype.h"

#include <string.h>

static int ends_with(const char *s, const char *suffix) {
    size_t ls = strlen(s), lx = strlen(suffix);
    return ls >= lx && strcmp(s + ls - lx, suffix) == 0;
}

gitex_file_type filetype_detect(const char *path) {
    if (ends_with(path, ".docx") || ends_with(path, ".hwpx") || ends_with(path, ".xlsx")) return GITEX_FT_XML_CONTAINER;
    if (ends_with(path, ".xml")) return GITEX_FT_XML_TEXT;
    if (ends_with(path, ".png") || ends_with(path, ".jpg") || ends_with(path, ".jpeg") ||
        ends_with(path, ".gif") || ends_with(path, ".mp4") || ends_with(path, ".mov") || ends_with(path, ".bin")) return GITEX_FT_BINARY;
    return GITEX_FT_BINARY;
}

const char *filetype_name(gitex_file_type t) {
    switch (t) {
        case GITEX_FT_XML_CONTAINER: return "xml-container";
        case GITEX_FT_XML_TEXT: return "xml-text";
        case GITEX_FT_BINARY: return "binary";
        default: return "unknown";
    }
}
