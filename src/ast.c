#include <stdlib.h>

#include "ast.h"
#include <stdio.h>

// Expression constructor
Expr *exprCreate(const ExprType type)
{
    Expr *expr = malloc(sizeof(Expr));
    if (expr == NULL)
    {
        abort();
    }
    expr->type = type;
    return expr;
}

// Expression destructor
void exprDestroy(Expr *expr)
{
    switch (expr->type)
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
    free(expr);
}

// Variable expression constructor
VariableExpr *variableExprCreate(char *ident)
{
    VariableExpr *expr = malloc(sizeof(VariableExpr));
    if (expr == NULL)
    {
        abort();
    }
    expr->ident = ident;
    return expr;
}

// Variable expresssion destructor
void variableExprDestroy(VariableExpr *expr)
{
    free(expr->ident);
    free(expr);
}

// Constant expression constructor
ConstantExpr *constantExprCreate(const TypeQualifier type, const bool isString)
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
void constantExprDestroy(ConstantExpr *expr)
{
    if (expr->isString)
    {
        free(expr->string_const);
    }
    free(expr);
}

// Operation expression constructor
OperationExpr *operationExprCreate(const Operator operator)
{
    OperationExpr *expr = malloc(sizeof(OperationExpr));
    if (expr == NULL)
    {
        abort();
    }
    expr->op1 = NULL;
    expr->op2 = NULL;
    expr->op3 = NULL;
    expr->operator= operator;
    return expr;
}

// Operation expression destructor
void operationExprDestroy(OperationExpr *expr)
{
    // Note: Do not change deallocation behaviour, parser relies on it
    if (expr->op1 != NULL)
    {
        exprDestroy(expr->op1);
    }
    if (expr->op2 != NULL)
    {
        exprDestroy(expr->op2);
    }
    if (expr->op3 != NULL)
    {
        exprDestroy(expr->op3);
    }
    free(expr);
}

// Assignment expression constructor
AssignExpr *assignExprCreate(Expr *op, const Operator operator)
{
    AssignExpr *expr = malloc(sizeof(AssignExpr));
    if (expr == NULL)
    {
        abort();
    }
    expr->ident = NULL;
    expr->lvalue = NULL;
    expr->op = op;
    expr->operator= operator;
    return expr;
}

// Assignment expression destructor
void assignExprDestroy(AssignExpr *expr)
{
    exprDestroy(expr->op);
    if (expr->lvalue != NULL)
    {
        exprDestroy(expr->lvalue);
    }
    free(expr->ident);
    free(expr);
}

// Function call expression constructor
// Important: if the allocation fails abort() is called
FuncExpr *funcExprCreate(const size_t argsSize)
{
    FuncExpr *expr = malloc(sizeof(FuncExpr));
    if (expr == NULL)
    {
        abort();
    }
    if (argsSize != 0)
    {
        expr->args = malloc(sizeof(Expr) * argsSize);
        if (expr->args == NULL)
        {
            abort();
        }
    }
    else
    {
        expr->args = NULL;
    }
    expr->ident = NULL;
    expr->argsSize = argsSize;
    expr->argsCapacity = argsSize;
    return expr;
}

// Resizes the size of the argument list
// Important: size must greater than 0
void funcExprArgsResize(FuncExpr *expr, const size_t argsSize)
{
    if (expr->argsSize != 0)
    {
        expr->argsSize = argsSize;
        if (expr->argsSize > expr->argsCapacity)
        {
            while (expr->argsSize > expr->argsCapacity)
            {
                expr->argsCapacity *= 2;
            }
            expr->args = realloc(expr->args, sizeof(Expr *) * expr->argsCapacity);
            if (expr->args == NULL)
            {
                abort();
            }
        }
    }
    else
    {
        expr->argsSize = argsSize;
        expr->args = malloc(sizeof(Expr *) * argsSize);
        if (expr->args == NULL)
        {
            abort();
        }
        expr->argsCapacity = argsSize;
    }
}

// Adds an argument to the end of the argument list
void funcExprArgsPush(FuncExpr *expr, Expr *arg)
{
    funcExprArgsResize(expr, expr->argsSize + 1);
    expr->args[expr->argsSize - 1] = arg;
}

