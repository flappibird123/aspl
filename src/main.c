#include <stdio.h>
#include <stdlib.h>

#include "io/fileop.h"
#include "io/ioformat.h"
#include "test.h"

int main(int argc, const char *argv[]) {
    if (argc != 2) {
        printf("%s%s%serror:%s no input file\n", ANSI_RESET, ANSI_BOLD,
               ANSI_BRED, ANSI_RESET);
    }

    size_t size;
    char *source = read_file(argv[1], &size);

    printf("%s\n", source);

    free(source);

    return run_test(argc, argv);
}
