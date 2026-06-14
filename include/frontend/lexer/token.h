#ifndef FRONTEND_LEXER_TOKEN_H
#define FRONTEND_LEXER_TOKEN_H

#include <stddef.h>

enum TokenType {
    TK_INTEGERLITERAL,

    TK_OPENPAREN,
    TK_CLOSEPAREN,

    TK_PLUS,
    TK_MINUS,
    TK_STAR,
    TK_SLASH,

    TK_PRINT,

    TK_SEMICOLON,
    TK_COLON,

    TK_EQUAL,

    TK_IDENTIFIER,

    TK_INT,

    TK_LET,

    TK_EOF
};

struct Token {
    enum TokenType type;
    size_t offest;
    size_t length;
    size_t line;
    size_t column;
};

#endif // FRONTEND_LEXER_TOKEN_H
