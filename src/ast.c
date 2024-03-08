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
ConstantExpr *constantExprCreate(const TypeSpecifier type, const bool isString)
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

// Statement List initializer
void statementListInit(StatementList *stmtList, const size_t size)
{
    stmtList->stmts = malloc(sizeof(Stmt *) * size);
    if (stmtList->stmts == NULL)
    {
       abort();
    }
    stmtList->size = size;
    stmtList->capacity = size;
}

// Statement List destructor
void statementListDestroy(StatementList *stmtList)
{
    free(stmtList->stmts);
    stmtList->stmts = NULL;
    stmtList->size = 0;
    stmtList->capacity = 0;
}

// Resizes the size of the statement list
// Important: size must greater than 0
void statementListResize(StatementList *stmtList, const size_t size)
{
    if (stmtList->size != 0)
    {
        stmtList->size = size;
        if (stmtList->size > stmtList->capacity)
        {
            while (stmtList->size > stmtList->capacity)
            {
                stmtList->capacity *= 2;
            }
            stmtList->stmts = realloc(stmtList->stmts, sizeof(Stmt *) * stmtList->capacity);
            if (stmtList->stmts == NULL)
            {
                abort();
            }
        }
    }
    else
    {
        stmtList->size = size;
        stmtList->stmts = malloc(sizeof(Stmt *) * size);
        if (stmtList->stmts == NULL)
        {
            abort();
        }
        stmtList->capacity = size;
    }
}

// Adds an argument to the end of the statement list
void statementListPush(StatementList *stmtList, Stmt *stmt)
{
    statementListResize(stmtList, stmtList->size + 1);
    stmtList->stmts[stmtList->size - 1] = stmt;
}

// Declaration List initializer
void declarationListInit(DeclarationList *declList, const size_t size)
{
    declList->decls = malloc(sizeof(Expr *) * size); // TODO: Change to declarations
    if (declList->decls == NULL)
    {
       abort();
    }
    declList->size = size;
    declList->capacity = size;
}

// Declaration List destructor
void declarationListDestroy(DeclarationList *declList)
{
    free(declList->decls);
    declList->decls = NULL;
    declList->size = 0;
    declList->capacity = 0;
}

// Resizes the size of the declaration list
// Important: size must greater than 0
void declarationListResize(DeclarationList *declList, const size_t size)
{
    if (declList->size != 0)
    {
        declList->size = size;
        if (declList->size > declList->capacity)
        {
            while (declList->size > declList->capacity)
            {
                declList->capacity *= 2;
            }
            declList->decls = realloc(declList->decls, sizeof(Expr *) * declList->capacity); // TODO: Change to declaration type
            if (declList->decls == NULL)
            {
                abort();
            }
        }
    }
    else
    {
        declList->size = size;
        declList->decls = malloc(sizeof(Stmt *) * size);
        if (declList->decls == NULL)
        {
            abort();
        }
        declList->capacity = size;
    }
}

