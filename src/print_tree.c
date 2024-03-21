#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "parser.tab.h"
#include "symbol.h"

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
void displayStmt(Stmt *stmt, int indent);

void printIndent(int indentSize)
{
    for (int i = 0; i < indentSize; i++)
    {
        printf(" ");
    }
}

void displayOpExpr(OperationExpr *opExpr)
{
    switch (opExpr->operator)
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
        printf("<");
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
        printf("CHAR %c\n", constExpr->char_const);
        break;
    case INT_TYPE:
        printf("INT %i\n", constExpr->int_const);
        break;
    case FLOAT_TYPE:
        printf("FLOAT %f\n", constExpr->float_const);
        break;
    case UNSIGNED_INT_TYPE:
        printf("UNSIGNED INT %i\n", constExpr->int_const);
        break;
    }
}

void displayAssignExpr(AssignExpr *assignExpr)
{
    switch (assignExpr->operator)
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
        printf("%%=");
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
    default:
        printf("unknown assignment");
    }
    printf("\n");
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
        printf("└── ");
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
        printf("└── %s\n", expr->assignment->ident);

        break;
    case FUNC_EXPR:
        displayFuncExpr(expr->function, indent);
        break;
    }
}

void displayWhileStmt(WhileStmt *whileStmt, int indent)
{
    if (whileStmt->doWhile)
    {
        printf("DO WHILE\n");
    }
    else
    {
        printf("WHILE\n");
    }

    displayExpr(whileStmt->condition, indent + 4);
    displayStmt(whileStmt->body, indent + 4);
}

void displayForStmt(ForStmt *forStmt, int indent)
{
    printf("FOR\n");
    displayStmt(forStmt->init, indent + 4);
    displayStmt(forStmt->condition, indent + 4);
    displayExpr(forStmt->modifier, indent + 4);
    displayStmt(forStmt->body, indent + 4);
}

void displayIfStmt(IfStmt *ifStmt, int indent)
{
    printf("IF\n");
    displayExpr(ifStmt->condition, indent + 4);
    displayStmt(ifStmt->trueBody, indent + 4);
    displayStmt(ifStmt->falseBody, indent + 4);
}

void displaySwitchStmt(SwitchStmt *switchStmt, int indent)
{
    printf("SWITCH\n");
    displayExpr(switchStmt->selector, indent + 4);
    displayStmt(switchStmt->body, indent + 4);
}

void displayExprStmt(ExprStmt *exprStmt, int indent)
{
    displayExpr(exprStmt->expr, indent);
}

void displayStructSpec(StructSpecifier *structSpec, int indent)
{
    printf("STRUCT (%s)\n", structSpec->ident);
    for (size_t i = 0; i < structSpec->structDeclList->structDeclListSize; i++)
    {
        printIndent(indent + 4);
        printf("└── ");
        StructDecl *currStructDecl = structSpec->structDeclList->structDecls[i];
        // display types:
        for (size_t i = 0; i < currStructDecl->typeSpecList->typeSpecSize; i++)
        {
            switch (currStructDecl->typeSpecList->typeSpecs[i]->dataType)
            {
            case VOID_TYPE:
                printf("VOID ");
                break;
            case CHAR_TYPE:
                printf("CHAR ");
                break;
            case SHORT_TYPE:
                printf("SHORT ");
                break;
            case INT_TYPE:
                printf("INT ");
                break;
            case LONG_TYPE:
                printf("LONG ");
                break;
            case FLOAT_TYPE:
                printf("FLOAT ");
                break;
            case DOUBLE_TYPE:
                printf("DOUBLE ");
                break;
            case SIGNED_TYPE:
                printf("SIGNED ");
                break;
            case UNSIGNED_TYPE:
                printf("UNSIGNED ");
                break;
            }
        }

        // display identifier
        printf("\n");
        printIndent(indent + 8);
        printf("└── ");
        printf("%s\n", currStructDecl->declarator->ident);

        // print bitfield
        if (currStructDecl->bitField != NULL)
        {
            printIndent(indent + 8);
            printf("└── BITFIELD\n");
            displayExpr(currStructDecl->bitField, indent + 12);
        }
    }
}

void displayTypeSpec(TypeSpecifier *typeSpec, int indent)
{
    printIndent(indent);
    printf("└── ");
    if (typeSpec->isStruct == false)
    {
        switch (typeSpec->dataType)
        {
        case VOID_TYPE:
            printf("VOID ");
            break;
        case CHAR_TYPE:
            printf("CHAR ");
            break;
        case SHORT_TYPE:
            printf("SHORT ");
            break;
        case INT_TYPE:
            printf("INT ");
            break;
        case LONG_TYPE:
            printf("LONG ");
            break;
        case FLOAT_TYPE:
            printf("FLOAT ");
            break;
        case DOUBLE_TYPE:
            printf("DOUBLE ");
            break;
        case SIGNED_TYPE:
            printf("SIGNED ");
            break;
        case UNSIGNED_TYPE:
            printf("UNSIGNED ");
            break;
        }
        printf("\n");
    }
    else
    {
        displayStructSpec(typeSpec->structSpecifier, indent);
    }
}

