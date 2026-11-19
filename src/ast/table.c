#include "table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Symbol *table = NULL;

void insertSymbol(char *name, char *type) {
    Symbol *s = table;
    while (s) {
        if (strcmp(s->name, name) == 0)
            return;
        s = s->next;
    }

    Symbol *newSymbol = malloc(sizeof(Symbol));
    if (newSymbol == NULL) {
        fprintf(stderr, "Error: out of memory\n");
        exit(EXIT_FAILURE);
    }
    
    newSymbol->name = malloc(strlen(name) + 1);
    newSymbol->type = malloc(strlen(type) + 1);
    if (newSymbol->name == NULL || newSymbol->type == NULL) {
        fprintf(stderr, "Error: out of memory\n");
        exit(EXIT_FAILURE);
    }
    
    strcpy(newSymbol->name, name);
    strcpy(newSymbol->type, type);
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
        free(table->name);
        free(table->type);
        free(table);
        table = next;
    }
}