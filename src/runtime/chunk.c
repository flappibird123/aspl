#include "chunk.h"

#include <stdlib.h>
#include <stdio.h>
#include "io/ioutils.h"

#define BYTECODEARRAY_INITIAL_CAPACITY 16

void bytecodearray_init(struct BytecodeArray *bytecodearray) {
    bytecodearray->capacity = BYTECODEARRAY_INITIAL_CAPACITY;
    bytecodearray->size = 0;
    bytecodearray->data = malloc(sizeof(Byte) * bytecodearray->capacity);
    if (!bytecodearray->data) {
        eprintf("memory allocation failure\n");
        exit(1);
    }
}

void bytecodearray_push(struct BytecodeArray *bytecodearray, Byte value) {
    if (bytecodearray->size >= bytecodearray->capacity) {
        size_t new_capacity = bytecodearray->capacity * 2;
        Byte *tmp = realloc(bytecodearray->data, new_capacity);
        if (!tmp) {
            free(bytecodearray->data);
            eprintf("memory allocation failure\n");
            exit(1);
        }
        bytecodearray->data = tmp;
        bytecodearray->capacity = new_capacity;
    }
    bytecodearray->data[bytecodearray->size] = value;
    bytecodearray->size++;
}

Byte bytecodearray_get(struct BytecodeArray *bytecodearray, size_t index) {
    if (index >= bytecodearray->size) {
        eprintf("access bytecodearray out of bounds. ");
        eprintf("access at index %zu, but size is %zu", index, bytecodearray->size);
        exit(1);
    }
    return bytecodearray->data[index];
}

void bytecodearray_free(struct BytecodeArray *bytecodearray) {
    free(bytecodearray->data);
    bytecodearray->data = NULL;
    bytecodearray->size = 0;
    bytecodearray->capacity = 0;
}

void chunk_init(struct Chunk *chunk) {

}

void chunk_free(struct Chunk *chunk) {

}

