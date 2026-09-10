%{
#include <stdio.h>
#include <stdlib.h>
#include "parser.tab.h"
%}

%option noinput
%option nounput
%option noyywrap

%%

[0-9]+(\.[0-9]+)?                    { yylval.intValue = atoi(yytext); return NUMBER; }

"+"                                  { return PLUS; }
"-"                                  { return MINUS; }
"*"                                  { return TIMES; }
"/"                                  { return DIVIDE; }
"%"                                  { return MODULE; }

"("                                  { return LPAREN; }
")"                                  { return RPAREN; }

\n                                   { return EOL; }

[ \t\r]+                            { /* espacos e quebras de linha: ignora */ }

.                                    { fprintf(stderr, "Erro léxico: %s\n", yytext); }

%%