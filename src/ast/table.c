#include "table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Symbol *table = NULL;
static int g_current_scope = 0;

void enter_scope(void) {
    g_current_scope++;
}

void exit_scope(void) {
    while (table != NULL && table->scope_level >= g_current_scope) {
        Symbol *next = table->next;
        free(table->name);
        free(table->type);
        free(table);
        table = next;
    }
    if (g_current_scope > 0) {
        g_current_scope--;
    }
}

int current_scope(void) {
    return g_current_scope;
}

int insertSymbol(char *name, char *type) {
    for (Symbol *s = table; s != NULL; s = s->next) {
        if (s->scope_level == g_current_scope && strcmp(s->name, name) == 0) {
            return 0;
        }
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
    newSymbol->scope_level = g_current_scope;
    newSymbol->next = table;
    table = newSymbol;

    return 1;
}

Symbol *searchSymbol(char *name) {
    for (Symbol *s = table; s; s = s->next) {
        if (strcmp(s->name, name) == 0)
            return s;
    }
    return NULL;
}

void printTable(void) {
    printf("\nSymbol Table:\n");
    for (Symbol *s = table; s; s = s->next)
        printf("Name: %s, Type: %s, Scope: %d\n", s->name, s->type, s->scope_level);
}

void freeTable(void) {
    while (table) {
        Symbol *next = table->next;
        free(table->name);
        free(table->type);
        free(table);
        table = next;
    }
    g_current_scope = 0;
}