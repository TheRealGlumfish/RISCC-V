// Adapted from: https://www.lysator.liu.se/c/ANSI-C-grammar-y.html
%define parse.error verbose
%code requires{
    #include <stdlib.h>
    #include <stdio.h>
    #include <stdint.h>
    #include <stdbool.h>

    #include "src/ast.h"

    extern FuncDef* rootExpr;
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
    Stmt* stmt_node;
    FuncExpr* func_node;
    Operator operator;

    Decl* decl_node;
    DeclarationList decl_list;
    StatementList stmt_list;

    DeclInitList* decl_init_list;
    DeclInit* decl_init_node;
    TypeSpecifier* type_spec;
    TypeSpecList* type_spec_list;

    FuncDef* func_def_node;

    Declarator* declarator_node;
    StructSpecifier* struct_spec_node;
    StructDeclList* struct_decl_list;
    StructDecl* struct_decl_node;

    size_t ptr_count;
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

%type <expr_node> translation_unit external_declaration primary_expression postfix_expression
%type <func_node> argument_expression_list
%type <expr_node> unary_expression cast_expression multiplicative_expression additive_expression shift_expression relational_expression
%type <expr_node> equality_expression and_expression exclusive_or_expression inclusive_or_expression logical_and_expression logical_or_expression
%type <expr_node> conditional_expression assignment_expression expression constant_expression


%type <func_def_node> function_definition

%type <node>  enum_specifier enumerator_list enumerator  
%type <node> identifier_list type_name abstract_declarator direct_abstract_declarator initializer_list  

%type <struct_spec_node> struct_specifier
%type <struct_decl_list> struct_declaration_list struct_declaration struct_declarator_list
%type <struct_decl_node> struct_declarator 

%type <ptr_count> pointer

%type <stmt_node> compound_statement labeled_statement expression_statement selection_statement iteration_statement jump_statement statement

%type <operator> unary_operator assignment_operator
%type <type_spec> type_specifier

%type <type_spec_list> declaration_specifiers specifier_qualifier_list

%type <string> storage_class_specifier 

%type <stmt_list> statement_list
%type <decl_list> declaration declaration_list  parameter_list 
%type <decl_init_list> init_declarator_list 
%type <decl_init_node> init_declarator
%type <declarator_node> declarator direct_declarator

%type <expr_node> initializer

%type <decl_node> parameter_declaration

%type <number_int> INT_CONSTANT
%type <number_float> FLOAT_CONSTANT
%type <string> IDENTIFIER STRING_LITERAL

%start ROOT
%%

ROOT
  : function_definition { // change back to translation_unit
        rootExpr = $1;
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
                // TODO: Add error message "out of spec".
	| declaration_specifiers declarator compound_statement {
		$$ = funcDefCreate($1, $2->pointerCount, $2->ident, $3);
                if($2->parameterList != NULL)
                {
                        $$->args = $2->parameterList; 
                }
                free($2);
	}
	| declarator declaration_list compound_statement
                //TODO: Add error message "out of spec"
	| declarator compound_statement
                // provides backwards compatibility (idk what)
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
        $$->constant->float_const = $1;
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
        $$->operation->op1->operation->op1 = $1; // Potentially may have to manually override the primitive type field
        $$->operation->op1->operation->op2 = $3;
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
            fprintf(stderr, "Called object is not a function, exiting..."); // TODO: Maybe add the type of the object in the error message
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
            fprintf(stderr, "Called object is not a function, exiting..."); // TODO: Maybe add the type of the object in the error message
            exit(-1);
        }   
        }
	| postfix_expression PERIOD IDENTIFIER {
        fprintf(stderr, "Operation not implemented, exiting...");
        exit(-1);
        }
	| postfix_expression PTR_OP IDENTIFIER {
        fprintf(stderr, "Operation not implemented, exiting...");
        exit(-1);
        }
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
	| unary_expression assignment_operator assignment_expression {
        $$ = exprCreate(ASSIGN_EXPR);
        $$->assignment = assignExprCreate($3, $2);
        if ($1->type == VARIABLE_EXPR) 
        {
            $$->assignment->ident = $1->variable->ident;
            $1->variable->ident = NULL;
            exprDestroy($1);
        }
        else if ($1->type == OPERATION_EXPR && $1->operation->operator == DEREF)
        {
            $$->assignment->lvalue = $1->operation->op1;
            $1->operation->op1 = NULL;
            exprDestroy($1);
        } else
        {
            fprintf(stderr, "Expression is not assignable, exiting..."); // TODO: Maybe add the type of the object in the error message
            exit(-1); // TODO: Maybe change error message
        }
        }
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
        }
	| expression COMMA assignment_expression {
        $$ = exprCreate(OPERATION_EXPR);
        $$->operation = operationExprCreate(COMMA_OP);
        $$->operation->op1 = $1;
        $$->operation->op2 = $3;
        }
	;

