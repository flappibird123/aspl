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



    return 0;

    // printf("%s\n", source);

    // free(source);

    // return run_test(argc, argv);
}
