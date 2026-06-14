#include "runtime/value.h"

#include <stdlib.h>

#include "io/ioutils.h"

#define VALUE_ARRAY_INITIAL_CAPACITY 16

void valuearray_init(struct ValueArray *arr) {
    arr->capacity = VALUE_ARRAY_INITIAL_CAPACITY;
    arr->size = 0;
    arr->data = malloc(sizeof(Value) * arr->capacity);
    if (!arr->data) {
        eprintf("memory allocation failure\n");
        exit(1);
    }
}

void valuearray_write(struct ValueArray *arr, Value value) {
    if (arr->size >= arr->capacity) {
        size_t new_cap = arr->capacity * 2;

        Value *new_data = realloc(arr->data, new_cap * sizeof(Value));
        if (!new_data) {
            eprintf("memory allocation failure\n");
            exit(1);
        }

        arr->data = new_data;
        arr->capacity = new_cap;
    }

    arr->data[arr->size++] = value;
}

void valuearray_free(struct ValueArray *arr) {
    free(arr->data);
    arr->data = NULL;
    arr->capacity = 0;
    arr->size = 0;
}
