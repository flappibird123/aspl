#ifndef IO_FILEOP_H
#define IO_FILEOP_H

#include <stddef.h>

// reads a file into a dynamically allocated buffer
// NOTE: caller free
char *read_file(const char *path, size_t *out_size);

#endif // IO_FILEOP_H