// Adds an argument to the end of the declaration list
void declarationListPush(DeclarationList *declList, Expr *decl)
{
    declarationListResize(declList, declList->size + 1);
    declList->decls[declList->size - 1] = decl;
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
TypeSpecList *typeSpecListCreate(const size_t typeSpecSize)
{
    TypeSpecList *typeSpecList = malloc(sizeof(TypeSpecList));
    if (typeSpecList == NULL)
    {
        abort();
    }

    if (typeSpecSize != 0)
    {
        typeSpecList->typeSpecs = malloc(sizeof(TypeSpecifier *) * typeSpecSize); 
        if (typeSpecList->typeSpecs == NULL)
        {
            abort();
        }
    }
    else
    {
        typeSpecList->typeSpecs = NULL;
    }

    typeSpecList->typeSpecSize = typeSpecSize;
    typeSpecList->typeSpecCapacity = typeSpecSize;
    return typeSpecList;
}

// Type Qualifier List destructor
void typeSpecListDestroy(TypeSpecList *typeSpecList)
{   
    if (typeSpecList->typeSpecs != NULL) // don't free a NULL pointer
    {
        free(typeSpecList->typeSpecs);
    }

    free(typeSpecList);
}


// Resizes the size of the type qualifier list
void typeSpecListResize(TypeSpecList *typeSpecList, const size_t typeSpecSize)
{
    if (typeSpecList->typeSpecSize != 0)
    {
        typeSpecList->typeSpecSize = typeSpecSize;
        if (typeSpecList->typeSpecSize > typeSpecList->typeSpecCapacity)
        {
            while (typeSpecList->typeSpecSize > typeSpecList->typeSpecCapacity)
            {
                typeSpecList->typeSpecCapacity *= 2;
            }
            typeSpecList->typeSpecs = realloc(typeSpecList->typeSpecs, sizeof(TypeSpecifier *) * typeSpecList->typeSpecCapacity);
            if (typeSpecList->typeSpecs == NULL)
            {
                abort();
            }
        }
    }
    else
    {
        typeSpecList->typeSpecSize = typeSpecSize;
        typeSpecList->typeSpecs = malloc(sizeof(TypeSpecifier *) * typeSpecSize);
        if (typeSpecList->typeSpecs == NULL)
        {
            abort();
        }
        typeSpecList->typeSpecCapacity = typeSpecSize;
    }
}

// Adds a type qualifier to a type qualifier list
void typeSpecListPush(TypeSpecList *typeSpecList, TypeSpecifier typeSpec)
{
    typeSpecListResize(typeSpecList, typeSpecList->typeSpecSize + 1);
    typeSpecList->typeSpecs[typeSpecList->typeSpecSize - 1] = typeSpec;
}

// Declaration-Initializer constructor
DeclInit *declInitCreate(const char* ident, const size_t pointerCount)
{
    DeclInit *declInit = malloc(sizeof(DeclInit));
    if (declInit == NULL)
    {
        abort();
    }
    declInit->ident = ident;
    declInit->pointerCount = pointerCount;
    return declInit;
}

// Declaration-Initializer destructor
void declInitDestroy(DeclInit *declInit)
{
    if (declInit->initExpr!=NULL)
    {
        free(declInit->initExpr);
    }
    free(declInit->ident);
    free(declInit);
}

// Declaration constructor
Declaration *declarationCreate(const size_t declInitSize)
{
    Declaration *decl = malloc(sizeof(Declaration));
    if (decl == NULL)
    {
        abort();
    }

    if (declInitSize != 0)
    {
        decl->declInits = malloc(sizeof(DeclInit *) * declInitSize); 
        if (decl->declInits == NULL)
        {
            abort();
        }
    }
    else
    {
        decl->declInits = NULL;
    }

    decl->declInitSize = declInitSize;
    decl->declInitCapacity = declInitSize;

    return decl;
}

// Declaration destructor
void declarationDestroy(Declaration *decl)
{   
    typeSpecListDestroy(decl->typeSpecList);
    
    if (decl->declInitCapacity != 0)
    {
        for (size_t i = 0; i < decl->declInitSize; i++)
        {
            declInitDestroy(decl->declInits[i]);
        }
        free(decl->declInits);
    }

    free(decl);
}


// Resizes the size of the declInit list in the declaration
void declarationResize(Declaration *decl, const size_t declInitSize)
{
    if (decl->declInitSize != 0)
    {
        decl->declInitSize = declInitSize;
        if (decl->declInitSize > decl->declInitCapacity)
        {
            while (decl->declInitSize > decl->declInitCapacity)
            {
                decl->declInitCapacity *= 2;
            }
            decl->declInits = realloc(decl->declInits, sizeof(DeclInit *) * decl->declInitCapacity);
            if (decl->declInits == NULL)
            {
                abort();
            }
        }
    }
    else
    {
        decl->declInitSize = declInitSize;
        decl->declInits = malloc(sizeof(DeclInit *) * declInitSize);
        if (decl->declInits == NULL)
        {
            abort();
        }
        decl->declInitCapacity = declInitSize;
    }
}

// Adds a declInit to the declaration node
void declInitPush(Declaration *decl, DeclInit *declInit)
{
    declarationResize(decl, decl->declInitSize + 1);
    decl->declInits[decl->declInitSize - 1] = declInit;
}




