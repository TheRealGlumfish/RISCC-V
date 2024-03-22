#include <stddef.h>
#include <stdlib.h>

#include "ast.h"
#include "symbol.h"
#include <stdio.h>
#include <string.h>

size_t whileCount = 0;
size_t switchCount = 0;
size_t forCount = 0;

// returns the value of integer expressions (only works for constant expressions)
int evaluateConstExpr(Expr *expr)
{
    switch (expr->type)
    {
    case VARIABLE_EXPR:
    {
        // not in constant expr
        break;
    }
    case CONSTANT_EXPR:
    {
        switch(expr->constant->type)
        {
            case INT_TYPE: // get working for other types
                return expr->constant->int_const;
            default:
                printf("Non-int constant expressionn evaluation not implemented\n");
                break;
        }
    }
    case OPERATION_EXPR:
    {
        int op1 = evaluateConstExpr(expr->operation->op1);
        int op2;
        int op3;
        if(expr->operation->op2 != NULL)
        {
            op2 = evaluateConstExpr(expr->operation->op2);
        }
        if(expr->operation->op3 != NULL)
        {
            op3 = evaluateConstExpr(expr->operation->op3);
        }

        switch(expr->operation->operator)
        {
            case ADD:
                return op1 + op2;
            case SUB:
                return op1 - op2;
            case MUL:
                return op1 * op2;
            case DIV:
                return op1 / op2;
            case AND:
                return op1 && op2;
            case MOD:
                return op1 % op2;
            case OR:
                return op1 || op2;
            case NOT:
                return !op1;
            case AND_BIT:
                return op1 & op2;
            case OR_BIT:
                return op1 | op2;
            case NOT_BIT:
                return ~op1;
            case XOR:
                return op1 ^ op2;
            case EQ:
                return op1 == op2;
            case NE:
                return op1 != op2;
            case LT:
                return op1 < op2;
            case GT:
                return op1 > op2;
            case LE:
                return op1 <= op2;
            case GE:
                return op1 >= op2;
            case LEFT_SHIFT:
                return op1 << op2;
            case RIGHT_SHIFT:
                return op1 >> op2;
            case TERN: // easy
                if(op1)
                {
                    return op2;
                }
                else{
                    return op3;
                }
            case SIZEOF_OP:
                printf("SizeOf Constant Expression Evaluation Not Implemented\n");
                break;
            case ADDRESS:
                // not in constant expr
                break;
            case DEREF:
                // not in constant expr
                break;
            
        }
    }
    case ASSIGN_EXPR:
    {
        // cant be in constant expression
        break;
    }
    case FUNC_EXPR:
    {
        // can't be in constant expression
        break;
    }
    }
}

// constructor for symbol entry
SymbolEntry *symbolEntryCreate(char *ident, size_t storageSize, size_t typeSize, EntryType entryType)
{
    SymbolEntry *symbolEntry = malloc(sizeof(SymbolEntry));
    if (symbolEntry == NULL)
    {
        abort();
    }
    symbolEntry->ident = ident;
    switch (entryType)
    {
    case FUNCTION_ENTRY:
        symbolEntry->storageSize = storageSize + (4 * (2 + 11 + 7)) + (8 * (12)); // space allocated for ra and fp and s1-s11 and t0-t6 and ft0-ft11
        symbolEntry->typeSize = typeSize;
        break;

    case VARIABLE_ENTRY:
        symbolEntry->storageSize = storageSize;
        symbolEntry->typeSize = typeSize;
        break;

    case WHILE_ENTRY:
        symbolEntry->storageSize = 0;
        symbolEntry->typeSize = 0;
        break;

    case ARRAY_ENTRY:
        symbolEntry->storageSize = storageSize;
        symbolEntry->typeSize = typeSize;
        break;
    
    case SWITCH_ENTRY:
        symbolEntry->storageSize = 0;
        symbolEntry->typeSize = 0;
    }
    symbolEntry->isGlobal = false;
    symbolEntry->entryType = entryType;
    return symbolEntry;
}

