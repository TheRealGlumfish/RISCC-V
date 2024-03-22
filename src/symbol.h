#ifndef SYMBOL_H
#define SYMBOL_H

#include "ast.h"
#include <stdbool.h>
#include <stddef.h>

typedef enum EntryType
{
    FUNCTION_ENTRY,
    VARIABLE_ENTRY,
    WHILE_ENTRY,
    SWITCH_ENTRY,
    FOR_ENTRY,
    ARRAY_ENTRY
} EntryType;

typedef struct SymbolEntry
{
    char *ident;
    TypeSpecifier type;
    size_t stackOffset;
    size_t typeSize;
    size_t storageSize;
    size_t isGlobal;
    EntryType entryType;
} SymbolEntry;

typedef struct SymbolTable SymbolTable;

typedef struct SymbolTable
{
    SymbolTable *parentTable;
    SymbolEntry *masterFunc;

    SymbolEntry **entries;
    size_t entrySize;
    size_t entryCapacity;

    SymbolTable **childrenTables;
    size_t childrenSize;
    size_t chldrenCapacity;
} SymbolTable;

SymbolEntry *symbolEntryCreate(char *ident, size_t storageSize, size_t typeSize, EntryType entryType);
void symbolEntryDestroy(SymbolEntry *symbolEntry);

SymbolTable *symbolTableCreate(size_t entryLength, size_t childrenLength, SymbolTable *parentTable, SymbolEntry *masterFunc);
void entryListResize(SymbolTable *symbolTable, size_t symbolTableSize);
void childrenListResize(SymbolTable *symbolTable, size_t childrenLength);

void displaySymbolTable(SymbolTable *symbolTable);
void displaySymbolEntry(SymbolEntry *symbolEntry);

size_t storageSize(DataType type);

void entryPush(SymbolTable *symbolTable, SymbolEntry *symbolEntry);
void symbolTableDestroy(SymbolTable *symbolTable);
void childTablePush(SymbolTable *symbolTable, SymbolTable *childTable);

SymbolEntry *getSymbolEntry(SymbolTable *symbolTable, char *ident, EntryType EntryType);

SymbolTable *populateSymbolTable(TranslationUnit *rootExpr);

size_t typeSize(DataType type);
int evaluateIntConstExpr(Expr *expr);

#endif
