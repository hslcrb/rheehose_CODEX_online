#include "gitex.h"

#include <ctype.h>
#include <string.h>

static const char *ext_lower(const char *path) {
    const char *dot = strrchr(path, '.');
    return dot ? dot + 1 : "";
}

TrackKind detect_track_kind(const char *path) {
    const char *ext = ext_lower(path);
    char low[32] = {0};
    for (size_t i = 0; ext[i] && i < sizeof(low) - 1; ++i) low[i] = (char)tolower((unsigned char)ext[i]);

    if (strcmp(low, "xml") == 0 || strcmp(low, "xhtml") == 0) return TRACK_XML;
    if (strcmp(low, "docx") == 0 || strcmp(low, "xlsx") == 0 || strcmp(low, "pptx") == 0 || strcmp(low, "hwpx") == 0) return TRACK_ZIP_XML;
    if (strcmp(low, "txt") == 0 || strcmp(low, "md") == 0 || strcmp(low, "c") == 0 || strcmp(low, "h") == 0 || strcmp(low, "json") == 0) return TRACK_TEXT;
    return TRACK_BINARY;
}

const char *track_kind_name(TrackKind kind) {
    switch (kind) {
        case TRACK_TEXT: return "text";
        case TRACK_XML: return "xml";
        case TRACK_ZIP_XML: return "zip-xml";
        case TRACK_BINARY: return "binary";
    }
    return "unknown";
}
