#include <stdio.h>
#include <stdlib.h>

#include "io/fileop.h"
#include "io/ioformat.h"

#include "runtime/vm.h"
#include "runtime/chunk.h"
#include "runtime/value.h"
#include "frontend/lexer/lexer.h"

int main(int argc, const char *argv[]) {
    if (argc != 2) {
        printf("%s%s%serror:%s no input file\n", ANSI_RESET, ANSI_BOLD,
               ANSI_BRED, ANSI_RESET);
        return 1;
    }

    size_t size;
    char *source = read_file(argv[1], &size);

    struct Lexer lexer;
    lexer_init(&lexer, source, size);
    while (1) {
        lexer_next(&lexer);
    } 
    lexer_free(&lexer);
    return 1;

    struct VM vm;
    vm_init(&vm);
    struct Chunk chunk;
    struct BytecodeArray bytecode;
    bytecodearray_init(&bytecode);
    bytecodearray_push(&bytecode, 4);
    bytecodearray_push(&bytecode, 0);
    bytecodearray_push(&bytecode, 4);
    bytecodearray_push(&bytecode, 1);
    bytecodearray_push(&bytecode, 1);
    bytecodearray_push(&bytecode, 6);
    bytecodearray_push(&bytecode, 5);
    struct ValueArray constants;
    valuearray_init(&constants);
    valuearray_push(&constants, 6);
    valuearray_push(&constants, 2);
    chunk_init(&chunk, &bytecode, &constants);
    vm_run(&vm, &chunk);

    bytecodearray_free(&bytecode);
    valuearray_free(&constants);


    return 0;

    // printf("%s\n", source);

    // free(source);

    // return run_test(argc, argv);
}
