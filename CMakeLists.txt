cmake_minimum_required(VERSION 3.16)
project(compiladores_eq11 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_DIR})

# O Bison e executado primeiro para que o header com os tokens
# (parser.tab.h) exista antes do Flex inclui-lo em src/lexer/scanner.l.
BISON_TARGET(Parser
    src/parser/parser.y
    ${GENERATED_DIR}/parser.tab.c
    DEFINES_FILE ${GENERATED_DIR}/parser.tab.h
)

FLEX_TARGET(Scanner
    src/lexer/scanner.l
    ${GENERATED_DIR}/lex.yy.c
)

ADD_FLEX_BISON_DEPENDENCY(Scanner Parser)

# Pipeline que roda hoje: scanner + parser. O main() esta em
# src/parser/parser.y e o parser avalia as expressoes direto, sem AST.
add_executable(interpretador
    ${FLEX_Scanner_OUTPUTS}
    ${BISON_Parser_OUTPUTS}
)
target_include_directories(interpretador PRIVATE ${GENERATED_DIR})

# Nucleo em C puro: AST + interpretador (tree-walking).
add_library(nucleo STATIC
    src/ast/ast.c
    src/interpreter/interpreter.c
)
target_include_directories(nucleo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# src/main.c ainda nao entra em nenhum executavel: ele e o ponto de entrada
# da versao com AST, e hoje colidiria com o main() de src/parser/parser.y.
# Fica compilado aqui para nao quebrar sem ninguem perceber. Quando o
# parser passar a preencher root_node, basta juntar src/main.c e o alvo
# nucleo ao executavel interpretador e remover este alvo.
add_library(entrada OBJECT src/main.c)
target_include_directories(entrada PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Teste simples do nucleo convertido (ver tests/unit/).
add_executable(teste_nucleo tests/unit/teste_ast_interpretador.c)
target_link_libraries(teste_nucleo PRIVATE nucleo)

foreach(alvo interpretador nucleo entrada teste_nucleo)
    target_compile_options(${alvo} PRIVATE -Wall -Wextra)
endforeach()
