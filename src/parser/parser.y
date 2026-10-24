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
%token <stringVal> IDENT STRING TYPE_CHAR TYPE_DOUBLE TYPE_FLOAT TYPE_INT TYPE_LONG TYPE_SHORT

%token PLUS MINUS TIMES DIVIDE MODULE
%token RPAREN LPAREN RBRACE LBRACE RBRACK LBRACK SEMICOLON DOT COMMA ASSIGN
%token EQUAL DIFF GREATER_E GREATER_T LESS_E LESS_T AND OR NOT

%token KW_IF KW_ELSE
%token KW_BREAK KW_CASE KW_CONTINUE KW_DEFAULT KW_DO KW_FOR KW_GOTO KW_PACKED KW_RETURN KW_SIZEOF KW_SWITCH KW_WHILE
%token BITWISE_AND BITWISE_NOT BITWISE_OR BITWISE_XOR
%token INCREMENT DECREMENT ARROW
%token ADD_ASSIGN DIV_ASSIGN MOD_ASSIGN MUL_ASSIGN SUB_ASSIGN BITWISE_AND_ASSIGN BITWISE_OR_ASSIGN BITWISE_XOR_ASSIGN
%token KW_QUALIFIER KW_STORAGE KW_VOID KW_STRUCT KW_UNION KW_ENUM KW_SIGNED

%right ASSIGN ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN
%nonassoc LOWER_THAN_ELSE
%nonassoc KW_ELSE
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

%type <intValue> expr stmt compound_stmt selection_stmt iteration_stmt jump_stmt for_init for_cond for_step

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

    /* Identificadores e Atribuições */
    | IDENT                 { $$ = 0; free($1); }
    | IDENT ASSIGN expr     { $$ = $3; free($1); }
    | IDENT ADD_ASSIGN expr { $$ = $3; free($1); }
    | IDENT SUB_ASSIGN expr { $$ = $3; free($1); }
    | IDENT MUL_ASSIGN expr { $$ = $3; free($1); }
    | IDENT DIV_ASSIGN expr { $$ = $3; free($1); }
    | IDENT MOD_ASSIGN expr { $$ = $3; free($1); }

    /* Literais String */
    | STRING                { $$ = 0; free($1); }
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
    | compound_stmt
    | selection_stmt
    | iteration_stmt
    | jump_stmt
    | SEMICOLON {
        $$ = 0;
      }
    ;

compound_stmt:
      LBRACE stmt_list RBRACE {
        $$ = 0;
        printf("Bloco de comandos ({ }) identificado.\n");
      }
    ;

stmt_list:
      %empty
    | stmt_list stmt
    | stmt_list error SEMICOLON {
          fprintf(stderr, "[Erro Sintatico] Erro recuperado no bloco ate ';'\n");
          yyerrok;
          yyclearin;
      }
    ;

selection_stmt:
      KW_IF LPAREN expr RPAREN stmt %prec KW_ELSE {
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

iteration_stmt:
      KW_WHILE LPAREN expr RPAREN stmt {
        $$ = 0;
        printf("Comando WHILE identificado.\n");
      }
    | KW_FOR LPAREN for_init SEMICOLON for_cond SEMICOLON for_step RPAREN stmt {
        $$ = 0;
        printf("Comando FOR identificado.\n");
      }
    ;

for_init:
      %empty                              { $$ = 0; }
    | expr                                { $$ = $1; }
    | type_specifier init_declarator_list { $$ = 0; }
    ;

for_cond:
      %empty                              { $$ = 1; }
    | expr                                { $$ = $1; }
    ;

for_step:
      %empty                              { $$ = 0; }
    | expr                                { $$ = $1; }
    ;

jump_stmt:
      KW_RETURN SEMICOLON {
        $$ = 0;
        printf("Comando RETURN identificado.\n");
      }
    | KW_RETURN expr SEMICOLON {
        $$ = $2;
        printf("Comando RETURN identificado (valor: %d).\n", $2);
      }
    | KW_BREAK SEMICOLON {
        $$ = 0;
        printf("Comando BREAK identificado.\n");
      }
    | KW_CONTINUE SEMICOLON {
        $$ = 0;
        printf("Comando CONTINUE identificado.\n");
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