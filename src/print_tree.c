#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "parser.tab.h"

Expr *rootExpr;

// will probably need this later...
// typedef struct {
//     ExprType type;
//     union {
//         struct VariableExpr *variable;
//         struct ConstantExpr *constant;
//         struct OperationExpr *operation;
//         struct AssignExpr *assignment;
//         struct FuncExpr *function;
//     };
// } GenericNode;

void displayExpr(Expr *expr, int indent);

void printIndent(int indentSize)
{
    for (int i = 0; i < indentSize; i++)
    {
        printf(" ");
    }
}

void displayOpExpr(OperationExpr *opExpr)
{
    Operator operator= opExpr->operator;
    switch (operator)
    {
    case ADD:
        printf("+");
        break;
    case SUB:
        printf("-");
        break;
    case MUL:
        printf("*");
        break;
    case DIV:
        printf("/");
        break;
    case AND:
        printf("&&");
        break;
    case OR:
        printf("||");
        break;
    case NOT:
        printf("!");
        break;
    case AND_BIT:
        printf("&");
        break;
    case OR_BIT:
        printf("|");
        break;
    case NOT_BIT:
        printf("~");
        break;
    case XOR:
        printf("^");
        break;
    case EQ:
        printf("==");
        break;
    case NE:
        printf("!=");
        break;
    case LT:
        printf(",");
        break;
    case GT:
        printf(">");
        break;
    case LE:
        printf("<=");
        break;
    case GE:
        printf(">=");
        break;
    case LEFT_SHIFT:
        printf("<<");
        break;
    case RIGHT_SHIFT:
        printf(">>");
        break;
    case DEC_POST:
        printf("-- (POST)");
        break;
    case INC_POST:
        printf("++ (POST)");
        break;
    case DEC:
        printf("--");
        break;
    case INC:
        printf("++");
        break;
    case TERN:
        printf("?");
        break;
    case COMMA_OP:
        printf(",");
        break;
    case SIZEOF_OP:
        printf("SIZEOF");
        break;
    case ADDRESS:
        printf("&");
        break;
    case DEREF:
        printf("DEREF");
        break;
    default:
        printf("UNKNOWN OP");
        break;
    }
    printf("\n");
}

void displayVarExpr(VariableExpr *varExpr)
{
    printf("%s\n", varExpr->ident);
}

void displayConstExpr(ConstantExpr *constExpr)
{
    switch (constExpr->type)
    { // doesnt do string literal
    case CHAR_TYPE:
        printf("%c\n", constExpr->char_const);
        break;
    case INT_TYPE:
        printf("%i\n", constExpr->int_const);
        break;
    case FLOAT_TYPE:
        printf("%f\n", constExpr->float_const);
        break;
    }
}

void displayAssignExpr(AssignExpr *assignExpr)
{
    Operator operator= assignExpr->operator;
    switch (operator)
    {
    case NOT:
        printf("=");
        break;
    case MUL:
        printf("*=");
        break;
    case DIV:
        printf("/=");
        break;
    case MOD:
        printf("%=");
        break;
    case ADD:
        printf("+=");
        break;
    case SUB:
        printf("-=");
        break;
    case LEFT_SHIFT:
        printf("<<=");
        break;
    case RIGHT_SHIFT:
        printf(">>=");
        break;
    case AND:
        printf("&=");
        break;
    case XOR:
        printf("^=");
        break;
    case OR:
        printf("|=");
        break;
    }
}

void displayFuncExpr(FuncExpr *funcExpr, int indent)
{
    printf("%s()\n", funcExpr->ident);
    for (size_t i = 0; i < funcExpr->argsSize; i++)
    {
        displayExpr(funcExpr->args[i], indent + 4);
    }
}

void displayExpr(Expr *expr, int indent)
{
    if (expr == NULL)
    {
        // some expr operands aren't set
        return;
    }

    printIndent(indent);
    if (indent > 0)
    {
        printf("└──");
    }

    switch (expr->type)
    {
    case VARIABLE_EXPR:
        displayVarExpr(expr->variable);
        break;
    case CONSTANT_EXPR:
        displayConstExpr(expr->constant);
        break;
    case OPERATION_EXPR:
        displayOpExpr(expr->operation);

        displayExpr(expr->operation->op3, indent + 4);
        displayExpr(expr->operation->op2, indent + 4);
        displayExpr(expr->operation->op1, indent + 4);
        break;
    case ASSIGN_EXPR:
        displayAssignExpr(expr->assignment);
        displayExpr(expr->assignment->op, indent + 4);

        // indented variable name
        printIndent(indent + 4);
        printf("└──%s\n", expr->assignment->ident);

        break;
    case FUNC_EXPR:
        displayFuncExpr(expr->function, indent);
        break;
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Info: No path provided, reading from the standard input...\n");
    }
    else
    {
        if (argc > 2)
        {
            fprintf(stderr, "Usage: print_tokens PATH\n");
            return EXIT_FAILURE;
        }
        yyin = fopen(argv[1], "r");
        if (yyin == NULL)
        {
            fprintf(stderr, "Error: Failed to open file, aborting...\n");
            return EXIT_FAILURE;
        }
    }
    if (yyparse())
    {
        fprintf(stderr, "Error: parsing unnsuccessful\n");
    }
    displayExpr(rootExpr, 0);
    exprDestroy(rootExpr);
    return EXIT_SUCCESS;
}