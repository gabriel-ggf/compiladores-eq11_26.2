// Ponto de entrada do interpretador.
//
// Uso: ./interpretador <arquivo-fonte>
//
// Fluxo: abre o arquivo -> Flex/Bison fazem análise léxica e sintática e
// constroem a AST (rootNode) -> Interpreter percorre a AST e executa o
// programa.

#include <cstdio>

#include "ast/AST.hpp"
#include "interpreter/Interpreter.hpp"

extern int yyparse();
extern FILE* yyin;
extern ProgramNode* rootNode;

int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr, "Uso: %s <arquivo-fonte>\n", argv[0]);
        return 1;
    }

    FILE* file = std::fopen(argv[1], "r");
    if (!file) {
        std::fprintf(stderr, "Nao foi possivel abrir o arquivo: %s\n", argv[1]);
        return 1;
    }

    yyin = file;
    int parseResult = yyparse();
    std::fclose(file);

    if (parseResult != 0 || rootNode == nullptr) {
        std::fprintf(stderr, "Falha ao analisar o programa.\n");
        return 1;
    }

    Interpreter interpreter;
    interpreter.run(rootNode);

    return 0;
}
