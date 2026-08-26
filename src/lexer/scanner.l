%{
#include <stdio.h>
%}

%option yylineno

%%

int|float|double|char|void|struct   { printf("KW_TYPE(%s)\n", yytext); }
if                                   { printf("KW_IF\n"); }
else                                 { printf("KW_ELSE\n"); }
while                                { printf("KW_WHILE\n"); }
for                                  { printf("KW_FOR\n"); }
do                                   { printf("KW_DO\n"); }
return                               { printf("KW_RETURN\n"); }
break                                { printf("KW_BREAK\n"); }
continue                            { printf("KW_CONTINUE\n"); }

"//".*                               { /* comentario de linha */ }

"/*"([^*]|\*+[^*/])*\*+\/            { /* comentario de bloco */ }

\"([^"\\]|\\.)*\"                    { printf("STRING(%s)\n", yytext); }

\'([^'\\]|\\.)\'                     { printf("CHAR(%s)\n", yytext); }

[a-zA-Z_][a-zA-Z0-9_]*               { printf("IDENT(%s)\n", yytext); }

[0-9]+(\.[0-9]+)?                    { printf("NUMBER(%s)\n", yytext); }

"=="|"!="|"<="|">="|"&&"|"||"|"++"|"--"|"+="|"-="|"*="|"/="|"->" {
                                        printf("OPERATOR(%s)\n", yytext);
                                      }

[+\-*/%=<>!&|^~]                     { printf("OPERATOR(%s)\n", yytext); }

[{}()\[\];,.]                        { printf("PUNCT(%s)\n", yytext); }

[ \t\r\n]+                           { /* espacos e quebras de linha: ignora */ }

.                                    { printf("UNKNOWN(%s)\n", yytext); }

%%

int yywrap(void) {
    return 1;
}

int main(void) {
    yylex();
    return 0;
}
