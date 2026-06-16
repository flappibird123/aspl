#ifndef FRONTEND_PARSER_AST_H
#define FRONTEND_PARSER_AST_H

#include <stddef.h>


struct NodeMetadata {
    size_t line;
    size_t column;
};

struct Expr;

enum ExprType {
    EX_BINARY,
    EX_INT_LITERAL,
    EX_VARIABLE
};

enum BinOpType {
    BIN_ADD,
    BIN_SUB,
    BIN_MUL,
    BIN_DIV,
};

struct BinaryOp {
    enum BinOpType op; 
    struct Expr* left;
    struct Expr* right;
};

struct IntegerLit {
    long value;
};

struct Variable {
    char *name;
};

struct Expr {
    enum ExprType type;
    union {
        struct BinaryOp binop;
        struct IntegerLit intliteral;
        struct Variable variable;
    } value;

    struct NodeMetadata metadata;
};

enum StmtType {
    STMT_PRINT,
    STMT_STMTEXPR,
    STMT_VARDECL,
    STMT_VARASSIGN,
    STMT_BLOCK,
};

struct Block {
    struct Stmt **stmts;
    size_t count;
};

struct VarAssign {
    char *name;
    struct Expr *value;
};

struct ExprStmt {
    struct Expr *expr;
};

enum Type {
    TYPE_INT,
};

struct VarDecl {
    const char *name;
    enum Type type;
    struct Expr *init;
};

struct PrintStmt {
    struct Expr *expr;
};
struct Stmt {
    enum StmtType type;

    union {
        struct ExprStmt exprstmt;
        struct PrintStmt printstmt;
        struct VarDecl variabledecl;
        struct VarAssign variableassignment;
        struct Block block;
    } value;

    struct NodeMetadata metadata;
};

struct Program {
    struct Stmt **stmts;
    size_t size;
};

void free_ast(struct Program *ast);

#endif // FRONTEND_PARSER_AST_H

