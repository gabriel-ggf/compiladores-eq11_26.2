# Decisões Técnicas

Registro das decisões técnicas do projeto e a justificativa por trás de
cada uma (exigido pela disciplina para a entrevista final).

## Linguagem de implementação: C++

- Time confortável com C/C++.
- Boa integração com Flex e Bison (ferramentas vistas em aula no material
  do professor — [COMP1](https://github.com/sergioaafreitas/COMP1)).
- Permite modelar a AST com classes/structs de forma direta.

## Abordagem: interpretador (tree-walking), não compilador

- O programa fonte é convertido em uma AST e executado diretamente,
  percorrendo essa árvore — não geramos código de máquina nem bytecode.
- Mais simples de implementar e de depurar dentro do prazo da disciplina.
- **Trade-off:** mais lento que um compilador para código gerado, mas
  suficiente para os objetivos de aprendizado do projeto.

## Análise léxica/sintática: Flex + Bison

- Ferramentas ensinadas no material da disciplina.
- Flex gera o *scanner* (tokens) a partir de expressões regulares
  (`src/lexer/scanner.l`).
- Bison gera o *parser* (regras de gramática) e monta a AST durante as
  reduções (`src/parser/parser.y`).
- O Bison é configurado para emitir os arquivos gerados com extensão
  `.cpp`, então são compilados como C++ (mesmo a gramática sendo escrita
  em estilo "clássico" C, não no modo `%language "c++"` do Bison).

## Build: CMake

- Portável entre Linux/macOS/Windows.
- Módulos `FindFLEX` e `FindBISON` nativos do CMake cuidam de gerar os
  arquivos do scanner/parser antes da compilação (`CMakeLists.txt`).

## Estrutura da AST e tabela de símbolos

- Nós da AST (`src/ast/AST.hpp`) usam ponteiros crus (`Node*`) e nunca são
  desalocados manualmente — simplificação aceita para o escopo do
  trabalho. Possível evolução futura: `std::unique_ptr` / `std::variant`.
- Tabela de símbolos: `std::unordered_map<std::string, double>` dentro do
  `Interpreter` — todas as variáveis são `double` por enquanto (sem
  tipagem estática). Ver `docs/planejamento-sprints.md` para os próximos
  passos (tipos, funções, estruturas de controle etc.).

## Convenções de código

- Nomes de arquivos/classes em `PascalCase` para tipos (`Interpreter`,
  `BinaryOpNode`), `camelCase` para variáveis e funções.
- Cada módulo (`ast`, `interpreter`, `lexer`, `parser`) fica em sua
  própria pasta dentro de `src/`.

> Atualizem este documento conforme novas decisões forem tomadas ao longo
> das sprints — é um dos itens avaliados na entrevista final.
