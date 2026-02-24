#ifndef GITEX_FILETYPE_H
#define GITEX_FILETYPE_H

#include "gitex.h"

gitex_file_type filetype_detect(const char *path);
const char *filetype_name(gitex_file_type t);

#endif
