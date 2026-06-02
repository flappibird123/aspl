#include "ioutils.h"

#include <stdio.h>
#include <stdarg.h>

int eprintf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int res = vfprintf(stderr, fmt, args);
    va_end(args);
    return res;
}
