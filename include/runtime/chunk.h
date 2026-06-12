#ifndef RUNTIME_CHUNK_H
#define RUNTIME_CHUNK_H

#include <stdint.h>
#include <stddef.h>

#include "value.h"

typedef uint8_t Byte;

struct BytecodeArray {
    Byte *data;
    size_t size;
    size_t capacity;     
};

void bytecodearray_init(struct BytecodeArray *bytecodearray);
void bytecodearray_push(struct BytecodeArray *bytecodearray, Byte value);
void bytecodearray_free(struct BytecodeArray *bytecodearray);

struct Chunk {
    struct BytecodeArray bytecode;
    struct ValueArray constants;
};

void chunk_init(struct Chunk *chunk, struct BytecodeArray* bytecode, struct ValueArray* constants);
void chunk_free(struct Chunk *chunk);

#endif // RUNTIME_CHUNK_H

