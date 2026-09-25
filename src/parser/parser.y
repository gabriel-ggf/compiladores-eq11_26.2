%{
    #include <stdio.h>

    int yylex(void);
    void yyerror(const char *mensagem);
%}

%union { int intValue; }

%token <intValue> NUMBER
%token PLUS MINUS TIMES DIVIDE MODULE RPAREN LPAREN SEMICOLON
%token EQUAL DIFF GREATER_E GREATER_T LESS_E LESS_T AND OR NOT
%token KW_IF KW_ELSE

%right KW_ELSE
%left OR
%left AND
%left EQUAL DIFF
%left GREATER_E GREATER_T LESS_E LESS_T
%left PLUS MINUS
%left TIMES DIVIDE MODULE
%right NOT

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

expr:
      expr PLUS expr    { $$ = $1 + $3; }
    | expr MINUS expr   { $$ = $1 - $3; }
    | expr TIMES expr   { $$ = $1 * $3; }
    | expr DIVIDE expr  { if ($3 == 0) { yyerror("Erro Semantico: divisão por zero"); $$ = 0; } else { $$ = $1 / $3; } }
    | expr MODULE expr  { if ($3 == 0) { yyerror("Erro Semantico: divisão por zero"); $$ = 0; } else { $$ = $1 % $3; } }
    | LPAREN expr RPAREN{ $$ = $2; }
    | NUMBER            { $$ = $1; }
    | expr EQUAL expr   { $$ = ($1 == $3); }
    | expr DIFF expr    { $$ = ($1 != $3); }
    | expr GREATER_E expr  { $$ = ($1 >= $3); }
    | expr LESS_E expr     { $$ = ($1 <= $3); }
    | expr GREATER_T expr  { $$ = ($1 > $3); }
    | expr LESS_T expr     { $$ = ($1 < $3); }
    | expr AND expr     { $$ = ($1 && $3); }
    | expr OR expr      { $$ = ($1 || $3); }
    | NOT expr          { $$ = (!$2); }
    ;

stmt:
      expr SEMICOLON                        { $$ = $1; printf("Resultado: %d\n", $1); }
    | KW_IF LPAREN expr RPAREN stmt %prec KW_ELSE {
        if ($3 != 0) {
            printf("IF TRUE\n");
        } else {
            printf("IF FALSE\n");
        }
     }

%%

int main(void) {
    return yyparse();
}

void yyerror(const char *s) {
    fprintf(stderr, "ERRO - %s\n", s);
}