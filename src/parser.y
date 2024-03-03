// Adapted from: https://www.lysator.liu.se/c/ANSI-C-grammar-y.html

%code requires{
    #include <stdlib.h>
    #include <stdio.h>
    #include <stdint.h>
    #include <stdbool.h>

    #include "src/ast.h"

    // extern Node *g_root;
    extern Expr *rootExpr;
    extern FILE *yyin;
    int yylex(void);
    void yyerror(const char *);
}

// Represents the value associated with any kind of AST node.
%union{
    char* string;
    int32_t number_int;
    float number_float;
    Expr* expr_node;
    FuncExpr* func_node;
    Operator operator;
}

%token IDENTIFIER INT_CONSTANT FLOAT_CONSTANT STRING_LITERAL
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP AND_OP OR_OP
%token MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN
%token TYPE_NAME TYPEDEF EXTERN STATIC AUTO REGISTER SIZEOF
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token STRUCT UNION ENUM ELLIPSIS
%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN
%token SEMI_COLON OPEN_BRACE CLOSE_BRACE COMMA COLON ASSIGN 
%token OPEN_BRACKET CLOSE_BRACKET OPEN_SQUARE CLOSE_SQUARE
%token PERIOD AND_LOGIC NOT_LOGIC NOT_OP SUB_OP ADD_OP MUL_OP DIV_OP MOD_OP
%token LT_OP GT_OP XOR_OP OR_LOGIC TERN_OP

%type <expr_node> translation_unit external_declaration function_definition primary_expression postfix_expression
%type <func_node> argument_expression_list
%type <expr_node> unary_expression cast_expression multiplicative_expression additive_expression shift_expression relational_expression
%type <expr_node> equality_expression and_expression exclusive_or_expression inclusive_or_expression logical_and_expression logical_or_expression
%type <expr_node> conditional_expression assignment_expression expression constant_expression declaration declaration_specifiers init_declarator_list
%type <node> init_declarator type_specifier struct_specifier struct_declaration_list struct_declaration specifier_qualifier_list struct_declarator_list
%type <node> struct_declarator enum_specifier enumerator_list enumerator declarator direct_declarator pointer parameter_list parameter_declaration
%type <node> identifier_list type_name abstract_declarator direct_abstract_declarator initializer initializer_list statement labeled_statement
%type <node> compound_statement declaration_list expression_statement selection_statement iteration_statement jump_statement

%type <nodes> statement_list
%type <operator> unary_operator assignment_operator

%type <string> storage_class_specifier

%type <number_int> INT_CONSTANT
%type <number_float> FLOAT_CONSTANT
%type <string> IDENTIFIER STRING_LITERAL


%start expression
%%

ROOT
  : translation_unit { 
        // g_root = $1; 
    }

translation_unit
	: external_declaration { $$ = $1; }
	| translation_unit external_declaration
	;

external_declaration
	: function_definition { $$ = $1; }
	| declaration
	;

function_definition
	: declaration_specifiers declarator declaration_list compound_statement
	| declaration_specifiers declarator compound_statement {
		// $$ = new FunctionDefinition($1, $2, $3);
	}
	| declarator declaration_list compound_statement
	| declarator compound_statement
	;


primary_expression
	: IDENTIFIER {
        $$ = exprCreate(VARIABLE_EXPR);
        $$->variable = variableExprCreate($1);
    }
	| INT_CONSTANT {
        $$ = exprCreate(CONSTANT_EXPR);
		$$->constant = constantExprCreate(INT_TYPE, false);
        $$->constant->int_const = $1;
	}
    | FLOAT_CONSTANT {
        $$ = exprCreate(CONSTANT_EXPR);
		$$->constant = constantExprCreate(FLOAT_TYPE, false);
        $$->constant->int_const = $1;
    }
	| STRING_LITERAL {
        $$ = exprCreate(CONSTANT_EXPR);
		$$->constant = constantExprCreate(CHAR_TYPE, true);
        $$->constant->string_const = $1;
    }
	| OPEN_BRACKET expression CLOSE_BRACKET { $$ = $2; }
	;

