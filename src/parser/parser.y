%code requires {
    #include "ast/ast.h"
}

%{
#include <stdio.h>
#include <stdlib.h>
#include "ast/ast.h"
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
%token <str> KW_TYPE
%token <str> STRING CHAR
%token ASSIGN KW_PRINT
%token PLUS MINUS TIMES DIVIDE MODULE
%token RPAREN LPAREN RBRACE LBRACE RBRACK LBRACK SEMICOLON DOT COMMA
%token EQUAL DIFF GREATER_E GREATER_T LESS_E LESS_T AND OR NOT
%token KW_IF KW_ELSE KW_WHILE KW_FOR
%token KW_QUALIFIER KW_STORAGE
%token KW_SWITCH KW_CASE KW_DEFAULT KW_DO KW_BREAK KW_CONTINUE KW_GOTO KW_RETURN KW_SIZEOF KW_PACKED
%token INCREMENT DECREMENT ARROW
%token ADD_ASSIGN DIV_ASSIGN MOD_ASSIGN MUL_ASSIGN SUB_ASSIGN 
%token BITWISE_AND_ASSIGN BITWISE_OR_ASSIGN BITWISE_XOR_ASSIGN
%token BITWISE_AND BITWISE_OR BITWISE_XOR BITWISE_NOT

%precedence THEN
%precedence KW_ELSE
%right ASSIGN
%left OR
%left AND
%left EQUAL DIFF
%left GREATER_T GREATER_E LESS_T LESS_E
%left PLUS MINUS
%left TIMES DIVIDE MODULE
%right NOT

%type <n> input stmt expr stmt_list

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

stmt_list:
      %empty                    { $$ = NULL; }
    | stmt_list stmt            { $$ = ast_block_add($1, $2); }
    ;

stmt:
      KW_TYPE IDENT SEMICOLON {
          insertSymbol($2, $1);
          $$ = ast_new_assign($2, ast_new_number(0.0));
          free($1);
          free($2);
      }
    | KW_TYPE IDENT ASSIGN expr SEMICOLON {
          insertSymbol($2, $1);
          $$ = ast_new_assign($2, $4);
          free($1);
          free($2);
      }
    | IDENT ASSIGN expr SEMICOLON {
          if (searchSymbol($1) == NULL) {
              yyerror("Erro Semantico: variavel nao declarada");
              free($1);
              YYERROR;
          }
          $$ = ast_new_assign($1, $3);
          free($1);
      }
    | LBRACE stmt_list RBRACE                     { $$ = $2; }
    | KW_IF LPAREN expr RPAREN stmt %prec THEN    { $$ = ast_new_if($3, $5, NULL); }
    | KW_IF LPAREN expr RPAREN stmt KW_ELSE stmt  { $$ = ast_new_if($3, $5, $7); }
    | KW_WHILE LPAREN expr RPAREN stmt            { $$ = ast_new_while($3, $5); }
    | KW_PRINT expr SEMICOLON                     { $$ = ast_new_print($2); }
    | expr SEMICOLON                              { $$ = $1; }
    ;

expr:
      expr PLUS expr         { $$ = ast_new_binary_op('+', $1, $3); }
    | expr MINUS expr        { $$ = ast_new_binary_op('-', $1, $3); }
    | expr TIMES expr        { $$ = ast_new_binary_op('*', $1, $3); }
    | expr DIVIDE expr       { $$ = ast_new_binary_op('/', $1, $3); }
    | expr MODULE expr       { $$ = ast_new_binary_op('%', $1, $3); }
    | expr EQUAL expr        { $$ = ast_new_binary_op('E', $1, $3); }
    | expr DIFF expr         { $$ = ast_new_binary_op('D', $1, $3); }
    | expr GREATER_T expr    { $$ = ast_new_binary_op('>', $1, $3); }
    | expr LESS_T expr       { $$ = ast_new_binary_op('<', $1, $3); }
    | expr GREATER_E expr    { $$ = ast_new_binary_op('G', $1, $3); }
    | expr LESS_E expr       { $$ = ast_new_binary_op('L', $1, $3); }
    | expr AND expr          { $$ = ast_new_binary_op('&', $1, $3); }
    | expr OR expr           { $$ = ast_new_binary_op('|', $1, $3); }
    | NOT expr               { $$ = ast_new_unary_op('!', $2); }
    | LPAREN expr RPAREN     { $$ = $2; }
    | NUMBER                 { $$ = ast_new_number($1); }
    | STRING                 { $$ = ast_new_string($1); free($1); }
    | CHAR                   { $$ = ast_new_char($1[1]); free($1); } /* Pega o caractere exato dentro das aspas simples */
    | IDENT                  {
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
}