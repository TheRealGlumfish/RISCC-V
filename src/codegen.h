#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>

#include "ast.h"

extern FILE *outFile;

typedef enum {
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
} Reg;

const char *regStr(Reg reg);

void compileExpr(Expr *expr, Reg dest);
void compileOperationExpr(OperationExpr *expr, Reg dest);
void compileConstantExpr(ConstantExpr *expr, Reg dest);

#endif