postfix_expression
	: primary_expression { $$ = $1; }
	| postfix_expression OPEN_SQUARE expression CLOSE_SQUARE {
        $$ = exprCreate(OPERATION_EXPR);
        $$->operation = operationExprCreate(DEREF);
        $$->operation->op1 = exprCreate(OPERATION_EXPR);
        $$->operation->op1->operation = operationExprCreate(ADD);
        $$->operation->op1->operation->op1 = $1; // Potentially may have to manually override the primative type field
        $$->operation->op1->operation->op1 = $3;
        }
	| postfix_expression OPEN_BRACKET CLOSE_BRACKET {
        if($1->type == VARIABLE_EXPR)
        {
           $$ = exprCreate(FUNC_EXPR);
           $$->function = funcExprCreate(0);
           $$->function->ident = $1->variable->ident;
           $1->variable->ident = NULL;
           exprDestroy($1);
        }
        else
        {
            fprintf(stderr, "Called object is not a function, exitting..."); // TODO: Maybe add the type of the object in the error message
            exit(-1);
        }   
        }
	| postfix_expression OPEN_BRACKET argument_expression_list CLOSE_BRACKET {
        if($1->type == VARIABLE_EXPR)
        {
           $$ = exprCreate(FUNC_EXPR);
           $$->function = $3;
           $$->function->ident = $1->variable->ident;
           $1->variable->ident = NULL;
           exprDestroy($1);
        }
        else
        {
            fprintf(stderr, "Called object is not a function, exitting..."); // TODO: Maybe add the type of the object in the error message
            exit(-1);
        }   
        }
	| postfix_expression PERIOD IDENTIFIER
	| postfix_expression PTR_OP IDENTIFIER
	| postfix_expression INC_OP {
        $$ = exprCreate(OPERATION_EXPR);
        $$->operation = operationExprCreate(INC_POST);
        $$->operation->op1 = $1;
        }
	| postfix_expression DEC_OP {
        $$ = exprCreate(OPERATION_EXPR);
        $$->operation = operationExprCreate(DEC_POST);
        $$->operation->op1 = $1;
        }
	;

argument_expression_list
	: assignment_expression { 
        $$ = funcExprCreate(1);
        $$->args[0] = $1;
        }
	| argument_expression_list COMMA assignment_expression {
        $$ = $1;
        funcExprArgsPush($$, $3);
        }
	;

unary_expression
	: postfix_expression { $$ = $1; }
	| INC_OP unary_expression {
        $$ = exprCreate(OPERATION_EXPR);
        $$->operation = operationExprCreate(INC);
        $$->operation->op1 = $2;
        }
	| DEC_OP unary_expression {
        $$ = exprCreate(OPERATION_EXPR);
        $$->operation = operationExprCreate(DEC);
        $$->operation->op1 = $2;
        }
	| unary_operator cast_expression {
        $$ = exprCreate(OPERATION_EXPR);
        $$->operation = operationExprCreate($1);
        $$->operation->op1 = $2;
        }
	| SIZEOF unary_expression {
        $$ = exprCreate(OPERATION_EXPR);
        $$->operation = operationExprCreate(SIZEOF_OP);
        $$->operation->op1 = $2;
        }
	| SIZEOF OPEN_BRACKET type_name CLOSE_BRACKET
	;
 
unary_operator
	: AND_OP { $$ = ADDRESS; } 
	| MUL_OP { $$ = DEREF; }
	| ADD_OP { $$ = ADD; }
	| SUB_OP { $$ = SUB; }
	| NOT_OP { $$ = NOT_BIT; }
	| NOT_LOGIC { $$ = NOT; }
	;

cast_expression
	: unary_expression { $$ = $1; }
	| OPEN_BRACKET type_name CLOSE_BRACKET cast_expression {
            $$ = $4;
            fprintf(stderr, "Casts not implemented, ignoring...");
        } // Casts out of spec
	;

multiplicative_expression
	: cast_expression { $$ = $1; }
	| multiplicative_expression MUL_OP cast_expression {
        $$ = exprCreate(OPERATION_EXPR);
        $$->operation = operationExprCreate(MUL);
        $$->operation->op1 = $1;
        $$->operation->op2 = $3;
        }
	| multiplicative_expression DIV_OP cast_expression {
        $$ = exprCreate(OPERATION_EXPR);
        $$->operation = operationExprCreate(DIV);
        $$->operation->op1 = $1;
        $$->operation->op2 = $3;
        }
	| multiplicative_expression MOD_OP cast_expression {
        $$ = exprCreate(OPERATION_EXPR);
        $$->operation = operationExprCreate(MOD);
        $$->operation->op1 = $1;
        $$->operation->op2 = $3;
        }
	;

