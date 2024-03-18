#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "codegen.h"
#include "symbol.h"

FILE *outFile;

uint64_t LCLabelId;
bool regs[32] = {0};

const char *regStr(Reg reg)
{
    switch (reg)
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
    case FT0:
        return "ft0";
    case FT1:
        return "ft1";
    case FT2:
        return "ft3";
    case FT3:
        return "ft3";
    case FT4:
        return "ft4";
    case FT5:
        return "ft5";
    case FT6:
        return "ft6";
    case FT7:
        return "ft7";
    case FS0:
        return "fs0";
    case FS1:
        return "fs1";
    case FA0:
        return "fa0";
    case FA1:
        return "fa1";
    case FA2:
        return "fa2";
    case FA3:
        return "fa3";
    case FA4:
        return "fa4";
    case FA5:
        return "fa5";
    case FA6:
        return "fa6";
    case FA7:
        return "fa7";
    case FS2:
        return "fs2";
    case FS3:
        return "fs3";
    case FS4:
        return "fs4";
    case FS5:
        return "fs5";
    case FS6:
        return "fs6";
    case FS7:
        return "fs7";
    case FS8:
        return "fs8";
    case FS9:
        return "fs9";
    case FS10:
        return "fs10";
    case FS11:
        return "fs11";
    case FT8:
        return "ft8";
    case FT9:
        return "ft9";
    case FT10:
        return "ft10";
    case FT11:
        return "ft11";
    }
}

// Returns a temporary register
Reg getTmpReg(void)
{
    for (size_t i = 0; i < 32; i++)
    {
        if (i == T0 || i == T1 || i == T2 || i == T3 || i == T4 || i == T5 || i == T6)
        {
            if (!regs[i])
            {
                regs[i] = true;
                return i;
            }
        }
    }
    fprintf(stderr, "All registers filled, exiting...\n");
    exit(-1);
}

// Returns a temporary register
Reg getTmpFltReg(void)
{
    for (size_t i = 32; i < 64; i++)
    {
        if (i == FT0 || i == FT1 || i == FT2 || i == FT3 || i == FT4 || i == FT5 || i == FT6 || i == FT7 || i == FT8 || i == FT9 || i == FT10 || i == FT11)
        {
            if (!regs[i])
            {
                regs[i] = true;
                return i;
            }
        }
    }
    fprintf(stderr, "All floating-point registers filled, exiting...\n");
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
        break;
    }
    case VARIABLE_EXPR:
    {
        compileVariableExpr(expr->variable, dest);
        break;
    }
    case ASSIGN_EXPR:
    {
        compileAssignExpr(expr->assignment, dest);
        break;
    }
    case FUNC_EXPR:
    {
        compileFuncExpr(expr->function, dest);
        break;
    }
    }
}

void compileConstantExpr(ConstantExpr *expr, const Reg dest)
{
    if (expr->isString)
    {
        uint64_t labelId = getId(&LCLabelId);
        fprintf(outFile, ".rodata\n");
        fprintf(outFile, ".LC%lu:\n", labelId);
        fprintf(outFile, "\t.string \"%s\"\n", expr->string_const);
        fprintf(outFile, ".text\n");
        fprintf(outFile, "\tla %s, .LC%lu\n", regStr(dest), labelId);
    }
    else
    {
        switch (expr->type)
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
        case FLOAT_TYPE:
        {
            Reg op1 = getTmpFltReg();
            fprintf(outFile, "\tli %s, %ui\n", regStr(op1), (unsigned int)expr->float_const);
            fprintf(outFile, "\tfmv.s %s, %s\n", regStr(dest), regStr(op1));
            break;
        }
        default:
        {
            fprintf(stderr, "Non-long types not supported, exiting...\n");
            exit(EXIT_FAILURE);
        }
        }
    }
}

