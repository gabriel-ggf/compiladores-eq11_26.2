#pragma once

// Definição da Árvore Sintática Abstrata (AST) do interpretador.
//
// Cada nó representa um pedaço do programa fonte já "compreendido" pelo
// parser (Bison). O Interpreter (src/interpreter) percorre essa árvore e
// executa/avalia cada nó (estratégia conhecida como "tree-walking
// interpreter").
//
// Simplificação deliberada: os nós são alocados com `new` e nunca
// desalocados (o processo libera tudo ao terminar). Para um trabalho de
// disciplina isso é aceitável e é um ótimo ponto para discutir na
// entrevista final (ex.: como migrar para std::unique_ptr). Ver
// docs/decisoes-tecnicas.md.

#include <string>
#include <vector>

enum class NodeType {
    Number,
    Variable,
    BinaryOp,
    Assign,
    Print,
    Program
};

struct Node {
    NodeType type;
    virtual ~Node() = default;
};

// Literal numérico, ex.: 10, 3.14
struct NumberNode : Node {
    double value;
    explicit NumberNode(double v) : value(v) { type = NodeType::Number; }
};

// Referência a uma variável, ex.: x
struct VariableNode : Node {
    std::string name;
    explicit VariableNode(std::string n) : name(std::move(n)) { type = NodeType::Variable; }
};

// Operação binária, ex.: a + b
struct BinaryOpNode : Node {
    char op; // '+', '-', '*', '/'
    Node* left;
    Node* right;
    BinaryOpNode(char o, Node* l, Node* r) : op(o), left(l), right(r) { type = NodeType::BinaryOp; }
};

// Atribuição, ex.: x = 10;
struct AssignNode : Node {
    std::string name;
    Node* expr;
    AssignNode(std::string n, Node* e) : name(std::move(n)), expr(e) { type = NodeType::Assign; }
};

// Comando de impressão, ex.: print x + 1;
struct PrintNode : Node {
    Node* expr;
    explicit PrintNode(Node* e) : expr(e) { type = NodeType::Print; }
};

// Raiz da árvore: lista de statements do programa.
struct ProgramNode : Node {
    std::vector<Node*> statements;
    ProgramNode() { type = NodeType::Program; }
};
