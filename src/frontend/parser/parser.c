#include "frontend/parser/parser.h"

#include "frontend/parser/ast.h"
#include "io/ioutils.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct Expr *parse_expression(struct Parser *parser);

void parser_init(struct Parser *parser, const char *source, size_t source_len) {
    parser->lexer = NULL;
    if (parser->lexer != NULL) {
        lexer_free(parser->lexer);
        free(parser->lexer);
    }

    parser->lexer = malloc(sizeof(struct Lexer));
    lexer_init(parser->lexer, source, source_len);
    parser->next = lexer_next(parser->lexer);
}

static struct Token peek(struct Parser *parser) { return parser->next; }

static struct Token advance(struct Parser *parser) {
    struct Token tmp = parser->next;
    parser->next = lexer_next(parser->lexer);
    return tmp;
}

static struct Expr *create_integer_literal(struct Parser *parser, struct Token tok) {
    struct Expr *expr = malloc(sizeof(struct Expr));
    expr->type = EX_INT_LITERAL;
    char *strnum = malloc(tok.length + 1);
    memcpy(strnum, parser->lexer->source + tok.offest, tok.length); 
    strnum[tok.length] = '\0';

    char *end;
    long value = strtol(strnum, &end, 10);

    if (*end != '\0') {
        printf("parsed value: %ld\n", value);
        fprintf(stderr, "parsing failed\n");
        exit(1);
    }

    expr->value.intliteral.value = value;

    free(strnum);

    expr->metadata.line = tok.line;
    expr->metadata.column = tok.column;

    return expr;
}

static void error(const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);

    va_end(args);
}

static struct Expr *parse_primary(struct Parser *parser) {
    if (peek(parser).type == TK_INTEGERLITERAL) {
        struct Token tok = advance(parser);
        return create_integer_literal(parser, tok);
    } else if (peek(parser).type == TK_OPENPAREN) {
        advance(parser);
        struct Expr *expr = parse_expression(parser);
        if (peek(parser).type != TK_CLOSEPAREN) {
            error("expected ')' at %zu:%zu\n", peek(parser).line,
                  peek(parser).column);
            exit(1);
        }
        advance(parser);
        
        return expr;
    } else {
        error("unexpected token\n");
        exit(1);
    }
}

static struct Expr *create_binary_op(struct Expr *left,
                                     struct Expr *right, enum BinOpType op, struct Token tok) {
    struct Expr *expr = malloc(sizeof(struct Expr));
    expr->type = EX_BINARY;
    expr->value.binop.op = op;
    expr->value.binop.left = left;
    expr->value.binop.right = right;

    expr->metadata.column = tok.column;
    expr->metadata.line = tok.line;

    return expr;
}

static struct Stmt *create_exprstmt(struct Expr *expr, struct Token tok) {
    struct Stmt *stmt = malloc(sizeof(struct Stmt));
    stmt->type = STMT_STMTEXPR;
    stmt->value.exprstmt.expr = expr;
    stmt->metadata.line = tok.line;
    stmt->metadata.column = tok.column;
    return stmt;
}

static struct Stmt *create_printstmt(struct Expr *expr, struct Token tok) {
    struct Stmt *stmt = malloc(sizeof(struct Stmt));
    stmt->type = STMT_PRINT;
    stmt->value.printstmt.expr = expr;
    stmt->metadata.line = tok.line;
    stmt->metadata.column = tok.column;
    return stmt;
}

static struct Expr *parse_factor(struct Parser *parser) {
    struct Expr *left = parse_primary(parser);
    while (peek(parser).type == TK_STAR || peek(parser).type == TK_SLASH) {
        struct Token tok = advance(parser);
        enum TokenType type = tok.type;
        struct Expr *right = parse_primary(parser);
        if (type == TK_STAR) {
            left = create_binary_op(left, right, BIN_MUL, tok);
        } else {
            left = create_binary_op(left, right, BIN_DIV, tok);
        }
    }
    return left;
}

static struct Expr *parse_term(struct Parser *parser) {
    struct Expr *left = parse_factor(parser);
    while (peek(parser).type == TK_PLUS || peek(parser).type == TK_MINUS) {
        struct Token tok = advance(parser);
        enum TokenType type = tok.type;
        struct Expr *right = parse_factor(parser);
        if (type == TK_PLUS) {
            left = create_binary_op(left, right, BIN_ADD, tok);
        } else {
            left = create_binary_op(left, right, BIN_SUB, tok);
        } 
    }
    return left;
}

static struct Expr *parse_expression(struct Parser *parser) {
    return parse_term(parser);
}

static struct Stmt *parse_stmt(struct Parser *parser) {
    switch (peek(parser).type) {
        case TK_PRINT: {
            struct Token start = advance(parser);
            struct Expr *expr = parse_expression(parser);
            struct Stmt *stmt = create_printstmt(expr, start);
            if (peek(parser).type != TK_SEMICOLON) {
                error("expected ';' at %zu:%zu\n", peek(parser).line,
                  peek(parser).column);
                exit(1);
            }
            advance(parser);
            return stmt;
        }    
        case TK_INTEGERLITERAL:
        case TK_OPENPAREN: {
            struct Token start = peek(parser);
            struct Expr *expr = parse_expression(parser);
            struct Stmt *stmt = create_exprstmt(expr, start);
            if (peek(parser).type != TK_SEMICOLON) {
                error("expected ';' at %zu:%zu\n", peek(parser).column,
                  peek(parser).line);
                exit(1);
            }
            advance(parser);
            return stmt;
        }
        default:
            error("unexpected token at statement start");
            exit(1);
    }
}

struct Program *parser_parse(struct Parser *parser) {
    struct Program *program = malloc(sizeof(struct Program));
    program->stmts = NULL;
    program->size = 0;

    while (peek(parser).type != TK_EOF) {
        struct Stmt *stmt = parse_stmt(parser);

        struct Stmt **tmp = realloc(program->stmts, sizeof(struct Stmt*) * (program->size + 1));
        if (!tmp) {
            eprintf("memory allocation failure\n");            
            exit(1); 
        }

        program->stmts = tmp;   
        program->stmts[program->size++] = stmt;
    }

    return program;
}

void parser_free(struct Parser *parser) {
    lexer_free(parser->lexer);
    free(parser->lexer);
    parser->lexer = NULL;
}
