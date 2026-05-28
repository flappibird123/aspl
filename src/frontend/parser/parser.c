#include "parser.h"

#include <stdlib.h>

void parser_init(struct Parser *parser, const char *source, size_t source_len) {
    struct Lexer *lexer = malloc(sizeof(struct Lexer));
    lexer_init(lexer, source, source_len);
}

static struct Expr *parse_term(struct Parser *parser) {

}

static struct Expr *parse_expression(struct Parser *parser) {
    return parse_term(parser);
}

struct Expr *parser_parse(struct Parser *parser) {
    return parse_expression(parser);  
}

void parser_free(struct Parser *parser) {
    lexer_free(parser->lexer);
    if (parser->lexer) {
        free(parser->lexer);
    }
    parser->lexer = NULL;
}
