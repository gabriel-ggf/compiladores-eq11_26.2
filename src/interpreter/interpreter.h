#ifndef INTERPRETER_H
#define INTERPRETER_H

/*
 * Interpretador propriamente dito: percorre a AST gerada pelo parser e
 * executa o programa diretamente (sem gerar codigo intermediario nem
 * bytecode).
 *
 * A classe Interpreter do C++ virou um struct + funcoes que recebem o
 * interpretador como primeiro parametro, e o std::unordered_map da tabela
 * de simbolos virou uma lista encadeada propria (nome -> valor), conforme
 * docs/decisoes-tecnicas.md.
 */

#include "ast/ast.h"

typedef struct symbol symbol;

typedef struct {
    symbol *symbols; /* tabela de simbolos: nome -> valor (double) */
} interpreter;

/* Prepara um interpretador com a tabela de simbolos vazia. */
void interpreter_init(interpreter *interp);

/* Executa um programa completo a partir do no raiz. */
void interpreter_run(interpreter *interp, node *program);

/* Libera a tabela de simbolos. Nao libera a AST (ver ast_free). */
void interpreter_destroy(interpreter *interp);

#endif /* INTERPRETER_H */
