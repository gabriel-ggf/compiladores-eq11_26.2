/*
 * Ponto de entrada do interpretador.
 *
 * Uso: ./interpretador <arquivo-fonte>
 *
 * Fluxo: abre o arquivo -> Flex/Bison fazem analise lexica e sintatica e
 * constroem a AST (root_node) -> o interpretador percorre a AST e executa
 * o programa.
 *
 * Observacao: src/parser/parser.y ainda avalia as expressoes direto e tem
 * o proprio main(), entao este arquivo ainda nao e ligado ao executavel
 * final (ver CMakeLists.txt). Ele passa a ser o ponto de entrada quando o
 * parser passar a preencher root_node.
 */

#include <stdio.h>
#include <stdlib.h>

#include "ast/ast.h"
#include "interpreter/interpreter.h"

extern int yyparse(void);
extern FILE *yyin;
extern node *root_node;

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo-fonte>\n", argv[0]);
        return 1;
    }

    FILE *arquivo = fopen(argv[1], "r");
    if (arquivo == NULL) {
        fprintf(stderr, "Nao foi possivel abrir o arquivo: %s\n", argv[1]);
        return 1;
    }

    yyin = arquivo;
    int resultado_parse = yyparse();
    fclose(arquivo);

    if (resultado_parse != 0 || root_node == NULL) {
        fprintf(stderr, "Falha ao analisar o programa.\n");
        return 1;
    }

    interpreter interp;
    interpreter_init(&interp);
    interpreter_run(&interp, root_node);
    interpreter_destroy(&interp);
    ast_free(root_node);

    return 0;
}
