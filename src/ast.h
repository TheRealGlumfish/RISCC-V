#ifndef AST_H
#define AST_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum { ADD, SUB, MUL, DIV, AND, OR, NOT } Operator; // TODO: Add full list of operators

typedef enum { CHAR_TYPE, INT_TYPE, FLOAT_TYPE } PrimativeType; // TODO: Add full list of types and deal with unsigned and void

typedef enum { VARIABLE_EXPR, CONSTANT_EXPR, OPERATION_EXPR, ASSIGN_EXPR, FUNC_EXPR } ExprType;

struct VariableExpr;
struct ConstantExpr;
struct OperationExpr;
struct AssignExpr;
struct FuncExpr;

typedef struct {
    ExprType type;
    union {
        struct VariableExpr *variable;
        struct ConstantExpr *constant;
        struct OperationExpr *operation;
        struct AssignExpr *assignment; 
        struct FuncExpr *function;
    };
} Expr;

typedef struct {
    char *ident;
    PrimativeType type;    
} VariableExpr;

typedef struct {
    union {
        int32_t int_const;
        float float_const;
        char char_const;
        char *string_const;
    };
    PrimativeType type;
    bool isString;
} ConstantExpr;

typedef struct {
    Expr op1;
    Expr op2;
    Expr op3;
    Operator operator;
    PrimativeType type;
} OperationExpr;

typedef struct {
    char *ident;
    Expr op;
    PrimativeType type; // TODO: Replace, could accept compound types
} AssignExpr;

typedef struct {
    const char *ident;
    size_t argsSize;
    size_t argsCapacity;
    Expr *args;
    PrimativeType type; // TODO: Replace, functions may not retrun primative types only
} FuncExpr;


ConstantExpr *constantExprCreate(const PrimativeType type, const bool isString);
void constantExprDestroy(ConstantExpr *dest);

FuncExpr *funcExprCreate(const char *ident, size_t argsSize, PrimativeType type);
void funcExprResize(FuncExpr *dest, size_t argsSize);
void funcExprDestroy(FuncExpr *dest);

#endif
