#include "gitex/filetype.h"
#include "gitex/fs.h"

#include <string.h>

static int ends_with(const char *s, const char *suffix) {
    size_t a = strlen(s), b = strlen(suffix);
    return a >= b && strcmp(s + a - b, suffix) == 0;
}

gitex_file_type detect_file_type(const char *path) {
    if (ends_with(path, ".docx") || ends_with(path, ".xlsx") || ends_with(path, ".hwpx")) return GITEX_FILE_ZIPXML;
    if (ends_with(path, ".xml")) return GITEX_FILE_XML;
    if (ends_with(path, ".png") || ends_with(path, ".jpg") || ends_with(path, ".jpeg") ||
        ends_with(path, ".mp4") || ends_with(path, ".mov") || ends_with(path, ".avi")) return GITEX_FILE_BINARY;
    return GITEX_FILE_TEXT;
}

const char *file_type_name(gitex_file_type type) {
    switch (type) {
        case GITEX_FILE_TEXT: return "text";
        case GITEX_FILE_XML: return "xml";
        case GITEX_FILE_ZIPXML: return "zipxml";
        case GITEX_FILE_BINARY: return "binary";
        default: return "unknown";
    }
}