// Removes an argument from the end of the argument list
Expr *funcExprArgsPop(FuncExpr *expr)
{
    Expr *arg = expr->args[expr->argsSize - 1];
    funcExprArgsResize(expr, expr->argsSize - 1);
    return arg;
}

// Function expression destructor
void funcExprDestroy(FuncExpr *expr)
{
    free(expr->ident);
    if (expr->argsCapacity != 0)
    {
        for (size_t i = 0; i < expr->argsSize; i++)
        {
            exprDestroy(expr->args[i]);
        }
        free(expr->args);
    }
    free(expr);
}

// Statement constructor
Stmt *stmtCreate(const StmtType type)
{
    Stmt *stmt = malloc(sizeof(Stmt));
    if (stmt == NULL)
    {
        abort();
    }
    stmt->type = type;
    return stmt;
}

// Statement destructor
void stmtDestroy(Stmt *stmt)
{
    switch (stmt->type)
    {
    case WHILE_STMT:
        whileStmtDestroy(stmt->whileStmt);
        break;
    case FOR_STMT:
        forStmtDestroy(stmt->forStmt);
        break;
    case IF_STMT:
        ifStmtDestroy(stmt->ifStmt);
        break;
    case SWITCH_STMT:
        switchStmtDestroy(stmt->switchStmt);
        break;
    case EXPR_STMT:
        exprStmtDestroy(stmt->exprStmt);
        break;
    case COMPOUND_STMT:
        compoundStmtDestroy(stmt->compoundStmt);
        break;
    case LABEL_STMT:
        labelStmtDestroy(stmt->labelStmt);
        break;
    case JUMP_STMT:
        jumpStmtDestroy(stmt->jumpStmt);
        break;
    }
    free(stmt);
}

// While statement constructor
WhileStmt *whileStmtCreate(Expr *condition, Stmt *body, const bool doWhile)
{
    WhileStmt *stmt = malloc(sizeof(WhileStmt));
    if (stmt == NULL)
    {
        abort();
    }
    stmt->condition = condition;
    stmt->body = body;
    stmt->doWhile = doWhile;
    return stmt;
}

// While statement destructor
void whileStmtDestroy(WhileStmt *stmt)
{
    exprDestroy(stmt->condition);
    stmtDestroy(stmt->body);
    free(stmt);
}

// For statement constructor
ForStmt *forStmtCreate(Stmt *init, Stmt *condition, Stmt *body)
{
    ForStmt *stmt = malloc(sizeof(ForStmt));
    if (stmt == NULL)
    {
        abort();
    }
    stmt->init = init;
    stmt->condition = condition;
    stmt->body = body;
    stmt->modifier = NULL;
    return stmt;
}

// For statement destructor
void forStmtDestroy(ForStmt *stmt)
{
    stmtDestroy(stmt->init);
    stmtDestroy(stmt->condition);
    stmtDestroy(stmt->body);
    if (stmt->modifier != NULL)
    {
        exprDestroy(stmt->modifier);
    }
    free(stmt);
}

// If statement constructor
IfStmt *ifStmtCreate(Expr *condition, Stmt *trueBody)
{
    IfStmt *stmt = malloc(sizeof(IfStmt));
    if (stmt == NULL)
    {
        abort();
    }
    stmt->condition = condition;
    stmt->trueBody = trueBody;
    stmt->falseBody = NULL;
    return stmt;
}

// If statement destructor
void ifStmtDestroy(IfStmt *stmt)
{
    exprDestroy(stmt->condition);
    stmtDestroy(stmt->trueBody);
    if (stmt->falseBody != NULL)
    {
        stmtDestroy(stmt->falseBody);
    }
    free(stmt);
}

// Switch statement constructor
SwitchStmt *switchStmtCreate(Expr *selector, Stmt *body)
{
    SwitchStmt *stmt = malloc(sizeof(SwitchStmt));
    stmt->selector = selector;
    stmt->body = body;
    return stmt;
}

// Switch statement destroy
void switchStmtDestroy(SwitchStmt *stmt)
{
    exprDestroy(stmt->selector);
    stmtDestroy(stmt->body);
    free(stmt);
}