// destructor for symbol entry
void symbolEntryDestroy(SymbolEntry *symbolEntry)
{
    if (symbolEntry->entryType == WHILE_ENTRY || symbolEntry->entryType == SWITCH_ENTRY)
    {
        free(symbolEntry->ident);
    }
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
        symbolTable->masterFunc->storageSize += symbolEntry->storageSize;
        symbolEntry->stackOffset = symbolTable->masterFunc->storageSize;
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

// recursively searches upwards through the symbol table for a symbol
SymbolEntry *getSymbolEntry(SymbolTable *symbolTable, char *ident, EntryType entryType)
{
    // base case
    if (symbolTable == NULL)
    {
        return NULL;
    }
    // search current table
    for (size_t i = 0; i < symbolTable->entrySize; i++)
    {
        if (strcmp(symbolTable->entries[i]->ident, ident) == 0 && entryType == symbolTable->entries[i]->entryType)
        {
            return symbolTable->entries[i];
        }
    }
    // search further tables
    return getSymbolEntry(symbolTable->parentTable, ident, entryType);
}

// prints a symbol entry to the terminal
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
    case INT_PTR_TYPE:
    {
        type = "INT*";
        break; 
    }
    case CHAR_PTR_TYPE:
    {
        type = "CHAR*";
        break; 
    }
    case VOID_PTR_TYPE:
    {
        type = "VOID*";
        break; 
    }
    case FLOAT_PTR_TYPE:
    {
        type = "FLOAT*";
        break;
    }
    case DOUBLE_PTR_TYPE:
    {
        type = "DOUBLE*";
        break;
    }
    default:
    {
        type = "NULL";
    }
    }

    char *localGlobal;
    if (symbolEntry->isGlobal)
    {
        localGlobal = "GLOBAL";
    }
    else
    {
        localGlobal = "LOCAL";
    }
    printf("0x%x | %s | %zu | %zu | %zu | %s | %s\n", symbolEntry, symbolEntry->ident, symbolEntry->stackOffset, symbolEntry->typeSize, symbolEntry->storageSize, type, localGlobal);
}

// prints a symbol table and it's children to the terminal
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

void scanInitList(InitList *initList, SymbolTable *parentTable)
{
    for (size_t i = 0; i < initList->size; i++){
        if (initList->inits[i]->expr != NULL) // expression initialiser
        {
            scanExpr(initList->inits[i]->expr, parentTable);
        }
        else // init list
        {
            scanInitList(initList->inits[i]->initList, parentTable);
        }
    }
}

// declaration second pass
void scanDecl(Decl *decl, SymbolTable *symbolTable)
{
    char *ident = decl->declInit->declarator->ident;
    TypeSpecifier type = *(decl->typeSpecList->typeSpecs[0]); // assumes a list of length 1 after type flattening stuff
    SymbolEntry *symbolEntry;

    if (decl->declInit->declarator->isArray)
    {
        int arraySize = evaluateConstExpr(decl->declInit->declarator->arraySize);
        symbolEntry = symbolEntryCreate(ident, storageSize(type.dataType) * arraySize, typeSize(type.dataType), ARRAY_ENTRY);
        symbolEntry->type.dataType = addPtrToType(type.dataType); // arrays are pointers#
    }
    else
    {
        symbolEntry = symbolEntryCreate(ident, storageSize(type.dataType), typeSize(type.dataType), VARIABLE_ENTRY);
        symbolEntry->type = type;
    }

    entryPush(symbolTable, symbolEntry);
    decl->symbolEntry = symbolEntry;

    if(symbolTable->parentTable == NULL)
    {
        symbolEntry->isGlobal = true;
    }

    if (decl->declInit->initList != NULL)
    {
        scanInitList(decl->declInit->initList, symbolTable);
    }
    if(decl->declInit->initExpr != NULL)
    {
        scanExpr(decl->declInit->initExpr, symbolTable);
    }
}

// function expression second pass
void scanFuncExpr(FuncExpr *funcExpr, SymbolTable *parentTable)
{
    funcExpr->symbolEntry = getSymbolEntry(parentTable, funcExpr->ident, FUNCTION_ENTRY);
    for (size_t i = 0; i < funcExpr->argsSize; i++)
    {
        scanExpr(funcExpr->args[i], parentTable);
    }
}

// assignment second pass
void scanAssignment(AssignExpr *assignExpr, SymbolTable *parentTable)
{
    if (assignExpr->lvalue != NULL)
    {
        scanExpr(assignExpr->lvalue, parentTable);
    }
    else
    {
        assignExpr->symbolEntry = getSymbolEntry(parentTable, assignExpr->ident, VARIABLE_ENTRY);
    }
    scanExpr(assignExpr->op, parentTable);
}

// variable second pass
void scanVariable(VariableExpr *variable, SymbolTable *parentTable)
{
    printf("scan %s \n", variable->ident);
    SymbolEntry *symbolEntry = getSymbolEntry(parentTable, variable->ident, VARIABLE_ENTRY);
    // overlapping namespace :/
    if(symbolEntry == NULL)
    {
        symbolEntry = getSymbolEntry(parentTable, variable->ident, ARRAY_ENTRY);
    }
    variable->symbolEntry = symbolEntry;
}

// operation expression second pass
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

