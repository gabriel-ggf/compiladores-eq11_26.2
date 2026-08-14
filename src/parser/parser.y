%{
/*
 * Analisador sintático (parser) gerado pelo Bison.
 * Reconhece a gramática da linguagem e monta a AST (ver src/ast/AST.hpp)
 * conforme cada regra é reduzida.
 */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "ast/AST.hpp"

extern int yylex();
extern int yylineno;
void yyerror(const char* s);

// Raiz da AST resultante do parse; consumida em src/main.cpp.
ProgramNode* rootNode = nullptr;
%}

%union {
    double dval;
    char*  sval;
    Node*  node;
}

%token <dval> NUMBER
%token <sval> IDENTIFIER
%token PRINT

%type <node> expr statement stmt_list

%left '+' '-'
%left '*' '/'

%%

program:
    stmt_list { rootNode = static_cast<ProgramNode*>($1); }
    ;

stmt_list:
      /* vazio */          { $$ = new ProgramNode(); }
    | stmt_list statement  {
        static_cast<ProgramNode*>($1)->statements.push_back($2);
        $$ = $1;
      }
    ;

statement:
      IDENTIFIER '=' expr ';' { $$ = new AssignNode($1, $3); free($1); }
    | PRINT expr ';'          { $$ = new PrintNode($2); }
    ;

expr:
      expr '+' expr   { $$ = new BinaryOpNode('+', $1, $3); }
    | expr '-' expr   { $$ = new BinaryOpNode('-', $1, $3); }
    | expr '*' expr   { $$ = new BinaryOpNode('*', $1, $3); }
    | expr '/' expr   { $$ = new BinaryOpNode('/', $1, $3); }
    | '(' expr ')'    { $$ = $2; }
    | NUMBER          { $$ = new NumberNode($1); }
    | IDENTIFIER      { $$ = new VariableNode($1); free($1); }
    ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "Erro de sintaxe na linha %d: %s\n", yylineno, s);
}
