%code requires {
    #include "ast/ast.h"
}

%{
#include <stdio.h>
#include <stdlib.h>
#include "ast/table.h"

int yylex(void);
void yyerror(const char *mensagem);

/* Raiz da AST: preenchida pelo parser, lida pelo main.c / interpretador. */
node *root_node = NULL;
%}

%union {
    double num;
    char  *str;
    node  *n;
}

%token <num> NUMBER
%token <str> IDENT
%token ASSIGN KW_PRINT
%token PLUS MINUS TIMES DIVIDE MODULE
%token RPAREN LPAREN RBRACE LBRACE RBRACK LBRACK SEMICOLON DOT COMMA
%token EQUAL DIFF GREATER_E GREATER_T LESS_E LESS_T AND OR NOT
%token KW_IF KW_ELSE

%token KW_BREAK KW_CASE KW_CONTINUE KW_DEFAULT KW_DO KW_FOR KW_GOTO KW_PACKED KW_RETURN KW_SIZEOF KW_SWITCH KW_WHILE
%token CHAR STRING
%token BITWISE_AND BITWISE_NOT BITWISE_OR BITWISE_XOR
%token INCREMENT DECREMENT ARROW
%token ADD_ASSIGN DIV_ASSIGN MOD_ASSIGN MUL_ASSIGN SUB_ASSIGN BITWISE_AND_ASSIGN BITWISE_OR_ASSIGN BITWISE_XOR_ASSIGN

%token KW_TYPE KW_QUALIFIER KW_STORAGE

%left PLUS MINUS
%left TIMES DIVIDE MODULE

%type <n> input stmt expr

%start input

%%

input:
      %empty                    { root_node = ast_new_program(); $$ = root_node; }
    | input stmt                { ast_program_add($1, $2); $$ = $1; }
    | input error SEMICOLON     {
          fprintf(stderr, "[Erro Sintatico] Erro recuperado ate ';'\n");
          yyerrok;
          yyclearin;
          $$ = $1;
      }
    ;

stmt:
      IDENT ASSIGN expr SEMICOLON   {
          insertSymbol($1, "double");
          $$ = ast_new_assign($1, $3);
          free($1);
      }
    | KW_PRINT expr SEMICOLON       { $$ = ast_new_print($2); }
    | expr SEMICOLON                { $$ = ast_new_print($1); }
    ;

expr:
      expr PLUS expr     { $$ = ast_new_binary_op('+', $1, $3); }
    | expr MINUS expr    { $$ = ast_new_binary_op('-', $1, $3); }
    | expr TIMES expr    { $$ = ast_new_binary_op('*', $1, $3); }
    | expr DIVIDE expr   { $$ = ast_new_binary_op('/', $1, $3); }
    | expr MODULE expr   { $$ = ast_new_binary_op('%', $1, $3); }
    | LPAREN expr RPAREN { $$ = $2; }
    | NUMBER             { $$ = ast_new_number($1); }
    | IDENT              {
          if (searchSymbol($1) == NULL) {
              yyerror("Erro Semantico: variavel nao declarada");
              free($1);
              YYERROR;
          }
          $$ = ast_new_variable($1);
          free($1);
      }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "ERRO - %s\n", s);