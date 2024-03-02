#include <stdlib.h>

#include "ast.h"

// Expression constructor
Expr *exprCreate(const ExprType type)
{
    Expr *expr = malloc(sizeof(Expr));
    if(expr == NULL)
    {
        abort();
    }
    expr->type = type;
    return expr;
}

// Expression destructor
void exprDestroy(Expr *expr)
{
    switch(expr->type)
    {
        case VARIABLE_EXPR:
        {
            variableExprDestroy(expr->variable);
            break;
        }
        case CONSTANT_EXPR:
        {
            constantExprDestroy(expr->constant);
            break;
        }
        case OPERATION_EXPR:
        {
            operationExprDestroy(expr->operation);
            break;
        }
        case ASSIGN_EXPR:
        {
            assignExprDestroy(expr->assignment);
            break;
        }
        case FUNC_EXPR:
        {
            funcExprDestroy(expr->function);
            break;
        }
    }
}

// Variable expression constructor
VariableExpr *variableExprCreate(char *ident)
{
    VariableExpr *expr = malloc(sizeof(VariableExpr));
    if(expr == NULL)
    {
        abort();
    }
    expr->ident = ident;
    return expr;
}

// Variable expresssion destructor
void variableExprDestroy(VariableExpr *dest)
{
    free(dest->ident);
    free(dest);
}

// Constant expression constructor
ConstantExpr *constantExprCreate(const PrimativeType type, const bool isString)
{
    ConstantExpr *expr = malloc(sizeof(ConstantExpr));
    if (expr == NULL)
    {
        abort();
    }
    expr->type = type;
    expr->isString = isString;
    return expr;
}

// Constant expression destructor
void constantExprDestroy(ConstantExpr *dest)
{
    if(dest->isString)
    {
        free(dest->string_const);
    }
    free(dest);
}

// Operation expression constructor
OperationExpr *operationExprCreate(const Operator operator)
{
    OperationExpr *expr = malloc(sizeof(OperationExpr));
    if(expr == NULL)
    {
        abort();
    }
    expr->operator = operator;
    return expr;
}

// Operation expression destructor
void operationExprDestroy(OperationExpr *dest)
{
    switch(dest->operator)
    {
        case NOT:
        {
            exprDestroy(dest->op1);
            break;
        }
    }
    free(dest);
}

// Assignment expression constructor
AssignExpr *assignExprCreate(char *ident, Expr *op)
{
    AssignExpr *expr = malloc(sizeof(AssignExpr));
    if(expr == NULL)
    {
        abort();
    }
    expr->ident = ident;
    expr->op = op;
    return expr;
}

// Assignment expression destructor
void assignExprDestroy(AssignExpr *dest)
{
    exprDestroy(dest->op);
    free(dest->ident);
    free(dest);
}

// Function call expression constructor
// Important: size must be greater than 0, if the allocation fails abort() is called
FuncExpr *funcExprCreate(char *ident, const size_t argsSize, const PrimativeType type)
{
    FuncExpr *dest = malloc(sizeof(FuncExpr));
    if(dest == NULL)
    {
        abort();
    }
    dest->args = malloc(sizeof(Expr) * argsSize);
    if(dest->args == NULL)
    {
        abort();
    }
    dest->ident = ident;
    dest->argsSize = argsSize;
    dest->argsCapacity = argsSize;
    dest->type = type;
    return dest;
}

// Resizes the size of the argument list
// Important: size must greater than 0
void funcExprResize(FuncExpr *dest, const size_t argsSize)
{
    if(dest->argsSize > dest->argsCapacity)
    {
        dest->args = realloc(dest->args, sizeof(Expr) * dest->argsCapacity * 2);
        if(dest->args == NULL)
        {
            abort();
        }
        dest->argsCapacity *= 2;
    }
    dest->argsSize = argsSize;
}

// Function expression destructor
void funcExprDestroy(FuncExpr *dest)
{
    free(dest->ident);
    free(dest->args);
    free(dest);
}
