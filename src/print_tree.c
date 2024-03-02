#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include "ast.h"

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

void printIndent(int indentSize){
    for(int i = 0; i < indentSize; i++){
        printf(" ");
    }
}

void displayOpExpr(OperationExpr *opExpr){
    Operator operator = opExpr->operator;
    switch(operator){
        case(ADD):
            printf("ADD");
        case(SUB):
            printf("SUB");
        case(MUL):
            printf("MUL");
        case(DIV):
            printf("DIV");
        case(AND):
            printf("AND");
        case(OR):
            printf("OR");
        case(NOT):
            printf("NOT");
    }
    printf("\n");
}

void displayVarExpr(VariableExpr *varExpr){
    printf("%s\n", varExpr->ident);
}

void displayConstExpr(ConstantExpr *constExpr){
    switch(constExpr->type){ // doesnt do string literal
        case(CHAR_TYPE):
            printf("%c\n", constExpr->char_const);
        case(INT_TYPE):
            printf("%i\n", constExpr->int_const);
        case(FLOAT_TYPE):
            printf("%f\n", constExpr->float_const); 
    }
}

void displayAssignExpr(AssignExpr *assignExpr){
    printf("ASSIGN OP\n");
}

void displayFuncExpr(FuncExpr *funcExpr){
    printf("%s()\n", funcExpr->ident);
}

void displayExpr(Expr *expr, int indent){
    if(expr == NULL){
        // some expr operands aren't set
        return;
    }
    
    printIndent(indent);
    if(indent > 0){
        printf("└──");
    }

    switch(expr->type){
        case VARIABLE_EXPR:
            displayVarExpr(expr->variable);
        case CONSTANT_EXPR:
            displayConstExpr(expr->constant);
        case OPERATION_EXPR:
            displayOpExpr(expr->operation);

            displayExpr(&expr->operation->op3, indent + 4);
            displayExpr(&expr->operation->op2, indent + 4);
            displayExpr(&expr->operation->op1, indent + 4);

        case ASSIGN_EXPR:
            displayAssignExpr(expr->assignment);
            displayExpr(&expr->assignment->op, indent + 4);
            
            // indented variable name
            printIndent(indent + 4);
            printf("└──%s\n", expr->assignment->ident);

        case FUNC_EXPR:
            displayFuncExpr(expr->function);

    }
}

int main() {

}
