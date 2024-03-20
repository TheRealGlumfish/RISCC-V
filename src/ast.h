#ifndef AST_H
#define AST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum
{
    ADD,
    SUB,
    MUL,
    DIV,
    AND,
    MOD,
    OR,
    NOT,
    AND_BIT,
    OR_BIT,
    NOT_BIT,
    XOR,
    EQ,
    NE,
    LT,
    GT,
    LE,
    GE,
    LEFT_SHIFT,
    RIGHT_SHIFT,
    DEC_POST,
    INC_POST,
    DEC,
    INC,
    TERN,
    COMMA_OP,
    SIZEOF_OP,
    ADDRESS,
    DEREF
} Operator; // TODO: Add full list of operators

typedef enum
{
    VOID_TYPE,           // 4 bytes
    CHAR_TYPE,           // 1 byte
    SIGNED_CHAR_TYPE,    // 1 byte
    SHORT_TYPE,          // 2 bytes
    INT_TYPE,            // 4 bytes
    LONG_TYPE,           // 8 bytes
    UNSIGNED_SHORT_TYPE, // 2 bytes
    UNSIGNED_INT_TYPE,   // 4 bytes
    UNSIGNED_LONG_TYPE,  // 8 bytes
    FLOAT_TYPE,          // 4 bytes
    DOUBLE_TYPE,         // 8 bytes
    SIGNED_TYPE,         // only used in parsing
    UNSIGNED_TYPE,       // only used in parsing
    INT_POINTER_TYPE,    // 4 bytes
    CHAR_POINTER_TYPE,   // 4 bytes
    VOID_POINTER_TYPE    // 4 bytes
} DataType;              // TODO: Add full list of types and deal with unsigned and void

typedef enum
{
    VARIABLE_EXPR,
    CONSTANT_EXPR,
    OPERATION_EXPR,
    ASSIGN_EXPR,
    FUNC_EXPR
} ExprType;

typedef enum
{
    WHILE_STMT,
    FOR_STMT,
    IF_STMT,
    SWITCH_STMT,
    EXPR_STMT,
    COMPOUND_STMT,
    LABEL_STMT,
    JUMP_STMT
} StmtType;

typedef enum
{
    GOTO_JUMP,
    CONTINUE_JUMP,
    BREAK_JUMP,
    RETURN_JUMP
} JumpType;

typedef struct Expr
{
    ExprType type;
    union
    {
        struct VariableExpr *variable;
        struct ConstantExpr *constant;
        struct OperationExpr *operation;
        struct AssignExpr *assignment;
        struct FuncExpr *function;
    };
} Expr;

typedef struct SymbolEntry SymbolEntry;

typedef struct VariableExpr
{
    char *ident;
    DataType type;
    SymbolEntry *symbolEntry;
} VariableExpr;

typedef struct ConstantExpr
{
    union
    {
        int32_t int_const;
        float float_const;
        char char_const;
        char *string_const;
    };
    DataType type;
    bool isString;
} ConstantExpr;

typedef struct OperationExpr
{
    Expr *op1;
    Expr *op2;
    Expr *op3;
    Operator operator;
    DataType type;
} OperationExpr;

typedef struct AssignExpr
{
    char *ident;
    SymbolEntry *symbolEntry;
    Expr *op;
    Expr *lvalue;
    Operator operator; // Info: when set to NOT, regular assignment
    DataType type;     // TODO: Replace, could accept compound types
} AssignExpr;

typedef struct FuncExpr
{
    char *ident;
    size_t argsSize;
    size_t argsCapacity;
    Expr **args;
    DataType type; // TODO: Replace, functions may not retrun primative types only
    SymbolEntry *symbolEntry;
} FuncExpr;

typedef struct Stmt
{
    StmtType type;
    union
    {
        struct WhileStmt *whileStmt;
        struct ForStmt *forStmt;
        struct IfStmt *ifStmt;
        struct SwitchStmt *switchStmt;
        struct ExprStmt *exprStmt;
        struct CompoundStmt *compoundStmt;
        struct LabelStmt *labelStmt;
        struct JumpStmt *jumpStmt;
    };
} Stmt;

typedef struct WhileStmt
{
    Expr *condition;
    Stmt *body;
    bool doWhile;
    SymbolEntry *symbolEntry;
} WhileStmt;

typedef struct ForStmt
{
    Stmt *init;
    Stmt *condition;
    Expr *modifier;
    Stmt *body;
    SymbolEntry *symbolEntry;
} ForStmt;