constant_expression
	: conditional_expression { $$ = $1; }
	;


// returns a list of declarations
declaration
	: declaration_specifiers SEMI_COLON{
        declarationListInit(&$$, 0);
        declarationListPush(&$$, declCreate($1));
        }
	| declaration_specifiers init_declarator_list SEMI_COLON{
        declarationListInit(&$$, 0);
        // assuming init_declarator_list is a list of decl_inits
        for (size_t i = 0; i < $2->declInitListSize; i++)
        {
            TypeSpecList * typeSpecList;
            if (i == 0)
            {
                typeSpecList = $1;
            }
            else
            {
                typeSpecList = typeSpecListCopy($1);
            }
            
            Decl* decl = declCreate(typeSpecList);
            decl->declInit = $2->declInits[i];
            declarationListPush(&$$, decl);
        }
        free($2->declInits);
        free($2);
        }
	;


// storage class specifiers aren't required
// should return a typespec list
declaration_specifiers 
	: storage_class_specifier
	| storage_class_specifier declaration_specifiers
	| type_specifier {
        $$ = typeSpecListCreate(1);
        $$->typeSpecs[0] = $1; 
        }
        // these are in a weird order - could cause issues
	| type_specifier declaration_specifiers {
        $$ = $2;
        typeSpecListPush($$, $1);
        }
	;

// returns a list of declaration inits
init_declarator_list
	: init_declarator{
        $$ = declInitListCreate(1);
        $$->declInits[0] = $1;
        }
	| init_declarator_list COMMA init_declarator{
        $$ = $1;
        declInitListPush($$, $3);
        }
	;

init_declarator
	: declarator {
        $$ = declInitCreate($1);
        }
	| declarator ASSIGN initializer {
		$$ = declInitCreate($1);
		$$->initExpr = $3;
        }
	;

// not required for our spec
storage_class_specifier 
	: TYPEDEF
	| EXTERN
	| STATIC
	| AUTO
	| REGISTER
	;

type_specifier
	: VOID { 
		$$ = typeSpecifierCreate(false);
		$$->dataType = VOID_TYPE;
	    }
	| CHAR { 
		$$ = typeSpecifierCreate(false);
		$$->dataType = CHAR_TYPE;
	    }
	| SHORT {
		$$ = typeSpecifierCreate(false);
		$$->dataType = SHORT_TYPE;
	    }
	| INT {
		$$ = typeSpecifierCreate(false);
		$$->dataType = INT_TYPE;
	    }
	| FLOAT {
		$$ = typeSpecifierCreate(false);
		$$->dataType = FLOAT_TYPE;
	    }
	| DOUBLE {
		$$ = typeSpecifierCreate(false);
		$$->dataType = INT_TYPE;
	    }
	| LONG {
		$$ = typeSpecifierCreate(false);
		$$->dataType = (false);
		$$->dataType = DOUBLE_TYPE;
	    }
	| SIGNED {
		$$ = typeSpecifierCreate(false);
		$$->dataType = SIGNED_TYPE;
	    }
	| UNSIGNED {
		$$ = typeSpecifierCreate(false);
		$$->dataType = UNSIGNED_TYPE;
	    }
    | struct_specifier {
		$$ = typeSpecifierCreate(true);
		$$->structSpecifier = $1;
	    } // later
	| enum_specifier // doesnt need to be implemented
	| TYPE_NAME // Never returned by the lexer
	;

struct_specifier
	: STRUCT IDENTIFIER OPEN_BRACE struct_declaration_list CLOSE_BRACE {
		$$ = structSpecifierCreate();
		$$->ident = $2;
		$$->structDeclList = $4;
	    }
	| STRUCT OPEN_BRACE struct_declaration_list CLOSE_BRACE {
		$$ = structSpecifierCreate();
		$$->structDeclList = $3;
	    }
	| STRUCT IDENTIFIER {
		$$ = structSpecifierCreate();
		$$->ident = $2;
	    }
	;

// struct declaration is also a struct decl list
struct_declaration_list
	: struct_declaration {
		$$ = $1;
	    }
	| struct_declaration_list struct_declaration {
		$$ = $1;
        for(size_t i = 0; i < $2->structDeclListSize; i++)
        {
            structDeclListPush($$, $2->structDecls[i]);
        }
        free($2->structDecls);
        free($2);
        }
	;

