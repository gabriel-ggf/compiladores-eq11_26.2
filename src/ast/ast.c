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

node *ast_new_inc_dec(const char *name, int op, int is_prefix) {
    node *n = new_node(NODE_INC_DEC);
    n->as.inc_dec.name = copy_string(name);
    n->as.inc_dec.op = op;
    n->as.inc_dec.is_prefix = is_prefix;
    return n;
}

node *ast_new_call(const char *name) {
    node *n = new_node(NODE_CALL);
    n->as.call.name = copy_string(name);
    n->as.call.args = NULL;
    n->as.call.arg_count = 0;
    n->as.call.arg_capacity = 0;
    return n;
}

void ast_call_add_arg(node *call, node *arg) {
    call_node *c = &call->as.call;
    if (c->arg_count == c->arg_capacity) {
        size_t new_cap = (c->arg_capacity == 0) ? INITIAL_CAPACITY : c->arg_capacity * 2;
        node **larger = realloc(c->args, new_cap * sizeof *larger);
        if (larger == NULL) {
            fprintf(stderr, "Out of memory while building AST\n");
            exit(EXIT_FAILURE);
        }
        c->args = larger;
        c->arg_capacity = new_cap;
    }
    c->args[c->arg_count++] = arg;
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

node *ast_new_do_while(node *loop_body, node *condition) {
    node *n = new_node(NODE_DO_WHILE);
    n->as.do_while_stmt.loop_body = loop_body;
    n->as.do_while_stmt.condition = condition;
    return n;
}

node *ast_new_break(void) {
    return new_node(NODE_BREAK);
}

node *ast_new_continue(void) {
    return new_node(NODE_CONTINUE);
}

node *ast_new_while(node *condition, node *loop_body) {
    node *n = new_node(NODE_WHILE);
    n->as.while_stmt.condition = condition;
    n->as.while_stmt.loop_body = loop_body;
    return n;
}

node *ast_new_for(node *init, node *condition, node *increment, node *body) {
    node *n = new_node(NODE_FOR);
    n->as.for_stmt.init = init;
    n->as.for_stmt.condition = condition;
    n->as.for_stmt.increment = increment;
    n->as.for_stmt.body = body;
    return n;
}

node *ast_new_return(node *expr) {
    node *n = new_node(NODE_RETURN);
    n->as.return_stmt.expr = expr;
    return n;
}

data_type ast_type_from_string(const char *type_str) {
    if (strcmp(type_str, "char") == 0) {
        return TYPE_CHAR;
    }
    if (strcmp(type_str, "float") == 0 || strcmp(type_str, "double") == 0) {
        return TYPE_DOUBLE;
    }
    if (strcmp(type_str, "string") == 0) {
        return TYPE_STRING;
    }
    return TYPE_INT; /* int, long, short, signed, unsigned */
}

node *ast_new_var_decl(data_type var_type, const char *name, node *init) {
    node *n = new_node(NODE_VAR_DECL);
    n->as.var_decl.var_type = var_type;
    n->as.var_decl.name = copy_string(name);
    n->as.var_decl.init = init;
    return n;
}

node *ast_new_string(const char *value) {
    node *n = new_node(NODE_STRING);
    size_t len = strlen(value);
    char *buf = allocate(len - 1);

    size_t out = 0;
    for (size_t i = 1; i + 1 < len; i++) {
        if (value[i] == '\\' && i + 2 < len) {
            i++;
            switch (value[i]) {
                case 'n':  buf[out++] = '\n'; break;
                case 't':  buf[out++] = '\t'; break;
                case 'r':  buf[out++] = '\r'; break;
                case '0':  buf[out++] = '\0'; break;
                case '\\': buf[out++] = '\\'; break;
                case '"':  buf[out++] = '"';  break;
                case '\'': buf[out++] = '\''; break;
                default:   buf[out++] = value[i]; break;
            }
        } else {
            buf[out++] = value[i];
        }
    }
    buf[out] = '\0';
    n->as.string_val.value = buf;
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

        case NODE_INC_DEC:
            free(root->as.inc_dec.name);
            break;

        case NODE_CALL:
            free(root->as.call.name);
            for (size_t i = 0; i < root->as.call.arg_count; i++) {
                ast_free(root->as.call.args[i]);
            }
            free(root->as.call.args);
            break;

        case NODE_VAR_DECL:
            free(root->as.var_decl.name);
            ast_free(root->as.var_decl.init);
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

        case NODE_DO_WHILE:
            ast_free(root->as.do_while_stmt.condition);
            ast_free(root->as.do_while_stmt.loop_body);
            break;

        case NODE_BREAK:
        case NODE_CONTINUE:
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
        
        case NODE_FOR:
            ast_free(root->as.for_stmt.init);
            ast_free(root->as.for_stmt.condition);
            ast_free(root->as.for_stmt.increment);
            ast_free(root->as.for_stmt.body);
            break;

        case NODE_STRING:
            free(root->as.string_val.value);
            break;

        case NODE_RETURN:
            ast_free(root->as.return_stmt.expr);
            break;

        case NODE_CHAR:
            break;
    }

    free(root);
}