/*
 * Teste de integracao: texto -> scanner -> parser -> AST.
 * Verifica a forma da arvore que o parser monta (nao executa nada).
 */
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "ast/ast.h"
#include "ast/table.h"
int yyparse(void);                /* parser entry point */

extern node *root_node;

/* Funcoes geradas pelo Flex para ler de uma string em vez de um arquivo. */
typedef struct yy_buffer_state *YY_BUFFER_STATE;
YY_BUFFER_STATE yy_scan_string(const char *text);
void yy_delete_buffer(YY_BUFFER_STATE buffer);

/* Faz o parse de um texto e devolve a raiz (o chamador libera com ast_free). */
static node *parse(const char *texto) {
    freeTable();                 /* cada teste comeca sem variaveis */
    root_node = NULL;
    YY_BUFFER_STATE buf = yy_scan_string(texto);
    int rc = yyparse();
    yy_delete_buffer(buf);
    assert(rc == 0);
    assert(root_node != NULL);
    return root_node;
}

static node *stmt(node *programa, size_t i) {
    assert(i < programa->as.program.count);
    return programa->as.program.statements[i];
}

static void test_atribuicao_e_print(void) {
    node *p = parse("x = 10; print x + 1;");
    assert(p->type == NODE_PROGRAM);
    assert(p->as.program.count == 2);

    node *a = stmt(p, 0);
    assert(a->type == NODE_ASSIGN);
    assert(strcmp(a->as.assign.name, "x") == 0);
    assert(a->as.assign.expr->type == NODE_NUMBER);
    assert(a->as.assign.expr->as.number.value == 10);

    node *pr = stmt(p, 1);
    assert(pr->type == NODE_PRINT);
    node *soma = pr->as.print.expr;
    assert(soma->type == NODE_BINARY_OP && soma->as.binary_op.op == '+');
    assert(soma->as.binary_op.left->type == NODE_VARIABLE);
    assert(strcmp(soma->as.binary_op.left->as.variable.name, "x") == 0);
    assert(soma->as.binary_op.right->as.number.value == 1);
    ast_free(p);
}

static void test_precedencia_multiplicacao_antes_de_soma(void) {
    node *p = parse("print 2 + 3 * 4;");
    node *e = stmt(p, 0)->as.print.expr;
    assert(e->as.binary_op.op == '+');                       /* raiz e a soma */
    assert(e->as.binary_op.right->as.binary_op.op == '*');   /* 3 * 4 fica embaixo */
    ast_free(p);
}

static void test_parenteses_mudam_a_arvore(void) {
    node *p = parse("print (2 + 3) * 4;");
    node *e = stmt(p, 0)->as.print.expr;
    assert(e->as.binary_op.op == '*');
    assert(e->as.binary_op.left->as.binary_op.op == '+');
    ast_free(p);
}

static void test_associatividade_a_esquerda(void) {
    node *p = parse("print 10 - 3 - 2;");                    /* (10 - 3) - 2 */
    node *e = stmt(p, 0)->as.print.expr;
    assert(e->as.binary_op.op == '-');
    assert(e->as.binary_op.left->type == NODE_BINARY_OP);
    assert(e->as.binary_op.right->as.number.value == 2);
    ast_free(p);
}

static void test_modulo(void) {
    node *p = parse("print 7 % 4;");
    assert(stmt(p, 0)->as.print.expr->as.binary_op.op == '%');
    ast_free(p);
}

static void test_decimal_nao_e_truncado(void) {
    node *p = parse("print 3.5;");
    assert(stmt(p, 0)->as.print.expr->as.number.value == 3.5);
    ast_free(p);
}

static void test_expressao_solta_vira_print(void) {
    node *p = parse("1 + 2;");
    assert(p->as.program.count == 1);
    assert(stmt(p, 0)->type == NODE_PRINT);
    ast_free(p);
}

static void test_variavel_nao_declarada_descarta_o_comando(void) {
    /* O erro semantico vai para o stderr; o parser se recupera e segue. */
    node *p = parse("print z; print 2 * 3;");
    assert(p->as.program.count == 1);                        /* so o segundo ficou */
    assert(stmt(p, 0)->as.print.expr->as.binary_op.op == '*');
    ast_free(p);
}

static void test_programa_vazio(void) {
    node *p = parse("");
    assert(p->type == NODE_PROGRAM);
    assert(p->as.program.count == 0);
    ast_free(p);
}

int main(void) {
    test_atribuicao_e_print();
    test_precedencia_multiplicacao_antes_de_soma();
    test_parenteses_mudam_a_arvore();
    test_associatividade_a_esquerda();
    test_modulo();
    test_decimal_nao_e_truncado();
    test_expressao_solta_vira_print();
    test_variavel_nao_declarada_descarta_o_comando();
    test_programa_vazio();
    freeTable();
    printf("test_parser: todos os testes passaram\n");
    return 0;
}
