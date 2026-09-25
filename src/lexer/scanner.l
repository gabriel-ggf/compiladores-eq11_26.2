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

";"                                  { return SEMICOLON; }

"if"                                 { return KW_IF; }
"else"                               { return KW_ELSE; }

"=="                                 { return EQUAL; }
"!="                                 { return DIFF; }
">="                                 { return GREATER_E; }
"<="                                 { return LESS_E; }
">"                                  { return GREATER_T; }
"<"                                  { return LESS_T; }

"||"                                 { return OR; }
"&&"                                 { return AND; }
"!"                                  { return NOT; }

[ \t\r\n]+                           { /* espacos e quebras de linha: ignora */ }

.                                    { fprintf(stderr, "[Erro Léxico] %s\n", yytext); }

%%