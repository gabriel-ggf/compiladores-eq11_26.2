#include "interpreter/interpreter.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    data_type type;
    union {
        long i;
        double d;
        char c;
        char *s;
    } as;
} runtime_value;

struct symbol {
    char *name;
    data_type type;
    int scope_level;
    union {
        long i;
        double d;
        char c;
        char *s;
    } as;
    struct symbol *next;
};

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

static double value_to_double(runtime_value v) {
    switch (v.type) {
        case TYPE_INT:    return (double)v.as.i;
        case TYPE_DOUBLE: return v.as.d;
        case TYPE_CHAR:   return (double)v.as.c;
        case TYPE_STRING: return v.as.s != NULL ? 1.0 : 0.0;
    }
    return 0.0;
}

static long value_to_long(runtime_value v) {
    switch (v.type) {
        case TYPE_INT:    return v.as.i;
        case TYPE_DOUBLE: return (long)v.as.d; /* trunca decimais como em C */
        case TYPE_CHAR:   return (long)v.as.c;
        case TYPE_STRING: runtime_error("Cannot convert string to integer");
    }
    return 0;
}

static int value_is_truthy(runtime_value v) {
    switch (v.type) {
        case TYPE_INT:    return v.as.i != 0;
        case TYPE_DOUBLE: return v.as.d != 0.0;
        case TYPE_CHAR:   return v.as.c != 0;
        case TYPE_STRING: return v.as.s != NULL;
    }
    return 0;
}

static runtime_value make_int(long v) {
    runtime_value r;
    r.type = TYPE_INT;
    r.as.i = v;
    return r;
}

static runtime_value make_double(double v) {
    runtime_value r;
    r.type = TYPE_DOUBLE;
    r.as.d = v;
    return r;
}

static runtime_value make_char(char v) {
    runtime_value r;
    r.type = TYPE_CHAR;
    r.as.c = v;
    return r;
}

static runtime_value make_string(char *v) {
    runtime_value r;
    r.type = TYPE_STRING;
    r.as.s = v;
    return r;
}

static symbol *search_symbol(interpreter *interp, const char *name) {
    for (symbol *current = interp->symbols; current != NULL; current = current->next) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
    }
    return NULL;
}

static void store_value_in_symbol(symbol *sym, runtime_value val) {
    switch (sym->type) {
        case TYPE_INT:
            if (val.type == TYPE_STRING) runtime_error("Cannot assign string to int '%s'", sym->name);
            sym->as.i = value_to_long(val);
            break;
        case TYPE_DOUBLE:
            if (val.type == TYPE_STRING) runtime_error("Cannot assign string to double '%s'", sym->name);
            sym->as.d = value_to_double(val);
            break;
        case TYPE_CHAR:
            if (val.type == TYPE_STRING) runtime_error("Cannot assign string to char '%s'", sym->name);
            sym->as.c = (char)value_to_long(val);
            break;
        case TYPE_STRING:
            if (val.type != TYPE_STRING) runtime_error("Cannot assign non-string to char* '%s'", sym->name);
            free(sym->as.s);
            sym->as.s = val.as.s != NULL ? copy_text(val.as.s) : NULL;
            break;
    }
}

static runtime_value read_symbol_value(symbol *sym) {
    switch (sym->type) {
        case TYPE_INT:    return make_int(sym->as.i);
        case TYPE_DOUBLE: return make_double(sym->as.d);
        case TYPE_CHAR:   return make_char(sym->as.c);
        case TYPE_STRING: return make_string(sym->as.s);
    }
    return make_int(0);
}

static void enter_runtime_scope(interpreter *interp) {
    interp->scope_level++;
}

static void exit_runtime_scope(interpreter *interp) {
    int target_scope = interp->scope_level;
    while (interp->symbols != NULL && interp->symbols->scope_level >= target_scope) {
        symbol *next = interp->symbols->next;
        if (interp->symbols->type == TYPE_STRING) {
            free(interp->symbols->as.s);
        }
        free(interp->symbols->name);
        free(interp->symbols);
        interp->symbols = next;
    }
    if (interp->scope_level > 0) {
        interp->scope_level--;
    }
}

static symbol *search_symbol_current_scope(interpreter *interp, const char *name) {
    for (symbol *current = interp->symbols; current != NULL && current->scope_level == interp->scope_level; current = current->next) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
    }
    return NULL;
}

