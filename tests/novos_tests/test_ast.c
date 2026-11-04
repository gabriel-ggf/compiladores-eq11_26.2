/*
 * Testes unitarios da AST (src/ast/ast.c).
 * Sem framework: usa assert. Se alguma verificacao falhar, o programa
 * aborta e mostra o arquivo e a linha do assert.
 */
#undef NDEBUG              /* garante que o assert funciona mesmo em Release */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "ast/ast.h"

static void test_number(void) {
    node *n = ast_new_number(3.5);
    assert(n != NULL);
    assert(n->type == NODE_NUMBER);
    assert(n->as.number.value == 3.5);
    ast_free(n);
}

static void test_variable_copia_o_nome(void) {
    char nome[] = "abc";
    node *n = ast_new_variable(nome);
    assert(n->type == NODE_VARIABLE);
    assert(strcmp(n->as.variable.name, "abc") == 0);

    /* O no tem copia propria: alterar o original nao muda a AST. */
    nome[0] = 'z';
    assert(strcmp(n->as.variable.name, "abc") == 0);
    assert(n->as.variable.name != nome);
    ast_free(n);
}

static void test_binary_op(void) {
    node *esq = ast_new_number(2);
    node *dir = ast_new_number(3);
    node *n = ast_new_binary_op('+', esq, dir);
    assert(n->type == NODE_BINARY_OP);
    assert(n->as.binary_op.op == '+');
    assert(n->as.binary_op.left == esq);
    assert(n->as.binary_op.right == dir);
    ast_free(n);                       /* libera os filhos tambem */
}

static void test_assign(void) {
    node *n = ast_new_assign("x", ast_new_number(10));
    assert(n->type == NODE_ASSIGN);
    assert(strcmp(n->as.assign.name, "x") == 0);
    assert(n->as.assign.expr->type == NODE_NUMBER);
    assert(n->as.assign.expr->as.number.value == 10);
    ast_free(n);
}

static void test_print(void) {
    node *n = ast_new_print(ast_new_variable("x"));
    assert(n->type == NODE_PRINT);
    assert(n->as.print.expr->type == NODE_VARIABLE);
    ast_free(n);
}

static void test_program_vazio(void) {
    node *p = ast_new_program();
    assert(p->type == NODE_PROGRAM);
    assert(p->as.program.count == 0);
    assert(p->as.program.statements == NULL);
    ast_free(p);
}

static void test_program_cresce_alem_da_capacidade_inicial(void) {
    /* A capacidade inicial e 8: 20 statements forcam o realloc. */
    node *p = ast_new_program();
    for (int i = 0; i < 20; i++) {
        ast_program_add(p, ast_new_print(ast_new_number(i)));
    }
    assert(p->as.program.count == 20);
    assert(p->as.program.capacity >= 20);
    for (int i = 0; i < 20; i++) {
        node *s = p->as.program.statements[i];
        assert(s->type == NODE_PRINT);
        assert(s->as.print.expr->as.number.value == i);   /* ordem preservada */
    }
    ast_free(p);
}

static void test_arvore_aninhada(void) {
    /* x = 2 + 3 * 4;  ->  assign(x, +(2, *(3, 4))) */
    node *mult = ast_new_binary_op('*', ast_new_number(3), ast_new_number(4));
    node *soma = ast_new_binary_op('+', ast_new_number(2), mult);
    node *p = ast_new_program();
    ast_program_add(p, ast_new_assign("x", soma));

    node *s = p->as.program.statements[0];
    assert(s->type == NODE_ASSIGN);
    assert(s->as.assign.expr->as.binary_op.op == '+');
    assert(s->as.assign.expr->as.binary_op.right->as.binary_op.op == '*');
    ast_free(p);
}

static void test_free_de_null_e_seguro(void) {
    ast_free(NULL);   /* nao deve falhar */
}

int main(void) {
    test_number();
    test_variable_copia_o_nome();
    test_binary_op();
    test_assign();
    test_print();
    test_program_vazio();
    test_program_cresce_alem_da_capacidade_inicial();
    test_arvore_aninhada();
    test_free_de_null_e_seguro();
    printf("test_ast: todos os testes passaram\n");
    return 0;
}
