cmake_minimum_required(VERSION 3.16)
project(compiladores_eq11 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_DIR})

# O Bison é executado primeiro para que o header com os tokens
# (parser.hpp) exista antes do Flex incluí-lo.
BISON_TARGET(Parser
    src/parser/parser.y
    ${GENERATED_DIR}/parser.cpp
    DEFINES_FILE ${GENERATED_DIR}/parser.hpp
)

FLEX_TARGET(Scanner
    src/lexer/scanner.l
    ${GENERATED_DIR}/scanner.cpp
)

ADD_FLEX_BISON_DEPENDENCY(Scanner Parser)

add_executable(interpretador
    src/main.cpp
    src/interpreter/Interpreter.cpp
    ${FLEX_Scanner_OUTPUTS}
    ${BISON_Parser_OUTPUTS}
)

target_include_directories(interpretador PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${GENERATED_DIR}
)

target_compile_options(interpretador PRIVATE -Wall -Wextra)
