#include <stdio.h>
#include <stdlib.h>

#include "parser.tab.h"

Stmt *rootExpr;

// Convert a token into a string representation
const char *token_to_string(yytoken_kind_t token)
{
    switch (token)
    {
    case YYEMPTY:
        return "EMPTY";
    case YYEOF:
        return "EOF";
    case YYerror:
        return "ERROR";
    case YYUNDEF:
        return "UNDEFINED";
    case IDENTIFIER:
        return "Identifier";
    case INT_CONSTANT:
        return "Int";
    case FLOAT_CONSTANT:
        return "Float";
    case STRING_LITERAL:
        return "String";
    case PTR_OP:
        return "Ptr";
    case INC_OP:
        return "Inc";
    case DEC_OP:
        return "Dec";
    case LEFT_OP: // TODO: what is this?
    case RIGHT_OP:
    case LE_OP:
        return "LessThanEqual";
    case GE_OP:
        return "GreaterThanEqual";
    case EQ_OP:
        return "Equal";
    case NE_OP:
        return "NotEqual";
    case AND_OP:
        return "AndBit";
    case OR_OP:
        return "OrBit";
    case MUL_ASSIGN:
        return "MulAssign";
    case DIV_ASSIGN:
        return "DivAssign";
    case MOD_ASSIGN:
        return "ModAssign";
    case ADD_ASSIGN:
        return "AddAssign";
    case SUB_ASSIGN:
        return "SubAssign";
    case LEFT_ASSIGN: // TODO: What is this?
    case RIGHT_ASSIGN:
    case AND_ASSIGN:
    case XOR_ASSIGN:
    case OR_ASSIGN:
    case TYPE_NAME: // TODO: What is this (never seen in lexer)?
    case TYPEDEF:
        return "Typedef";
    case EXTERN:
        return "Extern";
    case STATIC:
        return "Static";
    case AUTO:
        return "Auto";
    case REGISTER:
        return "Register";
    case SIZEOF:
        return "Sizeof";
    case CHAR:
        return "Char";
    case SHORT:
        return "Short";
    case INT:
        return "Int";
    case LONG:
        return "Long";
    case SIGNED:
        return "Signed";
    case UNSIGNED:
        return "Unsigned";
    case FLOAT:
        return "Float";
    case DOUBLE:
        return "Double";
    case CONST:
        return "Const";
    case VOLATILE:
        return "Volatile";
    case VOID:
        return "Void";
    case STRUCT:
        return "Struct";
    case UNION:
        return "Union";
    case ENUM:
        return "Enum";
    case ELLIPSIS:
        return "...";
    case CASE:
        return "Case";
    case DEFAULT:
        return "Default";
    case IF:
        return "If";
    case ELSE:
        return "Else";
    case SWITCH:
        return "Switch";
    case WHILE:
        return "While";
    case DO:
        return "Do";
    case FOR:
        return "For";
    case GOTO:
        return "Goto";
    case CONTINUE:
        return "Continue";
    case BREAK:
        return "Break";
    case RETURN:
        return "Return";
    case SEMI_COLON:
        return ":";
    case OPEN_BRACE:
        return "{";
    case CLOSE_BRACE:
        return "}";
    case COMMA:
        return ",";
    case COLON:
        return ":";
    case ASSIGN:
        return "Assign";
    case OPEN_BRACKET:
        return "(";
    case CLOSE_BRACKET:
        return ")";
    case OPEN_SQUARE:
        return "[";
    case CLOSE_SQUARE:
        return "]";
    case PERIOD:
        return ".";
    case AND_LOGIC:
        return "And";
    case NOT_LOGIC:
        return "Not";
    case NOT_OP:
        return "NotBit";
    case SUB_OP:
        return "Sub";
    case ADD_OP:
        return "Add";
    case MUL_OP:
        return "Mul";
    case DIV_OP:
        return "Div";
    case MOD_OP:
        return "Mod";
    case LT_OP:
        return "LessThan";
    case GT_OP:
        return "GreaterThan";
    case XOR_OP:
        return "Xor";
    case OR_LOGIC:
        return "Or";
    case TERN_OP:
        return "Ternary";
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

    yytoken_kind_t token;
    while ((token = yylex()) != 0)
    {
        printf("%s", token_to_string(token));
        if (token == IDENTIFIER || token == STRING_LITERAL)
        {
            printf("(%s)", yylval.string);
            free(yylval.string);
        }
        if (token == INT_CONSTANT)
        {
            printf("(%i)", yylval.number_int);
        }
        if (token == FLOAT_CONSTANT)
        {
            printf("(%f)", yylval.number_float);
        }
        printf(" ");
    }
    printf("\n");
    return EXIT_SUCCESS;
}
