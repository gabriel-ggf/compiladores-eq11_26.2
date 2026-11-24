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

- Nós da AST (`src/ast/ast.h`) são modelados com um único `struct node` que carrega uma *tag* (`node_type`) e uma `union` com os campos de cada tipo de nó — é a tradução direta da herança que existia em C++, já que C não tem classes.
- O gerenciamento de memória é manual: os construtores `ast_new_*` alocam com `malloc` e `ast_free()` libera a árvore inteira recursivamente, chamada no fim de `src/main.c`.

## Duas tabelas de símbolos, em momentos diferentes

O projeto mantém **duas** tabelas de símbolos, de propósito. Elas respondem a perguntas diferentes e vivem em fases diferentes do pipeline:

| Tabela | Arquivo | Quando | Guarda | Para quê |
|---|---|---|---|---|
| Do parser | `src/ast/table.c` | durante o parse | nome → tipo declarado (texto) | acusar *variável não declarada* e *variável já declarada* como erro semântico, antes de executar qualquer coisa |
| Do interpretador | `src/interpreter/interpreter.c` | durante a execução | nome → valor já tipado | resolver o valor de cada variável enquanto a AST é percorrida |

- Ambas são **listas encadeadas**, substituindo o `std::unordered_map` do C++. Para o tamanho de programa que a disciplina exige, a busca linear não é gargalo e o código fica muito mais fácil de explicar na entrevista do que uma tabela hash própria.
- **Trade-off:** há duplicação de informação entre as duas. A alternativa — uma tabela só, compartilhada entre parser e interpretador — acoplaria as duas fases e dificultaria detectar o erro semântico antes da execução.

## Tipagem dos valores em tempo de execução

- As variáveis deixaram de ser todas `double`. O interpretador trabalha com valores tipados (`int`, ponto flutuante, `char` e string), e a declaração define o tipo.
- `float` e `double` são tratados como o mesmo tipo interno de ponto flutuante; `int`, `long`, `short`, `signed` e `unsigned` caem todos no tipo inteiro. Aceitar as palavras-chave sem implementar as diferenças de precisão mantém os programas de teste com cara de C sem custo de implementação.
- Strings existem apenas como `char *` — não há aritmética de ponteiros nem vetores.

## Tratamento de erros sem exceções

- C não tem exceções. Onde a versão C++ lançava `std::runtime_error`, o interpretador imprime a mensagem em `stderr` e encerra com código de saída diferente de zero — mesmo efeito prático de uma exceção não capturada.
- Erros sintáticos têm recuperação: a regra `input: input error SEMICOLON` do Bison descarta tokens até o próximo `;` e continua o parse, de modo que um arquivo com vários erros reporta vários erros em uma só execução, em vez de parar no primeiro.
- O `main.c` só executa a AST se o parse terminou sem erros (`parse_errors == 0`), para não interpretar uma árvore incompleta.

## Escopo da linguagem: só a função `main`

- O parser aceita a definição de uma única função, `main`, e qualquer outro nome é recusado com erro semântico. Programas também podem ser escritos direto no nível do arquivo, sem `main`.
- **Motivo:** funções definidas pelo usuário exigiriam tabela de símbolos com escopos aninhados, pilha de chamadas e passagem de parâmetros — o que não cabe no prazo da disciplina. Manter a restrição explícita é melhor do que aceitar a sintaxe e falhar de forma confusa na execução.
- `printf` e `print` são as únicas funções chamáveis, implementadas direto no interpretador como embutidas.

## Testes automatizados e CI

- A suíte (`tests/run_tests.sh`) roda o interpretador contra pares `.lang`/`.expected`: nos casos válidos compara a saída exata, nos inválidos confere apenas o *tipo* de erro esperado (`ERRO_SEMANTICO`, `ERRO_SINTATICO`, `ERRO_RUNTIME`).
- **Por que comparar só o tipo do erro:** prender o teste ao texto exato da mensagem faria qualquer ajuste de redação quebrar a suíte. O que importa é que o erro certo seja detectado na fase certa.
- O runner é um script shell em vez de um framework (Catch2, GoogleTest): não adiciona dependência de build e trata o interpretador como caixa-preta, que é como o professor vai executá-lo.
- O GitHub Actions (`.github/workflows/ci.yml`) compila e roda a suíte a cada `push` e *pull request* em qualquer branch, no Ubuntu — o mesmo ambiente que o README recomenda via WSL.

## Convenções de código

- Nomes de arquivos/structs em `snake_case` (ex: `binary_op_node`, `interpreter`), alinhado com as práticas comuns da linguagem C.
- Cada módulo (`ast`, `interpreter`, `lexer`, `parser`) fica em sua própria pasta dentro de `src/`.

> Atualizem este documento conforme novas decisões forem tomadas ao longo das sprints — é um dos itens avaliados na entrevista final.
