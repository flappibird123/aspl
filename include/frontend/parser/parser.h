#ifndef FRONTEND_PARSER_PARSER_H
#define FRONTEND_PARSER_PARSER_H

#include <stdlib.h>

#include "../lexer/lexer.h"
#include "../lexer/token.h"
#include "ast.h"

struct Parser {
    struct Lexer *lexer;
    struct Token next;
};

void parser_init(struct Parser *parser, const char *source, size_t source_len);
struct Stmt *parser_parse(struct Parser *parser);
void parser_free(struct Parser *parser);

#endif // FRONTEND_PARSER_PARSER_H

