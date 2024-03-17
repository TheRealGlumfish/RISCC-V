#include <stddef.h>
#include <stdlib.h>

#include "ast.h"
#include "symbol.h"
#include <stdio.h>
#include <string.h>

// Constructor for symbol entry
SymbolEntry *symbolEntryCreate(char *ident, TypeSpecifier type, size_t size, bool isFunc)
{
    SymbolEntry *symbolEntry = malloc(sizeof(SymbolEntry));
    if (symbolEntry == NULL)
    {
        abort();
    }

    symbolEntry->ident = ident;
    symbolEntry->type = type;
    symbolEntry->size = size;
    symbolEntry->isFunc = isFunc;
    return symbolEntry;
}

// Destructor for symbol entry
void symbolEntryDestroy(SymbolEntry *symbolEntry)
{
    free(symbolEntry);
}

// Create symbol table
SymbolTable *symbolTableCreate(size_t symbolTableSize, SymbolTable *parentTable)
{
    SymbolTable *symbolTable = malloc(sizeof(SymbolTable));
    if (symbolTable == NULL)
    {
        abort();
    }
    if (symbolTableSize != 0)
    {
        symbolTable->entries = malloc(sizeof(SymbolEntry) * symbolTableSize);
        if (symbolTable->entries == NULL)
        {
            abort();
        }
    }
    else
    {
        symbolTable->entries = NULL;
    }

    symbolTable->currFrameOffset = 0;
    symbolTable->size = symbolTableSize;
    symbolTable->capacity = symbolTableSize;
    symbolTable->parentTable = parentTable;
    symbolTable->childTable = NULL;
    return symbolTable;
}

// Resizes a symbol table
void symbolTableResize(SymbolTable *symbolTable, const size_t symbolTableSize)
{
    if (symbolTable->size != 0)
    {
        symbolTable->size = symbolTableSize;
        if (symbolTable->size > symbolTable->capacity)
        {
            while (symbolTable->size > symbolTable->capacity)
            {
                symbolTable->capacity *= 2;
            }
            symbolTable->entries = realloc(symbolTable->entries, sizeof(SymbolEntry *) * symbolTable->capacity);
            if (symbolTable->entries == NULL)
            {
                abort();
            }
        }
    }
    else
    {
        symbolTable->size = symbolTableSize;
        symbolTable->entries = malloc(sizeof(SymbolEntry *) * symbolTableSize);
        if (symbolTable->entries == NULL)
        {
            abort();
        }
        symbolTable->capacity = symbolTableSize;
    }
}

// Adds a symbol table entry to a symbol table
void symbolTablePush(SymbolTable *symbolTable, SymbolEntry *symbolEntry)
{
    symbolTableResize(symbolTable, symbolTable->size + 1);
    symbolTable->entries[symbolTable->size - 1] = symbolEntry;
    symbolEntry->stackOffset = symbolTable->currFrameOffset;
    symbolTable->currFrameOffset += symbolEntry->size;
}

// Symbol table destructor
void symbolTableDestroy(SymbolTable *symbolTable)
{
    for (size_t i = 0; i < symbolTable->size; i++)
    {
        symbolEntryDestroy(symbolTable->entries[i]);
    }
    free(symbolTable->entries);
    if (symbolTable->childTable != NULL)
    {
        symbolTableDestroy(symbolTable->childTable);
    }
    free(symbolTable);
}

// overlapping names of variables and functions !?!?
SymbolEntry *getSymbolEntry(SymbolTable *symbolTable, char *ident)
{
    // base case
    if (symbolTable->parentTable == NULL)
    {
        return NULL;
    }
    // search current table
    for (size_t i = 0; i < symbolTable->size; i++)
    {
        if (strcmp(symbolTable->entries[i]->ident, ident) == 0)
        {
            return symbolTable->entries[i];
        }
    }
    // search further tables
    return getSymbolEntry(symbolTable->parentTable, ident);
}

void scanStmt(Stmt *stmt, SymbolTable *parentTable);
void scanExpr(Expr *expr, SymbolTable *parentTable);

void scanFuncExpr(FuncExpr *funcExpr, SymbolTable *parentTable)
{
    funcExpr->symbolEntry = getSymbolEntry(parentTable, funcExpr->ident);
}

void scanAssignment(AssignExpr *assignExpr, SymbolTable *parentTable)
{
    scanExpr(assignExpr->lvalue, parentTable);
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
        break;
    }
    case ASSIGN_EXPR:
    {
        scanAssignment(expr->assignment, parentTable);
        break;
    }
    case FUNC_EXPR:
    {
        scanFuncExpr(expr->function, parentTable);
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
    scanStmt(ifStmt->falseBody, parentTable);
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

void scanCompoundStmt(CompoundStmt *compoundStmt, SymbolTable *parentTable)
{
    // enter a new scope
    SymbolTable *childTable = symbolTableCreate(0, parentTable);
    parentTable->childTable = childTable;

    for (size_t i = 0; i < compoundStmt->stmtList.size; i++)
    {
        scanStmt(compoundStmt->stmtList.stmts[i], childTable);
    }

    for (size_t i = 0; i < compoundStmt->declList.size; i++)
    {
        char *ident = compoundStmt->declList.decls[i]->declInit->declarator->ident;
        TypeSpecifier type = *(compoundStmt->declList.decls[i]->typeSpecList->typeSpecs[0]); // assumes a list of length 1 after type resolution stuff
        size_t size = 8;                                                                     // everything has default 64 bit size at the moment
        SymbolEntry *symbolEntry = symbolEntryCreate(ident, type, size, false);
        symbolTablePush(childTable, symbolEntry);
        compoundStmt->declList.decls[i]->symbolEntry = symbolEntry;
    }
}

void scanLabelStmt(LabelStmt *labelStmt, SymbolTable *parentTable)
{
    scanExpr(labelStmt->caseLabel, parentTable);
    scanStmt(labelStmt->body, parentTable);
}

void scanJumpStmt(JumpStmt *jumpStmt, SymbolTable *parentTable)
{
    scanExpr(jumpStmt->expr, parentTable);
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
    symbolTablePush(parentTable, symbolEntryCreate(funcDef->ident, *(funcDef->retType->typeSpecs[0]), 32, true));

    for (size_t i = 0; i < funcDef->args.size; i++)
    {
        char *ident = funcDef->args.decls[i]->declInit->declarator->ident;
        TypeSpecifier type = *(funcDef->args.decls[i]->typeSpecList->typeSpecs[0]); // assumes a list of length 1 after type resolution stuff
        size_t size = 8;                                                            // everything has default 64 bit size at the moment
        SymbolEntry *symbolEntry = symbolEntryCreate(ident, type, size, false);
        symbolTablePush(parentTable, symbolEntry);
        funcDef->args.decls[i]->symbolEntry = symbolEntry;
    }

    scanStmt(funcDef->body, parentTable);
}

SymbolTable *populateSymbolTable(FuncDef *rootExpr)
{
    SymbolTable *globalTable = symbolTableCreate(0, NULL); // global scop
    scanFuncDef(rootExpr, globalTable);
    return globalTable;
}