void compileOperationExpr(OperationExpr *expr, const Reg dest)
{
    switch (expr->operator)
    {
    case ADD:
    {
        switch (expr->type)
        {
        case FLOAT_TYPE:
        {
            // TODO: Deal with non-long types
            Reg op1 = getTmpFltReg();
            Reg op2 = getTmpFltReg();
            compileExpr(expr->op1, op1);
            compileExpr(expr->op2, op2);
            fprintf(outFile, "\tfadd.s %s, %s, %s\n", regStr(dest), regStr(op1), regStr(op2));
            freeReg(op1);
            freeReg(op2);
            break;
        }
        case DOUBLE_TYPE:
        {
            // TODO: Deal with non-long types
            Reg op1 = getTmpFltReg();
            Reg op2 = getTmpFltReg();
            compileExpr(expr->op1, op1);
            compileExpr(expr->op2, op2);
            fprintf(outFile, "\tfadd.d %s, %s, %s\n", regStr(dest), regStr(op1), regStr(op2));
            freeReg(op1);
            freeReg(op2);
            break;
        }
        default:
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
        }
        break;
    }
    case SUB:
    {
        switch (expr->type)
        {
        case FLOAT_TYPE:
        {
            // TODO: Deal with non-long types
            Reg op1 = getTmpFltReg();
            Reg op2 = getTmpFltReg();
            compileExpr(expr->op1, op1);
            compileExpr(expr->op2, op2);
            fprintf(outFile, "\tfsub.s %s, %s, %s\n", regStr(dest), regStr(op1), regStr(op2));
            freeReg(op1);
            freeReg(op2);
            break;
        }
        case DOUBLE_TYPE:
        {
            // TODO: Deal with non-long types
            Reg op1 = getTmpFltReg();
            Reg op2 = getTmpFltReg();
            compileExpr(expr->op1, op1);
            compileExpr(expr->op2, op2);
            fprintf(outFile, "\tfsub.d %s, %s, %s\n", regStr(dest), regStr(op1), regStr(op2));
            freeReg(op1);
            freeReg(op2);
            break;
        }
        default:
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
        }
        break;
    }
    case MUL:
    {
        switch (expr->type)
        {
        case FLOAT_TYPE:
        {
            // TODO: Deal with non-long types
            Reg op1 = getTmpFltReg();
            Reg op2 = getTmpFltReg();
            compileExpr(expr->op1, op1);
            compileExpr(expr->op2, op2);
            fprintf(outFile, "\tfmul.s %s, %s, %s\n", regStr(dest), regStr(op1), regStr(op2));
            freeReg(op1);
            freeReg(op2);
            break;
        }
        case DOUBLE_TYPE:
        {
            // TODO: Deal with non-long types
            Reg op1 = getTmpFltReg();
            Reg op2 = getTmpFltReg();
            compileExpr(expr->op1, op1);
            compileExpr(expr->op2, op2);
            fprintf(outFile, "\tfmul.d %s, %s, %s\n", regStr(dest), regStr(op1), regStr(op2));
            freeReg(op1);
            freeReg(op2);
            break;
        }
        default:
        {
            // TODO: Deal with non-long types
            Reg op1 = getTmpReg();
            Reg op2 = getTmpReg();
            compileExpr(expr->op1, op1);
            compileExpr(expr->op2, op2);
            fprintf(outFile, "\tmul %s, %s, %s\n", regStr(dest), regStr(op1), regStr(op2));
            freeReg(op1);
            freeReg(op2);
            break;
        }
        }
        break;
    }
    case DIV:
    {
        switch (expr->type)
        {
        case FLOAT_TYPE:
        {
            // TODO: Deal with non-long types
            Reg op1 = getTmpFltReg();
            Reg op2 = getTmpFltReg();
            compileExpr(expr->op1, op1);
            compileExpr(expr->op2, op2);
            fprintf(outFile, "\tfdiv.s %s, %s, %s\n", regStr(dest), regStr(op1), regStr(op2));
            freeReg(op1);
            freeReg(op2);
            break;
        }
        case DOUBLE_TYPE:
        {
            // TODO: Deal with non-long types
            Reg op1 = getTmpFltReg();
            Reg op2 = getTmpFltReg();
            compileExpr(expr->op1, op1);
            compileExpr(expr->op2, op2);
            fprintf(outFile, "\tfdiv.d %s, %s, %s\n", regStr(dest), regStr(op1), regStr(op2));
            freeReg(op1);
            freeReg(op2);
            break;
        }
        default:
        {
            // TODO: Deal with non-long types
            Reg op1 = getTmpReg();
            Reg op2 = getTmpReg();
            compileExpr(expr->op1, op1);
            compileExpr(expr->op2, op2);
            fprintf(outFile, "\tdiv %s, %s, %s\n", regStr(dest), regStr(op1), regStr(op2));
            freeReg(op1);
            freeReg(op2);
            break;
        }
        }
        break;
    }
    case MOD:
    {
        // TODO: Deal with non-long types
        // TODO: Deal with unsigned division
        Reg op1 = getTmpReg();
        Reg op2 = getTmpReg();
        compileExpr(expr->op1, op1);
        compileExpr(expr->op2, op2);
        fprintf(outFile, "\trem %s, %s, %s\n", regStr(dest), regStr(op1), regStr(op2));
        freeReg(op1); // TODO: Test register eviction
        freeReg(op2);
        break;
    }
    case NOT:
    {
        // TODO: Deal with unsigned
        Reg op1 = getTmpReg();
        compileExpr(expr->op1, op1);
        fprintf(outFile, "\tsgtz %s, %s\n", regStr(op1), regStr(op1));
        fprintf(outFile, "\tnot %s, %s\n", regStr(dest), regStr(op1));
        freeReg(op1); // TODO: Test register eviction
        break;
    }
    case NOT_BIT:
    {
        // TODO: Deal with unsigned
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
    case NE:
    {
        // TODO: Deal with signs
        Reg op1 = getTmpReg();
        Reg op2 = getTmpReg();
        compileExpr(expr->op1, op1);
        compileExpr(expr->op2, op2);
        fprintf(outFile, "\tsub %s, %s, %s\n", regStr(dest), regStr(op1), regStr(op2));
        fprintf(outFile, "\tsnez %s, %s\n", regStr(dest), regStr(dest));
        freeReg(op1); // TODO: Test register eviction
        freeReg(op2);
        break;
    }
    case LT:
    {
        // TODO: Deal with signs
        Reg op1 = getTmpReg();
        Reg op2 = getTmpReg();
        compileExpr(expr->op1, op1);
        compileExpr(expr->op2, op2);
        fprintf(outFile, "\tslt %s, %s, %s\n", regStr(dest), regStr(op1), regStr(op2));
        freeReg(op1); // TODO: Test register eviction
        freeReg(op2);
        break;
    }
    case GT:
    {
        // TODO: Deal with signs
        // TODO: Test the damn code
        Reg op1 = getTmpReg();
        Reg op2 = getTmpReg();
        compileExpr(expr->op1, op1);
        compileExpr(expr->op2, op2);
        fprintf(outFile, "\tslt %s, %s, %s\n", regStr(dest), regStr(op2), regStr(op1));
        freeReg(op1); // TODO: Test register eviction
        freeReg(op2);
        break;
    }
    case LE:
    {
        // TODO: Deal with signs
        Reg op1 = getTmpReg();
        Reg op2 = getTmpReg();
        compileExpr(expr->op1, op1);
        compileExpr(expr->op2, op2);
        fprintf(outFile, "\tslt %s, %s, %s\n", regStr(dest), regStr(op2), regStr(op1));
        fprintf(outFile, "\txori %s, %s, 1\n", regStr(dest), regStr(dest));
        freeReg(op1); // TODO: Test register eviction
        freeReg(op2);
        break;
    }
    case GE:
    {
        // TODO: Deal with signs
        Reg op1 = getTmpReg();
        Reg op2 = getTmpReg();
        compileExpr(expr->op1, op1);
        compileExpr(expr->op2, op2);
        fprintf(outFile, "\tslt %s, %s, %s\n", regStr(dest), regStr(op1), regStr(op2));
        fprintf(outFile, "\txori %s, %s, 1\n", regStr(dest), regStr(dest));
        freeReg(op1); // TODO: Test register eviction
        freeReg(op2);
        break;
    }
    case OR:
    {
        // TODO: Deal with signs
        Reg op1 = getTmpReg();
        Reg op2 = getTmpReg();
        compileExpr(expr->op1, op1);
        compileExpr(expr->op2, op2);
        fprintf(outFile, "\tor %s, %s, %s\n", regStr(dest), regStr(op1), regStr(op2));
        fprintf(outFile, "\tsgtz %s, %s\n", regStr(dest), regStr(dest));
        freeReg(op1); // TODO: Test register eviction
        freeReg(op2);
        break;
    }
    case AND:
    {
        // TODO: Deal with signs
        Reg op1 = getTmpReg();
        Reg op2 = getTmpReg();
        compileExpr(expr->op1, op1);
        compileExpr(expr->op2, op2);
        fprintf(outFile, "\tsgtz %s, %s\n", regStr(op1), regStr(op1));
        fprintf(outFile, "\tsgtz %s, %s\n", regStr(op2), regStr(op2));
        fprintf(outFile, "\tand %s, %s, %s\n", regStr(dest), regStr(op1), regStr(op2));
        freeReg(op1); // TODO: Test register eviction
        freeReg(op2);
        break;
    }
    case OR_BIT:
    {
        // TODO: Deal with signs
        Reg op1 = getTmpReg();
        Reg op2 = getTmpReg();
        compileExpr(expr->op1, op1);
        compileExpr(expr->op2, op2);
        fprintf(outFile, "\tor %s, %s, %s\n", regStr(dest), regStr(op1), regStr(op2));
        freeReg(op1); // TODO: Test register eviction
        freeReg(op2);
        break;
    }
    case AND_BIT:
    {
        // TODO: Deal with signs
        Reg op1 = getTmpReg();
        Reg op2 = getTmpReg();
        compileExpr(expr->op1, op1);
        compileExpr(expr->op2, op2);
        fprintf(outFile, "\tand %s, %s, %s\n", regStr(dest), regStr(op1), regStr(op2));
        freeReg(op1); // TODO: Test register eviction
        freeReg(op2);
        break;
    }
    case XOR:
    {
        // TODO: Deal with signs
        Reg op1 = getTmpReg();
        Reg op2 = getTmpReg();
        compileExpr(expr->op1, op1);
        compileExpr(expr->op2, op2);
        fprintf(outFile, "\txor %s, %s, %s\n", regStr(dest), regStr(op1), regStr(op2));
        freeReg(op1); // TODO: Test register eviction
        freeReg(op2);
        break;
    }
    case LEFT_SHIFT:
    {
        switch (expr->type)
        {
        case FLOAT_TYPE:
        {
            fprintf(stderr, "Shift-operations cannot be done on floating-point types, exiting...\n");
            exit(EXIT_FAILURE);
            break;
        }
        case DOUBLE_TYPE:
        {
            fprintf(stderr, "Shift-operations cannot be done on floating-point types, exiting...\n");
            exit(EXIT_FAILURE);
            break;
        }
        default:
        {
            Reg op1 = getTmpReg();
            Reg op2 = getTmpReg();
            compileExpr(expr->op1, op1);
            compileExpr(expr->op2, op2);
            fprintf(outFile, "\tsll %s, %s, %s\n", regStr(dest), regStr(op1), regStr(op2));
            freeReg(op1);
            freeReg(op2);
            break;
        }
        }
        break;
    }
    case RIGHT_SHIFT:
    {
        switch (expr->type)
        {
        case FLOAT_TYPE:
        {
            fprintf(stderr, "Shift-operations cannot be done on floating-point types, exiting...\n");
            exit(EXIT_FAILURE);
            break;
        }
        case DOUBLE_TYPE:
        {
            fprintf(stderr, "Shift-operations cannot be done on floating-point types, exiting...\n");
            exit(EXIT_FAILURE);
            break;
        }
        case INT_TYPE: // Signed shift
        {
            Reg op1 = getTmpReg();
            Reg op2 = getTmpReg();
            compileExpr(expr->op1, op1);
            compileExpr(expr->op2, op2);
            fprintf(outFile, "\tsra %s, %s, %s\n", regStr(dest), regStr(op1), regStr(op2));
            freeReg(op1);
            freeReg(op2);
            break;
        }
        default:
        {
            Reg op1 = getTmpReg();
            Reg op2 = getTmpReg();
            compileExpr(expr->op1, op1);
            compileExpr(expr->op2, op2);
            fprintf(outFile, "\tsrl %s, %s, %s\n", regStr(dest), regStr(op1), regStr(op2));
            freeReg(op1);
            freeReg(op2);
            break;
        }
        }
        break;
    }
    case INC:
    {
        // TODO: Handle types (add float support)
        if(expr->op1->type != VARIABLE_EXPR)
        {
            fprintf(stderr, "Expression is not assignable, exiting...\n");
            exit(EXIT_FAILURE);
        }
        Expr *one = exprCreate(CONSTANT_EXPR);
        one->constant = constantExprCreate(expr->type, false);
        one->constant->int_const = 1;
        AssignExpr *assign = assignExprCreate(one, ADD);
        assign->type = expr->type;
        assign->lvalue = NULL;
        assign->ident = expr->op1->variable->ident;
        assign->symbolEntry = expr->op1->variable->symbolEntry;
        compileAssignExpr(assign, dest);
        exprDestroy(one);
        break;
    }
    case DEC:
    {
        // TODO: Handle types (add float code)
        if(expr->op1->type != VARIABLE_EXPR)
        {
            fprintf(stderr, "Expression is not assignable, exiting...\n");
            exit(EXIT_FAILURE);
        }
        Expr *one = exprCreate(CONSTANT_EXPR);
        one->constant = constantExprCreate(expr->type, false);
        one->constant->int_const = 1;
        AssignExpr *assign = assignExprCreate(one, SUB);
        assign->type = expr->type;
        assign->lvalue = NULL;
        assign->ident = expr->op1->variable->ident;
        assign->symbolEntry = expr->op1->variable->symbolEntry;
        compileAssignExpr(assign, dest);
        exprDestroy(one);
        break;
    }
        // default:
        // {
        //     fprintf(stderr, "Operation not supported, exiting...");
        //     exit(EXIT_FAILURE);
        // }
    }
}

void compileVariableExpr(VariableExpr *expr, const Reg dest)
{
    switch (expr->type)
    {
    case INT_TYPE:
    {
        fprintf(outFile, "\tlw %s, -%lu(fp)\n", regStr(dest), expr->symbolEntry->stackOffset);
        break;
    }
    case FLOAT_TYPE:
    {
        fprintf(outFile, "\tflw %s, -%lu(fp)\n", regStr(dest), expr->symbolEntry->stackOffset);
        break;
    }
    case DOUBLE_TYPE:
    {
        fprintf(outFile, "\tfld %s, -%lu(fp)\n", regStr(dest), expr->symbolEntry->stackOffset);
        break;
    }
    default:
    {
        fprintf(stderr, "Non-interger types not supported, exiting\n");
        exit(EXIT_FAILURE);
    }
    }
}

void compileAssignExpr(AssignExpr *expr, Reg dest)
{
    // compileExpr(expr->op, dest);
    switch (expr->type)
    {
    case INT_TYPE:
    {
        if (expr->operator!= NOT)
        {
            OperationExpr *rvalue = operationExprCreate(expr->operator);
            rvalue->op1 = expr->op;
            rvalue->op2 = exprCreate(VARIABLE_EXPR);
            // TODO: Resolve the type you set in op2
            rvalue->op2->variable = variableExprCreate(expr->ident);
            rvalue->op2->variable->symbolEntry = expr->symbolEntry;
            rvalue->op2->variable->type = INT_TYPE;
            compileOperationExpr(rvalue, dest);
            fprintf(outFile, "\tsw %s, -%lu(fp)\n", regStr(dest), expr->symbolEntry->stackOffset);
            free(rvalue->op2->assignment);
            free(rvalue->op2);
        }
        else
        {
            compileExpr(expr->op, dest);
            fprintf(outFile, "\tsw %s, -%lu(fp)\n", regStr(dest), expr->symbolEntry->stackOffset);
        }
        break;
    }
    case FLOAT_TYPE:
    {
        if (expr->operator!= NOT)
        {
            OperationExpr *rvalue = operationExprCreate(expr->operator);
            rvalue->op1 = expr->op;
            rvalue->op2 = exprCreate(VARIABLE_EXPR);
            // TODO: Resolve the type you set in op2
            rvalue->op2->variable = variableExprCreate(expr->ident);
            rvalue->op2->variable->symbolEntry = expr->symbolEntry;
            rvalue->op2->variable->type = FLOAT_TYPE;
            compileOperationExpr(rvalue, dest);
            fprintf(outFile, "\tfsw %s, -%lu(fp)\n", regStr(dest), expr->symbolEntry->stackOffset);
            free(rvalue->op2->assignment);
            free(rvalue->op2);
        }
        else
        {
            compileExpr(expr->op, dest);
            fprintf(outFile, "\tfsw %s, -%lu(fp)\n", regStr(dest), expr->symbolEntry->stackOffset);
        }
        break;
    }
    case DOUBLE_TYPE:
    {
        if (expr->operator!= NOT)
        {
            OperationExpr *rvalue = operationExprCreate(expr->operator);
            rvalue->op1 = expr->op;
            rvalue->op2 = exprCreate(VARIABLE_EXPR);
            // TODO: Resolve the type you set in op2
            rvalue->op2->variable = variableExprCreate(expr->ident);
            rvalue->op2->variable->symbolEntry = expr->symbolEntry;
            rvalue->op2->variable->type = DOUBLE_TYPE;
            compileOperationExpr(rvalue, dest);
            fprintf(outFile, "\tfsd %s, -%lu(fp)\n", regStr(dest), expr->symbolEntry->stackOffset);
            free(rvalue->op2->assignment);
            free(rvalue->op2);
        }
        else
        {
            compileExpr(expr->op, dest);
            fprintf(outFile, "\tfsd %s, -%lu(fp)\n", regStr(dest), expr->symbolEntry->stackOffset);
        }
        break;
    }
    default:
    {
        if (expr->operator!= NOT)
        {
            OperationExpr *rvalue = operationExprCreate(expr->operator);
            rvalue->op1 = expr->op;
            rvalue->op2 = exprCreate(VARIABLE_EXPR);
            // TODO: Resolve the type you set in op2
            rvalue->op2->variable = variableExprCreate(expr->ident);
            rvalue->op2->variable->symbolEntry = expr->symbolEntry;
            rvalue->op2->variable->type = expr->type;
            compileOperationExpr(rvalue, dest);
            fprintf(outFile, "\tsw %s, -%lu(fp)\n", regStr(dest), expr->symbolEntry->stackOffset);
            free(rvalue->op2->assignment);
            free(rvalue->op2);
            free(rvalue);
        }
        else
        {
            compileExpr(expr->op, dest);
            fprintf(outFile, "\tsw %s, -%lu(fp)\n", regStr(dest), expr->symbolEntry->stackOffset);
        }
        fprintf(stderr, "Type not supported\n");
        // exit(EXIT_FAILURE);
        break;
    }
    }
}

void compileFuncExpr(FuncExpr *expr, Reg dest)
{
    compileCallArgs(expr);
    fprintf(outFile, "\tcall %s\n", expr->ident);
    if (expr->type == FLOAT_TYPE || expr->type == DOUBLE_TYPE)
    {
        fprintf(outFile, "\tmv %s, fa0\n", regStr(dest));
    }
    else
    {
        fprintf(outFile, "\tmv %s, a0\n", regStr(dest));
    }
    fprintf(outFile, "\tmv fp, sp\n");
    fprintf(outFile, "\tlw ra, -4(fp)\n");
}

void compileStmt(Stmt *stmt)
{
    switch (stmt->type)
    {
    case EXPR_STMT:
    {
        compileExpr(stmt->exprStmt->expr, A0);
        break;
    }
    case JUMP_STMT:
    {
        compileJumpStmt(stmt->jumpStmt);
        break;
    }
    default:
    {
        fprintf(stderr, "Statement type: %iu, not supported...\n", stmt->type);
        exit(EXIT_FAILURE);
    }
    }
}

void compileJumpStmt(JumpStmt *stmt)
{
    switch (stmt->type)
    {
    case RETURN_JUMP:
    {
        if (stmt->expr == NULL)
        {
            fprintf(outFile, "\tret\n");
        }
        else
        {
            // TODO: Deal with other types
            switch (returnType(stmt->expr))
            {
            case FLOAT_TYPE:
            {
                compileExpr(stmt->expr, FA0);
                break;
            }
            case DOUBLE_TYPE:
            {
                compileExpr(stmt->expr, FA0);
                break;
            }
            default:
            {
                compileExpr(stmt->expr, A0);
                break;
            }
            }
            fprintf(outFile, "\tmv sp, fp\n");
            fprintf(outFile, "\tret\n");
        }
    }
    }
}

// void compileArg(Decl *decl, Reg dest)
// {
//     fprintf(outFile, "\tsw %s, -%lu(fp)\n", regStr(dest), decl->symbolEntry->stackOffset);
// }

void compileFunc(FuncDef *func)
{
    // displayParameterLocations(func->args);
    fprintf(outFile, ".globl %s\n", func->ident);
    fprintf(outFile, ".type %s, @function\n", func->ident);
    fprintf(outFile, "%s:\n", func->ident);
    fprintf(outFile, "\tmv fp, sp\n");
    fprintf(outFile, "\taddi sp, sp, -%lu\n", func->symbolEntry->size);
    // TODO: Figure out if FP needs to be restored
    fprintf(outFile, "\tsw fp, 0(fp)\n");  // Save FP, never gets restored
    fprintf(outFile, "\tsw ra, -4(fp)\n"); // Save RA
    
    if(func->isParam)
    {
        compileFuncArgs(func->args);
    }
    

    if(func->body != NULL)
    {
    for (size_t i = 0; i < func->body->compoundStmt->declList.size; i++)
    {
        if (func->body->compoundStmt->declList.decls[i]->declInit->initExpr != NULL)
        {
            compileExpr(func->body->compoundStmt->declList.decls[i]->declInit->initExpr, A0);
            fprintf(outFile, "\tsw %s, -%lu(fp)\n", regStr(A0), func->body->compoundStmt->declList.decls[i]->symbolEntry->stackOffset);
        }
    }

        for (size_t i = 0; i < func->body->compoundStmt->stmtList.size; i++)
        {
            compileStmt(func->body->compoundStmt->stmtList.stmts[i]);
        }
    }

    fprintf(outFile, "\tmv sp, fp\n");
    fprintf(outFile, "\tret\n");
}

void compileCallArgs(FuncExpr *expr)
{
    Reg intRegs[8] = {A0, A1, A2, A3, A4, A5, A6, A7};
    Reg floatRegs[8] = {FA0, FA1, FA2, FA3, FA4, FA5, FA6, FA7};

    size_t maxFloatRegs = 8;
    size_t maxIntRegs = 8;

    size_t usedFloatRegs = 0;
    size_t usedIntRegs = 0;

    for (size_t i = 0; i < expr->argsSize; i++)
    {
        // for primitive types
        DataType paramType = returnType(expr->args[i]);
        // DataType paramType = declList.decls[i]->symbolEntry->type.dataType;
        // char *ident = declList.decls[i]->symbolEntry->ident;
        // size_t stackOffset = declList.decls[i]->symbolEntry->stackOffset;

        if (paramType == INT_TYPE || paramType == CHAR_TYPE || paramType == SHORT_TYPE)
        {
            if (usedIntRegs != maxIntRegs)
            {
                for (size_t j = 0; j < 8; j++)
                {
                    if (intRegs[j] != ZERO)
                    {
                        compileExpr(expr->args[i], A0 + j);
                        // printf("%s : A%zu \n", ident, i);
                        intRegs[j] = ZERO;
                        usedIntRegs++;
                        break;
                    }
                }
            }
            // else
            // {
            //     printf("%s : SP(%zu) \n", ident, stackOffset);
            // }
        }
        else if (paramType == FLOAT_TYPE || paramType == DOUBLE_TYPE)
        {
            if (usedFloatRegs != maxFloatRegs)
            {
                for (size_t j = 0; j < 8; j++)
                {
                    if (floatRegs[j] != ZERO)
                    {
                        if (paramType == FLOAT_TYPE)
                        {
                            compileExpr(expr->args[i], FA0 + j);
                        }
                        else
                        {
                            compileExpr(expr->args[i], FA0 + j);
                        }
                        // printf("%s : FA%zu\n", ident, i);
                        floatRegs[i] = ZERO;
                        usedFloatRegs++;
                        break;
                    }
                }
            }
            // else
            // {
            //     printf("%s : SP(%zu) \n", ident, stackOffset);
            //
            // }
        }
        // else if (paramType == LONG_TYPE )
        // {
        //     // both halves stored in registers
        //     if(usedFloatRegs < maxFloatRegs - 2)
        //     {
        //         // find next even register
        //         for(size_t i = 0; i < 8; i++)
        //         {
        //             if(i%2 == 0 && intRegs[i] != ZERO && intRegs[i+1] != ZERO)
        //             {
        //                 printf("%s : A%zu \n", ident, i);
        //                 printf("%s : A%zu \n", ident, i+1);
        //                 intRegs[i] = ZERO;
        //                 intRegs[i+1] = ZERO;
        //                 usedIntRegs += 2;
        //                 break;
        //             }
        //         }
        //     }
        // }
    }
}

// just return number
// char = 1 byte
// short = 2 bytes (half word)
// long = int = float = 4 bytes (word)
// long long = double = 8 bytes (2 words)
// a0 - a7 are integer arg regs
// fa0 - fa07 are float regs

// floating point registers are 64 bit
// a floating point is stored in half of this register
// a double takes an entire register

// longs have their high half stored in an even register and lower in an odd
void compileFuncArgs(DeclarationList declList)
{
    Reg intRegs[8] = {A0, A1, A2, A3, A4, A5, A6, A7};
    Reg floatRegs[8] = {FA0, FA1, FA2, FA3, FA4, FA5, FA6, FA7};

    size_t maxFloatRegs = 8;
    size_t maxIntRegs = 8;

    size_t usedFloatRegs = 0;
    size_t usedIntRegs = 0;

    for (size_t i = 0; i < declList.size; i++)
    {
        // for primitive types
        DataType paramType = declList.decls[i]->symbolEntry->type.dataType;
        // char *ident = declList.decls[i]->symbolEntry->ident;
        size_t stackOffset = declList.decls[i]->symbolEntry->stackOffset;

        if (paramType == INT_TYPE || paramType == CHAR_TYPE || paramType == SHORT_TYPE)
        {
            if (usedIntRegs != maxIntRegs)
            {
                for (size_t j = 0; j < 8; j++)
                {
                    if (intRegs[j] != ZERO)
                    {
                        fprintf(outFile, "\tsw a%lu, -%lu(fp)\n", j, stackOffset);
                        intRegs[j] = ZERO;
                        usedIntRegs++;
                        break;
                    }
                }
            }
            // else
            // {
            //     printf("%s : SP(%zu) \n", ident, stackOffset);
            // }
        }
        else if (paramType == FLOAT_TYPE || paramType == DOUBLE_TYPE)
        {
            if (usedFloatRegs != maxFloatRegs)
            {
                for (size_t j = 0; j < 8; j++)
                {
                    if (floatRegs[j] != ZERO)
                    {
                        if (paramType == FLOAT_TYPE)
                        {
                            fprintf(outFile, "\tfsw fa%lu, -%lu(fp)\n", j, stackOffset);
                        }
                        else
                        {
                            fprintf(outFile, "\tfsd fa%lu, -%lu(fp)\n", j, stackOffset);
                        }
                        floatRegs[i] = ZERO;
                        usedFloatRegs++;
                        break;
                    }
                }
            }
            // else
            // {
            //     printf("%s : SP(%zu) \n", ident, stackOffset);
            //
            // }
        }
    }
}


void compileTranslationUnit(TranslationUnit *transUnit)
{
    for(size_t i = 0; i < transUnit->size; i++)
    {
        if(transUnit->externDecls[i]->isFunc)
        {
            compileFunc(transUnit->externDecls[i]->funcDef);
        }
    }
}
