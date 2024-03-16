#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ast.h"
#include "codegen.h"

FILE *outFile;

uint64_t LCLabelId;
bool regs[32] = { 0 };

const char *regStr(Reg reg)
{
    switch(reg)
    {
        case ZERO:
            return "zero";
        case RA:
            return "ra";
        case SP:
            return "sp";
        case GP:
            return "gp";
        case TP:
            return "tp";
        case T0:
            return "t0";
        case T1:
            return "t1";
        case T2:
            return "t2";
        case FP:
            return "fp";
        case S1:
            return "s1";
        case A0:
            return "a0";
        case A1:
            return "a1";
        case A2:
            return "a2";
        case A3:
            return "a3";
        case A4:
            return "a4";
        case A5:
            return "a5";
        case A6:
            return "a6";
        case A7:
            return "a7";
        case S2:
            return "s2";
        case S3:
            return "s3";
        case S4:
            return "s4";
        case S5:
            return "s5";
        case S6:
            return "s6";
        case S7:
            return "s7";
        case S8:
            return "s8";
        case S9:
            return "s9";
        case S10:
            return "s10";
        case S11:
            return "s11";
        case T3:
            return "t3";
        case T4:
            return "t4";
        case T5:
            return "t5";
        case T6:
            return "t6";
    }
}

// Returns a temporary register
Reg getTmpReg(void)
{
    for (size_t i = 0; i < 32; i++)
    {
        if(i == T0 || i == T1 || i == T2 || i == T3 || i == T4 || i == T5 || i == T6)
        {
            if (!regs[i]) 
            {
                regs[i] = true;
                return i;
            }
        }
    }
    fprintf(stderr, "All registers filled, exitting...\n");
    exit(-1);
}

// Free a register
void freeReg(Reg reg)
{
    regs[reg] = false; 
}

// Gets a "unique" number, aborts if we run out of numbers
uint64_t getId(uint64_t *num)
{
    if (num + 1 < num)
    {
        abort();
    }
    return *num++; // TODO: Check if this works as expected
}

void compileExpr(Expr *expr, Reg dest)
{
    switch (expr->type)
    {
        case OPERATION_EXPR:
        {
            compileOperationExpr(expr->operation, dest);
            break;
        }
        case CONSTANT_EXPR:
        {
            compileConstantExpr(expr->constant, dest);
        }
    }
}

void compileConstantExpr(ConstantExpr *expr, Reg dest)
{
    if(expr->isString)
    {
        uint64_t labelId = getId(&LCLabelId);
        fprintf(outFile, ".rodata\n");
        fprintf(outFile, ".LC%lu:\n", labelId);
        fprintf(outFile, "\t.string \"%s\"\n", expr->string_const);
        fprintf(outFile, ".text\n");
        fprintf(outFile, "\tla %s, .LC%lu\n", regStr(dest), labelId);
    } else
    {
        switch(expr->type)
        {
            case INT_TYPE:
            {
                fprintf(outFile, "\tli %s, %i\n", regStr(dest), expr->int_const);
                break;
            }
            case CHAR_TYPE:
            {
                fprintf(outFile, "\tli %s, %ui\n", regStr(dest), expr->char_const); // TODO: Switch to hex format, check if there is unsigned version, switch to non-pseudoinstruction for char
                break;
            }
            default:
            {
                fprintf(stderr, "Non-long types not supported, exitting...");
                exit(EXIT_FAILURE);
            }
        }
    }
}

void compileOperationExpr(OperationExpr *expr, Reg dest)
{
    switch(expr->operator)
    {
        case ADD:
        {
            // TODO: Deal with non-long types 
            Reg op1 = getTmpReg();
            Reg op2 = getTmpReg();
            compileExpr(expr->op1, op1);
            compileExpr(expr->op2, op2);
            fprintf(outFile, "\tadd %s, %s, %s\n", regStr(dest), regStr(op1), regStr(op2));
            freeReg(op1);
            freeReg(op2);
            break;
        }
        case SUB:
        {
            // TODO: Deal with non-long types 
            Reg op1 = getTmpReg();
            Reg op2 = getTmpReg();
            compileExpr(expr->op1, op1);
            compileExpr(expr->op2, op2);
            fprintf(outFile, "\tsub %s, %s, %s\n", regStr(dest), regStr(op1), regStr(op2));
            freeReg(op1);
            freeReg(op2);
            break;
        }
        case MUL:
        {
            // TODO: Deal with non-long types 
            Reg op1 = getTmpReg();
            Reg op2 = getTmpReg();
            compileExpr(expr->op1, op1);
            compileExpr(expr->op2, op2);
            fprintf(outFile, "\tmul %s, %s, %s\n", regStr(dest), regStr(op1), regStr(op2));
            freeReg(op1); // TODO: Test register eviction
            freeReg(op2);
            break;
        }
        case DIV:
        {
            // TODO: Deal with non-long types
            // TODO: Deal with unsigned division
            Reg op1 = getTmpReg();
            Reg op2 = getTmpReg();
            compileExpr(expr->op1, op1);
            compileExpr(expr->op2, op2);
            fprintf(outFile, "\tdiv %s, %s, %s\n", regStr(dest), regStr(op1), regStr(op2));
            freeReg(op1); // TODO: Test register eviction
            freeReg(op2);
            break;
        }
        case NOT:
        {
            Reg op1 = getTmpReg();
            compileExpr(expr->op1, op1); 
            fprintf(outFile, "\tnot %s, %s\n", regStr(dest), regStr(op1));
            freeReg(op1); // TODO: Test register eviction
            break;
        }
        case EQ:
        {
            // TODO: Deal with signs
            Reg op1 = getTmpReg();
            Reg op2 = getTmpReg();
            compileExpr(expr->op1, op1);
            compileExpr(expr->op2, op2);
            fprintf(outFile, "\tsub %s, %s, %s\n", regStr(dest), regStr(op1), regStr(op2));
            fprintf(outFile, "\tseqz %s, %s\n", regStr(dest), regStr(dest));
            freeReg(op1); // TODO: Test register eviction
            freeReg(op2);
            break;
        }
        default:
        {
            fprintf(stderr, "Operation not supported, exitting...");
            exit(EXIT_FAILURE);
        }
    }
}