typedef struct IfStmt
{
    Expr *condition;
    Stmt *trueBody;
    Stmt *falseBody; // NULL for else
} IfStmt;

typedef struct SwitchStmt
{
    Expr *selector;
    Stmt *body;
    SymbolEntry *symbolEntry;
} SwitchStmt;

typedef struct ExprStmt
{
    Expr *expr; // can be NULL
} ExprStmt;

// forward definition required
typedef struct StructDecl StructDecl;

typedef struct StructDeclList
{
    StructDecl **structDecls;
    size_t structDeclListSize;
    size_t structDeclListCapacity;
} StructDeclList;

typedef struct StructSpecifier
{
    char *ident; // can be null for anonymous structs
    StructDeclList *structDeclList;

} StructSpecifier;

// holds either a data type or a struct specifier
typedef struct TypeSpecifier
{
    bool isStruct;
    DataType dataType; // either of these can be NULL
    StructSpecifier *structSpecifier;
} TypeSpecifier;

typedef struct TypeSpecList
{
    TypeSpecifier **typeSpecs;
    size_t typeSpecSize;
    size_t typeSpecCapacity;
} TypeSpecList;

typedef struct DeclarationList DeclarationList;

typedef struct Declarator Declarator;

typedef struct StructDecl
{
    TypeSpecList *typeSpecList;
    Declarator *declarator;
    Expr *bitField; // can be NULL
} StructDecl;

typedef struct Initializer Initializer;

typedef struct InitList
{
    Initializer **inits;
    size_t size;
    size_t capacity;
} InitList;

typedef struct Decl Decl;

typedef struct DeclarationList
{
    size_t size;
    size_t capacity;
    Decl **decls;
} DeclarationList;

typedef struct Declarator
{
    size_t pointerCount;
    char *ident; // needs array and function definitions too
    bool isParam;
    DeclarationList parameterList;
    bool isArray;
    Expr *arraySize; // constant expression
    bool isFunc;
} Declarator;

typedef struct DeclInit // holds declarator and sometimes initializers
{
    Declarator *declarator;
    bool isArray;
    Expr *initExpr;
    InitList *initList;
} DeclInit;

typedef struct Decl
{
    TypeSpecList *typeSpecList;
    // struct pointer will be here eventually.

    // Try just one declInit for split
    DeclInit *declInit;
    SymbolEntry *symbolEntry;
} Decl;

typedef struct DeclInitList
{
    DeclInit **declInits;
    size_t declInitListSize;
    size_t declInitListCapacity;
} DeclInitList;

typedef struct StatementList
{
    size_t size;
    size_t capacity;
    Stmt **stmts;
} StatementList;

typedef struct CompoundStmt
{
    StatementList stmtList;
    DeclarationList declList;
} CompoundStmt;

typedef struct LabelStmt
{
    char *ident;     // not NULL for goto
    Expr *caseLabel; // not NULL for case
    Stmt *body;

} LabelStmt;

typedef struct JumpStmt
{
    JumpType type;
    char *ident; // can be NULL
    Expr *expr;  // can be NULL
    SymbolEntry *symbolEntry;
} JumpStmt;

typedef struct FuncDef
{
    TypeSpecList *retType;
    size_t ptrCount;
    char *ident;
    DeclarationList args;
    Stmt *body;
    SymbolEntry *symbolEntry;
    bool isParam;
    bool isPrototype;
} FuncDef;

typedef struct Initializer
{
    Expr *expr;
    InitList *initList;
} Initializer;

typedef struct ExternDecl
{
    bool isFunc;
    FuncDef *funcDef;
    Decl *decl;
} ExternDecl;

typedef struct TranslationUnit
{
    ExternDecl **externDecls;
    size_t size;
    size_t capacity;
} TranslationUnit;

Expr *exprCreate(ExprType type);
void exprDestroy(Expr *expr);

VariableExpr *variableExprCreate(char *ident);
void variableExprDestroy(VariableExpr *expr);

ConstantExpr *constantExprCreate(DataType type, bool isString);
void constantExprDestroy(ConstantExpr *expr);

OperationExpr *operationExprCreate(const Operator operator);
void operationExprDestroy(OperationExpr *expr);

AssignExpr *assignExprCreate(Expr *op, Operator operator);
void assignExprDestroy(AssignExpr *expr);

FuncExpr *funcExprCreate(size_t argsSize);
void funcExprArgsResize(FuncExpr *expr, size_t argsSize);
void funcExprArgsPush(FuncExpr *expr, Expr *arg);
Expr *funcExprArgsPop(FuncExpr *expr);
void funcExprDestroy(FuncExpr *expr);

