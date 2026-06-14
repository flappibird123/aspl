#include "codegen/compiler.h"

#include "runtime/opcode.h"

static void compile_expr(struct Compiler *compiler, struct Expr *expr, struct Chunk *chunk);

void compiler_init(struct Compiler *compiler) {
    (void)compiler;
}

static void emit_byte(struct Chunk *chunk, Byte byte, struct NodeMetadata metadata) {
    chunk_write(chunk, byte, metadata.line);
}

static void compile_literal(Value v, struct Chunk *chunk, struct NodeMetadata metadata) {
    size_t constant_index = chunk_addconstant(chunk, v);
    emit_byte(chunk, OP_LOADCONST, metadata);
    emit_byte(chunk, constant_index, metadata);
}


static void compile_binary(struct Compiler *compiler, struct Expr *expr, struct Chunk *chunk) {
    compile_expr(compiler, expr->value.binop.left, chunk);
    compile_expr(compiler, expr->value.binop.right, chunk);

    switch (expr->value.binop.op) {
        case BIN_ADD:  
            emit_byte(chunk, OP_IADD, expr->metadata); 
            break;
        case BIN_SUB: 
            emit_byte(chunk, OP_ISUB, expr->metadata); 
            break;
        case BIN_MUL:  
            emit_byte(chunk, OP_IMUL, expr->metadata); 
            break;
        case BIN_DIV: 
            emit_byte(chunk, OP_IDIV, expr->metadata); 
            break;
    }
}

static void compile_expr(struct Compiler *compiler, struct Expr *expr, struct Chunk *chunk) {
    switch (expr->type) {

        case EX_INT_LITERAL:
            compile_literal(expr->value.intliteral.value, chunk, expr->metadata);
            break;

        case EX_BINARY:
            compile_binary(compiler, expr, chunk);
            break;

        default:
            // handle error or unreachable
            break;
    }
}


static void compile_stmt(struct Compiler *compiler, struct Stmt *stmt, struct Chunk *chunk) {
    switch (stmt->type) {
        case STMT_PRINT:
            compile_expr(compiler, stmt->value.printstmt.expr, chunk);
            emit_byte(chunk, OP_IPRINT, stmt->metadata);
            break;
        case STMT_STMTEXPR:
            compile_expr(compiler, stmt->value.exprstmt.expr, chunk);
            break;
    }
}

void compiler_compile(struct Compiler *compiler, struct Program *ast, struct Chunk *chunk) {
    struct Stmt *current = *ast->stmts;

    size_t i = 0;

    while (i < ast->size) {
        compile_stmt(compiler, current, chunk);
        current = ast->stmts[i];
    }

    emit_byte(chunk, OP_HALT, (struct NodeMetadata) {0});
}

void compiler_free(struct Compiler *compiler) {
    (void)compiler;
}
