# Decisões Técnicas

Registro das decisões técnicas do projeto e a justificativa por trás de cada uma (exigido pela disciplina para a entrevista final).

## Linguagem de implementação: C (Substituindo C++)

- **Mudança:** Inicialmente o projeto começou em C++, mas mudamos para C puro por sugestão do professor.
- A integração com as ferramentas Flex e Bison (vistas no material de aula — [COMP1](https://github.com/sergioaafreitas/COMP1)) é nativa e mais direta em C, sem a necessidade de forçar o Bison a trabalhar com classes ou modos específicos.
- Simplifica o entendimento do pipeline clássico de compiladores, alinhando-se exatamente com os exemplos da disciplina.

## Abordagem: interpretador (tree-walking), não compilador

- O programa fonte é convertido em uma AST e executado diretamente, percorrendo essa árvore — não geramos código de máquina nem bytecode.
- Mais simples de implementar e de depurar dentro do prazo da disciplina.
- **Trade-off:** mais lento que um compilador para código gerado, mas suficiente para os objetivos de aprendizado do projeto.

## Análise léxica/sintática: Flex + Bison

- Ferramentas ensinadas no material da disciplina.
- Flex gera o *scanner* (tokens) a partir de expressões regulares (`src/lexer/scanner.l`).
- Bison gera o *parser* (regras de gramática) e monta a AST durante as reduções (`src/parser/parser.y`).
- Com a migração para C, o código gerado pelo Bison compila de forma natural, sem conflitos com paradigmas de orientação a objetos.

## Build: CMake

- Portável entre Linux/macOS/Windows.
- Módulos `FindFLEX` e `FindBISON` nativos do CMake cuidam de gerar os arquivos do scanner/parser antes da compilação (`CMakeLists.txt`).

## Estrutura da AST e tabela de símbolos

- Nós da AST (`src/ast/ast.h`) agora são modelados usando `structs` e uniões em C (em vez de classes). O gerenciamento de memória é manual, e a liberação da árvore completa ao final da execução precisa ser tratada explicitamente ou assumida como simplificação do escopo.
- Tabela de símbolos: Substituímos o `std::unordered_map` do C++ por uma implementação customizada em C (ex: tabela hash ou lista encadeada, dependendo da evolução) para armazenar variáveis, que continuam sendo tratadas como `double` inicialmente. Ver `docs/planejamento-sprints.md` para os próximos passos (tipos, funções, estruturas de controle etc.).

## Convenções de código

- Nomes de arquivos/structs em `snake_case` (ex: `binary_op_node`, `interpreter`), alinhado com as práticas comuns da linguagem C.
- Cada módulo (`ast`, `interpreter`, `lexer`, `parser`) fica em sua própria pasta dentro de `src/`.

> Atualizem este documento conforme novas decisões forem tomadas ao longo das sprints — é um dos itens avaliados na entrevista final.
