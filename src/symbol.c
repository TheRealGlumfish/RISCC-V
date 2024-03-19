#include <stddef.h>
#include <stdlib.h>

#include "ast.h"
#include "symbol.h"
#include <stdio.h>
#include <string.h>

int whileCount = 0;

// constructor for symbol entry
SymbolEntry *symbolEntryCreate(char *ident, TypeSpecifier type, size_t size, bool isFunc)
{
    SymbolEntry *symbolEntry = malloc(sizeof(SymbolEntry));
    if (symbolEntry == NULL)
    {
        abort();
    }

    symbolEntry->ident = ident;
    symbolEntry->type = type;

    if (isFunc)
    {
        symbolEntry->size = size + (4 * (2 + 11 + 7)) + (8 * (12)); // space allocated for ra and fp and s1-s11 and t0-t6 and ft0-ft11
    }
    else
    {
        symbolEntry->size = size;
    }

    symbolEntry->isFunc = isFunc;
    return symbolEntry;
}

// destructor for symbol entry
void symbolEntryDestroy(SymbolEntry *symbolEntry)
{
    free(symbolEntry);
}

// create symbol table
SymbolTable *symbolTableCreate(size_t entryLength, size_t childrenLength, SymbolTable *parentTable, SymbolEntry *masterFunc)
{
    SymbolTable *symbolTable = malloc(sizeof(SymbolTable));
    if (symbolTable == NULL)
    {
        abort();
    }

    // entry list constructor
    if (entryLength != 0)
    {
        symbolTable->entries = malloc(sizeof(SymbolEntry) * entryLength);
        if (symbolTable->entries == NULL)
        {
            abort();
        }
    }
    else
    {
        symbolTable->entries = NULL;
    }

    // child table list constructor
    if (childrenLength != 0)
    {
        symbolTable->childrenTables = malloc(sizeof(SymbolTable) * childrenLength);
        if (symbolTable->childrenTables == NULL)
        {
            abort();
        }
    }
    else
    {
        symbolTable->childrenTables = NULL;
    }

    symbolTable->parentTable = parentTable;
    symbolTable->masterFunc = masterFunc; // NULL for global scope

    symbolTable->entrySize = entryLength;
    symbolTable->entryCapacity = entryLength;

    symbolTable->childrenSize = childrenLength;
    symbolTable->chldrenCapacity = childrenLength;
    return symbolTable;
}

// resize the list of entries in a table
void entryListResize(SymbolTable *symbolTable, const size_t entryLength)
{
    if (symbolTable->entrySize != 0)
    {
        symbolTable->entrySize = entryLength;
        if (symbolTable->entrySize > symbolTable->entryCapacity)
        {
            while (symbolTable->entrySize > symbolTable->entryCapacity)
            {
                symbolTable->entryCapacity *= 2;
            }
            symbolTable->entries = realloc(symbolTable->entries, sizeof(SymbolEntry *) * symbolTable->entryCapacity);
            if (symbolTable->entries == NULL)
            {
                abort();
            }
        }
    }
    else
    {
        symbolTable->entrySize = entryLength;
        symbolTable->entries = malloc(sizeof(SymbolEntry *) * entryLength);
        if (symbolTable->entries == NULL)
        {
            abort();
        }
        symbolTable->entryCapacity = entryLength;
    }
}

// resize the list of children in a symbol table
void childrenListResize(SymbolTable *symbolTable, const size_t childrenLength)
{
    if (symbolTable->childrenSize != 0)
    {
        symbolTable->childrenSize = childrenLength;
        if (symbolTable->childrenSize > symbolTable->chldrenCapacity)
        {
            while (symbolTable->childrenSize > symbolTable->chldrenCapacity)
            {
                symbolTable->chldrenCapacity *= 2;
            }
            symbolTable->childrenTables = realloc(symbolTable->childrenTables, sizeof(SymbolTable *) * symbolTable->chldrenCapacity);
            if (symbolTable->childrenTables == NULL)
            {
                abort();
            }
        }
    }
    else
    {
        symbolTable->childrenSize = childrenLength;
        symbolTable->childrenTables = malloc(sizeof(SymbolTable *) * childrenLength);
        if (symbolTable->childrenTables == NULL)
        {
            abort();
        }
        symbolTable->chldrenCapacity = childrenLength;
    }
}

// Adds a symbol table entry to a symbol table
void entryPush(SymbolTable *symbolTable, SymbolEntry *symbolEntry)
{
    entryListResize(symbolTable, symbolTable->entrySize + 1);
    symbolTable->entries[symbolTable->entrySize - 1] = symbolEntry;

    // only update stack values of local decls, global is not on the stack
    if (symbolTable->masterFunc != NULL)
    {
        // first element stored at sp + 0
        symbolTable->masterFunc->size += symbolEntry->size;
        symbolEntry->stackOffset = symbolTable->masterFunc->size;
    }
}

