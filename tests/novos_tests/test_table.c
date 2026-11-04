/* Testes unitarios da tabela de simbolos (src/ast/table.c). */
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "ast/table.h"

static void test_busca_em_tabela_vazia(void) {
    assert(searchSymbol("x") == NULL);
}

static void test_insere_e_busca(void) {
    insertSymbol("x", "int");
    Symbol *s = searchSymbol("x");
    assert(s != NULL);
    assert(strcmp(s->name, "x") == 0);
    assert(strcmp(s->type, "int") == 0);
}

static void test_duplicata_e_ignorada(void) {
    insertSymbol("x", "float");                       /* x ja existe como int */
    assert(strcmp(searchSymbol("x")->type, "int") == 0);
}

static void test_varios_simbolos(void) {
    insertSymbol("y", "float");
    insertSymbol("z", "double");
    assert(searchSymbol("y") != NULL);
    assert(strcmp(searchSymbol("z")->type, "double") == 0);
    assert(searchSymbol("w") == NULL);
}

static void test_nome_longo_e_cortado_sem_estourar(void) {
    /* name tem 32 bytes: nomes maiores sao cortados em 31 caracteres.
     * O importante e nao escrever fora do array (buffer overflow). */
    char longo[100];
    memset(longo, 'a', sizeof longo - 1);
    longo[sizeof longo - 1] = '\0';
    insertSymbol(longo, "int");

    char truncado[32];
    memset(truncado, 'a', 31);
    truncado[31] = '\0';
    assert(searchSymbol(truncado) != NULL);   /* foi guardado cortado */
    assert(searchSymbol(longo) == NULL);      /* limitacao conhecida: o nome completo nao casa */
}

static void test_libera_tabela(void) {
    freeTable();
    assert(searchSymbol("x") == NULL);
}

int main(void) {
    test_busca_em_tabela_vazia();
    test_insere_e_busca();
    test_duplicata_e_ignorada();
    test_varios_simbolos();
    test_nome_longo_e_cortado_sem_estourar();
    test_libera_tabela();
    printf("test_table: todos os testes passaram\n");
    return 0;
}
