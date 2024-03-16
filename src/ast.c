#include <stddef.h>
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
ConstantExpr *constantExprCreate(const DataType type, const bool isString)
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
    if (size != 0)
    {
        stmtList->stmts = malloc(sizeof(Stmt *) * size);
        if (stmtList->stmts == NULL)
        {
            abort();
        }
    }
    else
    {
        stmtList->stmts = NULL;
    }
    stmtList->size = size;
    stmtList->capacity = size;
}

// Statement List destructor
void statementListDestroy(StatementList *stmtList)
{
    for (size_t i = 0; i < stmtList->size; i++)
    {
        stmtDestroy(stmtList->stmts[i]);
    }
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
    if (size != 0)
    {
        declList->decls = malloc(sizeof(Decl *) * size);
        if (declList->decls == NULL)
        {
            abort();
        }
    }
    else
    {
        declList->decls = NULL;
    }

    declList->size = size;
    declList->capacity = size;
}

// Declaration List destructor
void declarationListDestroy(DeclarationList *declList)
{
    for (size_t i = 0; i < declList->size; i++)
    {
        declDestroy(declList->decls[i]);
    }
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
            declList->decls = realloc(declList->decls, sizeof(Decl *) * declList->capacity);
            if (declList->decls == NULL)
            {
                abort();
            }
        }
    }
    else
    {
        declList->size = size;
        declList->decls = malloc(sizeof(Decl *) * size);
        if (declList->decls == NULL)
        {
            abort();
        }
        declList->capacity = size;
    }
}

// Adds an argument to the end of the declaration list
void declarationListPush(DeclarationList *declList, Decl *decl)
{
    declarationListResize(declList, declList->size + 1);
    declList->decls[declList->size - 1] = decl;
}

// Compound statement constructor
CompoundStmt *compoundStmtCreate(void)
{
    CompoundStmt *stmt = malloc(sizeof(CompoundStmt));
    if (stmt == NULL)
    {
        abort();
    }
    stmt->stmtList.size = 0;
    stmt->stmtList.capacity = 0;
    stmt->stmtList.stmts = NULL;
    stmt->declList.size = 0;
    stmt->declList.capacity = 0;
    stmt->declList.decls = NULL;
    return stmt;
}