// Adds a child symbol table to a symbol table
void childTablePush(SymbolTable *symbolTable, SymbolTable *childTable)
{
    childrenListResize(symbolTable, symbolTable->childrenSize + 1);
    symbolTable->childrenTables[symbolTable->childrenSize - 1] = childTable;
}

// destructor for symbol table
void symbolTableDestroy(SymbolTable *symbolTable)
{
    for (size_t i = 0; i < symbolTable->entrySize; i++)
    {
        symbolEntryDestroy(symbolTable->entries[i]);
    }
    free(symbolTable->entries);

    if (symbolTable->childrenTables != NULL)
    {
        for (size_t i = 0; i < symbolTable->childrenSize; i++)
        {
            symbolTableDestroy(symbolTable->childrenTables[i]);
        }
    }
    free(symbolTable->childrenTables);

    free(symbolTable);
}

// overlapping names of variables and functions !?!?
SymbolEntry *getSymbolEntry(SymbolTable *symbolTable, char *ident)
{
    // base case
    if (symbolTable == NULL)
    {
        return NULL;
    }
    // search current table
    for (size_t i = 0; i < symbolTable->entrySize; i++)
    {
        if (strcmp(symbolTable->entries[i]->ident, ident) == 0)
        {
            return symbolTable->entries[i];
        }
    }
    // search further tables
    return getSymbolEntry(symbolTable->parentTable, ident);
}

void displaySymbolEntry(SymbolEntry *symbolEntry)
{
    char *type;
    switch (symbolEntry->type.dataType)
    {
    case INT_TYPE:
    {
        type = "INT";
        break;
    }
    case FLOAT_TYPE:
    {
        type = "FLOAT";
        break;
    }
    case CHAR_TYPE:
    {
        type = "CHAR";
        break;
    }
    case LONG_TYPE:
    {
        type = "LONG";
        break;
    }
    default:
    {
        type = "NULL";
    }
    }
    printf("0x%x | %s | %zu | %zu | %s\n", symbolEntry, symbolEntry->ident, symbolEntry->stackOffset, symbolEntry->size, type);
}

void displaySymbolTable(SymbolTable *symbolTable)
{
    if (symbolTable->masterFunc == NULL)
    {
        printf("GLOBAL SCOPE\n");
    }
    else
    {
        printf("%s SCOPE\n", symbolTable->masterFunc->ident);
    }
    printf("===============================\n");

    for (size_t i = 0; i < symbolTable->entrySize; i++)
    {
        displaySymbolEntry(symbolTable->entries[i]);
    }
    printf("\n");

    if (symbolTable->childrenTables != NULL)
    {
        for (size_t i = 0; i < symbolTable->childrenSize; i++)
        {
            displaySymbolTable(symbolTable->childrenTables[i]);
        }
    }
}

void scanStmt(Stmt *stmt, SymbolTable *parentTable);
void scanExpr(Expr *expr, SymbolTable *parentTable);

void scanFuncExpr(FuncExpr *funcExpr, SymbolTable *parentTable)
{
    funcExpr->symbolEntry = getSymbolEntry(parentTable, funcExpr->ident);
    for (size_t i = 0; i < funcExpr->argsSize; i++)
    {
        scanExpr(funcExpr->args[i], parentTable);
    }
}

void scanAssignment(AssignExpr *assignExpr, SymbolTable *parentTable)
{
    assignExpr->symbolEntry = getSymbolEntry(parentTable, assignExpr->ident);

    if (assignExpr->lvalue != NULL)
    {
        scanExpr(assignExpr->lvalue, parentTable);
    }
    scanExpr(assignExpr->op, parentTable);
}

void scanVariable(VariableExpr *variable, SymbolTable *parentTable)
{
    variable->symbolEntry = getSymbolEntry(parentTable, variable->ident);
}

void scanOperationExpr(OperationExpr *opExpr, SymbolTable *parentTable)
{
    scanExpr(opExpr->op1, parentTable);
    if (opExpr->op2 != NULL)
    {
        scanExpr(opExpr->op2, parentTable);
    }
    if (opExpr->op3 != NULL)
    {
        scanExpr(opExpr->op3, parentTable);
    }
}

