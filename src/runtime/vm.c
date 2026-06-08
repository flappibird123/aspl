#include "vm.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "io/ioutils.h"
#include "opcode.h"

void vm_init(struct VM* vm) {
    (void)vm;
}

static Byte get_opcode(struct VM *vm, struct Chunk *chunk) {
    return *(chunk->bytecode.data + vm->ip);
}

#define OPCODE_COUNT 7

static enum OpCode byte_values[OPCODE_COUNT] = {
    OP_IADD,
    OP_ISUB,
    OP_IMUL,
    OP_IDIV,
    OP_LOADCONST,
    OP_HALT,
    OP_IPRINT,
};

static enum OpCode decode_byte(Byte byte) {
    return byte_values[byte];
}

static Value pop(struct VM *vm) {
    if (vm->stack.sp == 0) {
        eprintf("stack underflow\n");
        exit(1);        
    }
    return vm->stack.stack[--vm->stack.sp];
}

static void push(struct VM *vm, Value value) {
    if (vm->stack.sp >= STACK_SIZE) {
        eprintf("stack overflow\n");
        exit(1);        
    }
    vm->stack.stack[vm->stack.sp++] = value;
}

void vm_run(struct VM *vm, struct Chunk *chunk) {
    vm->stack.sp = 0;
    vm->ip = 0;
    while (true) {
        Byte byte = get_opcode(vm, chunk);
        enum OpCode opcode = decode_byte(byte);
        if (opcode == OP_HALT) {
            break;
        }
        switch (opcode) {
            case OP_IADD: {
                Value b = pop(vm);
                Value a = pop(vm);
                push(vm, a + b);
                vm->ip++;
                break;
            }
            case OP_ISUB: {
                Value b = pop(vm);
                Value a = pop(vm);
                push(vm, a - b);
                vm->ip++;
                break;
            }
            case OP_IMUL: {
                Value b = pop(vm);
                Value a = pop(vm);
                push(vm, a * b);
                vm->ip++;
                break;
            }
            case OP_IDIV: {
                Value b = pop(vm);
                if (b == 0) {
                    eprintf("division by 0\n");
                    exit(1);
                }
                Value a = pop(vm);
                push(vm, a / b);
                vm->ip++;
                break;
            }
            case OP_LOADCONST: {
                vm->ip++;
                Byte index = chunk->bytecode.data[vm->ip];
                Value value = chunk->constants.data[index];
                push(vm, value);
                vm->ip++; 
                break;
            }
            case OP_IPRINT: {
                Value value = pop(vm);
                int ivalue = (int)value;
                printf("%d\n", ivalue);
                vm->ip++;
                break;
            }
            default: {
                eprintf("unknown opcode: %d", opcode);
                exit(1);
            } 
        }
    }
}

void vm_free(struct VM *vm) {
    vm->ip = 0; 
}
 