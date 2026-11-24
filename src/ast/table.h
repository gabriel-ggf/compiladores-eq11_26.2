#ifndef TABLE_H
#define TABLE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct parser_symbol {
    char *name;
    char *type;
    struct parser_symbol *next;
} Symbol;

int insertSymbol(char *name, char *type);
Symbol *searchSymbol(char *name);
void printTable(void);
void freeTable(void);

#ifdef __cplusplus
}
#endif

#endif /* TABLE_H */