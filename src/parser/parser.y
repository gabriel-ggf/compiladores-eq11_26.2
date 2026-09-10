%{
    #include <stdio.h>
    int yylex(void);
    void yyerror(const char *mensagem);
%}

%union { int intValue; }

%token <intValue> NUMBER
%token PLUS MINUS TIMES DIVIDE MODULE RPAREN LPAREN

%token EOL

%left PLUS MINUS
%left TIMES DIVIDE MODULE

%type <intValue> expr

%%

input:
    %empty
    | input line
    ;

line:
    EOL
    | expr {printf("Resultado: %d\n", $1);}
    | error EOL {yyerrok;}
    ;

expr:
      expr PLUS expr    { $$ = $1 + $3; }
    | expr MINUS expr   { $$ = $1 - $3; }
    | expr TIMES expr   { $$ = $1 * $3; }
    | expr DIVIDE expr  { if ($3 == 0) { yyerror("divisão por zero"); $$ = 0; } else { $$ = $1 / $3; } }
    | expr MODULE expr  { $$ = $1 % $3; }
    | LPAREN expr RPAREN{ $$ = $2; }
    | NUMBER            { $$ = $1; }
    ;

%%

int main(void) {
    return yyparse();
}

void yyerror(const char *s) {
    fprintf(stderr, "Erro sintático: %s\n", s);
}