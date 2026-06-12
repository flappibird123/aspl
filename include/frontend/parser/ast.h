#ifndef FRONTEND_PARSER_AST_H
#define FRONTEND_PARSER_AST_H

struct Expr;

enum ExprType {
    EX_BINARY,
    EX_INT_LITERAL,
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

struct Expr {
    enum ExprType type;
    union {
        struct BinaryOp binop;
        struct IntegerLit intliteral;
    } value;
};

enum StmtType {
    STMT_PRINT,
    STMT_STMTEXPR,
};

struct ExprStmt {
    struct Expr *expr;
};

struct PrintStmt {
    struct Expr *expr;
};
struct Stmt {
    enum StmtType type;

    union {
        struct ExprStmt exprstmt;
        struct PrintStmt printstmt;
    } value;
};

void free_ast(struct Stmt *ast);

#endif // FRONTEND_PARSER_AST_H

