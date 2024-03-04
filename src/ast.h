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

#endif
