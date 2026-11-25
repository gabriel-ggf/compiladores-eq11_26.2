%code requires {
    #include "ast/ast.h"
}

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast/ast.h"
#include "ast/table.h"

int yylex(void);
void yyerror(const char *mensagem);

/* Raiz da AST: preenchida pelo parser, lida pelo main.c / interpretador. */
node *root_node = NULL;
int parse_errors = 0;

/* Tratamento de Escapes. */
static char parse_char_literal(const char *lit) {
    if (lit[1] != '\\') {
        return lit[1];
    }
    switch (lit[2]) {
        case 'n':  return '\n';
        case 't':  return '\t';
        case 'r':  return '\r';
        case '0':  return '\0';
        case '\\': return '\\';
        case '\'': return '\'';
        case '"':  return '"';
        default:   return lit[2];
    }
}

/* Tratemento de incrementos/decrementos. */
static node *make_compound_assign(const char *name, int op, node *rhs) {
    node *var = ast_new_variable(name);
    node *bin = ast_new_binary_op(op, var, rhs);
    return ast_new_assign(name, bin);
}

%}

%union {
    double num;
    char  *str;
    node  *n;
}

%token <num> NUMBER FLOAT_NUMBER
%token <str> IDENT
%token <str> KW_TYPE
%token <str> STRING CHAR
%token ASSIGN
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
%token KW_VOID KW_STRUCT KW_UNION KW_ENUM
%token LEX_ERROR

%precedence THEN
%precedence KW_ELSE
%right ASSIGN ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN
%left OR
%left AND
%left EQUAL DIFF
%left GREATER_T GREATER_E LESS_T LESS_E
%left PLUS MINUS
%left TIMES DIVIDE MODULE
%right NOT UMINUS

%type <n> input stmt expr stmt_list arg_list func_def for_init for_cond for_inc

%start input

%%

input:
      %empty                    { root_node = ast_new_program(); $$ = root_node; }
    | input func_def            { if ($2 != NULL) ast_program_add($1, $2); $$ = $1; }
    | input stmt                { ast_program_add($1, $2); $$ = $1; }
    | input error SEMICOLON     {
          fprintf(stderr, "[Erro Sintatico] Erro recuperado ate ';'\n");
          yyerrok;
          yyclearin;
          $$ = $1;
      }
    ;

func_def:
      KW_TYPE IDENT LPAREN RPAREN LBRACE stmt_list RBRACE {
          if (strcmp($2, "main") != 0) {
              yyerror("Erro Semantico: apenas a funcao 'main' e suportada no momento");
              free($1);
              free($2);
              ast_free($6);
              YYERROR;
          }
          free($1);
          free($2);
          $$ = $6;
      }
    | KW_TYPE IDENT LPAREN KW_VOID RPAREN LBRACE stmt_list RBRACE {
          if (strcmp($2, "main") != 0) {
              yyerror("Erro Semantico: apenas a funcao 'main' e suportada no momento");
              free($1);
              free($2);
              ast_free($7);
              YYERROR;
          }
          free($1);
          free($2);
          $$ = $7;
      }
    ;

stmt_list:
      %empty                    { $$ = NULL; }
    | stmt_list stmt            { $$ = ast_block_add($1, $2); }
    ;

stmt:
      KW_TYPE IDENT SEMICOLON {
          if (!insertSymbol($2, $1)) {
              yyerror("Erro Semantico: variavel ja declarada");
              free($1);
              free($2);
              YYERROR;
          }
          $$ = ast_new_var_decl(ast_type_from_string($1), $2, NULL);
          free($1);
          free($2);
      }
    | KW_TYPE IDENT ASSIGN expr SEMICOLON {
          if (!insertSymbol($2, $1)) {
              yyerror("Erro Semantico: variavel ja declarada");
              free($1);
              free($2);
              ast_free($4);
              YYERROR;
          }
          $$ = ast_new_var_decl(ast_type_from_string($1), $2, $4);
          free($1);
          free($2);
      }
    | KW_TYPE TIMES IDENT SEMICOLON {
          if (strcmp($1, "char") != 0) {
              yyerror("Erro Semantico: ponteiro suportado apenas para 'char *'");
              free($1);
              free($3);
              YYERROR;
          }
          if (!insertSymbol($3, "string")) {
              yyerror("Erro Semantico: variavel ja declarada");
              free($1);
              free($3);
              YYERROR;
          }
          $$ = ast_new_var_decl(TYPE_STRING, $3, NULL);
          free($1);
          free($3);
      }
    | KW_TYPE TIMES IDENT ASSIGN expr SEMICOLON {
          if (strcmp($1, "char") != 0) {
              yyerror("Erro Semantico: ponteiro suportado apenas para 'char *'");
              free($1);
              free($3);
              ast_free($5);
              YYERROR;
          }
          if (!insertSymbol($3, "string")) {
              yyerror("Erro Semantico: variavel ja declarada");
              free($1);
              free($3);
              ast_free($5);
              YYERROR;
          }
          $$ = ast_new_var_decl(TYPE_STRING, $3, $5);
          free($1);
          free($3);
      }
    | KW_RETURN expr SEMICOLON                    { $$ = ast_new_return($2); }
    | KW_RETURN SEMICOLON                         { $$ = ast_new_return(NULL); }
    | LBRACE stmt_list RBRACE                     { $$ = $2; }
    | KW_IF LPAREN expr RPAREN stmt %prec THEN    { $$ = ast_new_if($3, $5, NULL); }
    | KW_IF LPAREN expr RPAREN stmt KW_ELSE stmt  { $$ = ast_new_if($3, $5, $7); }
    | KW_DO stmt KW_WHILE LPAREN expr RPAREN SEMICOLON {
          $$ = ast_new_do_while($2, $5);
      }
    | KW_BREAK SEMICOLON                          { $$ = ast_new_break(); }
    | KW_CONTINUE SEMICOLON                       { $$ = ast_new_continue(); }
    | KW_WHILE LPAREN expr RPAREN stmt            { $$ = ast_new_while($3, $5); }
    | KW_FOR LPAREN for_init SEMICOLON for_cond SEMICOLON for_inc RPAREN stmt {
          $$ = ast_new_for($3, $5, $7, $9);
      }
    | expr SEMICOLON                              { $$ = $1; }
    ;