additive_expression
	: multiplicative_expression { $$ = $1; }
	| additive_expression ADD_OP multiplicative_expression {
        $$ = exprCreate(OPERATION_EXPR);
        $$->operation = operationExprCreate(ADD);
        $$->operation->op1 = $1;
        $$->operation->op2 = $3;
        }
	| additive_expression SUB_OP multiplicative_expression {
        $$ = exprCreate(OPERATION_EXPR);
        $$->operation = operationExprCreate(SUB);
        $$->operation->op1 = $1;
        $$->operation->op2 = $3;
        }
	;

shift_expression
	: additive_expression { $$ = $1; }
	| shift_expression LEFT_OP additive_expression {
        $$ = exprCreate(OPERATION_EXPR);
        $$->operation = operationExprCreate(LEFT_SHIFT);
        $$->operation->op1 = $1;
        $$->operation->op2 = $3;
        }
	| shift_expression RIGHT_OP additive_expression {
        $$ = exprCreate(OPERATION_EXPR);
        $$->operation = operationExprCreate(RIGHT_SHIFT);
        $$->operation->op1 = $1;
        $$->operation->op2 = $3;
        }
	;

relational_expression
	: shift_expression { $$ = $1; }
	| relational_expression LT_OP shift_expression {
        $$ = exprCreate(OPERATION_EXPR);
        $$->operation = operationExprCreate(LT);
        $$->operation->op1 = $1;
        $$->operation->op2 = $3;
        }
	| relational_expression GT_OP shift_expression {
        $$ = exprCreate(OPERATION_EXPR);
        $$->operation = operationExprCreate(GT);
        $$->operation->op1 = $1;
        $$->operation->op2 = $3;
        }
	| relational_expression LE_OP shift_expression {
        $$ = exprCreate(OPERATION_EXPR);
        $$->operation = operationExprCreate(LE);
        $$->operation->op1 = $1;
        $$->operation->op2 = $3;
        }
	| relational_expression GE_OP shift_expression {
        $$ = exprCreate(OPERATION_EXPR);
        $$->operation = operationExprCreate(GE);
        $$->operation->op1 = $1;
        $$->operation->op2 = $3;
        }
	;

equality_expression
	: relational_expression { $$ = $1; }
	| equality_expression EQ_OP relational_expression {
        $$ = exprCreate(OPERATION_EXPR);
        $$->operation = operationExprCreate(EQ);
        $$->operation->op1 = $1;
        $$->operation->op2 = $3;
        }
	| equality_expression NE_OP relational_expression {
        $$ = exprCreate(OPERATION_EXPR);
        $$->operation = operationExprCreate(NE);
        $$->operation->op1 = $1;
        $$->operation->op2 = $3;
        }
	;

and_expression
	: equality_expression { $$ = $1; }
	| and_expression AND_OP equality_expression {
        $$ = exprCreate(OPERATION_EXPR);
        $$->operation = operationExprCreate(AND_BIT);
        $$->operation->op1 = $1;
        $$->operation->op2 = $3;
        }
	;

exclusive_or_expression
	: and_expression { $$ = $1; }
	| exclusive_or_expression XOR_OP and_expression {
        $$ = exprCreate(OPERATION_EXPR);
        $$->operation = operationExprCreate(XOR);
        $$->operation->op1 = $1;
        $$->operation->op2 = $3;
        }
	;

inclusive_or_expression
	: exclusive_or_expression { $$ = $1; }
	| inclusive_or_expression OR_OP exclusive_or_expression {
        $$ = exprCreate(OPERATION_EXPR);
        $$->operation = operationExprCreate(OR_BIT);
        $$->operation->op1 = $1;
        $$->operation->op2 = $3;
        }
	;

logical_and_expression
	: inclusive_or_expression { $$ = $1; }
	| logical_and_expression AND_LOGIC inclusive_or_expression {
        $$ = exprCreate(OPERATION_EXPR);
        $$->operation = operationExprCreate(AND);
        $$->operation->op1 = $1;
        $$->operation->op2 = $3;
        }
	;

logical_or_expression
	: logical_and_expression { $$ = $1; }
	| logical_or_expression OR_LOGIC logical_and_expression {
        $$ = exprCreate(OPERATION_EXPR);
        $$->operation = operationExprCreate(OR);
        $$->operation->op1 = $1;
        $$->operation->op2 = $3;
        }
	;

