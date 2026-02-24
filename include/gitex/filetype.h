#ifndef GITEX_FILETYPE_H
#define GITEX_FILETYPE_H

#include "common.h"

gitex_file_type detect_file_type(const char *path);
const char *file_type_name(gitex_file_type type);

#endif