// Compound statement destructor
void compoundStmtDestroy(CompoundStmt *stmt)
{
    statementListDestroy(&stmt->stmtList);
    declarationListDestroy(&stmt->declList);
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
    for (size_t i = 0; i < typeSpecList->typeSpecSize; i++)
    {
        typeSpecifierDestroy(typeSpecList->typeSpecs[i]);
    }
    free(typeSpecList->typeSpecs);
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
void typeSpecListPush(TypeSpecList *typeSpecList, TypeSpecifier *typeSpec)
{
    typeSpecListResize(typeSpecList, typeSpecList->typeSpecSize + 1);
    typeSpecList->typeSpecs[typeSpecList->typeSpecSize - 1] = typeSpec;
}

// Copy constructor for type specifiers
TypeSpecifier *typeSpecifierCopy(TypeSpecifier *typeSpec)
{
    TypeSpecifier *newTypeSpec = typeSpecifierCreate(typeSpec->isStruct);
    if (newTypeSpec->isStruct)
    {
        // fatal flaw: needs to be implemented
    }
    else
    {
        newTypeSpec->dataType = typeSpec->dataType;
    }
    return newTypeSpec;
}

// Type Specifier List Copy Constructor
TypeSpecList *typeSpecListCopy(TypeSpecList *typeSpecList)
{
    TypeSpecList *newTypeSpecList = typeSpecListCreate(0);
    for (size_t i = 0; i < typeSpecList->typeSpecSize; i++)
    {
        TypeSpecifier *typeSpecCopy = typeSpecifierCopy(typeSpecList->typeSpecs[i]);
        typeSpecListPush(newTypeSpecList, typeSpecCopy);
    }
    return newTypeSpecList;
}

Declarator *declaratorCreate(void)
{
    Declarator *declarator = malloc(sizeof(Declarator));
    if (declarator == NULL)
    {
        abort();
    }
    declarator->pointerCount = 0;
    declarator->parameterList = NULL;
    declarator->isArray = false;
    return declarator;
}

void declaratorDestroy(Declarator *declarator)
{
    if (declarator->ident != NULL)
    {
        free(declarator->ident);
    }
    if (declarator->parameterList != NULL)
    {
        declarationListDestroy(declarator->parameterList);
    }
    if(declarator->isArray)
    {
        exprDestroy(declarator->arraySize);
    }
    free(declarator);
}

// Declaration-Initializer constructor
DeclInit *declInitCreate(Declarator *declarator)
{
    DeclInit *declInit = malloc(sizeof(DeclInit));
    if (declInit == NULL)
    {
        abort();
    }
    declInit->declarator = declarator;
    declInit->initExpr = NULL;
    declInit->initList = NULL;
    return declInit;
}

// Declaration-Initializer destructor
void declInitDestroy(DeclInit *declInit)
{
    if(declInit->initList != NULL) // may not be initialized
    {
        initListDestroy(declInit->initList);
    }
    if (declInit->initExpr != NULL) // may not be initialized
    {
        exprDestroy(declInit->initExpr);
    }
    declaratorDestroy(declInit->declarator);
    free(declInit);
}

// Declaration constructor
Decl *declCreate(TypeSpecList *typeSpecList)
{
    Decl *decl = malloc(sizeof(Decl));
    if (decl == NULL)
    {
        abort();
    }

    decl->typeSpecList = typeSpecList;
    decl->declInit = NULL;
    return decl;
}

// Declaration destructor
void declDestroy(Decl *decl)
{
    typeSpecListDestroy(decl->typeSpecList);
    if (decl->declInit != NULL)
    {
        declInitDestroy(decl->declInit);
    }
    free(decl);
}

DeclInitList *declInitListCreate(const size_t declInitListSize)
{
    DeclInitList *declInitList = malloc(sizeof(DeclInitList));
    if (declInitList == NULL)
    {
        abort();
    }

    if (declInitListSize != 0)
    {
        // recent change
        declInitList->declInits = malloc(sizeof(DeclInit *) * declInitListSize);
        if (declInitList->declInits == NULL)
        {
            abort();
        }
    }
    else
    {
        declInitList->declInits = NULL;
    }

    declInitList->declInitListSize = declInitListSize;
    declInitList->declInitListCapacity = declInitListSize;
    return declInitList;
}

// Declaration Initializer List Destructor
void declInitListDestroy(DeclInitList *declInitList)
{
    for (size_t i = 0; i < declInitList->declInitListSize; i++)
    {
        declInitDestroy(declInitList->declInits[i]);
    }
    free(declInitList->declInits);
    free(declInitList);
}

// Resizes the size of the declaration initializer list
void declInitListResize(DeclInitList *declInitList, const size_t declInitListSize)
{
    if (declInitList->declInitListSize != 0)
    {
        declInitList->declInitListSize = declInitListSize;
        if (declInitList->declInitListSize > declInitList->declInitListCapacity)
        {
            while (declInitList->declInitListSize > declInitList->declInitListCapacity)
            {
                declInitList->declInitListCapacity *= 2;
            }
            declInitList->declInits = realloc(declInitList->declInits, sizeof(DeclInit *) * declInitList->declInitListCapacity);
            if (declInitList->declInits == NULL)
            {
                abort();
            }
        }
    }
    else
    {
        declInitList->declInitListSize = declInitListSize;
        declInitList->declInits = malloc(sizeof(DeclInit *) * declInitListSize);
        if (declInitList->declInits == NULL)
        {
            abort();
        }
        declInitList->declInitListCapacity = declInitListSize;
    }
}

// Adds a declaration initializer to a declaration list
void declInitListPush(DeclInitList *declInitList, DeclInit *declInit)
{
    declInitListResize(declInitList, declInitList->declInitListSize + 1);
    declInitList->declInits[declInitList->declInitListSize - 1] = declInit;
}

// Constructor for struct declaration
StructDecl *structDeclCreate(void)
{
    StructDecl *structDecl = malloc(sizeof(StructDecl));
    if (structDecl == NULL)
    {
        abort();
    }
    structDecl->bitField = NULL;
    return structDecl;
}

// Destructor for struct declaration
void structDeclDestroy(StructDecl *structDecl)
{
    typeSpecListDestroy(structDecl->typeSpecList);

    if (structDecl->declarator != NULL)
    {
        declaratorDestroy(structDecl->declarator);
    }
    if (structDecl->bitField != NULL)
    {
        exprDestroy(structDecl->bitField);
    }
    free(structDecl);
}

// Constructor for struct declaration list
StructDeclList *structDeclListCreate(size_t structDeclListSize)
{
    StructDeclList *structDeclList = malloc(sizeof(StructDeclList));
    if (structDeclList == NULL)
    {
        abort();
    }

    if (structDeclListSize != 0)
    {
        structDeclList->structDecls = malloc(sizeof(StructDecl *) * structDeclListSize);
        if (structDeclList->structDecls == NULL)
        {
            abort();
        }
    }
    else
    {
        structDeclList->structDecls = NULL;
    }

    structDeclList->structDeclListSize = structDeclListSize;
    structDeclList->structDeclListCapacity = structDeclListSize;
    return structDeclList;
}

// Destructor for struct declaration list
void structDeclListDestroy(StructDeclList *structDeclList)
{
    for (size_t i = 0; i < structDeclList->structDeclListSize; i++)
    {
        structDeclDestroy(structDeclList->structDecls[i]);
    }
    free(structDeclList->structDecls);
    free(structDeclList);
}

// Resize the array inside struct specifier types
void structDeclListResize(StructDeclList *structDeclList, const size_t structDeclListSize)
{
    if (structDeclList->structDeclListSize != 0)
    {
        structDeclList->structDeclListSize = structDeclListSize;
        if (structDeclList->structDeclListSize > structDeclList->structDeclListCapacity)
        {
            while (structDeclList->structDeclListSize > structDeclList->structDeclListCapacity)
            {
                structDeclList->structDeclListCapacity *= 2;
            }
            structDeclList->structDecls = realloc(structDeclList->structDecls, sizeof(StructDecl *) * structDeclList->structDeclListCapacity);
            if (structDeclList->structDecls == NULL)
            {
                abort();
            }
        }
    }
    else
    {
        structDeclList->structDeclListSize = structDeclListSize;
        structDeclList->structDecls = malloc(sizeof(StructDecl *) * structDeclListSize);
        if (structDeclList->structDecls == NULL)
        {
            abort();
        }
        structDeclList->structDeclListCapacity = structDeclListSize;
    }
}

// Adds a declaration to a struct declaration list
void structDeclListPush(StructDeclList *structDeclList, StructDecl *structDecl)
{
    structDeclListResize(structDeclList, structDeclList->structDeclListSize + 1);
    structDeclList->structDecls[structDeclList->structDeclListSize - 1] = structDecl;
}

// Constructor for struct specifier
StructSpecifier *structSpecifierCreate(void)
{
    StructSpecifier *structSpec = malloc(sizeof(StructSpecifier));
    if (structSpec == NULL)
    {
        abort();
    }
    return structSpec;
}

// Destructor for struct specifier
void structSpecifierDestroy(StructSpecifier *structSpec)
{
    if (structSpec->ident != NULL)
    {
        free(structSpec->ident);
    }
    structDeclListDestroy(structSpec->structDeclList);
    free(structSpec);
}

// Constructor for type specifiers
TypeSpecifier *typeSpecifierCreate(bool isStruct)
{
    TypeSpecifier *typeSpecifier = malloc(sizeof(TypeSpecifier));
    if (typeSpecifier == NULL)
    {
        abort();
    }
    typeSpecifier->isStruct = isStruct;
    return typeSpecifier;
}

// Destructor for type specifiers
void typeSpecifierDestroy(TypeSpecifier *typeSpecifier)
{
    if (typeSpecifier->isStruct == true)
    {
        structSpecifierDestroy(typeSpecifier->structSpecifier);
    }
    free(typeSpecifier);
}

// Constructor for function definition
FuncDef *funcDefCreate(TypeSpecList *retType, size_t ptrCount, char *ident, Stmt *body)
{
    FuncDef *funcDef = malloc(sizeof(FuncDef));
    if (funcDef == NULL)
    {
        abort();
    }
    funcDef->retType = retType;
    funcDef->ptrCount = ptrCount;
    funcDef->ident = ident;
    funcDef->args = NULL;
    funcDef->body = body;
    return funcDef;
}

// Destructor for function definition
void funcDefDestroy(FuncDef *funcDef)
{
    typeSpecListDestroy(funcDef->retType);
    free(funcDef->ident);
    if (funcDef->args != NULL)
    {
        declarationListDestroy(funcDef->args);
    }
    stmtDestroy(funcDef->body);
    free(funcDef);
}

// constructor for initializer list
InitList *initListCreate(size_t initListSize)
{
    InitList *initList = malloc(sizeof(InitList));
    if (initList == NULL)
    {
        abort();
    }

    if(initListSize != 0)
    {
        initList->inits = malloc(sizeof(Expr *) * initListSize);
        if (initList->inits == NULL)
        {
            abort();
        }
    }
    else {
        initList->inits = NULL;
    }

    initList->size = initListSize;
    initList->size = initListSize;
    return initList;
}


// Destructor for initialiser list
void initListDestroy(InitList *initList)
{
    for(size_t i = 0; i < initList->size; i++)
    {
        initDestroy(initList->inits[i]);
    }
    free(initList->inits);
    free(initList);
}

// Resize the initialiser list
void initListResize(InitList *initList, const size_t initListSize)
{
    if (initList->size != 0)
    {
        initList->size = initListSize;
        if (initList->size > initList->capacity)
        {
            while (initList->size > initList->capacity)
            {
                initList->capacity *= 2;
            }
            initList->inits = realloc(initList->inits, sizeof(Expr *) * initList->capacity);
            if (initList->inits == NULL)
            {
                abort();
            }
        }
    }
    else
    {
        initList->size = initListSize;
        initList->inits = malloc(sizeof(Expr *) * initListSize);
        if (initList->inits == NULL)
        {
            abort();
        }
        initList->capacity = initListSize;
    }
}

// Adds an expression to an initializer list
void initListPush(InitList *initList, Initializer *init)
{
    initListResize(initList, initList->size + 1);
    initList->inits[initList->size - 1] = init;
}

// constructor for an initializer
Initializer *initCreate()
{
    Initializer *initializer = malloc(sizeof(Initializer));
    if (initializer == NULL)
    {
        abort();
    }
    initializer->initList = NULL;
    initializer->expr = NULL;
    return initializer;
}

// destructor for an initializer
void initDestroy(Initializer *init)
{
    if(init->initList != NULL)
    {
        initListDestroy(init->initList);
    }
    if(init->expr != NULL) {
        exprDestroy(init->expr);
    }
    free(init);
}