conditional_expression
	: logical_or_expression { $$ = $1; }
	| logical_or_expression TERN_OP expression COLON conditional_expression {
        $$ = exprCreate(OPERATION_EXPR);
        $$->operation = operationExprCreate(TERN);
        $$->operation->op1 = $1;
        $$->operation->op2 = $3;
        $$->operation->op3 = $5;
        }
	;

assignment_expression
	: conditional_expression { $$ = $1; }
	| unary_expression assignment_operator assignment_expression
	;

assignment_operator
	: ASSIGN { $$ = NOT; }
	| MUL_ASSIGN { $$ = MUL; } 
	| DIV_ASSIGN { $$ = DIV; } 
	| MOD_ASSIGN { $$ = MOD; } 
	| ADD_ASSIGN { $$ = ADD; } 
	| SUB_ASSIGN { $$ = SUB; } 
	| LEFT_ASSIGN { $$ = LEFT_SHIFT; } 
	| RIGHT_ASSIGN { $$ = RIGHT_SHIFT; } 
	| AND_ASSIGN { $$ = AND; } 
	| XOR_ASSIGN { $$ = XOR; } 
	| OR_ASSIGN { $$ = OR; } 
	;

expression
	: assignment_expression { 
        $$ = $1;
        rootExpr = $1; }
	| expression COMMA assignment_expression
	;

constant_expression
	: conditional_expression { $$ = $1; }
	;

declaration
	: declaration_specifiers SEMI_COLON
	| declaration_specifiers init_declarator_list SEMI_COLON
	;

declaration_specifiers
	: storage_class_specifier
	| storage_class_specifier declaration_specifiers
	| type_specifier {
        // $$ = $1; 
        }
	| type_specifier declaration_specifiers
	;

init_declarator_list
	: init_declarator
	| init_declarator_list COMMA init_declarator
	;

init_declarator
	: declarator
	| declarator ASSIGN initializer
	;

storage_class_specifier
	: TYPEDEF
	| EXTERN
	| STATIC
	| AUTO
	| REGISTER
	;

type_specifier
	: VOID
	| CHAR
	| SHORT
	| INT {
		// $$ = new TypeSpecifier("int");
	}
	| LONG
	| FLOAT
	| DOUBLE
	| SIGNED
	| UNSIGNED
    | struct_specifier
	| enum_specifier
	| TYPE_NAME // Never returned by the lexer
	;

struct_specifier
	: STRUCT IDENTIFIER OPEN_BRACE struct_declaration_list CLOSE_BRACE
	| STRUCT OPEN_BRACE struct_declaration_list CLOSE_BRACE
	| STRUCT IDENTIFIER
	;

struct_declaration_list
	: struct_declaration
	| struct_declaration_list struct_declaration
	;

struct_declaration
	: specifier_qualifier_list struct_declarator_list SEMI_COLON
	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list
	| type_specifier
	;

struct_declarator_list
	: struct_declarator
	| struct_declarator_list COMMA struct_declarator
	;

struct_declarator
	: declarator
	| COLON constant_expression
	| declarator COLON constant_expression
	;

enum_specifier
	: ENUM OPEN_BRACE enumerator_list CLOSE_BRACE
	| ENUM IDENTIFIER OPEN_BRACE enumerator_list CLOSE_BRACE
	| ENUM IDENTIFIER
	;

enumerator_list
	: enumerator
	| enumerator_list COMMA enumerator
	;

enumerator
	: IDENTIFIER
	| IDENTIFIER ASSIGN constant_expression
	;

declarator
	: pointer direct_declarator
	| direct_declarator { 
        // $$ = $1; 
        }
	;

direct_declarator
	: IDENTIFIER {
		// $$ = new Identifier(*$1);
		// delete $1;
	}
	| OPEN_BRACKET declarator CLOSE_BRACKET
	| direct_declarator OPEN_SQUARE constant_expression CLOSE_SQUARE
	| direct_declarator OPEN_SQUARE CLOSE_SQUARE
	| direct_declarator OPEN_BRACKET parameter_list CLOSE_BRACKET
	| direct_declarator OPEN_BRACKET identifier_list CLOSE_BRACKET
	| direct_declarator OPEN_BRACKET CLOSE_BRACKET {
		// $$ = new DirectDeclarator($1);
	}
	;

