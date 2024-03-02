#ifndef AST_H
#define AST_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum { ADD, SUB, MUL, DIV, AND, OR, NOT, AND_BIT, OR_BIT, NOT_BIT, XOR , DEC_POST, INC_POST, DEC, INC } Operator; // TODO: Add full list of operators

typedef enum { CHAR_TYPE, INT_TYPE, FLOAT_TYPE } PrimativeType; // TODO: Add full list of types and deal with unsigned and void

typedef enum { VARIABLE_EXPR, CONSTANT_EXPR, OPERATION_EXPR, ASSIGN_EXPR, FUNC_EXPR } ExprType;

typedef struct Expr {
    ExprType type;
    union {
        struct VariableExpr *variable;
        struct ConstantExpr *constant;
        struct OperationExpr *operation;
        struct AssignExpr *assignment; 
        struct FuncExpr *function;
    };
} Expr;

typedef struct VariableExpr {
    char *ident;
    PrimativeType type;    
} VariableExpr;

typedef struct ConstantExpr {
    union {
        int32_t int_const;
        float float_const;
        char char_const;
        char *string_const;
    };
    PrimativeType type;
    bool isString;
} ConstantExpr;

typedef struct OperationExpr {
    Expr *op1;
    Expr *op2;
    Expr *op3;
    Operator operator;
    PrimativeType type;
} OperationExpr;

typedef struct AssignExpr {
    char *ident;
    Expr *op;
    PrimativeType type; // TODO: Replace, could accept compound types
} AssignExpr;

typedef struct FuncExpr {
    char *ident;
    size_t argsSize;
    size_t argsCapacity;
    Expr *args;
    PrimativeType type; // TODO: Replace, functions may not retrun primative types only
} FuncExpr;

Expr *exprCreate(const ExprType type);
void exprDestroy(Expr *expr);

VariableExpr *variableExprCreate(char *ident);
void variableExprDestroy(VariableExpr *dest);

ConstantExpr *constantExprCreate(const PrimativeType type, const bool isString);
void constantExprDestroy(ConstantExpr *dest);

OperationExpr *operationExprCreate(const Operator operator);
void operationExprDestroy(OperationExpr *dest);

AssignExpr *assignExprCreate(char *ident, Expr *op);
void assignExprDestroy(AssignExpr *dest);

FuncExpr *funcExprCreate(char *ident, size_t argsSize, PrimativeType type);
void funcExprResize(FuncExpr *dest, size_t argsSize);
void funcExprDestroy(FuncExpr *dest);

#endif