static void declare_symbol(interpreter *interp, const char *name, data_type type, int has_init, runtime_value init_val) {
    symbol *sym = search_symbol_current_scope(interp, name);
    if (sym == NULL) {
        sym = malloc(sizeof *sym);
        if (sym == NULL) {
            runtime_error("Out of memory in symbol table");
        }
        sym->name = copy_text(name);
        sym->type = type;
        sym->scope_level = interp->scope_level;
        sym->as.s = NULL;
        sym->next = interp->symbols;
        interp->symbols = sym;
    } else {
        if (sym->type == TYPE_STRING) {
            free(sym->as.s);
            sym->as.s = NULL;
        }
        sym->type = type;
    }

    if (has_init) {
        store_value_in_symbol(sym, init_val);
    } else {
        memset(&sym->as, 0, sizeof(sym->as));
    }
}

static runtime_value assign_symbol(interpreter *interp, const char *name, runtime_value val) {
    symbol *existing = search_symbol(interp, name);
    if (existing == NULL) {
        /* Fallback para testes unitarios que criam NODE_ASSIGN sem NODE_VAR_DECL */
        declare_symbol(interp, name, val.type, 1, val);
        return val;
    }
    store_value_in_symbol(existing, val);
    return read_symbol_value(existing);
}

static runtime_value evaluate(interpreter *interp, node *n) {
    if (n == NULL) return make_int(0);

    switch (n->type) {
        case NODE_NUMBER:
            if (n->as.number.is_float) {
                return make_double(n->as.number.value);
            }
            return make_int((long)n->as.number.value);

        case NODE_CHAR:
            return make_char(n->as.char_val.value);

        case NODE_STRING:
            return make_string(n->as.string_val.value);

        case NODE_VARIABLE: {
            symbol *found = search_symbol(interp, n->as.variable.name);
            if (found == NULL) {
                runtime_error("Undefined variable: %s", n->as.variable.name);
            }
            return read_symbol_value(found);
        }

        case NODE_ASSIGN: {
            runtime_value val = evaluate(interp, n->as.assign.expr);
            return assign_symbol(interp, n->as.assign.name, val);
        }

        case NODE_INC_DEC: {
            symbol *found = search_symbol(interp, n->as.inc_dec.name);
            if (found == NULL) {
                runtime_error("Undefined variable: %s", n->as.inc_dec.name);
            }
            if (found->type == TYPE_STRING) {
                runtime_error("Cannot increment/decrement string '%s'", found->name);
            }
            runtime_value old_val = read_symbol_value(found);
            long delta = (n->as.inc_dec.op == '+') ? 1 : -1;
            if (found->type == TYPE_INT) {
                found->as.i += delta;
            } else if (found->type == TYPE_CHAR) {
                found->as.c = (char)(found->as.c + delta);
            } else {
                found->as.d += (double)delta;
            }
            return n->as.inc_dec.is_prefix ? read_symbol_value(found) : old_val;
        }

        case NODE_UNARY_OP: {
            runtime_value expr = evaluate(interp, n->as.unary_op.expr);
            if (expr.type == TYPE_STRING) {
                runtime_error("Invalid unary operation on string");
            }
            if (n->as.unary_op.op == '!') {
                return make_int(!value_is_truthy(expr));
            }
            if (n->as.unary_op.op == '-') {
                if (expr.type == TYPE_DOUBLE) return make_double(-expr.as.d);
                return make_int(-value_to_long(expr));
            }
            runtime_error("Unknown unary operator");
            return make_int(0);
        }

        case NODE_BINARY_OP: {
            runtime_value left = evaluate(interp, n->as.binary_op.left);

            if (n->as.binary_op.op == '&') {
                return make_int(value_is_truthy(left) && value_is_truthy(evaluate(interp, n->as.binary_op.right)));
            }
            if (n->as.binary_op.op == '|') {
                return make_int(value_is_truthy(left) || value_is_truthy(evaluate(interp, n->as.binary_op.right)));
            }

            runtime_value right = evaluate(interp, n->as.binary_op.right);
            if (left.type == TYPE_STRING || right.type == TYPE_STRING) {
                runtime_error("Binary operators are not supported on strings");
            }

            int is_float = (left.type == TYPE_DOUBLE || right.type == TYPE_DOUBLE);
            double ld = value_to_double(left);
            double rd = value_to_double(right);
            long li = value_to_long(left);
            long ri = value_to_long(right);

            switch (n->as.binary_op.op) {
                case '+': return is_float ? make_double(ld + rd) : make_int(li + ri);
                case '-': return is_float ? make_double(ld - rd) : make_int(li - ri);
                case '*': return is_float ? make_double(ld * rd) : make_int(li * ri);
                case '/':
                    if (is_float) {
                        if (rd == 0.0) runtime_error("Division by zero");
                        return make_double(ld / rd);
                    } else {
                        if (ri == 0) runtime_error("Division by zero");
                        return make_int(li / ri); /* divisao inteira como em C */
                    }
                case '%':
                    if (is_float) {
                        long ld_i = (long)ld;
                        long rd_i = (long)rd;
                        if (rd_i == 0) runtime_error("Division by zero in modulo");
                        return make_int(ld_i % rd_i);
                    }
                    if (ri == 0) runtime_error("Division by zero in modulo");
                    return make_int(li % ri);

                case 'E': return make_int(is_float ? (ld == rd) : (li == ri));
                case 'D': return make_int(is_float ? (ld != rd) : (li != ri));
                case '>': return make_int(is_float ? (ld > rd)  : (li > ri));
                case '<': return make_int(is_float ? (ld < rd)  : (li < ri));
                case 'G': return make_int(is_float ? (ld >= rd) : (li >= ri));
                case 'L': return make_int(is_float ? (ld <= rd) : (li <= ri));

                default:
                    runtime_error("Unknown binary operator");
            }
            break;
        }

        case NODE_CALL: {
            if (strcmp(n->as.call.name, "printf") == 0) {
                if (n->as.call.arg_count == 0) {
                    runtime_error("printf expects at least 1 argument");
                }
                runtime_value fmt_val = evaluate(interp, n->as.call.args[0]);
                if (fmt_val.type != TYPE_STRING || fmt_val.as.s == NULL) {
                    runtime_error("printf expects a format string as first argument");
                }
                const char *fmt = fmt_val.as.s;
                size_t arg_idx = 1;

                for (size_t i = 0; fmt[i] != '\0'; i++) {
                    if (fmt[i] == '%' && fmt[i + 1] != '\0') {
                        i++;
                        if (fmt[i] == '%') {
                            putchar('%');
                            continue;
                        }
                        if (arg_idx >= n->as.call.arg_count) {
                            runtime_error("Too few arguments for printf format");
                        }
                        runtime_value arg = evaluate(interp, n->as.call.args[arg_idx++]);
                        switch (fmt[i]) {
                            case 'd':
                            case 'i':
                                printf("%ld", value_to_long(arg));
                                break;
                            case 'f':
                            case 'g':
                                printf("%g", value_to_double(arg));
                                break;
                            case 'c':
                                putchar((char)value_to_long(arg));
                                break;
                            case 's':
                                if (arg.type != TYPE_STRING) {
                                    runtime_error("Expected string for %%s");
                                }
                                printf("%s", arg.as.s != NULL ? arg.as.s : "(null)");
                                break;
                            default:
                                runtime_error("Unsupported format specifier %%%c", fmt[i]);
                        }
                    } else {
                        putchar(fmt[i]);
                    }
                }
                return make_int(0);
            } else if (strcmp(n->as.call.name, "print") == 0) {
                for (size_t i = 0; i < n->as.call.arg_count; i++) {
                    runtime_value arg = evaluate(interp, n->as.call.args[i]);
                    switch (arg.type) {
                        case TYPE_INT:    printf("%ld", arg.as.i); break;
                        case TYPE_DOUBLE: printf("%g", arg.as.d); break;
                        case TYPE_CHAR:   printf("%c", arg.as.c); break;
                        case TYPE_STRING: printf("%s", arg.as.s != NULL ? arg.as.s : "(null)"); break;
                    }
                }
                putchar('\n');
                return make_int(0);
            }
            runtime_error("Undefined function: %s", n->as.call.name);
            return make_int(0);
        }

        default:
            runtime_error("Invalid node in expression context");
    }

    return make_int(0);
}

