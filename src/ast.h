#ifndef AST_H
#define AST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum
{
    ADD,
    SUB,
    MUL,
    DIV,
    AND,
    MOD,
    OR,
    NOT,
    AND_BIT,
    OR_BIT,
    NOT_BIT,
    XOR,
    EQ,
    NE,
    LT,
    GT,
    LE,
    GE,
    LEFT_SHIFT,
    RIGHT_SHIFT,
    DEC_POST,
    INC_POST,
    DEC,
    INC,
    TERN,
    COMMA_OP,
    SIZEOF_OP,
    ADDRESS,
    DEREF
} Operator; // TODO: Add full list of operators

typedef enum
{
    CHAR_TYPE,
    INT_TYPE,
    LONG_TYPE,
    FLOAT_TYPE,
    DOUBLE_TYPE
} PrimativeType; // TODO: Add full list of types and deal with unsigned and void

typedef enum
{
    VARIABLE_EXPR,
    CONSTANT_EXPR,
    OPERATION_EXPR,
    ASSIGN_EXPR,
    FUNC_EXPR
} ExprType;

typedef enum
{
    WHILE_STMT,
    FOR_STMT,
    IF_STMT,
    SWITCH_STMT,
    EXPR_STMT,
    COMPOUND_STMT,
    LABEL_STMT,
    JUMP_STMT
} StmtType;

typedef enum
{
    GOTO_JUMP,
    CONTINUE_JUMP,
    BREAK_JUMP,
    RETURN_JUMP
} JumpType;


typedef struct Expr
{
    ExprType type;
    union
    {
        struct VariableExpr *variable;
        struct ConstantExpr *constant;
        struct OperationExpr *operation;
        struct AssignExpr *assignment;
        struct FuncExpr *function;
    };
} Expr;

typedef struct VariableExpr
{
    char *ident;
    PrimativeType type;
} VariableExpr;

typedef struct ConstantExpr
{
    union
    {
        int32_t int_const;
        float float_const;
        char char_const;
        char *string_const;
    };
    PrimativeType type;
    bool isString;
} ConstantExpr;

typedef struct OperationExpr
{
    Expr *op1;
    Expr *op2;
    Expr *op3;
    Operator operator;
    PrimativeType type;
} OperationExpr;

typedef struct AssignExpr
{
    char *ident;
    Expr *op;
    Operator operator;  // Info: when set to NOT, regular assignment
    PrimativeType type; // TODO: Replace, could accept compound types
} AssignExpr;

typedef struct FuncExpr
{
    char *ident;
    size_t argsSize;
    size_t argsCapacity;
    Expr **args;
    PrimativeType type; // TODO: Replace, functions may not retrun primative types only
} FuncExpr;

typedef struct Stmt
{
    StmtType type;
    union
    {
        struct WhileStmt *whileStmt;
        struct ForStmt *forStmt;
        struct IfStmt *ifStmt;
        struct SwitchStmt *switchStmt;
        struct ExprStmt *exprStmt;
        struct CompoundStmt *compoundStmt;
        struct LabelStmt *labelStmt;
        struct JumpStmt *jumpStmt;
    };
} Stmt;

typedef struct WhileStmt
{
    Expr *condition;
    Stmt *body;
    bool doWhile;
} WhileStmt;

typedef struct ForStmt
{
    Stmt *init;
    Stmt *condition;
    Expr *modifier;
    Stmt *body;
} ForStmt;

typedef struct IfStmt
{
    Expr *condition;
    Stmt *trueBody;
    Stmt *falseBody; // NULL for else
} IfStmt;

typedef struct SwitchStmt
{
    Expr *selector;
    Stmt *body;
} SwitchStmt;

typedef struct ExprStmt
{
    Expr* expr; // can be NULL
} ExprStmt;

typedef struct CompoundStmt
{
    // declarations are not expressions - need to fix.
    size_t declSize;
    size_t declCapacity;
    Expr **decls;

    size_t stmtSize;
    size_t stmtCapacity;
    Stmt **stmts;

} CompoundStmt;

typedef struct LabelStmt
{
    char *ident; // not NULL for goto
    Expr *caseLabel; // not NULL for case
    Stmt *body;

} LabelStmt;

typedef struct JumpStmt
{
    JumpType type;
    char *ident; // can be NULL
    Expr* expr; // can be NULL
} JumpStmt;

Expr *exprCreate(const ExprType type);
void exprDestroy(Expr *expr);

VariableExpr *variableExprCreate(char *ident);
void variableExprDestroy(VariableExpr *expr);

ConstantExpr *constantExprCreate(const PrimativeType type, const bool isString);
void constantExprDestroy(ConstantExpr *expr);

OperationExpr *operationExprCreate(const Operator operator);
void operationExprDestroy(OperationExpr *expr);

AssignExpr *assignExprCreate(char *ident, Expr *op, const Operator operator);
void assignExprDestroy(AssignExpr *expr);

FuncExpr *funcExprCreate(size_t argsSize);
void funcExprArgsResize(FuncExpr *expr, size_t argsSize);
void funcExprArgsPush(FuncExpr *expr, Expr *arg);
Expr *funcExprArgsPop(FuncExpr *expr);
void funcExprDestroy(FuncExpr *expr);

Stmt *stmtCreate(const StmtType type);
void stmtDestroy(Stmt* stmt);

Stmt *whileStmtCreate(Expr *condition, Stmt *body, bool doWhile);
void *whileStmtDestroy(WhileStmt *whileStmt);

ForStmt *forStmtCreate(Stmt *init, Stmt *condition, Stmt *body);
void forStmtDestroy(ForStmt *forStmt);

IfStmt *ifStmtCreate(Expr *condition, Stmt *trueBody);
void ifStmtDestroy(IfStmt *ifStmt);

SwitchStmt *switchStmtCreate(Expr *selector, Stmt *body);
void switchStmtDestroy(SwitchStmt *switchStmt);

ExprStmt *exprStmtCreate();
void exprStmtDestroy(ExprStmt *exprStmt);

CompoundStmt *compoundStmtCreate(const size_t declSize, const size_t stmtSize);
void compoundStmtDeclResize(CompoundStmt *stmt, const size_t declSize);
void compoundStmtStmtResize(CompoundStmt *stmt, const size_t stmtSize);
void compoundStmtDeclPush(CompoundStmt *compoundStmt, Expr *decl);
void compoundStmtStmtPush(CompoundStmt *compoundStmt, Stmt *stmt);
void compoundStmtDestroy(CompoundStmt *stmt);

LabelStmt *labelStmtCreate(Stmt *body);
void labelStmtDestroy(LabelStmt *labelStmt);

JumpStmt *jumpStmtCreate(const JumpType type);
void jumpStmtDestroy(JumpStmt *jumpStmt);

#endif