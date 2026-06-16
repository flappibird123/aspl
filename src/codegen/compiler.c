#include "codegen/compiler.h"

#include "runtime/opcode.h"
#include "io/ioutils.h"

#include <string.h>
#include <stdlib.h>

static void compile_expr(struct Compiler *compiler, struct Expr *expr, struct Chunk *chunk);

void compiler_init(struct Compiler *compiler) {
    compiler->local_count = 0;
}

static void emit_byte(struct Chunk *chunk, Byte byte, struct NodeMetadata metadata) {
    chunk_write(chunk, byte, metadata.line);
}

static Byte add_local(struct Compiler *compiler, const char *name) {
    int slot = compiler->local_count++;
    compiler->locals[slot].name = strdup(name);
    compiler->locals[slot].slot = slot;
    return slot;
}

static int find_local(struct Compiler *compiler, const char *name) {
    for (int i = 0; i < compiler->local_count; i++) {
        if (strcmp(compiler->locals[i].name, name) == 0)
            return compiler->locals[i].slot;
    }
    return -1; // not found
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
        case EX_VARIABLE: {
            int slot = find_local(compiler, expr->value.variable.name);

            if (slot < 0) {
                eprintf("undefined variable: %s\n", expr->value.variable.name);
                exit(1);
            }

            emit_byte(chunk, OP_LOADLOCAL, expr->metadata);
            emit_byte(chunk, slot, expr->metadata);
            break;
        }
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
        case STMT_VARDECL: {
            int slot = add_local(compiler, stmt->value.variabledecl.name);
            if (stmt->value.variabledecl.init != NULL) {
                compile_expr(compiler, stmt->value.variabledecl.init, chunk);
            } else {
                emit_byte(chunk, OP_LOADCONST, stmt->metadata);
                emit_byte(chunk, 0, stmt->metadata); // default value
            }

            emit_byte(chunk, OP_STORELOCAL, stmt->metadata);
            emit_byte(chunk, slot, stmt->metadata);
            break;
        }
        case STMT_VARASSIGN: {
            compile_expr(compiler, stmt->value.variableassignment.value, chunk);
            int slot = find_local(compiler, stmt->value.variableassignment.name);
            if (slot < 0) {
                eprintf("Undeclared variable: %s\n", stmt->value.variableassignment.name);
                exit(1);
            }
            emit_byte(chunk, OP_STORELOCAL, stmt->metadata);
            emit_byte(chunk, slot, stmt->metadata);
            break;
        }
    }
}


void compiler_compile(struct Compiler *compiler, struct Program *program, struct Chunk *chunk) {
    for (size_t i = 0; i < program->size; i++) {
        compile_stmt(compiler, program->stmts[i], chunk);
    }

    emit_byte(chunk, OP_HALT, (struct NodeMetadata){0});
}

void compiler_free(struct Compiler *compiler) {
    (void)compiler;
}
