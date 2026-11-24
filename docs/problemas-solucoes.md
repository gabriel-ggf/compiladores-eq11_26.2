# Problemas Encontrados e Soluções Adotadas

Log de problemas técnicos relevantes durante o desenvolvimento e como
foram resolvidos. Este documento é um dos itens pedidos pela disciplina
para a entrega final (mostra o processo de resolução de problemas do
time, não só o resultado).

Formato por entrada: **Contexto**, **Problema**, **Solução** e **Aprendizado**.

---

## [24/11] Trocar `print` por `printf` quebrou a suíte de testes inteira

**Contexto:** ajuste na forma de imprimir do interpretador, padronizando a saída no formato de C.

**Problema:** os testes de ponta a ponta comparam a saída obtida com um arquivo `.expected` caractere a caractere. Como vários casos usavam `print`, que quebra linha sozinho, a troca para `printf` — que só imprime o que está no formato — mudou a saída de todos eles de uma vez e a suíte ficou vermelha.

**Solução:** os casos de teste foram reescritos usando `printf("%g\n", ...)` e o interpretador manteve as duas funções embutidas, já que `print` continua útil para imprimir sem se preocupar com formato.

**Aprendizado:** teste que compara saída exata é sensível a qualquer mudança de formatação. É um preço aceitável nos casos válidos (é exatamente o que se quer verificar), mas foi o motivo de os testes de erro conferirem só o *tipo* do erro, e não o texto da mensagem.

---

## [23/11] Programa com erro de sintaxe era executado mesmo assim

**Contexto:** a gramática tem recuperação de erro (`input: input error SEMICOLON`), que descarta tokens até o próximo `;` para conseguir reportar vários erros numa só execução.

**Problema:** justamente por se recuperar, o `yyparse()` terminava retornando `0` (sucesso). O `main.c` interpretava isso como "parse deu certo" e mandava o interpretador executar uma AST montada a partir de um programa que tinha erro.

**Solução:** o parser passou a contar os erros numa variável `parse_errors`, incrementada no `yyerror()`. O `main.c` só executa a árvore se `yyparse()` retornou zero **e** `parse_errors == 0`.

**Aprendizado:** recuperação de erro e sucesso do parse são coisas diferentes. Quem recupera precisa registrar que recuperou, senão a informação se perde para quem chamou.

---

## [08/11] Ambiguidade do `else` pendente (*dangling else*)

**Contexto:** inclusão do `if`/`else` na gramática do Bison.

**Problema:** em `if (a) if (b) X; else Y;` o `else` pode ser ligado a qualquer um dos dois `if`. A gramática fica ambígua e o Bison acusa conflito *shift/reduce*.

**Solução:** declarar as precedências `%precedence THEN` e `%precedence KW_ELSE` (nessa ordem) e marcar a regra do `if` sem `else` com `%prec THEN`. Isso faz o Bison preferir o *shift*, ligando o `else` ao `if` mais próximo — a mesma regra de C.

**Aprendizado:** é o conflito clássico da disciplina. Resolver por precedência é mais simples e mais legível do que reescrever a gramática para eliminar a ambiguidade.

---

## [08/11] Dois commits consertando a mesma coisa deram conflito

**Contexto:** dois integrantes mexendo em paralelo no `parser.y` e no `scanner.l` durante a integração do build.

**Problema:** os dois atacaram o mesmo defeito em commits separados na mesma branch, e o merge deu conflito nos mesmos trechos dos dois arquivos.

**Solução:** o conflito foi resolvido na mão no merge, mantendo uma das versões.

**Aprendizado:** `parser.y` e `scanner.l` são os arquivos mais disputados do projeto. Antes de mexer neles, combinar no grupo quem está com o arquivo — foi o que motivou a divisão por issues com responsável definido.

---

## [03/11] Dois `main()` disputando o ponto de entrada

**Contexto:** ligação do parser com a AST e o interpretador, para fechar o pipeline.

**Problema:** o `parser.y` tinha o próprio `int main(void) { return yyparse(); }`, herdado da fase em que ele era um avaliador independente que imprimia o resultado direto. O `src/main.c` também define `main`. Com os dois no mesmo executável, o linker acusava símbolo duplicado — ou seja, não dava para ter o pipeline completo e o parser standalone ao mesmo tempo.

**Solução:** o `main()` saiu do `parser.y`, que passou apenas a preencher a variável global `root_node` com a AST montada. O `src/main.c` virou o único ponto de entrada: abre o arquivo, chama `yyparse()`, e entrega a árvore ao interpretador.

**Aprendizado:** o parser tem uma responsabilidade só — reconhecer a entrada e construir a árvore. Quem decide o que fazer com ela é outra camada. Enquanto ele também executava e imprimia, as duas fases ficavam impossíveis de testar separadamente.

---

## [26/10] Bison gerava `parser.hpp`, mas o scanner incluía `parser.tab.h`

**Contexto:** migração do build de C++ para C.

**Problema:** o `CMakeLists.txt`, herdado do esqueleto em C++, mandava o Bison gerar `parser.cpp` e `parser.hpp`. Só que o `scanner.l` incluía `"parser.tab.h"` — o nome padrão do Bison na linha de comando. O arquivo incluído nunca existia no diretório de build, e o build pelo CMake quebrava mesmo com o time conseguindo compilar na mão.

**Solução:** o `BISON_TARGET` passou a gerar `parser.tab.c` e `parser.tab.h`, batendo com o `#include` que já estava no scanner.

**Aprendizado:** quando uma ferramenta gera um arquivo e outra o consome, o nome é um contrato entre as duas. O build na mão e o build pelo CMake precisam gerar exatamente os mesmos nomes, senão um funciona e o outro não — e a divergência só aparece para quem usa o caminho menos testado.

---

## [20/10] Migração de C++ para C: sem herança, sem destrutor e sem exceção

**Contexto:** decisão de abandonar o C++ e passar o núcleo (AST, interpretador e `main`) para C puro.

**Problema:** o código em C++ dependia de três recursos que simplesmente não existem em C. Os nós da AST eram `structs` derivados de um `Node` base; a liberação da memória era automática via destrutores; e os erros de execução eram sinalizados com `throw std::runtime_error`. Além disso, `std::string`, `std::vector` e `std::unordered_map` precisavam de substituto.

**Solução:** a herança virou um único `struct node` com uma *tag* (`node_type`) e uma `union`; a liberação virou a função recursiva `ast_free()`, chamada explicitamente no fim do `main.c`; as exceções viraram mensagem em `stderr` seguida de `exit(EXIT_FAILURE)`, que tem o mesmo efeito prático de uma exceção não capturada. `std::vector` virou vetor dinâmico manual, `std::string` virou `char *` com cópia própria (porque `strdup` não é padrão C) e `std::unordered_map` virou lista encadeada.

**Aprendizado:** a tradução não é linha a linha — cada recurso do C++ exige escolher uma estrutura equivalente em C e assumir o custo dela. O que o compilador fazia sozinho (destruir objetos, propagar exceções) passa a ser responsabilidade explícita do código, e o que some do tipo estático vira verificação em tempo de execução.

---

<!-- Adicionem novas entradas acima desta linha, mais recente no topo. -->