void scanExpr(Expr *expr, SymbolTable *parentTable)
{
    switch (expr->type)
    {
    case VARIABLE_EXPR:
    {
        scanVariable(expr->variable, parentTable);
        expr->variable->type = expr->variable->symbolEntry->type.dataType;
        break;
    }
    case CONSTANT_EXPR:
    {
        // not scanned atm
        break;
    }
    case OPERATION_EXPR:
    {
        scanOperationExpr(expr->operation, parentTable);
        if (expr->operation->operator== SIZEOF_OP)
        {
            expr->operation->type = UNSIGNED_INT_TYPE;
        }
        else
        {
            expr->operation->type = returnType(expr->operation->op1);
        }
        break;
    }
    case ASSIGN_EXPR:
    {
        scanAssignment(expr->assignment, parentTable);
        expr->assignment->type = returnType(expr->assignment->op);
        break;
    }
    case FUNC_EXPR:
    {
        scanFuncExpr(expr->function, parentTable);
        if (expr->function->symbolEntry != NULL)
        {
            expr->function->type = expr->function->symbolEntry->type.dataType;
        }
        else
        {
            printf("NULL");
        }
        break;
    }
    }
}

void scanSwitchStmt(SwitchStmt *switchStmt, SymbolTable *parentTable)
{
    scanExpr(switchStmt->selector, parentTable);
    scanStmt(switchStmt->body, parentTable);
}

void scanIfStmt(IfStmt *ifStmt, SymbolTable *parentTable)
{
    scanExpr(ifStmt->condition, parentTable);
    scanStmt(ifStmt->trueBody, parentTable);
    if (ifStmt->falseBody != NULL)
    {
        scanStmt(ifStmt->falseBody, parentTable);
    }
}

void scanForStmt(ForStmt *forStmt, SymbolTable *parentTable)
{
    scanStmt(forStmt->init, parentTable);
    scanStmt(forStmt->condition, parentTable);
    scanStmt(forStmt->body, parentTable);
    scanExpr(forStmt->modifier, parentTable);
}

void scanWhileStmt(WhileStmt *whileStmt, SymbolTable *parentTable)
{
    scanExpr(whileStmt->condition, parentTable);
    scanStmt(whileStmt->body, parentTable);
}

// compound statements (not in function defs) create a new scope table but not a new stack frame
void scanCompoundStmt(CompoundStmt *compoundStmt, SymbolTable *parentTable)
{
    // enter a new scope but not a new stack frame
    SymbolTable *childTable = symbolTableCreate(0, 0, parentTable, parentTable->masterFunc);
    childTablePush(parentTable, childTable);

    for (size_t i = 0; i < compoundStmt->declList.size; i++)
    {
        char *ident = compoundStmt->declList.decls[i]->declInit->declarator->ident;
        TypeSpecifier type = *(compoundStmt->declList.decls[i]->typeSpecList->typeSpecs[0]); // assumes a list of length 1 after type resolution stuff
        size_t size;
        size = typeSize(type.dataType);
        SymbolEntry *symbolEntry = symbolEntryCreate(ident, type, size, false);
        entryPush(childTable, symbolEntry);
        compoundStmt->declList.decls[i]->symbolEntry = symbolEntry;

        if (compoundStmt->declList.decls[i]->declInit->initExpr != NULL)
        {
            scanExpr(compoundStmt->declList.decls[i]->declInit->initExpr, childTable);
        }
    }

    for (size_t i = 0; i < compoundStmt->stmtList.size; i++)
    {
        scanStmt(compoundStmt->stmtList.stmts[i], childTable);
    }
}

void scanLabelStmt(LabelStmt *labelStmt, SymbolTable *parentTable)
{
    scanExpr(labelStmt->caseLabel, parentTable);
    scanStmt(labelStmt->body, parentTable);
}

void scanJumpStmt(JumpStmt *jumpStmt, SymbolTable *parentTable)
{
    if (jumpStmt->expr != NULL)
    {
        scanExpr(jumpStmt->expr, parentTable);
    }
}

void scanStmt(Stmt *stmt, SymbolTable *parentTable)
{
    switch (stmt->type)
    {
    case WHILE_STMT:
        scanWhileStmt(stmt->whileStmt, parentTable);
        break;
    case FOR_STMT:
        scanForStmt(stmt->forStmt, parentTable);
        break;
    case IF_STMT:
        scanIfStmt(stmt->ifStmt, parentTable);
        break;
    case SWITCH_STMT:
        scanSwitchStmt(stmt->switchStmt, parentTable);
        break;
    case EXPR_STMT:
        scanExpr(stmt->exprStmt->expr, parentTable);
        break;
    case COMPOUND_STMT:
        scanCompoundStmt(stmt->compoundStmt, parentTable);
        break;
    case LABEL_STMT:
        scanLabelStmt(stmt->labelStmt, parentTable);
        break;
    case JUMP_STMT:
        scanJumpStmt(stmt->jumpStmt, parentTable);
        break;
    }
}