// assume struct declarator returns a list of structDecl
struct_declaration
	: specifier_qualifier_list struct_declarator_list SEMI_COLON {
	    for(size_t i = 0; i < $2->structDeclListSize; i++)
        {
            if(i == 0)
            {
                $2->structDecls[i]->typeSpecList = $1;
            }
            else
            {
                $2->structDecls[i]->typeSpecList = typeSpecListCopy($1);
            }
        }
        $$ = $2;
	}
	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list {
        $$ = $2;
        typeSpecListPush($$, $1);
        }
	| type_specifier {
        $$ = typeSpecListCreate(1);
        $$->typeSpecs[0] = $1; 
        }
	;

// returns a list of declarations
struct_declarator_list
	: struct_declarator {
        $$ = structDeclListCreate(0);
        structDeclListPush($$, $1);
        }
	| struct_declarator_list COMMA struct_declarator {
        $$ = $1;
        structDeclListPush($$, $3);
        }
	;

// make declarations here
struct_declarator
	: declarator {
        $$ = structDeclCreate();
        $$->declarator = $1;
        }
	| COLON constant_expression {
        $$ = structDeclCreate();
        $$->bitField = $2;
        }
	| declarator COLON constant_expression {
        $$ = structDeclCreate();
        $$->declarator = $1;
        $$->bitField = $3;
        }
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

// pointer is just a number here!
declarator 
	: pointer direct_declarator {
        $2->pointerCount = $1;
        $$ = $2;
        }
	| direct_declarator { 
        $$ = $1; 
        }
	;

direct_declarator
	: IDENTIFIER {
		$$ = declaratorCreate();
		$$->ident = $1;
	    }
	| OPEN_BRACKET declarator CLOSE_BRACKET
	| direct_declarator OPEN_SQUARE constant_expression CLOSE_SQUARE
	| direct_declarator OPEN_SQUARE CLOSE_SQUARE
	| direct_declarator OPEN_BRACKET parameter_list CLOSE_BRACKET {
        $$ = declaratorCreate();
        $$->ident = $1->ident;
        $$->parameterList = &$3;
        free($1);
        }
	| direct_declarator OPEN_BRACKET identifier_list  CLOSE_BRACKET // just for K&R style
	| direct_declarator OPEN_BRACKET CLOSE_BRACKET {
		// $$ = new DirectDeclarator($1);
	    }
	;

pointer
	: MUL_OP { $$ = 1; }
	| MUL_OP pointer { $$ = $2 + 1; }
	;

// declaration list
parameter_list
	: parameter_declaration {
        declarationListInit(&$$, 1);
        $$.decls[0] = $1;
        }
	| parameter_list COMMA parameter_declaration {
        $$ = $1;
        declarationListPush(&$$, $3);
        }
	;

// declaration (not list)
parameter_declaration
	: declaration_specifiers declarator {
        $$ = declCreate($1);
        $$->declInit = declInitCreate($2);
        }
	| declaration_specifiers abstract_declarator // not implementing atm
	| declaration_specifiers {
        $$ = declCreate($1);
        }
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

// returns an expresson
initializer
	: assignment_expression {
        $$ = $1;
        }
	| OPEN_BRACE initializer_list CLOSE_BRACE
	| OPEN_BRACE initializer_list COMMA CLOSE_BRACE
	;

initializer_list
	: initializer
	| initializer_list COMMA initializer
	;

statement
	: labeled_statement { 
        $$ = $1; 
        }
	| compound_statement { 
        $$ = $1; 
        }
	| expression_statement {
        $$ = $1;
        }
	| selection_statement {
        $$ = $1;
        }
	| iteration_statement {
        $$ = $1;
        }
	| jump_statement {
        $$ = $1;
        }
	;

labeled_statement
	: IDENTIFIER COLON statement {
        $$ = stmtCreate(LABEL_STMT);
        $$->labelStmt = labelStmtCreate($3);
        $$->labelStmt->ident = $1;
        }
	| CASE constant_expression COLON statement {
        $$ = stmtCreate(LABEL_STMT);
        $$->labelStmt = labelStmtCreate($4);
        $$->labelStmt->caseLabel = $2;
        }
	| DEFAULT COLON statement {
        $$ = stmtCreate(LABEL_STMT);
        $$->labelStmt = labelStmtCreate($3);
        }
	;

