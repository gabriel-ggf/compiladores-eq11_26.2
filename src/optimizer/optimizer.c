#include "optimizer/optimizer.h"

#include <limits.h>
#include <math.h>
#include <string.h>

typedef enum {
    CONST_INT,
    CONST_DOUBLE,
    CONST_CHAR
} constant_type;

typedef struct {
    constant_type type;
    long i;
    double d;
    char c;
} constant_value;

static int safe_add(long a, long b, long *result) {
    if (b > 0 && a > LONG_MAX - b) return 0;
    if (b < 0 && a < LONG_MIN - b) return 0;

    *result = a + b;
    return 1;
}

static int safe_sub(long a, long b, long *result) {
    if (b < 0 && a > LONG_MAX + b) return 0;
    if (b > 0 && a < LONG_MIN + b) return 0;

    *result = a - b;
    return 1;
}

static int safe_mul(long a, long b, long *result) {
    if (a == 0 || b == 0) {
        *result = 0;
        return 1;
    }

    if (a == -1) {
        if (b == LONG_MIN) return 0;
        *result = -b;
        return 1;
    }

    if (b == -1) {
        if (a == LONG_MIN) return 0;
        *result = -a;
        return 1;
    }

    if (a > 0) {
        if (b > 0 && a > LONG_MAX / b) return 0;
        if (b < 0 && b < LONG_MIN / a) return 0;
    } else {
        if (b > 0 && a < LONG_MIN / b) return 0;
        if (b < 0 && a < LONG_MAX / b) return 0;
    }

    *result = a * b;
    return 1;
}

static int get_constant(const node *n, constant_value *value) {
    if (n == NULL || value == NULL) {
        return 0;
    }

    switch (n->type) {
        case NODE_NUMBER: {
            double number = n->as.number.value;

            if (n->as.number.is_float) {
                value->type = CONST_DOUBLE;
                value->d = number;
                return 1;
            }

            if (!isfinite(number) ||
                number > 9007199254740991.0 ||
                number < -9007199254740991.0) {
                return 0;
            }

            if ((long double)number < (long double)LONG_MIN || (long double)number > (long double)LONG_MAX) {
                return 0;
            }

            value->type = CONST_INT;
            value->i = (long)number;
            return 1;
        }

        case NODE_CHAR:
            value->type = CONST_CHAR;
            value->c = n->as.char_val.value;
            return 1;

        default:
            return 0;
    }
}

static long constant_to_long(constant_value value) {
    switch (value.type) {
        case CONST_INT:
            return value.i;

        case CONST_CHAR:
            return (long)value.c;

        case CONST_DOUBLE:
            return (long)value.d;
    }

    return 0;
}

static double constant_to_double(constant_value value) {
    switch (value.type) {
        case CONST_INT:
            return (double)value.i;

        case CONST_CHAR:
            return (double)value.c;

        case CONST_DOUBLE:
            return value.d;
    }

    return 0.0;
}

static int constant_is_truthy(constant_value value) {
    switch (value.type) {
        case CONST_INT:
            return value.i != 0;

        case CONST_CHAR:
            return value.c != 0;

        case CONST_DOUBLE:
            return value.d != 0.0;
    }

    return 0;
}

static int replace_with_int(node *n, long value) {
    if (value > 9007199254740991LL ||
        value < -9007199254740991LL) {
        return 0;
    }

    if (n->type == NODE_BINARY_OP) {
        ast_free(n->as.binary_op.left);
        ast_free(n->as.binary_op.right);
    } else if (n->type == NODE_UNARY_OP) {
        ast_free(n->as.unary_op.expr);
    }

    memset(&n->as, 0, sizeof n->as);

    n->type = NODE_NUMBER;
    n->as.number.value = (double)value;
    n->as.number.is_float = 0;

    return 1;
}

static void replace_with_double(node *n, double value) {
    if (n->type == NODE_BINARY_OP) {
        ast_free(n->as.binary_op.left);
        ast_free(n->as.binary_op.right);
    } else if (n->type == NODE_UNARY_OP) {
        ast_free(n->as.unary_op.expr);
    }

    memset(&n->as, 0, sizeof n->as);

    n->type = NODE_NUMBER;
    n->as.number.value = value;
    n->as.number.is_float = 1;
}