void displayDecl(Decl *decl, int indent)
{
    // Do we need this for statements
    if (decl == NULL)
    {
        // some expr operands aren't set
        return;
    }

    printIndent(indent);
    if (indent > 0)
    {
        printf("└── ");
    }

    printf("DECL \n");

    // print type specifiers
    for (size_t i = 0; i < decl->typeSpecList->typeSpecSize; i++)
    {
        displayTypeSpec(decl->typeSpecList->typeSpecs[i], indent + 4);
    }

    if (decl->declInit != NULL)
    {
        printIndent(indent + 4);
        printf("└── ");
        printf("%s \n", decl->declInit->declarator->ident);
        if (decl->declInit->declarator->isArray)
        {
            printIndent(indent + 4);
            printf("└── ");
            printf("ARRAY SIZE\n");
            displayExpr(decl->declInit->declarator->arraySize, indent + 8);
        }

        if (decl->declInit->initList != NULL)
        {
            printIndent(indent + 4);
            printf("└── ");
            printf("ARRAY INIT\n");
        }
        if (decl->declInit->initExpr != NULL)
        {
            displayExpr(decl->declInit->initExpr, indent + 8);
        }
    }
}

void displayCompoundStmt(CompoundStmt *compoundStmt, int indent)
{
    printf("CMPD STMT\n");

    if (compoundStmt->declList.size != 0)
    {
        for (size_t i = 0; i < compoundStmt->declList.size; i++)
        {
            displayDecl(compoundStmt->declList.decls[i], indent + 4);
        }
    }

    if (compoundStmt->stmtList.size != 0)
    {
        for (size_t i = 0; i < compoundStmt->stmtList.size; i++)
        {
            displayStmt(compoundStmt->stmtList.stmts[i], indent + 4);
        }
    }
}

void displayLabelStmt(LabelStmt *labelStmt, int indent)
{
    if (labelStmt->ident != NULL) // label:
    {
        printf("LABEL: %s\n", labelStmt->ident);
    }

    if (labelStmt->caseLabel != NULL) // case:
    {
        printf("CASE\n");
        displayExpr(labelStmt->caseLabel, indent + 4);
    }

    if (labelStmt->caseLabel == NULL && labelStmt->ident == NULL) // default:
    {
        printf("DEFAULT\n");
    }
    displayStmt(labelStmt->body, indent + 4);
}

void displayJumpStmt(JumpStmt *jumpStmt, int indent)
{
    switch (jumpStmt->type)
    {
    case GOTO_JUMP:
        printf("GOTO %s\n", jumpStmt->ident);
        break;

    case CONTINUE_JUMP:
        printf("CONTINUE\n");
        break;

    case BREAK_JUMP:
        printf("BREAK\n");
        break;

    case RETURN_JUMP:
        printf("RETURN\n");
        if (jumpStmt->expr != NULL)
        {
            displayExpr(jumpStmt->expr, indent + 4);
        }
    }
}

void displayStmt(Stmt *stmt, int indent)
{
    // Do we need this for statements
    if (stmt == NULL)
    {
        // some expr operands aren't set
        return;
    }

    printIndent(indent);
    if (indent > 0)
    {
        printf("└── ");
    }

    switch (stmt->type)
    {
    case WHILE_STMT:
        displayWhileStmt(stmt->whileStmt, indent);
        break;
    case FOR_STMT:
        displayForStmt(stmt->forStmt, indent);
        break;
    case IF_STMT:
        displayIfStmt(stmt->ifStmt, indent);
        break;
    case SWITCH_STMT:
        displaySwitchStmt(stmt->switchStmt, indent);
        break;
    case EXPR_STMT:
        displayExprStmt(stmt->exprStmt, indent);
        break;
    case COMPOUND_STMT:
        displayCompoundStmt(stmt->compoundStmt, indent);
        break;
    case LABEL_STMT:
        displayLabelStmt(stmt->labelStmt, indent);
        break;
    case JUMP_STMT:
        displayJumpStmt(stmt->jumpStmt, indent);
        break;
    }
}

void displayFuncDef(FuncDef *funcDef, int indent)
{
    printIndent(indent);
    if (indent > 0)
    {
        printf("└── ");
    }
    printf("FUNC DEF (%s)\n", funcDef->ident);

    // print return type
    printIndent(indent + 4);
    for (size_t i = 0; i < funcDef->retType->typeSpecSize; i++)
    {
        displayTypeSpec(funcDef->retType->typeSpecs[i], indent);
    }

    if (funcDef->isParam)
    {
        for (size_t i = 0; i < funcDef->args.size; i++)
        {
            displayDecl(funcDef->args.decls[i], indent + 4);
        }
    }

    if (funcDef->body != NULL)
    {
        displayStmt(funcDef->body, indent + 4);
    }
}

void displayTranslationUnit(TranslationUnit *transUnit, int indent)
{
    for (size_t i = 0; i < transUnit->size; i++)
    {
        

        if (transUnit->externDecls[i]->isFunc)
        {
            displayFuncDef(transUnit->externDecls[i]->funcDef, indent);
        }
        else
        {
            displayDecl(transUnit->externDecls[i]->decl, 0);
        }
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

    yyparse();
    // if (yyparse())
    // {
    //     fprintf(stderr, "Error: parsing unsuccessful\n");
    // }

    displayTranslationUnit(root, 0);

    SymbolTable *globalTable = populateSymbolTable(root);
    displaySymbolTable(globalTable);
    transUnitDestroy(root);
    symbolTableDestroy(globalTable);

    if (yyin != NULL)
    {
        fclose(yyin);
    }
    return EXIT_SUCCESS;
}
