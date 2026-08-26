%{
#include <stdio.h>
%}

%option yylineno

%%

char|double|float|int|long|short|signed|unsigned|void|struct|union|enum {
                                        printf("KW_TYPE(%s)\n", yytext);
                                      }
const|volatile                      { printf("KW_QUALIFIER(%s)\n", yytext); }
auto|extern|register|static|typedef { printf("KW_STORAGE(%s)\n", yytext); }

if                                   { printf("KW_IF\n"); }
else                                 { printf("KW_ELSE\n"); }
switch                               { printf("KW_SWITCH\n"); }
case                                 { printf("KW_CASE\n"); }
default                              { printf("KW_DEFAULT\n"); }
while                                { printf("KW_WHILE\n"); }
do                                   { printf("KW_DO\n"); }
for                                  { printf("KW_FOR\n"); }
break                                { printf("KW_BREAK\n"); }
continue                            { printf("KW_CONTINUE\n"); }
goto                                 { printf("KW_GOTO\n"); }
return                               { printf("KW_RETURN\n"); }
sizeof                               { printf("KW_SIZEOF\n"); }
_Packed                              { printf("KW_PACKED\n"); }

"//".*                               { /* comentario de linha */ }

"/*"([^*]|\*+[^*/])*\*+\/            { /* comentario de bloco */ }

\"([^"\\]|\\.)*\"                    { printf("STRING(%s)\n", yytext); }

\'([^'\\]|\\.)\'                     { printf("CHAR(%s)\n", yytext); }

[a-zA-Z_][a-zA-Z0-9_]*               { printf("IDENT(%s)\n", yytext); }

[0-9]+(\.[0-9]+)?                    { printf("NUMBER(%s)\n", yytext); }

"<<="|">>="|"=="|"!="|"<="|">="|"&&"|"||"|"++"|"--"|"+="|"-="|"*="|"/="|"%="|"&="|"^="|"|="|"->"|"<<"|">>" {
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
