#ifndef SYMBOL_H
#define SYMBOL_H

#include "ast.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct SymbolEntry
{
    char *ident;
    TypeSpecifier type;
    size_t stackOffset;
    size_t size;
    bool isFunc;
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

SymbolEntry *symbolEntryCreate(char *ident, TypeSpecifier type, size_t size, bool isFunc);
void symbolEntryDestroy(SymbolEntry *symbolEntry);

SymbolTable *symbolTableCreate(size_t entryLength, size_t childrenLength, SymbolTable *parentTable, SymbolEntry *masterFunc);
void entryListResize(SymbolTable *symbolTable, size_t symbolTableSize);
void childrenListResize(SymbolTable *symbolTable, size_t childrenLength);

void displaySymbolTable(SymbolTable *symbolTable);
void displaySymbolEntry(SymbolEntry *symbolEntry);

void entryPush(SymbolTable *symbolTable, SymbolEntry *symbolEntry);
void symbolTableDestroy(SymbolTable *symbolTable);
void childTablePush(SymbolTable *symbolTable, SymbolTable *childTable);

SymbolEntry *getSymbolEntry(SymbolTable *symbolTable, char *ident);

SymbolTable *populateSymbolTable(TranslationUnit *rootExpr);

#endif