// expression second pass
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
        // doesnt require a second pass
        break;
    }
    case OPERATION_EXPR:
    {
        scanOperationExpr(expr->operation, parentTable);

        // this is botttom-up because of the recursive call previously
        Operator *operator;
        if (expr->operation->operator== SIZEOF_OP)
        {
            expr->operation->type = UNSIGNED_INT_TYPE;
        }
        //else if(expr->operation->ty)
        else if (expr->operation->operator == ADDRESS)
        {
            expr->operation->type = addPtrToType(returnType(expr->operation->op1));
        }
        else if(expr->operation->operator == DEREF)
        {
            expr->operation->type = removerPtrFromType(returnType(expr->operation->op1));
        }
        else
        {
            DataType op1Type = returnType(expr->operation->op1);
            // by default the type is just that of op1
            expr->operation->type = op1Type;
            // check for pointer types (these override the default case)
            if(isPtr(op1Type))
            {
                expr->operation->type = op1Type;
            }
            if(expr->operation->op2 != NULL)
            {
                DataType op2Type = returnType(expr->operation->op2);
                if(isPtr(op2Type))
                {
                    expr->operation->type = op2Type;
                }
            }
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
        break;
    }
    }
}

// converts an integer to a string
char *IntToStr(int integer)
{
    int strSize = snprintf(NULL, 0, "%lu", integer);
    char *string = malloc((strSize + 1) * sizeof(char));
    if (string == NULL)
    {
        abort();
    }
    sprintf(string, "%lu", integer);
    return string;
}

// switch statement second pass
void scanSwitchStmt(SwitchStmt *switchStmt, SymbolTable *parentTable)
{
    SymbolEntry *switchEntry = symbolEntryCreate(IntToStr(switchCount), 0, 0, SWITCH_ENTRY);
    entryPush(parentTable, switchEntry);
    switchStmt->symbolEntry = switchEntry;
    switchCount += 1;
    scanExpr(switchStmt->selector, parentTable);
    scanStmt(switchStmt->body, parentTable);
}

// if statement second pass
void scanIfStmt(IfStmt *ifStmt, SymbolTable *parentTable)
{
    scanExpr(ifStmt->condition, parentTable);
    scanStmt(ifStmt->trueBody, parentTable);
    if (ifStmt->falseBody != NULL)
    {
        scanStmt(ifStmt->falseBody, parentTable);
    }
}

// for statement second pass
void scanForStmt(ForStmt *forStmt, SymbolTable *parentTable)
{
    SymbolEntry *forEntry = symbolEntryCreate(IntToStr(forCount), 0, 0, FOR_ENTRY); // make identifier work
    entryPush(parentTable, forEntry);
    forStmt->symbolEntry = forEntry;
    forCount += 1;

    scanStmt(forStmt->init, parentTable);
    scanStmt(forStmt->condition, parentTable);
    scanStmt(forStmt->body, parentTable);
    scanExpr(forStmt->modifier, parentTable);
}

// while statement second pass
void scanWhileStmt(WhileStmt *whileStmt, SymbolTable *parentTable)
{
    SymbolEntry *whileEntry = symbolEntryCreate(IntToStr(whileCount), 0, 0, WHILE_ENTRY); // make identifier work
    entryPush(parentTable, whileEntry);
    whileStmt->symbolEntry = whileEntry;
    whileCount += 1;
    scanExpr(whileStmt->condition, parentTable);
    scanStmt(whileStmt->body, parentTable);
}

// compound statement second pass
void scanCompoundStmt(CompoundStmt *compoundStmt, SymbolTable *parentTable)
{
    // enter a new scope but not a new stack frame
    SymbolTable *childTable = symbolTableCreate(0, 0, parentTable, parentTable->masterFunc);
    childTablePush(parentTable, childTable);
    for (size_t i = 0; i < compoundStmt->declList.size; i++)
    {
        scanDecl(compoundStmt->declList.decls[i], childTable);
    }
    for (size_t i = 0; i < compoundStmt->stmtList.size; i++)
    {
        scanStmt(compoundStmt->stmtList.stmts[i], childTable);
    }
}

// finds closest switch
SymbolEntry *getClosestSwitch(SymbolTable *symbolTable)
{
    // base case
    if (symbolTable == NULL)
    {
        return NULL;
    }
    // search in reverse order (most recent switch)
    for (size_t i = 0; i < symbolTable->entrySize; i++)
    {
        SymbolEntry *currEntry = symbolTable->entries[symbolTable->entrySize - i - 1];
        if (currEntry->entryType == SWITCH_ENTRY)
        {
            return symbolTable->entries[symbolTable->entrySize - i - 1];
        }
    }
    // search further tables
    return getClosestSwitch(symbolTable->parentTable);
}

// label second pass
void scanLabelStmt(LabelStmt *labelStmt, SymbolTable *parentTable)
{
    labelStmt->symbolEntry = getClosestSwitch(parentTable);
    if(labelStmt->caseLabel!= NULL)
    {
        scanExpr(labelStmt->caseLabel, parentTable);
    }
    scanStmt(labelStmt->body, parentTable);
}

