#include <stdlib.h>
#include <string.h>
#include "table.h"

typedef struct Entry {
    char *name;
    double value;
    struct Entry *next;
} Entry;

static Entry *head = NULL;

static Entry *find(const char *name) {
    for (Entry *e = head; e; e = e->next)
        if (strcmp(e->name, name) == 0) return e;
    return NULL;
}

void table_insert(const char *name, double value) {
    Entry *e = find(name);
    if (e) { e->value = value; return; }      // already exists: update
    e = malloc(sizeof(Entry));
    e->name = strdup(name);
    e->value = value;
    e->next = head;                            // insert at head (O(1))
    head = e;
}

int table_search(const char *name, double *value) {
    Entry *e = find(name);
    if (!e) return 0;
    *value = e->value;
    return 1;
}

void table_free(void) {
    while (head) {
        Entry *next = head->next;
        free(head->name);
        free(head);
        head = next;
    }
}