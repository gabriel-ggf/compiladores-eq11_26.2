%{
#include <stdio.h>
#include <stdlib.h>
#include "parser.tab.h"
%}

%option noinput
%option nounput
%option noyywrap

%%

\"([^"\\]|\\.)*\"                    { return STRING; }

\'([^'\\]|\\.)\'                     { return CHAR; }

[a-zA-Z_][a-zA-Z0-9_]*               { return IDENT; }

[0-9]+(\.[0-9]+)?                    { yylval.intValue = atoi(yytext); return NUMBER; }

"("                                  { return LPAREN; }
")"                                  { return RPAREN; }

";"                                  { return SEMICOLON; }

"if"                                 { return KW_IF; }
"else"                               { return KW_ELSE; }

"switch"                              { return KW_SWITCH; }
"case"                                { return KW_CASE; }
"default"                             { return KW_DEFAULT; }
"while"                               { return KW_WHILE; }
"do"                                  { return KW_DO; }
"for"                                 { return KW_FOR; }
"break"                               { return KW_BREAK; }
"continue"                            { return KW_CONTINUE; }
"goto"                                { return KW_GOTO; }
"return"                              { return KW_RETURN; }
"sizeof"                              { return KW_SIZEOF; }
"_Packed"                             { return KW_PACKED; }

"++"                                { return INCREMENT; }
"--"                                { return DECREMENT; }
"+="                                { return ADD_ASSIGN; }
"-="                                { return SUB_ASSIGN; }
"*="                                { return MUL_ASSIGN; }
"/="                                { return DIV_ASSIGN; }
"%="                                { return MOD_ASSIGN; }
"&="                                { return BITWISE_AND_ASSIGN; }
"^="                                { return BITWISE_XOR_ASSIGN; }
"|="                                { return BITWISE_OR_ASSIGN; }
"->"                                { return ARROW; }

"=="                                 { return EQUAL; }
"!="                                 { return DIFF; }
">="                                 { return GREATER_E; }
"<="                                 { return LESS_E; }
">"                                  { return GREATER_T; }
"<"                                  { return LESS_T; }

"+"                                  { return PLUS; }
"-"                                  { return MINUS; }
"*"                                  { return TIMES; }
"/"                                  { return DIVIDE; }
"%"                                  { return MODULE; }

"||"                                 { return OR; }
"&&"                                 { return AND; }
"!"                                  { return NOT; }

"&"                                 { return BITWISE_AND; }
"|"                                 { return BITWISE_OR; }
"^"                                 { return BITWISE_XOR; }
"~"                                 { return BITWISE_NOT; }

[ \t\r\n]+                           { /* espacos e quebras de linha: ignora */ }

.                                    { fprintf(stderr, "[Erro Léxico] %s\n", yytext); }

%%