%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    int yylex(void);
    void yyerror(const char *mensagem);
    extern FILE *yyin;
    extern int yylineno;

    char current_type[32];
%}

%union {
    char *stringVal;
    int intValue;
}

%token <intValue> NUMBER CHAR
%token <stringVal> IDENT TYPE_CHAR TYPE_DOUBLE TYPE_FLOAT TYPE_INT TYPE_LONG TYPE_SHORT

%token PLUS MINUS TIMES DIVIDE MODULE
%token RPAREN LPAREN RBRACE LBRACE RBRACK LBRACK SEMICOLON DOT COMMA ASSIGN
%token EQUAL DIFF GREATER_E GREATER_T LESS_E LESS_T AND OR NOT

%token KW_IF KW_ELSE
%token KW_BREAK KW_CASE KW_CONTINUE KW_DEFAULT KW_DO KW_FOR KW_GOTO KW_PACKED KW_RETURN KW_SIZEOF KW_SWITCH KW_WHILE
%token BITWISE_AND BITWISE_NOT BITWISE_OR BITWISE_XOR
%token INCREMENT DECREMENT ARROW
%token ADD_ASSIGN DIV_ASSIGN MOD_ASSIGN MUL_ASSIGN SUB_ASSIGN BITWISE_AND_ASSIGN BITWISE_OR_ASSIGN BITWISE_XOR_ASSIGN
%token KW_QUALIFIER KW_STORAGE KW_VOID KW_STRUCT KW_UNION KW_ENUM KW_SIGNED

%right KW_ELSE
%left OR
%left AND
%left BITWISE_OR
%left BITWISE_XOR
%left BITWISE_AND
%left EQUAL DIFF
%left GREATER_E GREATER_T LESS_E LESS_T
%left PLUS MINUS
%left TIMES DIVIDE MODULE
%right NOT BITWISE_NOT INCREMENT DECREMENT

%type <intValue> expr stmt

%start input

%%

input:
      %empty
    | input stmt
    | input error SEMICOLON     { 
          fprintf(stderr, "[Erro Sintatico] Erro recuperado ate ';'\n");
          yyerrok; 
          yyclearin; 
      }
    ;



/* OPERADORES */

expr:
    /* Aritméticas */
      expr PLUS expr    { $$ = $1 + $3; }
    | expr MINUS expr   { $$ = $1 - $3; }
    | expr TIMES expr   { $$ = $1 * $3; }
    | expr DIVIDE expr  { if ($3 == 0) { yyerror("Erro Semantico: divisão por zero"); $$ = 0; } else { $$ = $1 / $3; } }
    | expr MODULE expr  { if ($3 == 0) { yyerror("Erro Semantico: divisão por zero"); $$ = 0; } else { $$ = $1 % $3; } }
    | LPAREN expr RPAREN{ $$ = $2; }
    | NUMBER            { $$ = $1; }

    /* Caracteres (ASCII) */
    | CHAR                  { $$ = $1; }

    /* Lógicas */
    | expr EQUAL expr   { $$ = ($1 == $3); }
    | expr DIFF expr    { $$ = ($1 != $3); }
    | expr GREATER_E expr  { $$ = ($1 >= $3); }
    | expr LESS_E expr     { $$ = ($1 <= $3); }
    | expr GREATER_T expr  { $$ = ($1 > $3); }
    | expr LESS_T expr     { $$ = ($1 < $3); }
    | expr AND expr     { $$ = ($1 && $3); }
    | expr OR expr      { $$ = ($1 || $3); }
    | NOT expr          { $$ = (!$2); }

    /* Bit a Bit */
    | expr BITWISE_AND expr { $$ = $1 & $3; }
    | expr BITWISE_OR expr  { $$ = $1 | $3; }
    | expr BITWISE_XOR expr { $$ = $1 ^ $3; }
    | BITWISE_NOT expr      { $$ = ~$2; }

    /* Incremento e Decremento */
    | INCREMENT expr        { $$ = $2 + 1; }
    | DECREMENT expr        { $$ = $2 - 1; }
    | expr INCREMENT        { $$ = $1; }
    | expr DECREMENT        { $$ = $1; }
    ;


/* STATEMENTS */

stmt:
      expr SEMICOLON { 
        $$ = $1; 
        printf("Resultado: %d\n", $1); 
      }
    | type_specifier init_declarator_list SEMICOLON { 
        $$ = 0; 
        printf("Declaração finalizada com sucesso.\n\n"); 
      }
    | KW_IF LPAREN expr RPAREN stmt %prec KW_ELSE {
        $$ = 0;
        if ($3 != 0) {
            printf("IF TRUE\n");
        } else {
            printf("IF FALSE\n");
        }
      }
    | KW_IF LPAREN expr RPAREN stmt KW_ELSE stmt {
        $$ = 0;
        printf("IF-ELSE executado\n");
      }
    ;



/* ÁREA DE DECLARAÇÃO DE VARIÁVEIS */

type_specifier:
    TYPE_INT    { strcpy(current_type, $1); free($1); }
  | TYPE_FLOAT  { strcpy(current_type, $1); free($1); }
  | TYPE_CHAR   { strcpy(current_type, $1); free($1); }
  | TYPE_DOUBLE { strcpy(current_type, $1); free($1); }
  | TYPE_LONG   { strcpy(current_type, $1); free($1); }
  | TYPE_SHORT  { strcpy(current_type, $1); free($1); }
;

init_declarator_list:
    init_declarator
  | init_declarator_list COMMA init_declarator
;

init_declarator:
    IDENT {
        printf("Variável identificada: '%s' | Tipo: %s\n", $1, current_type);
        free($1);
    }
  | IDENT ASSIGN expr {
        printf("Variável identificada: '%s' | Tipo: %s | Valor inicial: %d\n", $1, current_type, $3);
        free($1);
    }
;

%%

int main(void) {
    return yyparse();
}

void yyerror(const char *s) {
    fprintf(stderr, "ERRO - %s\n", s);
}