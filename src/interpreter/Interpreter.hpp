#pragma once

// Interpretador propriamente dito: percorre a AST gerada pelo parser e
// executa o programa diretamente (sem gerar código intermediário nem
// bytecode). Mantém uma tabela de símbolos simples (nome -> valor) para
// suportar variáveis.

#include "ast/AST.hpp"
#include <string>
#include <unordered_map>

class Interpreter {
public:
    // Executa um programa completo a partir do nó raiz.
    void run(ProgramNode* program);

private:
    std::unordered_map<std::string, double> symbolTable;

    // Avalia uma expressão e retorna o valor numérico resultante.
    double evaluate(Node* node);

    // Executa um statement (atribuição, print, ...).
    void execute(Node* node);
};
