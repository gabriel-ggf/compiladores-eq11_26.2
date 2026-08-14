%{
/*
 * Analisador léxico (scanner) gerado pelo Flex.
 * Reconhece os tokens da linguagem e os repassa para o parser (Bison).
 */
#include <cstdlib>
#include <cstring>
#include "parser.hpp"
%}

%option noyywrap
%option yylineno

DIGIT   [0-9]
ID      [a-zA-Z_][a-zA-Z0-9_]*

%%

"print"                  { return PRINT; }

{DIGIT}+(\.{DIGIT}+)?    { yylval.dval = atof(yytext); return NUMBER; }
{ID}                     { yylval.sval = strdup(yytext); return IDENTIFIER; }

"+"                      { return '+'; }
"-"                      { return '-'; }
"*"                      { return '*'; }
"/"                      { return '/'; }
"="                      { return '='; }
"("                      { return '('; }
")"                      { return ')'; }
";"                      { return ';'; }

"//".*                   { /* comentario de linha: ignora */ }
[ \t\r]+                 { /* espacos: ignora */ }
\n                       { /* quebra de linha: ignora (yylineno cuida da contagem) */ }

.                        { fprintf(stderr, "Caractere invalido na linha %d: %s\n", yylineno, yytext); }

%%