pointer
	: MUL_OP
	| MUL_OP pointer
	;

parameter_list
	: parameter_declaration
	| parameter_list COMMA parameter_declaration
	;

parameter_declaration
	: declaration_specifiers declarator
	| declaration_specifiers abstract_declarator
	| declaration_specifiers
	;

identifier_list
	: IDENTIFIER
	| identifier_list COMMA IDENTIFIER
	;

type_name
	: specifier_qualifier_list
	| specifier_qualifier_list abstract_declarator
	;

abstract_declarator
	: pointer
	| direct_abstract_declarator
	| pointer direct_abstract_declarator
	;

direct_abstract_declarator
	: OPEN_BRACKET abstract_declarator CLOSE_BRACKET
	| OPEN_SQUARE CLOSE_SQUARE
	| OPEN_SQUARE constant_expression CLOSE_SQUARE
	| direct_abstract_declarator OPEN_SQUARE CLOSE_SQUARE
	| direct_abstract_declarator OPEN_SQUARE constant_expression CLOSE_SQUARE
	| OPEN_BRACKET CLOSE_BRACKET
	| OPEN_BRACKET parameter_list CLOSE_BRACKET
	| direct_abstract_declarator OPEN_BRACKET CLOSE_BRACKET
	| direct_abstract_declarator OPEN_BRACKET parameter_list CLOSE_BRACKET
	;

initializer
	: assignment_expression
	| OPEN_BRACE initializer_list CLOSE_BRACE
	| OPEN_BRACE initializer_list COMMA CLOSE_BRACE
	;

initializer_list
	: initializer
	| initializer_list COMMA initializer
	;

statement
	: labeled_statement
	| compound_statement
	| expression_statement
	| selection_statement
	| iteration_statement
	| jump_statement { 
        // $$ = $1; 
        }
	;

labeled_statement
	: IDENTIFIER COLON statement
	| CASE constant_expression COLON statement
	| DEFAULT COLON statement
	;

compound_statement
	: OPEN_BRACE CLOSE_BRACE {
		// TODO: correct this
		// $$ = nullptr;
	    }
	| OPEN_BRACE statement_list CLOSE_BRACE {
		// $$ = $2;
	    }
	| OPEN_BRACE declaration_list CLOSE_BRACE {
		// TODO: correct this
		// $$ = nullptr;
	    }
	| OPEN_BRACE declaration_list statement_list CLOSE_BRACE  {
		// TODO: correct this
		// $$ = nullptr;
	    }
	;

declaration_list
	: declaration
	| declaration_list declaration
	;

statement_list
	: statement { // $$ = new NodeList($1); 
        }
	| statement_list statement { // $1->PushBack($2); $$=$1; 
        }
	;

expression_statement
	: SEMI_COLON
	| expression SEMI_COLON { // $$ = $1; 
        }
	;

selection_statement
	: IF OPEN_BRACKET expression CLOSE_BRACKET statement
	| IF OPEN_BRACKET expression CLOSE_BRACKET statement ELSE statement
	| SWITCH OPEN_BRACKET expression CLOSE_BRACKET statement
	;

iteration_statement
	: WHILE OPEN_BRACKET expression CLOSE_BRACKET statement
	| DO statement WHILE OPEN_BRACKET expression CLOSE_BRACKET SEMI_COLON
	| FOR OPEN_BRACKET expression_statement expression_statement CLOSE_BRACKET statement
	| FOR OPEN_BRACKET expression_statement expression_statement expression CLOSE_BRACKET statement
	;

jump_statement
	: GOTO IDENTIFIER SEMI_COLON
	| CONTINUE SEMI_COLON
	| BREAK SEMI_COLON
	| RETURN SEMI_COLON {
		// $$ = new ReturnStatement(nullptr);
	    }
	| RETURN expression SEMI_COLON {
		// $$ = new ReturnStatement($2);
	    }
	;



%%

// Node *g_root;

// Node *ParseAST(std::string file_name)
// {
//  yyin = fopen(file_name.c_str(), "r");
//  if(yyin == NULL){
//    std::cerr << "Couldn't open input file: " << file_name << std::endl;
//    exit(1);
//  }
//  g_root = nullptr;
//  yyparse();
//  return g_root;
// }
