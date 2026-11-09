#include "interpreter/interpreter.h"

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct symbol {
    char *name;
    double value;
    struct symbol *next;
};

/*
 * C nao tem excecoes: onde a versao C++ lancava std::runtime_error, aqui a
 * mensagem vai para stderr e o programa encerra — mesmo efeito pratico de
 * uma excecao nao capturada.
 */


static void runtime_error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}

static char *copy_text(const char *text) {
    size_t size = strlen(text) + 1;
    char *copy = malloc(size);
    if (copy == NULL) {
        runtime_error("Out of memory in symbol table");
    }
    memcpy(copy, text, size);
    return copy;
}

static symbol *search_symbol(interpreter *interp, const char *name) {
    for (symbol *current = interp->symbols; current != NULL; current = current->next) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
    }
    return NULL;
}

static void define_symbol(interpreter *interp, const char *name, double value) {
    symbol *existing = search_symbol(interp, name);
    if (existing != NULL) {
        existing->value = value;
        return;
    }

    symbol *new_symbol = malloc(sizeof *new_symbol);
    if (new_symbol == NULL) {
        runtime_error("Out of memory in symbol table");
    }
    new_symbol->name = copy_text(name);
    new_symbol->value = value;
    new_symbol->next = interp->symbols;
    interp->symbols = new_symbol;
}

static double evaluate(interpreter *interp, node *n) {
    switch (n->type) {
        case NODE_NUMBER:
            return n->as.number.value;

        case NODE_VARIABLE: {
            symbol *found = search_symbol(interp, n->as.variable.name);
            if (found == NULL) {
                runtime_error("Undefined variable: %s", n->as.variable.name);
            }
            return found->value;
        }

        case NODE_BINARY_OP: {
            double left = evaluate(interp, n->as.binary_op.left);
            double right = evaluate(interp, n->as.binary_op.right);
            switch (n->as.binary_op.op) {
                case '+': return left + right;
                case '-': return left - right;
                case '*': return left * right;
                case '/':
                    if (right == 0.0) {
                        runtime_error("Division by zero");
                    }
                    return left / right;
                case '%':
                    if (right == 0.0) {
                        runtime_error("Division by zero");
                    }
                    return fmod(left, right);
                default:
                    runtime_error("Unknown operator");
            }
            break;
        }

        default:
            runtime_error("Invalid node in expression context");
    }

    return 0.0; /* unreachable: runtime_error terminates the program */
}

static void execute(interpreter *interp, node *n) {
    switch (n->type) {
        case NODE_ASSIGN:
            define_symbol(interp,
                          n->as.assign.name,
                          evaluate(interp, n->as.assign.expr));
            break;

        case NODE_PRINT:
            printf("%g\n", evaluate(interp, n->as.print.expr));
            break;

        default:
            runtime_error("Invalid statement");
    }
}

void interpreter_init(interpreter *interp) {
    interp->symbols = NULL;
}

void interpreter_run(interpreter *interp, node *program) {
    /* In C++ the signature required ProgramNode*; in C the check is at runtime. */
    if (program == NULL || program->type != NODE_PROGRAM) {
        runtime_error("Invalid root: expected a program node");
    }

    for (size_t i = 0; i < program->as.program.count; i++) {
        execute(interp, program->as.program.statements[i]);
    }
}

void interpreter_destroy(interpreter *interp) {
    symbol *current = interp->symbols;
    while (current != NULL) {
        symbol *next = current->next;
        free(current->name);
        free(current);
        current = next;
    }
    interp->symbols = NULL;
}