void scanFuncDef(FuncDef *funcDef, SymbolTable *parentTable)
{
    // new function def symbol entry
    SymbolEntry *funcDefEntry = symbolEntryCreate(funcDef->ident, *(funcDef->retType->typeSpecs[0]), 0, true);
    entryPush(parentTable, funcDefEntry);
    funcDef->symbolEntry = funcDefEntry;

    // new scope and new stack frame
    SymbolTable *childTable = symbolTableCreate(0, 0, parentTable, funcDefEntry);
    childTablePush(parentTable, childTable);

    if (funcDef->isParam)
    {
        // arguments added to child scope
        for (size_t i = 0; i < funcDef->args.size; i++)
        {
            if (funcDef->args.decls[i]->declInit != NULL)
            {
                char *ident = funcDef->args.decls[i]->declInit->declarator->ident;
                TypeSpecifier type = *(funcDef->args.decls[i]->typeSpecList->typeSpecs[0]); // assumes a list of length 1 after type resolution stuff

                size_t size = typeSize(type.dataType);
                SymbolEntry *symbolEntry = symbolEntryCreate(ident, type, size, false);
                entryPush(childTable, symbolEntry);
                funcDef->args.decls[i]->symbolEntry = symbolEntry;
            }
        }
    }

    if (funcDef->body != NULL)
    {
        // add body to child table
        for (size_t i = 0; i < funcDef->body->compoundStmt->declList.size; i++)
        {
            char *ident = funcDef->body->compoundStmt->declList.decls[i]->declInit->declarator->ident;
            TypeSpecifier type = *(funcDef->body->compoundStmt->declList.decls[i]->typeSpecList->typeSpecs[0]); // assumes a list of length 1 after type resolution stuff

            size_t size = typeSize(type.dataType);
            SymbolEntry *symbolEntry = symbolEntryCreate(ident, type, size, false);
            entryPush(childTable, symbolEntry);
            funcDef->body->compoundStmt->declList.decls[i]->symbolEntry = symbolEntry;

            if (funcDef->body->compoundStmt->declList.decls[i]->declInit->initExpr != NULL)
            {
                scanExpr(funcDef->body->compoundStmt->declList.decls[i]->declInit->initExpr, childTable);
            }
        }

        for (size_t i = 0; i < funcDef->body->compoundStmt->stmtList.size; i++)
        {
            scanStmt(funcDef->body->compoundStmt->stmtList.stmts[i], childTable);
        }
    }
}

void scanTransUnit(TranslationUnit *transUnit, SymbolTable *parentTable)
{
    for (size_t i = 0; i < transUnit->size; i++)
    {
        if (transUnit->externDecls[i]->isFunc)
        {
            scanFuncDef(transUnit->externDecls[i]->funcDef, parentTable);
        }
        else
        {
            for (size_t i = 0; i < transUnit->externDecls[i]->declList.size; i++)
            {
                char *ident = transUnit->externDecls[i]->declList.decls[i]->declInit->declarator->ident;
                TypeSpecifier type = *(transUnit->externDecls[i]->declList.decls[i]->typeSpecList->typeSpecs[0]); // assumes a list of length 1 after type resolution stuff
                size_t size = typeSize(type.dataType);
                SymbolEntry *symbolEntry = symbolEntryCreate(ident, type, size, false);
                entryPush(parentTable, symbolEntry);
                transUnit->externDecls[i]->declList.decls[i]->symbolEntry = symbolEntry;
            }
        }
    }
}

SymbolTable *populateSymbolTable(TranslationUnit *rootExpr)
{
    SymbolTable *globalTable = symbolTableCreate(0, 0, NULL, NULL); // global scope
    scanTransUnit(rootExpr, globalTable);
    return globalTable;
}

size_t typeSize(DataType type)
{
    switch (type)
    {
    case VOID_TYPE:
        return 4;
    case CHAR_TYPE:
        return 1;
    case SIGNED_CHAR_TYPE:
        return 1;
    case SHORT_TYPE:
        return 2;
    case UNSIGNED_SHORT_TYPE:
        return 2;
    case INT_TYPE:
        return 4;
    case UNSIGNED_INT_TYPE:
        return 4;
    case LONG_TYPE:
        return 8;
    case UNSIGNED_LONG_TYPE:
        return 8;
    case FLOAT_TYPE:
        return 4;
    case DOUBLE_TYPE:
        return 8;
    default:
    {
        fprintf(stderr, "Type does not have a size, exiting...\n");
        exit(EXIT_FAILURE);
    }
    }
}
