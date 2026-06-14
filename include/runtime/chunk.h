#ifndef RUNTIME_CHUNK_H
#define RUNTIME_CHUNK_H

#include <stdint.h>
#include <stddef.h>

#include "value.h"

typedef uint8_t Byte;

struct BytecodeArray {
    Byte *data;
    size_t *lines;
    size_t size;
    size_t capacity;     
};

void bytecodearray_init(struct BytecodeArray *bytecodearray);
void bytecodearray_write(struct BytecodeArray *bytecodearray, Byte value, size_t line);
void bytecodearray_free(struct BytecodeArray *bytecodearray);

struct Chunk {
    struct BytecodeArray *bytecode;
    struct ValueArray *constants;
};

void chunk_init(struct Chunk *chunk);
void chunk_free(struct Chunk *chunk);

void chunk_write(struct Chunk *chunk, Byte value, size_t line);
size_t chunk_addconstant(struct Chunk *chunk, Value value);

void dump_chunk(struct Chunk *chunk);


#endif // RUNTIME_CHUNK_H

