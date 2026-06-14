#include "runtime/chunk.h"

#include <stdlib.h>
#include <stdio.h>
#include "io/ioutils.h"

#define BYTECODEARRAY_INITIAL_CAPACITY 16

void bytecodearray_init(struct BytecodeArray *bytecodearray) {
    bytecodearray->capacity = BYTECODEARRAY_INITIAL_CAPACITY;
    bytecodearray->size = 0;
    bytecodearray->data = malloc(sizeof(Byte) * bytecodearray->capacity);
    bytecodearray->lines = malloc(sizeof(size_t) * bytecodearray->capacity);
    if (!bytecodearray->data || !bytecodearray->lines) {
        eprintf("memory allocation failure\n");
        exit(1);
    }
}

void bytecodearray_write(struct BytecodeArray *bytecodearray, Byte value) {
    if (bytecodearray->size >= bytecodearray->capacity) {
        size_t new_capacity = bytecodearray->capacity * 2;
        Byte *tmp = realloc(bytecodearray->data, new_capacity);
        if (!tmp) {
            free(bytecodearray->data);
            eprintf("memory allocation failure\n");
            exit(1);
        }
        bytecodearray->data = tmp;
        size_t *tmp1 = realloc(bytecodearray->lines, new_capacity);
        if (!tmp1) {
            free(bytecodearray->lines);
            eprintf("memory allocation failure\n");
            exit(1);
        }
        bytecodearray->lines = tmp1;
        bytecodearray->capacity = new_capacity;
    }
    bytecodearray->data[bytecodearray->size] = value;
    bytecodearray->size++;
}

void bytecodearray_free(struct BytecodeArray *bytecodearray) {
    free(bytecodearray->data);
    free(bytecodearray->lines);
    bytecodearray->data = NULL;
    bytecodearray->lines = NULL;
    bytecodearray->size = 0;
    bytecodearray->capacity = 0;
}

void chunk_init(struct Chunk *chunk) {
    
}

void chunk_free(struct Chunk *chunk) {
    (void)chunk;
}

