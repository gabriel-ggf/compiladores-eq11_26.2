#include "interpreter/interpreter.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct symbol {
    char *name;
    double value;
    symbol *next;
};

/*
 * C nao tem excecoes: onde a versao C++ lancava std::runtime_error, aqui a
 * mensagem vai para stderr e o programa encerra — mesmo efeito pratico de
 * uma excecao nao capturada.
 */
static void erro_execucao(const char *formato, ...) {
    va_list argumentos;
    va_start(argumentos, formato);
    vfprintf(stderr, formato, argumentos);
    va_end(argumentos);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}

static char *copiar_texto(const char *texto) {
    size_t tamanho = strlen(texto) + 1;
    char *copia = malloc(tamanho);
    if (copia == NULL) {
        erro_execucao("Memoria insuficiente na tabela de simbolos");
    }
    memcpy(copia, texto, tamanho);
    return copia;
}

static symbol *buscar_simbolo(interpreter *interp, const char *name) {
    for (symbol *atual = interp->symbols; atual != NULL; atual = atual->next) {
        if (strcmp(atual->name, name) == 0) {
            return atual;
        }
    }
    return NULL;
}

static void definir_simbolo(interpreter *interp, const char *name, double value) {
    symbol *existente = buscar_simbolo(interp, name);
    if (existente != NULL) {
        existente->value = value;
        return;
    }

    symbol *novo = malloc(sizeof *novo);
    if (novo == NULL) {
        erro_execucao("Memoria insuficiente na tabela de simbolos");
    }
    novo->name = copiar_texto(name);
    novo->value = value;
    novo->next = interp->symbols;
    interp->symbols = novo;
}

static double evaluate(interpreter *interp, node *n) {
    switch (n->type) {
        case NODE_NUMBER:
            return n->as.number.value;

        case NODE_VARIABLE: {
            symbol *encontrado = buscar_simbolo(interp, n->as.variable.name);
            if (encontrado == NULL) {
                erro_execucao("Variavel nao definida: %s", n->as.variable.name);
            }
            return encontrado->value;
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
                        erro_execucao("Divisao por zero");
                    }
                    return left / right;
                default:
                    erro_execucao("Operador desconhecido");
            }
            break;
        }

        default:
            erro_execucao("No invalido em contexto de expressao");
    }

    return 0.0; /* inalcancavel: erro_execucao encerra o programa */
}

static void execute(interpreter *interp, node *n) {
    switch (n->type) {
        case NODE_ASSIGN:
            definir_simbolo(interp,
                            n->as.assign.name,
                            evaluate(interp, n->as.assign.expr));
            break;

        case NODE_PRINT:
            printf("%g\n", evaluate(interp, n->as.print.expr));
            break;

        default:
            erro_execucao("Statement invalido");
    }
}

void interpreter_init(interpreter *interp) {
    interp->symbols = NULL;
}

void interpreter_run(interpreter *interp, node *program) {
    /* Em C++ a assinatura exigia ProgramNode*; em C a checagem e em tempo
       de execucao. */
    if (program == NULL || program->type != NODE_PROGRAM) {
        erro_execucao("Raiz invalida: esperado um no de programa");
    }

    for (size_t i = 0; i < program->as.program.count; i++) {
        execute(interp, program->as.program.statements[i]);
    }
}

void interpreter_destroy(interpreter *interp) {
    symbol *atual = interp->symbols;
    while (atual != NULL) {
        symbol *proximo = atual->next;
        free(atual->name);
        free(atual);
        atual = proximo;
    }
    interp->symbols = NULL;
}
