#include "frontend/parser/parser.h"

#include "frontend/parser/ast.h"
#include "io/ioutils.h"
#include "io/ioformat.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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
    parser->lookahead1 = lexer_next(parser->lexer);
}

static struct Token peek(struct Parser *parser) { 
    return parser->next; 
}

static struct Token peek1(struct Parser *parser) {
    return parser->lookahead1;
}

static struct Token advance(struct Parser *parser) {
    struct Token tmp = parser->next;
    parser->next = parser->lookahead1;
    parser->lookahead1 = lexer_next(parser->lexer);
    return tmp;
}

static struct Expr *create_integer_literal(struct Parser *parser, struct Token tok) {
    struct Expr *expr = malloc(sizeof(struct Expr));
    expr->type = EX_INT_LITERAL;
    char buf[32];
    memcpy(buf, parser->lexer->source + tok.offest, tok.length);
    buf[tok.length] = '\0';
    long value = strtol(buf, NULL, 10);

    
    expr->value.intliteral.value = value;

    expr->metadata.line = tok.line;
    expr->metadata.column = tok.column;

    return expr;
}

static void error(struct Token tok, const char *fmt, ...) {
    fprintf(stderr, "%s%zu:%zu: %serror: %s", ANSI_RESET, tok.line, tok.column, ANSI_RED, ANSI_RESET);
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
            error(peek(parser), "expected ')'\n");
            exit(1);
        }
        advance(parser);
        
        
        return expr;
    } else if (peek(parser).type == TK_IDENTIFIER) {
        struct Token tok = advance(parser);

        struct Expr *expr = malloc(sizeof(struct Expr));
        expr->type = EX_VARIABLE;

        expr->value.variable.name = malloc(tok.length + 1);
        memcpy(expr->value.variable.name,
            parser->lexer->source + tok.offest,
            tok.length);
        expr->value.variable.name[tok.length] = '\0';

        expr->metadata.line = tok.line;
        expr->metadata.column = tok.column;
        return expr;
    }else {
        error(peek(parser), "unexpected token\n");
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

static bool is_var_type(enum TokenType type) {
    switch (type) {
        case TK_INT:
            return true;
        default:
            return false;
    }
}

static struct Stmt *create_vardecl(const char *name, enum Type type, struct Expr *init, struct Token tok) {
    struct Stmt *stmt = malloc(sizeof(struct Stmt));
    stmt->type = STMT_VARDECL;
    stmt->value.variabledecl.name = name;
    stmt->value.variabledecl.type = type;
    stmt->value.variabledecl.init = init;
    stmt->metadata.line = tok.line;
    stmt->metadata.column = tok.column;
    return stmt;
}

static struct Stmt *parse_vardecl(struct Parser* parser, struct Token start) {
    struct Token tok = advance(parser);

    if (tok.type != TK_IDENTIFIER) {
        error(tok, "expected identifier\n");
        exit(1);
    }
    char *var_name = malloc(tok.length + 1);
    memcpy(var_name, parser->lexer->source + tok.offest, tok.length);
    var_name[tok.length] = '\0';

    // force type annotation
    if (peek(parser).type != TK_COLON) {
        error(peek(parser), "expected explicit type annotation\n");
        exit(1);
    }

    // consume ':'
    advance(parser);

    if (!is_var_type(peek(parser).type)) {
        error(peek(parser), "expected type after ':'\n");
        exit(1);
    }

    struct Token tok1 = advance(parser);
    enum TokenType type = tok1.type;
    
    enum Type var_type;
    if (type == TK_INT) {
        var_type = TYPE_INT;
    } else {
        error(tok, "unexpected tokentype: %d\n", type);
        exit(1);
    }

    if (peek(parser).type == TK_EQUAL) {
        advance(parser);
        struct Expr *expr = parse_expression(parser);
        if (peek(parser).type != TK_SEMICOLON) {
            error(peek(parser), "expected ';'\n");
            exit(1);
        }
        advance(parser);
        return create_vardecl(var_name, var_type, expr, start);
    } else {
        if (peek(parser).type != TK_SEMICOLON) {
            error(peek(parser), "unexpected token: %d\n", peek(parser).type);
            exit(1);
        }
        advance(parser);
        return create_vardecl(var_name, var_type, NULL, start);
    }
}

static struct Stmt *create_varassign(char *name, struct Expr *value, struct Token tok) {
    struct Stmt *stmt = malloc(sizeof(struct Stmt));
    stmt->type = STMT_VARASSIGN;
    stmt->metadata.line = tok.line;
    stmt->metadata.column = tok.column;
    stmt->value.variableassignment.name = name;
    stmt->value.variableassignment.value = value;
    return stmt;
}

static struct Stmt *parse_varassign(struct Parser *parser) {
    struct Token name_token = advance(parser);

    if (peek(parser).type != TK_EQUAL) {
        error(peek(parser), "expected '=' after identifier");
    }

    advance(parser); // consume =


    struct Expr *value = parse_expression(parser);

    if (peek(parser).type != TK_SEMICOLON) {
        error(peek(parser), "expected ';\n");
    }

    advance(parser);

    char *name = malloc(name_token.length + 1);
    memcpy(name, parser->lexer->source + name_token.offest, name_token.length);
    name[name_token.length] = '\0';

    return create_varassign(name, value, name_token);
}

static struct Stmt *parse_stmt(struct Parser *parser) {
    switch (peek(parser).type) {
        case TK_PRINT: {
            struct Token start = advance(parser);
            struct Expr *expr = parse_expression(parser);
            struct Stmt *stmt = create_printstmt(expr, start);
            if (peek(parser).type != TK_SEMICOLON) {
                error(peek(parser), "expected ';'\n");
                exit(1);
            }
            advance(parser);
            return stmt;
        }    
        case TK_LET: {
            struct Token start = advance(parser);
            struct Stmt *stmt = parse_vardecl(parser, start);
            return stmt;
        }
        case TK_IDENTIFIER: {
            if (peek1(parser).type == TK_EQUAL) {
                return parse_varassign(parser);
            }
            // if not '=' then fall through to expression parsing 
            // because we assume rn that if it isnt x = value; then x is being used in an expression
        }
        case TK_INTEGERLITERAL:
        case TK_OPENPAREN: {
            struct Token start = peek(parser);
            struct Expr *expr = parse_expression(parser);
            struct Stmt *stmt = create_exprstmt(expr, start);
            if (peek(parser).type != TK_SEMICOLON) {
                error(peek(parser), "expected ';'\n");
                exit(1);
            }
            advance(parser);
            return stmt;
        }
        default:
            error(peek(parser), "unexpected token at statement start");
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
