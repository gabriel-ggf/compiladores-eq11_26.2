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
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_CHAR,
    TYPE_STRING
} data_type;

typedef enum {
    NODE_NUMBER,
    NODE_VARIABLE,
    NODE_BINARY_OP,
    NODE_UNARY_OP,
    NODE_VAR_DECL,
    NODE_ASSIGN,
    NODE_INC_DEC,
    NODE_CALL,
    NODE_RETURN,
    NODE_PROGRAM,
    NODE_BLOCK,
    NODE_IF,
    NODE_WHILE,
    NODE_DO_WHILE,
    NODE_FOR,
    NODE_BREAK,
    NODE_CONTINUE,
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
    int is_float;
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
    node **statements;
    size_t count;
    size_t capacity;
    int has_stdio;
} program_node;

typedef struct {
    node **statements;
    size_t count;
    size_t capacity;
} block_node;

typedef struct {
    char *name;
    int op;
    int is_prefix;
} inc_dec_node;

typedef struct {
    node *condition;
    node *if_branch;
    node *else_branch;
} if_node;

typedef struct {
    node *condition;
    node *loop_body;
} while_node;

typedef struct {
    node *init;
    node *condition;
    node *increment;
    node *body;
} for_node;

typedef struct {
    data_type var_type;
    char *name;
    node *init;
} var_decl_node;

typedef struct {
    char *name;
    node **args;
    size_t arg_count;
    size_t arg_capacity;
} call_node;

typedef struct {
    node *expr;
} return_node;

struct node {
    node_type type;
    union {
        number_node    number;
        variable_node  variable;
        binary_op_node binary_op;
        unary_op_node  unary_op;
        var_decl_node  var_decl;
        assign_node    assign;
        inc_dec_node   inc_dec;
        call_node      call;
        return_node    return_stmt;
        program_node   program;
        block_node     block;
        if_node        if_stmt;
        while_node     while_stmt;
        while_node     do_while_stmt;
        for_node       for_stmt;
        string_node    string_val;
        char_node      char_val;
    } as;
};

node *ast_new_number(double value);
node *ast_new_float(double value);
node *ast_new_variable(const char *name);
node *ast_new_binary_op(int op, node *left, node *right);
node *ast_new_assign(const char *name, node *expr);
node *ast_new_inc_dec(const char *name, int op, int is_prefix);
node *ast_new_return(node *expr);
node *ast_new_call(const char *name);
void ast_call_add_arg(node *call, node *arg);
node *ast_new_program(void);

node *ast_new_unary_op(int op, node *expr);
node *ast_new_do_while(node *loop_body, node *condition);
node *ast_new_break(void);
node *ast_new_continue(void);
node *ast_new_if(node *condition, node *if_branch, node *else_branch);
node *ast_new_while(node *condition, node *loop_body);
node *ast_new_for(node *init, node *condition, node *increment, node *body);
node *ast_new_block(void);
node *ast_block_add(node *block, node *stmt);

data_type ast_type_from_string(const char *type_str);
node *ast_new_var_decl(data_type var_type, const char *name, node *init);

node *ast_new_string(const char *value);
node *ast_new_char(char value);

void ast_program_add(node *program, node *statement);
void ast_program_set_stdio(node *program, int has_stdio);
int ast_program_has_stdio(const node *program);
void ast_free(node *root);

#endif /* AST_H */