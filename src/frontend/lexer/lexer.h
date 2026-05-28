#ifndef FRONTEND_LEXER_LEXER_H
#define FRONTEND_LEXER_LEXER_H

#include <stddef.h>

#include "token.h"

struct Lexer {
    const char *source;
    size_t current;
    size_t start;
    size_t column;
    size_t start_column;
    size_t line;
    size_t source_len;
};

void lexer_init(struct Lexer *lexer, const char *source, size_t source_len);
struct Token lexer_next(struct Lexer *lexer);
void lexer_free(struct Lexer *lexer);

#endif // FRONTEND_LEXER_LEXER_H
