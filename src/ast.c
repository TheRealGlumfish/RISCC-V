#include <stdlib.h>

#include "ast.h"

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

// Function call expression constructor
// Important: size must be greater than 0, if the allocation fails abort() is called
FuncExpr *funcExprCreate(const char *ident, const size_t argsSize, const PrimativeType type)
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
    free(dest->args);
    free(dest);
    dest = NULL;
}
