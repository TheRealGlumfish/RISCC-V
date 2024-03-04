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
    exprDestroy(expr->op1);
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
AssignExpr *assignExprCreate(char *ident, Expr *op, const Operator operator)
{
    AssignExpr *expr = malloc(sizeof(AssignExpr));
    if (expr == NULL)
    {
        abort();
    }
    expr->ident = ident;
    expr->op = op;
    expr->operator= operator;
    return expr;
}

// Assignment expression destructor
void assignExprDestroy(AssignExpr *expr)
{
    exprDestroy(expr->op);
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
void stmtDestroy(Stmt* stmt)
{
    switch(stmt->type)
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
Stmt *whileStmtCreate(Expr *condition, Stmt *body, bool doWhile)
{
    WhileStmt *stmt = malloc(sizeof(WhileStmt));
    if (stmt == NULL){
        abort();
    }
    stmt->condition = condition;
    stmt->body = body;
    stmt->doWhile = doWhile;
}

// While statement destructor
void *whileStmtDestroy(WhileStmt *stmt)
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
ExprStmt *exprStmtCreate()
{
    ExprStmt *stmt = malloc(sizeof(ExprStmt));
    if (stmt == NULL){
        abort();
    }
    return stmt;
}

// Expression statement destructor
void exprStmtDestroy(ExprStmt *stmt)
{
    if(stmt->expr != NULL)
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
        stmt->declSize = malloc(sizeof(Expr) * declSize);
        if (stmt->declSize == NULL)
        {
            abort();
        }
    }
    else
    {
        stmt->declSize = NULL;
    }

    if (stmtSize != 0)
    {
        stmt->stmtSize = malloc(sizeof(Expr) * stmtSize);
        if (stmt->stmtSize == NULL)
        {
            abort();
        }
    }
    else
    {
        stmt->stmtSize = NULL;
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
    compoundStmt->decls[compoundStmt->declSize - 1] = stmt;
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
            exprDestroy(stmt->stmts[i]);
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
    return stmt;
}

// Label statement destructor
void labelStmtDestroy(LabelStmt *stmt)
{
    stmtDestroy(stmt->body);
    if(stmt->ident != NULL)
    {
        free(stmt->ident);
    }
    if(stmt->caseLabel != NULL)
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
    stmt->type;
    return stmt;
}

// Jump statement destuctor
void jumpStmtDestroy(JumpStmt *stmt)
{
    if(stmt->ident != NULL)
    {
        free(stmt->ident);
    }
    if(stmt->expr != NULL)
    {
        exprDestroy(stmt->expr);
    }
    free(stmt);
}