for_init:
      %empty                    { $$ = NULL; }
    | KW_TYPE IDENT ASSIGN expr {
          if (!insertSymbol($2, $1)) {
              yyerror("Erro Semantico: variavel ja declarada");
              free($1);
              free($2);
              ast_free($4);
              YYERROR;
          }
          $$ = ast_new_var_decl(ast_type_from_string($1), $2, $4);
          free($1);
          free($2);
      }
    | expr                      { $$ = $1; }
    ;

for_cond:
      %empty                    { $$ = NULL; }
    | expr                      { $$ = $1; }
    ;

for_inc:
      %empty                    { $$ = NULL; }
    | expr                      { $$ = $1; }
    ;

arg_list:
      expr                   { $$ = ast_new_call(""); ast_call_add_arg($$, $1); }
    | arg_list COMMA expr    { ast_call_add_arg($1, $3); $$ = $1; }
    ;

expr:
      IDENT ASSIGN expr {
          if (searchSymbol($1) == NULL) {
              yyerror("Erro Semantico: variavel nao declarada");
              free($1);
              ast_free($3);
              YYERROR;
          }
          $$ = ast_new_assign($1, $3);
          free($1);
      }
    | IDENT ADD_ASSIGN expr {
          if (searchSymbol($1) == NULL) { yyerror("Erro Semantico: variavel nao declarada"); free($1); ast_free($3); YYERROR; }
          $$ = make_compound_assign($1, '+', $3);
          free($1);
      }
    | IDENT SUB_ASSIGN expr {
          if (searchSymbol($1) == NULL) { yyerror("Erro Semantico: variavel nao declarada"); free($1); ast_free($3); YYERROR; }
          $$ = make_compound_assign($1, '-', $3);
          free($1);
      }
    | IDENT MUL_ASSIGN expr {
          if (searchSymbol($1) == NULL) { yyerror("Erro Semantico: variavel nao declarada"); free($1); ast_free($3); YYERROR; }
          $$ = make_compound_assign($1, '*', $3);
          free($1);
      }
    | IDENT DIV_ASSIGN expr {
          if (searchSymbol($1) == NULL) { yyerror("Erro Semantico: variavel nao declarada"); free($1); ast_free($3); YYERROR; }
          $$ = make_compound_assign($1, '/', $3);
          free($1);
      }
    | IDENT MOD_ASSIGN expr {
          if (searchSymbol($1) == NULL) { yyerror("Erro Semantico: variavel nao declarada"); free($1); ast_free($3); YYERROR; }
          $$ = make_compound_assign($1, '%', $3);
          free($1);
      }
    | INCREMENT IDENT {
          if (searchSymbol($2) == NULL) { yyerror("Erro Semantico: variavel nao declarada"); free($2); YYERROR; }
          $$ = ast_new_inc_dec($2, '+', 1);
          free($2);
      }
    | DECREMENT IDENT {
          if (searchSymbol($2) == NULL) { yyerror("Erro Semantico: variavel nao declarada"); free($2); YYERROR; }
          $$ = ast_new_inc_dec($2, '-', 1);
          free($2);
      }
    | IDENT INCREMENT {
          if (searchSymbol($1) == NULL) { yyerror("Erro Semantico: variavel nao declarada"); free($1); YYERROR; }
          $$ = ast_new_inc_dec($1, '+', 0);
          free($1);
      }
    | IDENT DECREMENT {
          if (searchSymbol($1) == NULL) { yyerror("Erro Semantico: variavel nao declarada"); free($1); YYERROR; }
          $$ = ast_new_inc_dec($1, '-', 0);
          free($1);
      }
    | expr PLUS expr         { $$ = ast_new_binary_op('+', $1, $3); }
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
    | MINUS expr %prec UMINUS { $$ = ast_new_unary_op('-', $2); }
    | LPAREN expr RPAREN     { $$ = $2; }
    | NUMBER                 { $$ = ast_new_number($1); }
    | FLOAT_NUMBER           { $$ = ast_new_float($1); }
    | STRING                 { $$ = ast_new_string($1); free($1); }
    | CHAR                   { $$ = ast_new_char(parse_char_literal($1)); free($1); }
    | IDENT LPAREN RPAREN {
          $$ = ast_new_call($1);
          free($1);
      }
    | IDENT LPAREN arg_list RPAREN {
          free($3->as.call.name);
          $3->as.call.name = $1;
          $$ = $3;
      }
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
    parse_errors++;
    fprintf(stderr, "ERRO - %s\n", s);
}