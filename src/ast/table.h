#ifndef TABLE_H
#define TABLE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct parser_symbol {
    char *name;
    char *type;
    int scope_level;
    struct parser_symbol *next;
} Symbol;

void enter_scope(void);
void exit_scope(void);
int current_scope(void);
int insertSymbol(char *name, char *type);
Symbol *searchSymbol(char *name);
void printTable(void);
void freeTable(void);

#ifdef __cplusplus
}
#endif

#endif /* TABLE_H */