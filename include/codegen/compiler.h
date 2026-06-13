#ifndef CODEGEN_COMPILER_H
#define CODEGEN_COMPILER_H

#include "frontend/parser/ast.h"
#include "runtime/chunk.h"

struct Compiler {

};

void compiler_init(struct Compiler *compiler);
void compiler_compile(struct Compiler *compiler, struct Stmt *ast, struct Chunk *chunk);
void compiler_free(struct Compiler *compiler);

#endif // CODEGEN_COMPILER_H
