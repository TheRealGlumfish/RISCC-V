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
    VOID_TYPE,
    CHAR_TYPE,
    SHORT_TYPE,
    INT_TYPE,
    LONG_TYPE,
    FLOAT_TYPE,
    DOUBLE_TYPE,
    SIGNED_TYPE,
    UNSIGNED_TYPE
} DataType; // TODO: Add full list of types and deal with unsigned and void

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

typedef struct VariableExpr
{
    char *ident;
    DataType type;
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
    Expr *op;
    Expr *lvalue;
    Operator operator;  // Info: when set to NOT, regular assignment
    DataType type; // TODO: Replace, could accept compound types
} AssignExpr;

typedef struct FuncExpr
{
    char *ident;
    size_t argsSize;
    size_t argsCapacity;
    Expr **args;
    DataType type; // TODO: Replace, functions may not retrun primative types only
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
} WhileStmt;

typedef struct ForStmt
{
    Stmt *init;
    Stmt *condition;
    Expr *modifier;
    Stmt *body;
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
} SwitchStmt;

typedef struct ExprStmt
{
    Expr *expr; // can be NULL
} ExprStmt;

typedef struct StructSpecifier
{
    char* ident; // can be null for anonymous structs
    
    struct StructDecl **structDecls;
    size_t structDeclSize;
    size_t structDeclCapacity;
} StructSpecifier;

// holds either a data type or a struct specifier
typedef struct TypeSpecifier
{
    bool isStruct;
    union
    {
        DataType dataType; // either of these can be NULL
        StructSpecifier* structSpecifier;
    };
    
} TypeSpecifier;

typedef struct TypeSpecList
{
    TypeSpecifier **typeSpecs;
    size_t typeSpecSize;
    size_t typeSpecCapacity;
} TypeSpecList;

typedef struct StructDecl{
    TypeSpecList *typeSpecList;
    char* ident;
    Expr* bitField; // can be NULL
} StructDecl;

// intermediary class used for building the AST
typedef struct DeclInit // holds declarator and sometimes an initializer
{ 
    size_t pointerCount;
    char *ident; // needs array and function definitions too
    Expr *initExpr; // both initExpr and initArray can be NULL when defining a struct
    // array initializer defined here
} DeclInit;

typedef struct Decl
{
    TypeSpecList *typeSpecList;
    // struct pointer will be here eventually.

    // Try just one declInit for split
    DeclInit *declInit;
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

typedef struct DeclarationList
{
    size_t size;
    size_t capacity;
    Decl **decls;
} DeclarationList;

typedef struct CompoundStmt
{
    // declarations are not expressions - need to fix.
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
} JumpStmt;



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

TypeSpecList *typeSpecListCreate(const size_t typeSpecSize);
void typeSpecListDestroy(TypeSpecList *typeSpecList);
void typeSpecListResize(TypeSpecList *typeSpecList, const size_t typeSpecSize);
void typeSpecListPush(TypeSpecList *typeSpecList, TypeSpecifier* typeSpec);
TypeSpecList *typeSpecListCopy(TypeSpecList *typeSpecList);

DeclInit *declInitCreate(char *ident, const size_t pointerCount);
void declInitDestroy(DeclInit *declInit);

Decl *declCreate(TypeSpecList *typeSpecList);
void declDestroy(Decl *decl);

DeclInitList *declInitListCreate(const size_t declInitListSize);
void declInitListDestroy(DeclInitList *declInitList);
void declInitListResize(DeclInitList *declInitList, const size_t declInitListSize);
void declInitListPush(DeclInitList *declInitList, DeclInit *declInit);

StructDecl *structDeclCreate(TypeSpecList *typeSpecList, char *ident);
void structDeclDestroy(StructDecl *structDecl);

StructSpecifier *structSpecifierCreate();
void structSpecifierDestroy(StructSpecifier *structSpec);
void structSpecifierResize(StructSpecifier *structSpecifier, const size_t structDeclSize);
void structSpecifierPush(StructSpecifier *structSpecifier, StructDecl *structDecl);

TypeSpecifier *typeSpecifierCreate(bool isStruct);
void typeSpecifierDestroy(TypeSpecifier *typeSpecifier);

#endif
