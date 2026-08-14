#include "interpreter/Interpreter.hpp"

#include <cstdio>
#include <stdexcept>

double Interpreter::evaluate(Node* node) {
    switch (node->type) {
        case NodeType::Number:
            return static_cast<NumberNode*>(node)->value;

        case NodeType::Variable: {
            auto* var = static_cast<VariableNode*>(node);
            auto it = symbolTable.find(var->name);
            if (it == symbolTable.end()) {
                throw std::runtime_error("Variavel nao definida: " + var->name);
            }
            return it->second;
        }

        case NodeType::BinaryOp: {
            auto* bin = static_cast<BinaryOpNode*>(node);
            double left = evaluate(bin->left);
            double right = evaluate(bin->right);
            switch (bin->op) {
                case '+': return left + right;
                case '-': return left - right;
                case '*': return left * right;
                case '/':
                    if (right == 0.0) {
                        throw std::runtime_error("Divisao por zero");
                    }
                    return left / right;
                default:
                    throw std::runtime_error("Operador desconhecido");
            }
        }

        default:
            throw std::runtime_error("No invalido em contexto de expressao");
    }
}

void Interpreter::execute(Node* node) {
    switch (node->type) {
        case NodeType::Assign: {
            auto* assign = static_cast<AssignNode*>(node);
            symbolTable[assign->name] = evaluate(assign->expr);
            break;
        }

        case NodeType::Print: {
            auto* print = static_cast<PrintNode*>(node);
            std::printf("%g\n", evaluate(print->expr));
            break;
        }

        default:
            throw std::runtime_error("Statement invalido");
    }
}

void Interpreter::run(ProgramNode* program) {
    for (Node* stmt : program->statements) {
        execute(stmt);
    }
}
