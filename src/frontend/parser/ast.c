#include "frontend/parser/ast.h"

#include <stdlib.h>
#include <stddef.h>

static void free_expr(struct Expr *expr) {
    if (!expr) return;

    switch (expr->type) {
        case EX_INT_LITERAL:
            // nothing else allocated inside
            break;

        case EX_BINARY:
            free_expr(expr->value.binop.left);
            free_expr(expr->value.binop.right);
            break;

        default:
            break;
    }

    free(expr);
}

static void free_stmt(struct Stmt *ast) {
    if (!ast) return;

    switch (ast->type) {
        case STMT_STMTEXPR:
        case STMT_PRINT:
            free_expr(ast->value.exprstmt.expr);
            break;

        default:
            break;
    }

    free(ast);
}

void free_ast(struct Program *program) {
    for (size_t i = 0; i < program->size; ++i) {
        free_stmt(program->stmts[i]);
    }
    free(program->stmts);
    free(program);
}