// finds closest while/for/switch (closest scope)
SymbolEntry *getClosestBreakable(SymbolTable *symbolTable)
{
    // base case
    if (symbolTable == NULL)
    {
        return NULL;
    }
    // search in reverse order (most recent breakable)
    for (size_t i = 0; i < symbolTable->entrySize; i++)
    {
        SymbolEntry *currEntry = symbolTable->entries[symbolTable->entrySize - i - 1];
        if (currEntry->entryType == WHILE_ENTRY || currEntry->entryType == SWITCH_ENTRY || currEntry->entryType == FOR_ENTRY)
        {
            return symbolTable->entries[symbolTable->entrySize - i - 1];
        }
    }
    // search further tables
    return getClosestBreakable(symbolTable->parentTable);
}

// jump statement second pass
void scanJumpStmt(JumpStmt *jumpStmt, SymbolTable *parentTable)
{
    jumpStmt->symbolEntry = getClosestBreakable(parentTable->parentTable);
    if (jumpStmt->expr != NULL)
    {
        scanExpr(jumpStmt->expr, parentTable);
    }
}

// statement second pass
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

// function definition second pass
void scanFuncDef(FuncDef *funcDef, SymbolTable *parentTable)
{
    // new function def symbol entry
    SymbolEntry *funcDefEntry = symbolEntryCreate(funcDef->ident, 0, 0, FUNCTION_ENTRY);
    funcDefEntry->type = *(funcDef->retType->typeSpecs[0]);
    funcDefEntry->isGlobal = true;
    entryPush(parentTable, funcDefEntry);
    funcDef->symbolEntry = funcDefEntry;

    // new scope and new stack frame
    SymbolTable *childTable = symbolTableCreate(0, 0, parentTable, funcDefEntry);
    childTablePush(parentTable, childTable);

    // second pass of arguments
    if (funcDef->isParam)
    {   
        for (size_t i = 0; i < funcDef->args.size; i++)
        {
            if (funcDef->args.decls[i]->declInit != NULL)
            {
                scanDecl(funcDef->args.decls[i], childTable);
            }
        }
    }

    // second pass of function body
    if (funcDef->body != NULL)
    {    
        for (size_t i = 0; i < funcDef->body->compoundStmt->declList.size; i++)
        {
            scanDecl(funcDef->body->compoundStmt->declList.decls[i], childTable);
        }
        for (size_t i = 0; i < funcDef->body->compoundStmt->stmtList.size; i++)
        {
            scanStmt(funcDef->body->compoundStmt->stmtList.stmts[i], childTable);
        }
    }
}

// translation unit second pass
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
            scanDecl(transUnit->externDecls[i]->decl, parentTable);
        }
    }
}

// starts the second pass
SymbolTable *populateSymbolTable(TranslationUnit *rootExpr)
{
    SymbolTable *globalTable = symbolTableCreate(0, 0, NULL, NULL); // global scope
    scanTransUnit(rootExpr, globalTable);
    return globalTable;
}

// returns the size of a given datatype
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
    case INT_PTR_TYPE:
        return 4;
    case CHAR_PTR_TYPE:
        return 4;
    case VOID_PTR_TYPE:
        return 4;
    case FLOAT_PTR_TYPE:
        return 4;
    case DOUBLE_PTR_TYPE:
        return 4;

    default:
    {
        fprintf(stderr, "Type: %i does not have a size, exiting...\n", type);
        exit(EXIT_FAILURE);
    }
    }
}

// returns the memory required for a given datatype (different to type size)
size_t storageSize(DataType type)
{
    switch (type)
    {
    case VOID_TYPE:
        return 4;
    case CHAR_TYPE:
        return 4;
    case SIGNED_CHAR_TYPE:
        return 4;
    case SHORT_TYPE:
        return 4;
    case UNSIGNED_SHORT_TYPE:
        return 4;
    case INT_TYPE:
        return 4;
    case UNSIGNED_INT_TYPE:
        return 4;
    case LONG_TYPE:
        return 8;
    case UNSIGNED_LONG_TYPE:
        return 8;
    case FLOAT_TYPE:
        return 8;
    case DOUBLE_TYPE:
        return 8;
    case INT_PTR_TYPE:
        return 4;
    case CHAR_PTR_TYPE:
        return 4;
    case VOID_PTR_TYPE:
        return 4;
    case FLOAT_PTR_TYPE:
        return 4;
    case DOUBLE_PTR_TYPE:
        return 4;

    default:
    {
        fprintf(stderr, "Type: %i does not have a storage size, exiting...\n", type);
        exit(EXIT_FAILURE);
    }
    }
}

