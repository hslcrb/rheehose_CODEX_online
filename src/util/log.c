#include "gitex/common.h"

#include <stdarg.h>
#include <stdio.h>

static void vlog(FILE *stream, const char *prefix, const char *fmt, va_list ap) {
    fprintf(stream, "%s", prefix);
    vfprintf(stream, fmt, ap);
    fprintf(stream, "\n");
}

void gitex_log_info(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vlog(stdout, "[gitex] ", fmt, ap);
    va_end(ap);
}

void gitex_log_warn(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vlog(stderr, "[gitex:warn] ", fmt, ap);
    va_end(ap);
}

void gitex_log_error(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vlog(stderr, "[gitex:error] ", fmt, ap);
    va_end(ap);
}
