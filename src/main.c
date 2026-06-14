#include <stdio.h>
#include <stdlib.h>

#include "io/fileop.h"
#include "io/ioformat.h"

#include "runtime/vm.h"
#include "runtime/chunk.h"

#include "frontend/parser/parser.h"

#include "codegen/compiler.h"

int main(int argc, const char *argv[]) {
    if (argc != 2) {
        printf("%s%s%serror:%s no input file\n", ANSI_RESET, ANSI_BOLD,
               ANSI_BRED, ANSI_RESET);
        return 1;
    }

    size_t size;
    char *source = read_file(argv[1], &size);

    struct Parser parser;
    parser_init(&parser, source, size);
    struct Program *program = parser_parse(&parser);
    parser_free(&parser);

    struct Chunk chunk;
    chunk_init(&chunk);
    struct Compiler compiler;
    compiler_init(&compiler);
    compiler_compile(&compiler, program, &chunk);
    compiler_free(&compiler);

    free_ast(program);

    struct VM vm;
    vm_init(&vm);
    vm_run(&vm, &chunk);
    vm_free(&vm); 

    return 0;

    // printf("%s\n", source);

    // free(source);

    // return run_test(argc, argv);
}
