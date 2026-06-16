#ifndef RUNTIME_VM_H
#define RUNTIME_VM_H

#include <stddef.h>

#include "value.h"
#include "chunk.h"

#define STACK_SIZE 1024

struct Stack {
    Value stack[STACK_SIZE];
    size_t sp;
};

struct VM {
    size_t ip;
    struct Stack stack;
    Value locals[256];
};

void vm_init(struct VM* vm);
void vm_run(struct VM *vm, struct Chunk *chunk);
void vm_free(struct VM *vm);

#endif // RUUNTIME_VM_H