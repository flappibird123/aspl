#include "test.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "frontend/lexer/lexer.h"
#include "frontend/lexer/token.h"
#include "io/fileop.h"

static const char *tokentype_to_string(enum TokenType type) {
    switch (type) {
    case TK_INTEGERLITERAL:
        return "TK_INTEGERLITERAL";

    case TK_OPENPAREN:
        return "TK_OPENPAREN";

    case TK_CLOSEPAREN:
        return "TK_CLOSEPAREN";

    case TK_PLUS:
        return "TK_PLUS";

    case TK_MINUS:
        return "TK_MINUS";

    case TK_STAR:
        return "TK_STAR";

    case TK_SLASH:
        return "TK_SLASH";

    case TK_EOF:
        return "TK_EOF";

    default:
        return "UNKNOWN_TOKEN";
    }
}

static void print_token(struct Token *tok) {
    printf("TYPE: %s\n", tokentype_to_string(tok->type));
    printf("OFFSET: %zu\n", tok->offest);
    printf("LENGTH: %zu\n", tok->length);
    printf("LINE: %zu\n", tok->line);
    printf("COLUMN: %zu\n", tok->column);
}

static int run_lexer_test(int argc, const char **argv) {
    if (argc != 2) {
        return 1;
    }
    size_t size;
    char *source = read_file(argv[1], &size);

    struct Lexer lexer;
    lexer_init(&lexer, source, size);
    enum TokenType t = TK_CLOSEPAREN;
    while (t != TK_EOF) {
        struct Token tok = lexer_next(&lexer);
        t = tok.type;
        print_token(&tok);
    }
    lexer_free(&lexer);
    free(source);
    return 0;
}

int run_test(int argc, const char **argv) { return run_lexer_test(argc, argv); }
