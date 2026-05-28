#include "fileop.h"

#include <stdio.h>
#include <stdlib.h>

char *read_file(const char *path, size_t *out_size) {
  FILE *file = fopen(path, "rb");
  if (!file) {
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  rewind(file);

  if (size < 0) {
    fclose(file);
    return NULL;
  }

  // +1 for null terminator
  char *buffer = malloc((size_t)size + 1);
  if (!buffer) {
    fclose(file);
    return NULL;
  }

  size_t read = fread(buffer, 1, (size_t)size, file);
  fclose(file);

  if (read != (size_t)size) {
    free(buffer);
    return NULL;
  }

  buffer[size] = '\0';

  if (out_size) {
    *out_size = (size_t)size;
  }

  return buffer;
}
