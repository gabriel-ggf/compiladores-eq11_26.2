#include "table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Symbol *table = NULL;

void insertSymbol(char *name, char *type) {
    // Verifica se o símbolo já existe na tabela.
    Symbol *s = table;
    while (s) {
        if (strcmp(s->name, name) == 0)
            return;
        s = s->next;
    }

    Symbol *newSymbol = malloc(sizeof(Symbol));
    if (newSymbol == NULL) {
        fprintf(stderr, "Error: out of memory\n");
        return;
    }
    snprintf(newSymbol->name, sizeof(newSymbol->name), "%s", name);
    snprintf(newSymbol->type, sizeof(newSymbol->type), "%s", type);
    newSymbol->next = NULL;

    if (table == NULL) {
        table = newSymbol;
    } else {
        Symbol *last = table;
        while (last->next)
            last = last->next;
        last->next = newSymbol;
    }
}

Symbol *searchSymbol(char *name) {
    for (Symbol *s = table; s; s = s->next)
        if (strcmp(s->name, name) == 0)
            return s;
    return NULL;
}

void printTable(void) {
    printf("\nSymbol Table:\n");
    for (Symbol *s = table; s; s = s->next)
        printf("Name: %s, Type: %s\n", s->name, s->type);
}

void freeTable(void) {
    while (table) {
        Symbol *next = table->next;
        free(table);
        table = next;
    }
}