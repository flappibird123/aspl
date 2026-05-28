#include "lexer.h"
#include "token.h"

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void lexer_init(struct Lexer *lexer, const char *source, size_t source_len) {
    lexer->source = source;
    lexer->current = 0;
    lexer->start = lexer->current;
    lexer->column = 0;
    lexer->start_column = 0;
    lexer->line = 0;
    lexer->source_len = source_len;
}

#define EOF_CHAR '\0'

bool is_at_end(struct Lexer *lexer) {
    if (lexer->current < lexer->source_len) {
        return false;
    }
    return true;
}

static char peek(struct Lexer *lexer) {
    if (is_at_end(lexer)) {
        return EOF_CHAR;
    }
    return lexer->source[lexer->current];
}

static char advance(struct Lexer *lexer) {
    if (peek(lexer) == EOF_CHAR) {
        return EOF_CHAR;
    }
    char c = peek(lexer);
    if (c == '\n') {
        lexer->column = 0;
        lexer->line++;
    } else {
        lexer->column++;
    }
    lexer->current++;
    return c;
}

static struct Token gen_token(struct Lexer *lexer, enum TokenType type) {
    return (struct Token){type, lexer->start, lexer->current - lexer->start,
                          lexer->line, lexer->start_column};
}

static char peekn(struct Lexer *lexer, size_t n) {
    if (lexer->current + n >= lexer->source_len) {
        return EOF_CHAR;
    }
    return lexer->source[lexer->current + n];
}

static bool skip_whitespace(struct Lexer *lexer) {
    bool modified = false;
    while (!is_at_end(lexer) && isspace((unsigned char)peek(lexer))) {
        modified = true;
        advance(lexer);
    }
    return modified;
}

// TODO: Add support for block comments i.e. /* */
static bool skip_comment(struct Lexer *lexer) {
    bool modified = false;
    if (peek(lexer) == '/' && peekn(lexer, 1) == '\n') {
        advance(lexer);
        advance(lexer);
        modified = true;
        while (!is_at_end(lexer) && peek(lexer) != '\n') {
            advance(lexer);
        }
    }
    return modified;
}

struct Token lexer_next(struct Lexer *lexer) {
    if (is_at_end(lexer)) {
        return (struct Token){TK_EOF, lexer->current, 0, lexer->line,
                              lexer->column};
    }

    while (true) {
        if (skip_comment(lexer)) {
            continue;
        } else if (skip_whitespace(lexer)) {
            continue;
        } else {
            break;
        }
    }

    if (is_at_end(lexer)) {
        return (struct Token){TK_EOF, lexer->current, 0, lexer->line, lexer->column};
    }

    lexer->start = lexer->current;
    lexer->start_column = lexer->column;


    char c = peek(lexer);

    switch (c) {
    case '(':
        advance(lexer);
        return gen_token(lexer, TK_OPENPAREN);
    case ')':
        advance(lexer);
        return gen_token(lexer, TK_CLOSEPAREN);
    case '+':
        advance(lexer);
        return gen_token(lexer, TK_PLUS);
    case '-':
        advance(lexer);
        return gen_token(lexer, TK_MINUS);
    case '*':
        advance(lexer);
        return gen_token(lexer, TK_STAR);
    case '/':
        advance(lexer);
        return gen_token(lexer, TK_SLASH);
    default:
        if (isdigit((unsigned char)c)) {
            advance(lexer);
            while (isdigit((unsigned char)peek(lexer))) {
                advance(lexer);
            }
            return gen_token(lexer, TK_INTEGERLITERAL);
        } else {
            fprintf(stderr, "Unknown character: '%c'\n", c);
            exit(1);
        }
    }
}

void lexer_free(struct Lexer *lexer) {
    lexer->source = NULL;
    lexer->current = 0;
    lexer->column = 0;
    lexer->line = 0;
    lexer->start = 0;
    lexer->start_column = 0;
}
