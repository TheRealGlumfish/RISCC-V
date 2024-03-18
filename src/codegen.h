#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>

#include "ast.h"

extern FILE *outFile;

typedef enum
{
    ZERO,
    RA,
    SP,
    GP,
    TP,
    T0,
    T1,
    T2,
    FP,
    S1,
    A0,
    A1,
    A2,
    A3,
    A4,
    A5,
    A6,
    A7,
    S2,
    S3,
    S4,
    S5,
    S6,
    S7,
    S8,
    S9,
    S10,
    S11,
    T3,
    T4,
    T5,
    T6,
    FT0,
    FT1,
    FT2,
    FT3,
    FT4,
    FT5,
    FT6,
    FT7,
    FS0,
    FS1,
    FA0,
    FA1,
    FA2,
    FA3,
    FA4,
    FA5,
    FA6,
    FA7,
    FS2,
    FS3,
    FS4,
    FS5,
    FS6,
    FS7,
    FS8,
    FS9,
    FS10,
    FS11,
    FT8,
    FT9,
    FT10,
    FT11,
} Reg;

typedef struct ParamRegCounts
{
    size_t intRegs;
    size_t floatRegs;
} ParamRegCounts;

const char *regStr(Reg reg);
Reg getTmpReg(void);
Reg getTmpFltReg(void);

void compileExpr(Expr *expr, Reg dest);
void compileOperationExpr(OperationExpr *expr, Reg dest);
void compileConstantExpr(ConstantExpr *expr, Reg dest);
void compileVariableExpr(VariableExpr *expr, Reg dest);
void compileAssignExpr(AssignExpr *expr, Reg dest);
void compileFuncExpr(FuncExpr *expr, Reg dest);

void compileStmt(Stmt *stmt);
void compileJumpStmt(JumpStmt *stmt);

void compileFunc(FuncDef *func);
void compileFuncArgs(DeclarationList declList);
void compileCallArgs(FuncExpr *expr);

#endif
