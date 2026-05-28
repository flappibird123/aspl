#ifndef FRONTEND_PARSER_PARSER_H
#define FRONTEND_PARSER_PARSER_H

#include <stdlib.h>

#include "../lexer/lexer.h"
#include "ast.h"

struct Parser {
    struct Lexer *lexer;
};

void parser_init(struct Parser *parser, const char *source, size_t source_len);
struct Expr *parser_parse(struct Parser *parser);
void parser_free(struct Parser *parser);

#endif // FRONTEND_PARSER_PARSER_H

