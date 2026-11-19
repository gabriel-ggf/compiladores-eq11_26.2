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
    NODE_UNARY_OP,
    NODE_ASSIGN,
    NODE_PRINT,
    NODE_PROGRAM,
    NODE_BLOCK,
    NODE_IF,
    NODE_WHILE,
    NODE_STRING,
    NODE_CHAR
} node_type;

typedef struct node node;

typedef struct {
    char *value;
} string_node;

typedef struct {
    char value;
} char_node;

typedef struct {
    double value;
} number_node;

typedef struct {
    char *name;
} variable_node;

typedef struct {
    int op;
    node *left;
    node *right;
} binary_op_node;

typedef struct {
    int op;
    node *expr;
} unary_op_node;

typedef struct {
    char *name;
    node *expr;
} assign_node;

typedef struct {
    node *expr;
} print_node;

typedef struct {
    node **statements;
    size_t count;
    size_t capacity;
} program_node;

typedef struct {
    node **statements;
    size_t count;
    size_t capacity;
} block_node;

typedef struct {
    node *condition;
    node *if_branch;
    node *else_branch;
} if_node;

typedef struct {
    node *condition;
    node *loop_body;
} while_node;

struct node {
    node_type type;
    union {
        number_node    number;
        variable_node  variable;
        binary_op_node binary_op;
        unary_op_node  unary_op;
        assign_node    assign;
        print_node     print;
        program_node   program;
        block_node     block;
        if_node        if_stmt;
        while_node     while_stmt;
        string_node    string_val;
        char_node      char_val;
    } as;
};

node *ast_new_number(double value);
node *ast_new_variable(const char *name);
node *ast_new_binary_op(int op, node *left, node *right);
node *ast_new_assign(const char *name, node *expr);
node *ast_new_print(node *expr);
node *ast_new_program(void);

node *ast_new_unary_op(int op, node *expr);
node *ast_new_if(node *condition, node *if_branch, node *else_branch);
node *ast_new_while(node *condition, node *loop_body);
node *ast_block_add(node *block, node *stmt);

node *ast_new_string(const char *value);
node *ast_new_char(char value);

void ast_program_add(node *program, node *statement);
void ast_free(node *root);

#endif /* AST_H */