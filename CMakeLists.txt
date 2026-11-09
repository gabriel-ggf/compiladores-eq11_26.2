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

# Nucleo em C puro: AST + tabela de simbolos + interpretador (tree-walking).
add_library(nucleo STATIC
    src/ast/ast.c
    src/ast/table.c
    src/interpreter/interpreter.c
)
target_include_directories(nucleo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(nucleo PUBLIC m)

# Pipeline completo: scanner + parser (preenchem a AST) + main + nucleo.
# O root_node e definido em src/parser/parser.y e usado em src/main.c.
add_executable(interpretador
    src/main.c
    ${FLEX_Scanner_OUTPUTS}
    ${BISON_Parser_OUTPUTS}
)
target_include_directories(interpretador PRIVATE ${GENERATED_DIR})
target_link_libraries(interpretador PRIVATE nucleo)

# Teste simples do nucleo convertido (ver tests/unit/).
add_executable(teste_nucleo tests/unit/teste_ast_interpretador.c)
target_link_libraries(teste_nucleo PRIVATE nucleo)

foreach(alvo interpretador nucleo teste_nucleo)
    target_compile_options(${alvo} PRIVATE -Wall -Wextra)
endforeach()