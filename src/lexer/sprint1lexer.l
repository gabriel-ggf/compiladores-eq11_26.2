%{
#include <stdio.h>
#include <stdlib.h>
#include "parser.tab.h"
%}

%option yylineno

%%

char|double|float|int|long|short|signed|unsigned|void|struct|union|enum { return KW_TYPE; }
const|volatile                      { return KW_QUALIFIER; }
auto|extern|register|static|typedef { return KW_STORAGE; }

if                                  { return KW_IF; }
else                                { return KW_ELSE; }
switch                              { return KW_SWITCH; }
case                                { return KW_CASE; }
default                             { return KW_DEFAULT; }
while                               { return KW_WHILE; }
do                                  { return KW_DO; }
for                                 { return KW_FOR; }
break                               { return KW_BREAK; }
continue                            { return KW_CONTINUE; }
goto                                { return KW_GOTO; }
return                              { return KW_RETURN; }
sizeof                              { return KW_SIZEOF; }
_Packed                             { return KW_PACKED; }

"//".*                               { /* comentario de linha */ }

"/*"([^*]|\*+[^*/])*\*+\/            { /* comentario de bloco */ }

\"([^"\\]|\\.)*\"                    { return STRING; }

\'([^'\\]|\\.)\'                     { return CHAR; }

[a-zA-Z_][a-zA-Z0-9_]*               { return IDENT; }

[0-9]+(\.[0-9]+)?                    { yylval.intValue = atoi(yytext); return NUMBER; }

"<<"                                { return SHIFT_LEFT; }
">>"                                { return SHIFT_RIGHT; }
"<<="                               { return SHIFT_LEFT_ASSIGN; }
">>="                               { return SHIFT_RIGHT_ASSIGN; }
"=="                                { return EQUAL; }
"!="                                { return NOT_EQUAL; }
"<="                                { return LESS_EQUAL; }
">="                                { return GREATER_EQUAL; }
"&&"                                { return LOGICAL_AND; }
"||"                                { return LOGICAL_OR; }
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

"+"                                 { return PLUS; }
"-"                                 { return MINUS; }
"*"                                 { return TIMES; }
"/"                                 { return DIVIDE; }
"%"                                 { return MODULE; }
"="                                 { return ASSIGN; }
"<"                                 { return LESS_THAN; }
">"                                 { return GREATER_THAN; }
"!"                                 { return NOT; }
"&"                                 { return BITWISE_AND; }
"|"                                 { return BITWISE_OR; }
"^"                                 { return BITWISE_XOR; }
"~"                                 { return BITWISE_NOT; }

"{"                                 { return LBRACE; }
"}"                                 { return RBRACE; }
"("                                 { return LPAREN; }
")"                                 { return RPAREN; }
"["                                 { return LBRACK; }
"]"                                 { return RBRACK; }
";"                                 { return SEMICOLON; }
","                                 { return COMMA; }
"."                                 { return DOT; }

\n                                  { return EOL; }

[ \t\r]+                            { /* espacos e quebras de linha: ignora */ }

.                                    { fprintf(stderr, "Erro léxico: %s\n", yytext); }

%%

int yywrap(void) {
    return 1;
}

int main(void) {
    yylex();
    return 0;
}
