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

#define INITIAL_CAPACITY 8

static void *allocate(size_t size) {
    void *block = malloc(size);
    if (block == NULL) {
        fprintf(stderr, "Out of memory while building AST\n");
        exit(EXIT_FAILURE);
    }
    return block;
}

static char *copy_string(const char *text) {
    size_t size = strlen(text) + 1;
    char *copy = allocate(size);
    memcpy(copy, text, size);
    return copy;
}

static node *new_node(node_type type) {
    node *n = allocate(sizeof *n);
    n->type = type;
    return n;
}

node *ast_new_number(double value) {
    node *n = new_node(NODE_NUMBER);
    n->as.number.value = value;
    return n;
}

node *ast_new_variable(const char *name) {
    node *n = new_node(NODE_VARIABLE);
    n->as.variable.name = copy_string(name);
    return n;
}

node *ast_new_binary_op(int op, node *left, node *right) {
    node *n = new_node(NODE_BINARY_OP);
    n->as.binary_op.op = op;
    n->as.binary_op.left = left;
    n->as.binary_op.right = right;
    return n;
}

node *ast_new_unary_op(int op, node *expr) {
    node *n = new_node(NODE_UNARY_OP);
    n->as.unary_op.op = op;
    n->as.unary_op.expr = expr;
    return n;
}

node *ast_new_assign(const char *name, node *expr) {
    node *n = new_node(NODE_ASSIGN);
    n->as.assign.name = copy_string(name);
    n->as.assign.expr = expr;
    return n;
}

node *ast_new_print(node *expr) {
    node *n = new_node(NODE_PRINT);
    n->as.print.expr = expr;
    return n;
}

node *ast_new_program(void) {
    node *n = new_node(NODE_PROGRAM);
    n->as.program.statements = NULL;
    n->as.program.count = 0;
    n->as.program.capacity = 0;
    return n;
}

void ast_program_add(node *program, node *statement) {
    program_node *p = &program->as.program;

    if (p->count == p->capacity) {
        size_t new_capacity = (p->capacity == 0) ? INITIAL_CAPACITY : p->capacity * 2;
        node **larger = realloc(p->statements, new_capacity * sizeof *larger);
        if (larger == NULL) {
            fprintf(stderr, "Out of memory while building AST\n");
            exit(EXIT_FAILURE);
        }
        p->statements = larger;
        p->capacity = new_capacity;
    }

    p->statements[p->count++] = statement;
}

node *ast_block_add(node *block, node *stmt) {
    if (block == NULL) {
        block = new_node(NODE_BLOCK);
        block->as.block.statements = NULL;
        block->as.block.count = 0;
        block->as.block.capacity = 0;
    }

    block_node *b = &block->as.block;

    if (b->count == b->capacity) {
        size_t new_capacity = (b->capacity == 0) ? INITIAL_CAPACITY : b->capacity * 2;
        node **larger = realloc(b->statements, new_capacity * sizeof *larger);
        if (larger == NULL) {
            fprintf(stderr, "Out of memory while building AST\n");
            exit(EXIT_FAILURE);
        }
        b->statements = larger;
        b->capacity = new_capacity;
    }

    b->statements[b->count++] = stmt;
    return block;
}

node *ast_new_if(node *condition, node *if_branch, node *else_branch) {
    node *n = new_node(NODE_IF);
    n->as.if_stmt.condition = condition;
    n->as.if_stmt.if_branch = if_branch;
    n->as.if_stmt.else_branch = else_branch;
    return n;
}

node *ast_new_while(node *condition, node *loop_body) {
    node *n = new_node(NODE_WHILE);
    n->as.while_stmt.condition = condition;
    n->as.while_stmt.loop_body = loop_body;
    return n;
}

node *ast_new_string(const char *value) {
    node *n = new_node(NODE_STRING);
    // Removemos as aspas iniciais e finais durante a cópia para a AST
    size_t len = strlen(value);
    n->as.string_val.value = malloc(len - 1);
    strncpy(n->as.string_val.value, value + 1, len - 2);
    n->as.string_val.value[len - 2] = '\0';
    return n;
}

node *ast_new_char(char value) {
    node *n = new_node(NODE_CHAR);
    n->as.char_val.value = value;
    return n;
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

        case NODE_UNARY_OP:
            ast_free(root->as.unary_op.expr);
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

        case NODE_BLOCK:
            for (size_t i = 0; i < root->as.block.count; i++) {
                ast_free(root->as.block.statements[i]);
            }
            free(root->as.block.statements);
            break;

        case NODE_IF:
            ast_free(root->as.if_stmt.condition);
            ast_free(root->as.if_stmt.if_branch);
            if (root->as.if_stmt.else_branch != NULL) {
                ast_free(root->as.if_stmt.else_branch);
            }
            break;

        case NODE_WHILE:
            ast_free(root->as.while_stmt.condition);
            ast_free(root->as.while_stmt.loop_body);
            break;

        case NODE_STRING:
            free(root->as.string_val.value);
            break;

        case NODE_CHAR:
            break;
    }

    free(root);
}