compound_statement
	: OPEN_BRACE CLOSE_BRACE {
        $$ = stmtCreate(COMPOUND_STMT);
        $$->compoundStmt = compoundStmtCreate();
	    }
	| OPEN_BRACE statement_list CLOSE_BRACE {
        $$ = stmtCreate(COMPOUND_STMT);
        $$->compoundStmt = compoundStmtCreate();
        $$->compoundStmt->stmtList = $2;
	    }
	| OPEN_BRACE declaration_list CLOSE_BRACE {
        $$ = stmtCreate(COMPOUND_STMT);
        $$->compoundStmt = compoundStmtCreate();
        $$->compoundStmt->declList = $2;
	    }
	| OPEN_BRACE declaration_list statement_list CLOSE_BRACE  {
        $$ = stmtCreate(COMPOUND_STMT);
        $$->compoundStmt = compoundStmtCreate();
        $$->compoundStmt->declList = $2;
        $$->compoundStmt->stmtList = $3;
    	}
	;

declaration_list
	: declaration {
		$$ = $1;
        }
	| declaration_list declaration {
        $$ = $1;
        for(size_t i = 0; i < $2.size; i++)
        {
            declarationListPush(&$$, $2.decls[i]);
        }
		free($2.decls);
        }
	;

statement_list
	: statement {
        statementListInit(&$$, 1);
        $$.stmts[0] = $1;
        }
	| statement_list statement { 
        $$ = $1;
        statementListPush(&$$, $2);
        }
	;

expression_statement
	: SEMI_COLON {
        $$ = stmtCreate(EXPR_STMT);
        $$->exprStmt = exprStmtCreate();
        }
	| expression SEMI_COLON {
        $$ = stmtCreate(EXPR_STMT);
        $$->exprStmt = exprStmtCreate();
        $$->exprStmt->expr = $1;
        }
	;

selection_statement
	: IF OPEN_BRACKET expression CLOSE_BRACKET statement {
        $$ = stmtCreate(IF_STMT);
        $$->ifStmt = ifStmtCreate($3, $5);
        }
	| IF OPEN_BRACKET expression CLOSE_BRACKET statement ELSE statement {
        $$ = stmtCreate(IF_STMT);
        $$->ifStmt = ifStmtCreate($3, $5);
        $$->ifStmt->falseBody = $7;
        }
	| SWITCH OPEN_BRACKET expression CLOSE_BRACKET statement {
        $$ = stmtCreate(SWITCH_STMT);
        $$->switchStmt = switchStmtCreate($3, $5);
        }
	;

iteration_statement
	: WHILE OPEN_BRACKET expression CLOSE_BRACKET statement {
        $$ = stmtCreate(WHILE_STMT);
        $$->whileStmt = whileStmtCreate($3, $5, false);
        }
	| DO statement WHILE OPEN_BRACKET expression CLOSE_BRACKET SEMI_COLON {
        $$ = stmtCreate(WHILE_STMT);
        $$->whileStmt = whileStmtCreate($5, $2, true);
        }
	| FOR OPEN_BRACKET expression_statement expression_statement CLOSE_BRACKET statement{
        $$ = stmtCreate(FOR_STMT);
        $$->forStmt = forStmtCreate($3, $4, $6);
        }
	| FOR OPEN_BRACKET expression_statement expression_statement expression CLOSE_BRACKET statement {
        $$ = stmtCreate(FOR_STMT);
        $$->forStmt = forStmtCreate($3, $4, $7);
        $$->forStmt->modifier = $5;
        }
	;

jump_statement
	: GOTO IDENTIFIER SEMI_COLON {
        $$ = stmtCreate(JUMP_STMT);
        $$->jumpStmt = jumpStmtCreate(GOTO_JUMP);
        $$->jumpStmt->ident = $2;
        }
	| CONTINUE SEMI_COLON {
        $$ = stmtCreate(JUMP_STMT);
        $$->jumpStmt = jumpStmtCreate(CONTINUE_JUMP);
        }
	| BREAK SEMI_COLON {
        $$ = stmtCreate(JUMP_STMT);
        $$->jumpStmt = jumpStmtCreate(BREAK_JUMP);
        }
	| RETURN SEMI_COLON {
        $$ = stmtCreate(JUMP_STMT);
        $$->jumpStmt = jumpStmtCreate(RETURN_JUMP);
	    }
	| RETURN expression SEMI_COLON {
        $$ = stmtCreate(JUMP_STMT);
        $$->jumpStmt = jumpStmtCreate(RETURN_JUMP);
        $$->jumpStmt->expr = $2;
	    }
	;



%%

FuncDef* rootExpr;
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