static void execute(interpreter *interp, node *n) {
    if (n == NULL) return;

    switch (n->type) {
        case NODE_VAR_DECL: {
            if (n->as.var_decl.init != NULL) {
                runtime_value init_val = evaluate(interp, n->as.var_decl.init);
                declare_symbol(interp, n->as.var_decl.name, n->as.var_decl.var_type, 1, init_val);
            } else {
                declare_symbol(interp, n->as.var_decl.name, n->as.var_decl.var_type, 0, make_int(0));
            }
            break;
        }

        case NODE_BLOCK:
            enter_runtime_scope(interp);
            for (size_t i = 0;
                 i < n->as.block.count && !interp->has_returned && !interp->has_broken && !interp->has_continued;
                 i++) {
                execute(interp, n->as.block.statements[i]);
            }
            exit_runtime_scope(interp);
            break;

        case NODE_IF:
            if (value_is_truthy(evaluate(interp, n->as.if_stmt.condition))) {
                execute(interp, n->as.if_stmt.if_branch);
            } else if (n->as.if_stmt.else_branch != NULL) {
                execute(interp, n->as.if_stmt.else_branch);
            }
            break;

        case NODE_WHILE:
            interp->loop_depth++;
            while (!interp->has_returned && value_is_truthy(evaluate(interp, n->as.while_stmt.condition))) {
                execute(interp, n->as.while_stmt.loop_body);
                if (interp->has_broken) {
                    interp->has_broken = 0;
                    break;
                }
                if (interp->has_continued) {
                    interp->has_continued = 0;
                }
            }
            interp->loop_depth--;
            break;

        case NODE_DO_WHILE:
            interp->loop_depth++;
            do {
                execute(interp, n->as.do_while_stmt.loop_body);
                if (interp->has_broken) {
                    interp->has_broken = 0;
                    break;
                }
                if (interp->has_continued) {
                    interp->has_continued = 0;
                }
            } while (!interp->has_returned && value_is_truthy(evaluate(interp, n->as.do_while_stmt.condition)));
            interp->loop_depth--;
            break;

        case NODE_FOR:
            if (n->as.for_stmt.init != NULL) {
                execute(interp, n->as.for_stmt.init);
            }
            interp->loop_depth++;
            while (!interp->has_returned &&
                   (n->as.for_stmt.condition == NULL || value_is_truthy(evaluate(interp, n->as.for_stmt.condition)))) {
                execute(interp, n->as.for_stmt.body);
                if (interp->has_broken) {
                    interp->has_broken = 0;
                    break;
                }
                if (interp->has_continued) {
                    interp->has_continued = 0;
                }
                if (!interp->has_returned && n->as.for_stmt.increment != NULL) {
                    execute(interp, n->as.for_stmt.increment);
                }
            }
            interp->loop_depth--;
            break;

        case NODE_BREAK:
            if (interp->loop_depth == 0) {
                runtime_error("break statement not within a loop");
            }
            interp->has_broken = 1;
            break;

        case NODE_CONTINUE:
            if (interp->loop_depth == 0) {
                runtime_error("continue statement not within a loop");
            }
            interp->has_continued = 1;
            break;

        case NODE_RETURN:
            if (n->as.return_stmt.expr != NULL) {
                evaluate(interp, n->as.return_stmt.expr);
            }
            interp->has_returned = 1;
            break;

        /* Expressoes que funcionam como comandos (ex: "x = 2;", "i++;", "printf(...);") */
        case NODE_ASSIGN:
        case NODE_INC_DEC:
        case NODE_BINARY_OP:
        case NODE_UNARY_OP:
        case NODE_NUMBER:
        case NODE_CHAR:
        case NODE_STRING:
        case NODE_VARIABLE:
        case NODE_CALL:
            evaluate(interp, n);
            break;

        default:
            runtime_error("Invalid statement");
    }
}

void interpreter_init(interpreter *interp) {
    interp->symbols = NULL;
    interp->scope_level = 0;
    interp->has_returned = 0;
    interp->has_broken = 0;
    interp->has_continued = 0;
    interp->loop_depth = 0;
}

void interpreter_run(interpreter *interp, node *program) {
    if (program == NULL || program->type != NODE_PROGRAM) {
        runtime_error("Invalid root: expected a program node");
    }

    for (size_t i = 0; i < program->as.program.count && !interp->has_returned; i++) {
        execute(interp, program->as.program.statements[i]);
    }
}

void interpreter_destroy(interpreter *interp) {
    symbol *current = interp->symbols;
    while (current != NULL) {
        symbol *next = current->next;
        if (current->type == TYPE_STRING) {
            free(current->as.s);
        }
        free(current->name);
        free(current);
        current = next;
    }
    interp->symbols = NULL;
    interp->scope_level = 0;
}