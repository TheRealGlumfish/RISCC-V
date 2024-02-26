%option noyywrap

%{
  // A lot of this lexer is based off the ANSI C grammar:
  // https://www.lysator.liu.se/c/ANSI-C-grammar-l.html#MUL-ASSIGN
  // Avoid error "error: `fileno' was not declared in this scope"
  extern "C" int fileno(FILE *stream);

  #include "parser.tab.hpp"
%}

D	  [0-9]
L	  [a-zA-Z_]
H   [a-fA-F0-9]
E	  [Ee][+-]?{D}+
FS  (f|F|l|L)
IS  (u|U|l|L)*

%%
"/*"			{/* consumes comment - TODO you might want to process and emit it in your assembly for debugging */}

"auto"			{return(AUTO);}
"break"			{return(BREAK);}
"case"			{return(CASE);}
"char"			{return(CHAR);}
"const"			{return(CONST);}
"continue"  {return(CONTINUE);}
"default"		{return(DEFAULT);}
"do"			  {return(DO);}
"double"		{return(DOUBLE);}
"else"			{return(ELSE);}
"enum"			{return(ENUM);}
"extern"		{return(EXTERN);}
"float"			{return(FLOAT);}
"for"			  {return(FOR);}
"goto"			{return(GOTO);}
"if"			  {return(IF);}
"int"			  {return(INT);}
"long"			{return(LONG);}
"register"	{return(REGISTER);}
"return"		{return(RETURN);}
"short"			{return(SHORT);}
"signed"		{return(SIGNED);}
"sizeof"		{return(SIZEOF);}
"static"		{return(STATIC);}
"struct"		{return(STRUCT);}
"switch"		{return(SWITCH);}
"typedef"		{return(TYPEDEF);}
"union"			{return(UNION);}
"unsigned"	{return(UNSIGNED);}
"void"			{return(VOID);}
"volatile"	{return(VOLATILE);}
"while"			{return(WHILE);}

// identifier definition
// number_int, number_float, string values can be accessed by parser
{L}({L}|{D})*		{yylval.string = new std::string(yytext); return(IDENTIFIER);}

0[xX]{H}+{IS}?		{yylval.number_int = (int)strtol(yytext, NULL, 0); return(INT_CONSTANT);}
0{D}+{IS}?		    {yylval.number_int = (int)strtol(yytext, NULL, 0); return(INT_CONSTANT);}
{D}+{IS}?		      {yylval.number_int = (int)strtol(yytext, NULL, 0); return(INT_CONSTANT);}
L?'(\\.|[^\\'])+'	{yylval.number_int = (int)strtol(yytext, NULL, 0); return(INT_CONSTANT);}

{D}+{E}{FS}?		        {yylval.number_float = strtod(yytext, NULL); return(FLOAT_CONSTANT);}
{D}*"."{D}+({E})?{FS}?	{yylval.number_float = strtod(yytext, NULL); return(FLOAT_CONSTANT);}
{D}+"."{D}*({E})?{FS}?	{yylval.number_float = strtod(yytext, NULL); return(FLOAT_CONSTANT);}

//new code to store string value
L?\"(\\.|[^\\"])*\"	{yylval.string = new std::string(yytext.substr(1, yytext.size() - 2)); return(STRING_LITERAL);}

"..."      {return(ELLIPSIS);}
">>="			 {return(RIGHT_ASSIGN);}
"<<="      {return(LEFT_ASSIGN);}
"+="			 {return(ADD_ASSIGN);}
"-="       {return(SUB_ASSIGN);}
"*="       {return(MUL_ASSIGN);}
"/="			 {return(DIV_ASSIGN);}
"%="			 {return(MOD_ASSIGN);}
"&="       {return(AND_ASSIGN);}
"^="			 {return(XOR_ASSIGN);}
"|="       {return(OR_ASSIGN);}

">>"       {return(RIGHT_OP);}
"<<"       {return(LEFT_OP);}

"++"			 {return(INC_OP);}
"--"			 {return(DEC_OP);}

"->"			 {return(PTR_OP);}

"&&"			 {return(AND_OP);}
"||"			 {return(OR_OP);}
"~"			   {return(NOT_OP);}

"<="			 {return(LE_OP);}
">="			 {return(GE_OP);}
"=="			 {return(EQ_OP);}
"!="			 {return(NE_OP);}

";"			   {return(COLON);}
("{"|"<%") {return(OPEN_BRACE);}
("}"|"%>") {return(CLOSE_BRACE);}
","			   {return(COMMA);}
"="			   {return(ASSIGN);}
"("		     {return(OPEN_BRACKET);}
")"			   {return(CLOSE_BRACKET);}
("["|"<:") {return(OPEN_SQUARE);}
("]"|":>") {return(CLOSE_SQUARE);}
"."			   {return(PERIOD);}

"&"			   {return(AND_LOGIC);}
"|"			   {return(OR_LOGIC);}
"!"			   {return(NOT_LOGIC);}

"-"			   {return(SUB_OP);}
"+"			   {return(ADD_OP);}
"*"			   {return(MUL_OP);}
"/"			   {return(DIV_OP);}
"%"			   {return(MOD_OP);}
"<"			   {return(LT_OP);}
">"			   {return(GT_OP);}
"^"			   {return(EXP_OP);}

"?"			   {return(TERN_OP);}
":"			   {return(COLON);}

[ \a\b\t\v\f\n\r]		{/* ignore new lines and special sequences */}
.			              {/* ignore bad characters */}

%%

void yyerror (char const *s)
{
  fprintf(stderr, "Lexing error: %s\n", s);
  exit(1);
}
