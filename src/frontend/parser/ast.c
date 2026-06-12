#include "ast.h"

#include <stdlib.h>
#include <stddef.h>

void free_ast(struct Stmt *ast) {
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