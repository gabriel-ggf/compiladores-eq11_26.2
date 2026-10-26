#ifndef AST_H
#define AST_H

/*
 * Definicao da Arvore Sintatica Abstrata (AST) do interpretador.
 *
 * Cada no representa um pedaco do programa fonte ja "compreendido" pelo
 * parser (Bison). O interpretador (src/interpreter) percorre essa arvore e
 * executa/avalia cada no (estrategia conhecida como "tree-walking
 * interpreter").
 *
 * C nao tem heranca: o que em C++ eram structs derivados de Node virou um
 * unico struct com uma tag (node_type) e uma union com os campos de cada
 * tipo de no. Ver docs/decisoes-tecnicas.md.
 *
 * A memoria e gerenciada na mao: os construtores alocam com malloc e
 * ast_free() libera a arvore inteira de uma vez.
 */

#include <stddef.h>

typedef enum {
    NODE_NUMBER,
    NODE_VARIABLE,
    NODE_BINARY_OP,
    NODE_ASSIGN,
    NODE_PRINT,
    NODE_PROGRAM
} node_type;

typedef struct node node;

/* Literal numerico, ex.: 10, 3.14 */
typedef struct {
    double value;
} number_node;

/* Referencia a uma variavel, ex.: x */
typedef struct {
    char *name;
} variable_node;

/* Operacao binaria, ex.: a + b */
typedef struct {
    char op; /* '+', '-', '*', '/' */
    node *left;
    node *right;
} binary_op_node;

/* Atribuicao, ex.: x = 10; */
typedef struct {
    char *name;
    node *expr;
} assign_node;

/* Comando de impressao, ex.: print x + 1; */
typedef struct {
    node *expr;
} print_node;

/* Raiz da arvore: lista de statements do programa. */
typedef struct {
    node **statements;
    size_t count;
    size_t capacity;
} program_node;

struct node {
    node_type type;
    union {
        number_node    number;
        variable_node  variable;
        binary_op_node binary_op;
        assign_node    assign;
        print_node     print;
        program_node   program;
    } as;
};

/* Construtores: substituem os construtores das classes da versao C++. */
node *ast_new_number(double value);
node *ast_new_variable(const char *name);
node *ast_new_binary_op(char op, node *left, node *right);
node *ast_new_assign(const char *name, node *expr);
node *ast_new_print(node *expr);
node *ast_new_program(void);

/* Acrescenta um statement a raiz do programa. */
void ast_program_add(node *program, node *statement);

/* Libera recursivamente a arvore inteira. */
void ast_free(node *root);

#endif /* AST_H */