// Expression statement constructor
ExprStmt *exprStmtCreate(void)
{
    ExprStmt *stmt = malloc(sizeof(ExprStmt));
    if (stmt == NULL)
    {
        abort();
    }
    stmt->expr = NULL;
    return stmt;
}

// Expression statement destructor
void exprStmtDestroy(ExprStmt *stmt)
{
    if (stmt->expr != NULL)
    {
        exprDestroy(stmt->expr);
    }
    free(stmt);
}

// Compound statement constructor
CompoundStmt *compoundStmtCreate(const size_t declSize, const size_t stmtSize)
{
    CompoundStmt *stmt = malloc(sizeof(CompoundStmt));
    if (stmt == NULL)
    {
        abort();
    }

    if (declSize != 0)
    {
        stmt->decls = malloc(sizeof(Expr *) * declSize); // TODO: change to decls
        if (stmt->decls == NULL)
        {
            abort();
        }
    }
    else
    {
        stmt->decls = NULL;
    }

    if (stmtSize != 0)
    {
        stmt->stmts = malloc(sizeof(Expr *) * stmtSize);
        if (stmt->stmts == NULL)
        {
            abort();
        }
    }
    else
    {
        stmt->stmts = NULL;
    }

    stmt->declSize = declSize;
    stmt->declCapacity = declSize;
    stmt->stmtSize = stmtSize;
    stmt->stmtCapacity = stmtSize;
    return stmt;
}

// Resizes the size of the declaration list
void compoundStmtDeclResize(CompoundStmt *stmt, const size_t declSize)
{
    if (stmt->declSize != 0)
    {
        stmt->declSize = declSize;
        if (stmt->declSize > stmt->declCapacity)
        {
            while (stmt->declSize > stmt->declCapacity)
            {
                stmt->declCapacity *= 2;
            }
            stmt->decls = realloc(stmt->decls, sizeof(Expr *) * stmt->declCapacity);
            if (stmt->decls == NULL)
            {
                abort();
            }
        }
    }
    else
    {
        stmt->declSize = declSize;
        stmt->decls = malloc(sizeof(Expr *) * declSize);
        if (stmt->decls == NULL)
        {
            abort();
        }
        stmt->declCapacity = declSize;
    }
}

// Resizes the size of the statement list
void compoundStmtStmtResize(CompoundStmt *stmt, const size_t stmtSize)
{
    if (stmt->stmtSize != 0)
    {
        stmt->stmtSize = stmtSize;
        if (stmt->stmtSize > stmt->stmtCapacity)
        {
            while (stmt->stmtSize > stmt->stmtCapacity)
            {
                stmt->stmtCapacity *= 2;
            }
            stmt->stmts = realloc(stmt->stmts, sizeof(Stmt *) * stmt->stmtCapacity);
            if (stmt->stmts == NULL)
            {
                abort();
            }
        }
    }
    else
    {
        stmt->stmtSize = stmtSize;
        stmt->stmts = malloc(sizeof(Stmt *) * stmtSize);
        if (stmt->stmts == NULL)
        {
            abort();
        }
        stmt->stmtCapacity = stmtSize;
    }
}

// Adds a declaration to the compound statement
void compoundStmtDeclPush(CompoundStmt *compoundStmt, Expr *decl)
{
    compoundStmtDeclResize(compoundStmt, compoundStmt->declSize + 1);
    compoundStmt->decls[compoundStmt->declSize - 1] = decl;
}

// Adds a statement to the compound statement
void compoundStmtStmtPush(CompoundStmt *compoundStmt, Stmt *stmt)
{
    compoundStmtDeclResize(compoundStmt, compoundStmt->stmtSize + 1);
    compoundStmt->stmts[compoundStmt->declSize - 1] = stmt;
}

// Function expression destructor
void compoundStmtDestroy(CompoundStmt *stmt)
{
    if (stmt->declCapacity != 0)
    {
        for (size_t i = 0; i < stmt->declSize; i++)
        {
            exprDestroy(stmt->decls[i]);
        }
        free(stmt->decls);
    }

    if (stmt->stmtCapacity != 0)
    {
        for (size_t i = 0; i < stmt->stmtSize; i++)
        {
            stmtDestroy(stmt->stmts[i]);
        }
        free(stmt->stmts);
    }

    free(stmt);
}

