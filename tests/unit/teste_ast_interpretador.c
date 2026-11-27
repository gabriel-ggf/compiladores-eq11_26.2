/*
 * Teste simples do nucleo convertido para C (AST + interpretador).
 *
 * Monta na mao a mesma AST que o parser deve produzir para
 * examples/exemplo1.lang e executa, conferindo a saida documentada no
 * README:
 *
 *     30
 *     60
 *     15
 *
 * Compilar e rodar sem CMake:
 *     gcc -Wall -Wextra -I src -o teste_nucleo \
 *         tests/unit/teste_ast_interpretador.c \
 *         src/ast/ast.c src/interpreter/interpreter.c
 *     ./teste_nucleo
 */

#include "ast/ast.h"
#include "interpreter/interpreter.h"

static node *make_printf(node *expr) {
    node *call = ast_new_call("printf");
    ast_call_add_arg(call, ast_new_string("\"%g\\n\""));
    ast_call_add_arg(call, expr);
    return call;
}

int main(void) {
    node *programa = ast_new_program();
    ast_program_set_stdio(programa, 1);

    /* x = 10; */
    ast_program_add(programa, ast_new_assign("x", ast_new_number(10)));

    /* y = 20; */
    ast_program_add(programa, ast_new_assign("y", ast_new_number(20)));

    /* print x + y;  -> 30 */
    ast_program_add(programa, make_printf(
        ast_new_binary_op('+', ast_new_variable("x"), ast_new_variable("y"))));

    /* z = (x + y) * 2; */
    ast_program_add(programa, ast_new_assign("z",
        ast_new_binary_op('*',
            ast_new_binary_op('+', ast_new_variable("x"), ast_new_variable("y")),
            ast_new_number(2))));

    /* print z;      -> 60 */
    ast_program_add(programa, make_printf(ast_new_variable("z")));

    /* print z / 4;  -> 15 */
    ast_program_add(programa, make_printf(
        ast_new_binary_op('/', ast_new_variable("z"), ast_new_number(4))));

    interpreter interp;
    interpreter_init(&interp);
    interpreter_run(&interp, programa);
    interpreter_destroy(&interp);
    ast_free(programa);

    return 0;
}