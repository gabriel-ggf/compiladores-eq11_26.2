/*
 * Construcao e liberacao dos nos da AST.
 *
 * Em C++ o trabalho destes construtores ficava inline no header (AST.hpp);
 * em C ele vira um modulo proprio, como ja previsto no README e em
 * docs/decisoes-tecnicas.md.
 */

#include "ast/ast.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAPACIDADE_INICIAL 8

static void *alocar(size_t tamanho) {
    void *bloco = malloc(tamanho);
    if (bloco == NULL) {
        fprintf(stderr, "Memoria insuficiente ao montar a AST\n");
        exit(EXIT_FAILURE);
    }
    return bloco;
}

/* strdup nao e padrao C: copiamos na mao para manter o codigo portavel. */
static char *copiar_texto(const char *texto) {
    size_t tamanho = strlen(texto) + 1;
    char *copia = alocar(tamanho);
    memcpy(copia, texto, tamanho);
    return copia;
}

static node *novo_no(node_type type) {
    node *n = alocar(sizeof *n);
    n->type = type;
    return n;
}

node *ast_new_number(double value) {
    node *n = novo_no(NODE_NUMBER);
    n->as.number.value = value;
    return n;
}

node *ast_new_variable(const char *name) {
    node *n = novo_no(NODE_VARIABLE);
    n->as.variable.name = copiar_texto(name);
    return n;
}

node *ast_new_binary_op(char op, node *left, node *right) {
    node *n = novo_no(NODE_BINARY_OP);
    n->as.binary_op.op = op;
    n->as.binary_op.left = left;
    n->as.binary_op.right = right;
    return n;
}

node *ast_new_assign(const char *name, node *expr) {
    node *n = novo_no(NODE_ASSIGN);
    n->as.assign.name = copiar_texto(name);
    n->as.assign.expr = expr;
    return n;
}

node *ast_new_print(node *expr) {
    node *n = novo_no(NODE_PRINT);
    n->as.print.expr = expr;
    return n;
}

node *ast_new_program(void) {
    node *n = novo_no(NODE_PROGRAM);
    n->as.program.statements = NULL;
    n->as.program.count = 0;
    n->as.program.capacity = 0;
    return n;
}

/* Vetor dinamico simples, no lugar do std::vector da versao C++. */
void ast_program_add(node *program, node *statement) {
    program_node *p = &program->as.program;

    if (p->count == p->capacity) {
        size_t nova_capacidade = (p->capacity == 0)
            ? CAPACIDADE_INICIAL
            : p->capacity * 2;
        node **maior = realloc(p->statements, nova_capacidade * sizeof *maior);
        if (maior == NULL) {
            fprintf(stderr, "Memoria insuficiente ao montar a AST\n");
            exit(EXIT_FAILURE);
        }
        p->statements = maior;
        p->capacity = nova_capacidade;
    }

    p->statements[p->count++] = statement;
}

void ast_free(node *root) {
    if (root == NULL) {
        return;
    }

    switch (root->type) {
        case NODE_NUMBER:
            break;

        case NODE_VARIABLE:
            free(root->as.variable.name);
            break;

        case NODE_BINARY_OP:
            ast_free(root->as.binary_op.left);
            ast_free(root->as.binary_op.right);
            break;

        case NODE_ASSIGN:
            free(root->as.assign.name);
            ast_free(root->as.assign.expr);
            break;

        case NODE_PRINT:
            ast_free(root->as.print.expr);
            break;

        case NODE_PROGRAM:
            for (size_t i = 0; i < root->as.program.count; i++) {
                ast_free(root->as.program.statements[i]);
            }
            free(root->as.program.statements);
            break;
    }

    free(root);
}