// Label statement constructor
LabelStmt *labelStmtCreate(Stmt *body)
{
    LabelStmt *stmt = malloc(sizeof(LabelStmt));
    if (stmt != NULL)
    {
        abort();
    }
    stmt->body = body;
    stmt->ident = NULL;
    stmt->caseLabel = NULL;
    return stmt;
}

// Label statement destructor
void labelStmtDestroy(LabelStmt *stmt)
{
    stmtDestroy(stmt->body);
    if (stmt->ident != NULL)
    {
        free(stmt->ident);
    }
    if (stmt->caseLabel != NULL)
    {
        exprDestroy(stmt->caseLabel);
    }
    stmtDestroy(stmt->body);
    free(stmt);
}

// Jump statement constructor
JumpStmt *jumpStmtCreate(const JumpType type)
{
    JumpStmt *stmt = malloc(sizeof(JumpStmt));
    if (stmt == NULL)
    {
        abort();
    }
    stmt->type = type;
    stmt->ident = NULL;
    stmt->expr = NULL;
    return stmt;
}

// Jump statement destuctor
void jumpStmtDestroy(JumpStmt *stmt)
{
    if (stmt->expr != NULL)
    {
        exprDestroy(stmt->expr);
    }
    free(stmt->ident);
    free(stmt);
}

// Type Qualifier List constructor
TypeQualList *typeQualListCreate(const size_t typeQualSize)
{
    TypeQualList *typeQualList = malloc(sizeof(typeQualList));
    if (typeQualList == NULL)
    {
        abort();
    }

    if (typeQualSize != 0)
    {
        typeQualList->typeQuals = malloc(sizeof(TypeQualifier *) * typeQualSize); // TODO: change to decls
        if (typeQualList->typeQuals == NULL)
        {
            abort();
        }
    }
    else
    {
        typeQualList->typeQuals = NULL;
    }

    typeQualList->typeQualSize = typeQualSize;
    typeQualList->typeQualCapacity = typeQualSize;
    return typeQualList;
}

// Type Qualifier List destructor
void typeQualListDestroy(TypeQualList *typeQualList)
{   
    if (typeQualList->typeQuals != NULL) // don't free a NULL pointer
    {
        free(typeQualList->typeQuals);
    }

    free(typeQualList);
}


// Resizes the size of the type qualifier list
void typeQualListResize(TypeQualList *typeQualList, const size_t typeQualSize)
{
    if (typeQualList->typeQualSize != 0)
    {
        typeQualList->typeQualSize = typeQualSize;
        if (typeQualList->typeQualSize > typeQualList->typeQualCapacity)
        {
            while (typeQualList->typeQualSize > typeQualList->typeQualCapacity)
            {
                typeQualList->typeQualCapacity *= 2;
            }
            typeQualList->typeQuals = realloc(typeQualList->typeQuals, sizeof(TypeQualifier *) * typeQualList->typeQualCapacity);
            if (typeQualList->typeQuals == NULL)
            {
                abort();
            }
        }
    }
    else
    {
        typeQualList->typeQualSize = typeQualSize;
        typeQualList->typeQuals = malloc(sizeof(TypeQualifier *) * typeQualSize);
        if (typeQualList->typeQuals == NULL)
        {
            abort();
        }
        typeQualList->typeQualCapacity = typeQualSize;
    }
}

// Adds a type qualifier to a type qualifier list
void typeQualListPush(TypeQualList *typeQualList, TypeQualifier typeQual)
{
    typeQualListResize(typeQualList, typeQualList->typeQualSize + 1);
    typeQualList->typeQuals[typeQualList->typeQualSize - 1] = typeQual;
}

// Declaration constructor
Declaration *declarationCreate()
{
    Declaration *decl = malloc(sizeof(Declaration));
    if (decl == NULL)
    {
        abort();
    }

    decl->typeQualList = typeQualListCreate(0);
    decl->pointerCount = 0;
    return decl;
}

// Declaration destructor
void declarationDestroy(Declaration *decl)
{   
    typeQualListDestroy(decl->typeQualList);
    free(decl->ident);
    if(decl->initExpr != NULL)
    {
        exprDestroy(decl->initExpr);
    }
    free(decl);
}