static int fold_binary(node *n) {
    constant_value left;
    constant_value right;

    int op = n->as.binary_op.op;

    if (!get_constant(n->as.binary_op.left, &left)) {
        return 0;
    }

    if (op == '&' && !constant_is_truthy(left)) {
        return replace_with_int(n, 0);
    }

    if (op == '|' && constant_is_truthy(left)) {
        return replace_with_int(n, 1);
    }

    if (!get_constant(n->as.binary_op.right, &right)) {
        return 0;
    }

    if (op == '&') {
        return replace_with_int(n, constant_is_truthy(left) && constant_is_truthy(right));
    }

    if (op == '|') {
        return replace_with_int(n, constant_is_truthy(left) || constant_is_truthy(right));
    }

    int is_float =
        left.type == CONST_DOUBLE ||
        right.type == CONST_DOUBLE;

    double ld = constant_to_double(left);
    double rd = constant_to_double(right);

    long li = 0;
    long ri = 0;

    if (!is_float) {
        li = constant_to_long(left);
        ri = constant_to_long(right);
    }

    long int_result = 0;
    double double_result = 0.0;

    if (is_float) {
        switch (op) {
            case '+':
                double_result = ld + rd;
                break;

            case '-':
                double_result = ld - rd;
                break;

            case '*':
                double_result = ld * rd;
                break;

            case '/':
                if (rd == 0.0) return 0;
                double_result = ld / rd;
                break;

            case '%':
                return 0;

            case 'E':
                return replace_with_int(n, ld == rd);

            case 'D':
                return replace_with_int(n, ld != rd);

            case '>':
                return replace_with_int(n, ld > rd);

            case '<':
                return replace_with_int(n, ld < rd);

            case 'G':
                return replace_with_int(n, ld >= rd);

            case 'L':
                return replace_with_int(n, ld <= rd);

            default:
                return 0;
        }

        replace_with_double(n, double_result);
        return 1;
    }

    switch (op) {
        case '+':
            if (!safe_add(li, ri, &int_result)) return 0;
            break;

        case '-':
            if (!safe_sub(li, ri, &int_result)) return 0;
            break;

        case '*':
            if (!safe_mul(li, ri, &int_result)) return 0;
            break;

        case '/':
            if (ri == 0 || (li == LONG_MIN && ri == -1)) return 0;
            int_result = li / ri;
            break;

        case '%':
            if (ri == 0 || (li == LONG_MIN && ri == -1)) return 0;
            int_result = li % ri;
            break;

        case 'E':
            int_result = (li == ri);
            break;

        case 'D':
            int_result = (li != ri);
            break;

        case '>':
            int_result = (li > ri);
            break;

        case '<':
            int_result = (li < ri);
            break;

        case 'G':
            int_result = (li >= ri);
            break;

        case 'L':
            int_result = (li <= ri);
            break;

        default:
            return 0;
    }

    return replace_with_int(n, int_result);
}

static int fold_unary(node *n) {
    constant_value value;

    if (!get_constant(n->as.unary_op.expr, &value)) {
        return 0;
    }

    int op = n->as.unary_op.op;

    if (op == '!') {
        return replace_with_int(n, !constant_is_truthy(value));
    }

    if (op == '-') {
        if (value.type == CONST_DOUBLE) {
            replace_with_double(n, -value.d);
            return 1;
        }

        long integer = constant_to_long(value);

        if (integer == LONG_MIN) {
            return 0;
        }

        return replace_with_int(n, -integer);
    }

    return 0;
}

static node *optimize_node(node *n) {
    if (n == NULL) {
        return NULL;
    }

    switch (n->type) {
        case NODE_PROGRAM:
            for (size_t i = 0; i < n->as.program.count; i++) {
                n->as.program.statements[i] =
                    optimize_node(n->as.program.statements[i]);
            }
            break;

        case NODE_BLOCK:
            for (size_t i = 0; i < n->as.block.count; i++) {
                n->as.block.statements[i] =
                    optimize_node(n->as.block.statements[i]);
            }
            break;

        case NODE_VAR_DECL:
            n->as.var_decl.init =
                optimize_node(n->as.var_decl.init);
            break;

        case NODE_ASSIGN:
            n->as.assign.expr =
                optimize_node(n->as.assign.expr);
            break;

        case NODE_BINARY_OP:
            n->as.binary_op.left =
                optimize_node(n->as.binary_op.left);

            n->as.binary_op.right =
                optimize_node(n->as.binary_op.right);

            fold_binary(n);
            break;

        case NODE_UNARY_OP:
            n->as.unary_op.expr =
                optimize_node(n->as.unary_op.expr);

            fold_unary(n);
            break;

        case NODE_CALL:
            for (size_t i = 0; i < n->as.call.arg_count; i++) {
                n->as.call.args[i] =
                    optimize_node(n->as.call.args[i]);
            }
            break;

        case NODE_RETURN:
            n->as.return_stmt.expr =
                optimize_node(n->as.return_stmt.expr);
            break;

        case NODE_IF:
            n->as.if_stmt.condition =
                optimize_node(n->as.if_stmt.condition);

            n->as.if_stmt.if_branch =
                optimize_node(n->as.if_stmt.if_branch);

            n->as.if_stmt.else_branch =
                optimize_node(n->as.if_stmt.else_branch);
            break;

        case NODE_WHILE:
            n->as.while_stmt.condition =
                optimize_node(n->as.while_stmt.condition);

            n->as.while_stmt.loop_body =
                optimize_node(n->as.while_stmt.loop_body);
            break;

        case NODE_DO_WHILE:
            n->as.do_while_stmt.loop_body =
                optimize_node(n->as.do_while_stmt.loop_body);

            n->as.do_while_stmt.condition =
                optimize_node(n->as.do_while_stmt.condition);
            break;

        case NODE_FOR:
            n->as.for_stmt.init =
                optimize_node(n->as.for_stmt.init);

            n->as.for_stmt.condition =
                optimize_node(n->as.for_stmt.condition);

            n->as.for_stmt.increment =
                optimize_node(n->as.for_stmt.increment);

            n->as.for_stmt.body =
                optimize_node(n->as.for_stmt.body);
            break;

        case NODE_NUMBER:
        case NODE_VARIABLE:
        case NODE_INC_DEC:
        case NODE_BREAK:
        case NODE_CONTINUE:
        case NODE_STRING:
        case NODE_CHAR:
            break;
    }

    return n;
}

node *optimizer_optimize(node *root) {
    return optimize_node(root);
}