Stmt *stmtCreate(StmtType type);
void stmtDestroy(Stmt *stmt);

WhileStmt *whileStmtCreate(Expr *condition, Stmt *body, bool doWhile);
void whileStmtDestroy(WhileStmt *whileStmt);

ForStmt *forStmtCreate(Stmt *init, Stmt *condition, Stmt *body);
void forStmtDestroy(ForStmt *forStmt);

IfStmt *ifStmtCreate(Expr *condition, Stmt *trueBody);
void ifStmtDestroy(IfStmt *ifStmt);

SwitchStmt *switchStmtCreate(Expr *selector, Stmt *body);
void switchStmtDestroy(SwitchStmt *switchStmt);

ExprStmt *exprStmtCreate(void);
void exprStmtDestroy(ExprStmt *exprStmt);

void statementListInit(StatementList *stmtList, size_t size);
void statementListDestroy(StatementList *stmtList);
void statementListResize(StatementList *stmtList, size_t size);
void statementListPush(StatementList *stmtList, Stmt *stmt);

void declarationListInit(DeclarationList *declList, size_t size);
void declarationListDestroy(DeclarationList *declList);
void declarationListResize(DeclarationList *declList, size_t size);
void declarationListPush(DeclarationList *declList, Decl *decl);

CompoundStmt *compoundStmtCreate(void);
void compoundStmtDestroy(CompoundStmt *stmt);

LabelStmt *labelStmtCreate(Stmt *body);
void labelStmtDestroy(LabelStmt *labelStmt);

JumpStmt *jumpStmtCreate(JumpType type);
void jumpStmtDestroy(JumpStmt *jumpStmt);

TypeSpecList *typeSpecListCreate(size_t typeSpecSize);
void typeSpecListDestroy(TypeSpecList *typeSpecList);
void typeSpecListResize(TypeSpecList *typeSpecList, const size_t typeSpecSize);
void typeSpecListPush(TypeSpecList *typeSpecList, TypeSpecifier *typeSpec);
TypeSpecList *typeSpecListCopy(TypeSpecList *typeSpecList);

DeclInit *declInitCreate(Declarator *declarator);
void declInitDestroy(DeclInit *declInit);

Decl *declCreate(TypeSpecList *typeSpecList);
void declDestroy(Decl *decl);

DeclInitList *declInitListCreate(size_t declInitListSize);
void declInitListDestroy(DeclInitList *declInitList);
void declInitListResize(DeclInitList *declInitList, size_t declInitListSize);
void declInitListPush(DeclInitList *declInitList, DeclInit *declInit);

StructDecl *structDeclCreate(void);
void structDeclDestroy(StructDecl *structDecl);

StructDeclList *structDeclListCreate(size_t structDeclListSize);
void structDeclListDestroy(StructDeclList *structDeclList);
void structDeclListResize(StructDeclList *structDeclList, const size_t structDeclListSize);
void structDeclListPush(StructDeclList *structDeclList, StructDecl *structDecl);

StructSpecifier *structSpecifierCreate(void);
void structSpecifierDestroy(StructSpecifier *structSpec);

TypeSpecifier *typeSpecifierCreate(bool isStruct);
void typeSpecifierDestroy(TypeSpecifier *typeSpecifier);
TypeSpecifier *typeSpecifierCopy(TypeSpecifier *typeSpec);

Declarator *declaratorCreate(void);
void declaratorDestroy(Declarator *declarator);

FuncDef *funcDefCreate(TypeSpecList *retType, size_t ptrCount, char *ident);
void funcDefDestroy(FuncDef *funcDef);

InitList *initListCreate(size_t initListSize);
void initListDestroy(InitList *initList);
void initListResize(InitList *initList, const size_t initListSize);
void initListPush(InitList *initList, Initializer *init);

Initializer *initCreate(void);
void initDestroy(Initializer *init);

DataType returnType(Expr *expr);
void resolveType(Expr *expr);

ExternDecl *externDeclCreate(bool isFunc);
void externDeclDestroy(ExternDecl *externDecl);

TranslationUnit *transUnitCreate(size_t size);
void transUnitDestroy(TranslationUnit *transUnit);
void transUnitResize(TranslationUnit *transUnit, const size_t size);
void transUnitPush(TranslationUnit *transUnit, ExternDecl *externDecl);

TypeSpecList *flattenTypeSpecs(TypeSpecList *typeSpecList);

#endif
