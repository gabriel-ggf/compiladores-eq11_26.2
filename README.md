# Compiladores — Equipe 11 (2026.2)

Interpretador desenvolvido para a disciplina de **Compiladores** (curso de Engenharia de Software), seguindo a proposta e o material de apoio do professor: [github.com/sergioaafreitas/COMP1](https://github.com/sergioaafreitas/COMP1).

## Equipe

| Nome | Matrícula | GitHub |
|------|-----------|--------|
| Arthur Souto Santos | 222006570 | [@arthursouto09](https://github.com/arthursouto09) |
| Gabriel Guedes Fernandes | 232014656 | [@gabriel-ggf](https://github.com/gabriel-ggf) |
| João Vitor Mendonça Merlin | 222006848 | [@jvopBR](https://github.com/jvopBR) |
| Levi Evangelista Santos | 241012042 | [@leviaqui](https://github.com/leviaqui) |
| Vitor Guilherme Lustosa de Carvalho | 232014342 | [@Vitorlustosa](https://github.com/Vitorlustosa) |

## Histórico de Versões

| Versão | Data | Descrição | Autor(es) |
|:------:|:----:|-----------|-----------|
| `0.1.0` | 26/08/2026 | Estruturação inicial do projeto, pipeline Flex/Bison e documentação base (C++). | Equipe 11 |
| `0.2.0` | 02/09/2026 | Atualização do ciclo de sprints e link para o planejamento. | Equipe 11 |
| `0.3.0` | 09/09/2026 | Implementação dos tokens essenciais no scanner.l e seus respectivos testes | Equipe 11 |
| `0.4.0` | 26/10/2026 | Migração do núcleo (AST, interpretador e main) de C++ para C puro. | Equipe 11 |
| `0.5.0` | 03/11/2026 | Tabela de símbolos e novos nós da AST (controle de fluxo, tipos, chamadas). | Equipe 11 |
| `0.6.0` | 08/11/2026 | Pipeline completo: o parser monta a AST e o interpretador a executa. | Equipe 11 |
| `0.7.0` | 18/11/2026 | Suíte de testes automatizados (`tests/run_tests.sh`) e CI no GitHub Actions. | Equipe 11 |
| `0.8.0` | 24/11/2026 | Atualização geral da documentação para o estado atual do projeto. | Equipe 11 |

## O que é este projeto

Um **interpretador** para uma linguagem simples, criada pelo próprio time. Diferente de um compilador (que traduz o código fonte para outra linguagem/código de máquina), o interpretador lê o programa fonte e o **executa diretamente**.

O pipeline é o clássico de compiladores/interpretadores:

    código fonte (.lang)
            │
            ▼
      Análise Léxica      (Flex)      →  tokens
            │
            ▼
      Análise Sintática   (Bison)     →  Árvore Sintática Abstrata (AST)
            │
            ▼
      Interpretação       (C)         →  execução direta da AST

A análise semântica acontece em dois momentos: durante o parse, uma tabela de símbolos (`src/ast/table.c`) verifica declaração e redeclaração de variáveis; em tempo de execução, o interpretador mantém a própria tabela, com os valores já tipados.

## A linguagem

A sintaxe é um subconjunto de C. Um programa pode ser escrito direto no nível do arquivo ou dentro de `int main() { ... }` — no momento `main` é a única função que pode ser definida.

```c
// Calcula o fatorial de 5 e mostra os pares ate 10
int main() {
    int n = 5;
    int fat = 1;

    for (int i = 1; i <= n; i++) {
        fat *= i;
    }

    printf("fatorial de %d = %d\n", n, fat);

    int k = 0;
    while (k <= 10) {
        if (k % 2 == 0) {
            printf("%d ", k);
        }
        k++;
    }

    return 0;
}
```

### Tipos

| Declaração | Tipo interno |
|---|---|
| `int`, `long`, `short`, `signed`, `unsigned` | inteiro |
| `float`, `double` | ponto flutuante |
| `char` | caractere |
| `char *` | string |

Variáveis precisam ser declaradas antes do uso e não podem ser redeclaradas — os dois casos geram erro semântico ainda durante o parse.

### Operadores

- **Aritméticos:** `+` `-` `*` `/` `%`
- **Comparação:** `==` `!=` `<` `>` `<=` `>=`
- **Lógicos:** `&&` `||` `!`
- **Unário:** `-` (negação)
- **Atribuição:** `=` e os compostos `+=` `-=` `*=` `/=` `%=`
- **Incremento/decremento:** `++x` `--x` `x++` `x--`

Precedência e associatividade seguem as de C e estão declaradas em `src/parser/parser.y`.

### Controle de fluxo

`if` / `else`, `while`, `do ... while`, `for`, `break`, `continue`, `return` e blocos `{ }`.

### Saída

Duas funções embutidas:

- `printf(formato, ...)` — aceita os especificadores `%d` `%i` `%f` `%g` `%c` `%s` e `%%`.
- `print(a, b, ...)` — imprime os argumentos na sequência e quebra a linha no final.

### Literais e comentários

- Números inteiros e com ponto decimal (`10`, `3.14`).
- Strings entre aspas duplas e caracteres entre aspas simples, com suporte aos escapes de nova linha, tabulação, retorno de carro, nulo, barra invertida e aspas.
- Comentários de linha (`// ...`) e de bloco (`/* ... */`).
- Linhas iniciadas por `#` (diretivas de pré-processador) são ignoradas pelo scanner.

### Erros reportados

| Tipo | Quando ocorre | Formato |
|---|---|---|
| Léxico | caractere desconhecido | `[Erro Léxico] <caractere>` |
| Sintático | regra não reconhecida | `ERRO - ...`, com recuperação até o próximo `;` |
| Semântico | variável não declarada ou já declarada, função não suportada | `ERRO - Erro Semantico: ...` |
| Execução | divisão por zero, função indefinida, etc. | mensagem no `stderr` e código de saída diferente de zero |

## Estrutura do repositório

    .
    ├── CMakeLists.txt              # build (CMake + módulos FindFLEX/FindBISON)
    ├── .github/workflows/ci.yml    # CI: compila e roda a suíte a cada push/PR
    ├── src/
    │   ├── main.c                  # ponto de entrada
    │   ├── lexer/
    │   │   └── scanner.l           # analisador léxico (Flex)
    │   ├── parser/
    │   │   └── parser.y            # analisador sintático (Bison) — monta a AST
    │   ├── ast/
    │   │   ├── ast.h / ast.c       # nós da AST e funções de criação/liberação
    │   │   └── table.h / table.c   # tabela de símbolos do parser (análise semântica)
    │   └── interpreter/
    │       ├── interpreter.h
    │       └── interpreter.c       # percorre a AST e executa o programa
    ├── examples/
    │   └── exemplo1.lang           # programa de exemplo na linguagem
    ├── tests/                      # suíte automatizada (ver tests/README.md)
    │   ├── run_tests.sh            # runner dos testes
    │   ├── e2e/valid/              # programas válidos + saída esperada
    │   ├── e2e/invalid/            # programas com erro + tipo de erro esperado
    │   └── unit/                   # teste unitário do núcleo (AST + interpretador)
    └── docs/
        ├── decisoes-tecnicas.md    # justificativa das escolhas técnicas
        ├── problemas-solucoes.md   # log de problemas e soluções (dev log)
        ├── planejamento-sprints.md # cronograma, papéis e backlog
        ├── ata-sprints/            # atas de cada sprint
        └── modelos/                # modelos de ata, issue e PR

## Como compilar e rodar

### Pré-requisitos

- CMake >= 3.16
- Um compilador C (gcc ou clang)
- Flex
- Bison

No Ubuntu/Debian:

    sudo apt install build-essential cmake flex bison

### Windows

O projeto precisa rodar em Windows, mas **a forma recomendada é via WSL (Windows Subsystem for Linux)**, não compilando nativamente com MSVC — assim o time usa o mesmo ambiente (Flex/Bison/gcc/CMake via apt) em qualquer sistema operacional, evitando divergências de build entre Windows/Linux/macOS.

1. Instalar o WSL (PowerShell como administrador):

       wsl --install
   
2. Abrir o terminal Ubuntu/WSL e seguir os mesmos passos do Ubuntu/Debian acima (`sudo apt install build-essential cmake flex bison`) e o restante deste guia normalmente dentro do WSL.
3. O código do repositório pode continuar em `C:\...` e ser acessado de dentro do WSL em `/mnt/c/...`, ou ser clonado direto dentro do filesystem do WSL.

### Build

    cmake -S . -B build
    cmake --build build

São gerados dois executáveis em `build/`:

- `interpretador` — o interpretador completo (scanner + parser + AST + interpretador)
- `teste_nucleo` — teste unitário do núcleo, usado pela suíte de testes

### Executar

    ./build/interpretador examples/exemplo1.lang

Saída esperada:

    Hello World!
    This is an example.

## Testes

A suíte completa roda com um comando a partir da raiz do repositório:

    ./tests/run_tests.sh --build

O `--build` recompila antes de testar e `--verbose` mostra o diff completo quando um caso falha. Os detalhes de cada categoria de teste e de como adicionar novos casos estão em [`tests/README.md`](tests/README.md).

A mesma suíte roda automaticamente no **GitHub Actions** a cada `push` e cada *pull request*, em todas as branches (ver [`.github/workflows/ci.yml`](.github/workflows/ci.yml)).

## Metodologia e entregas

O time segue **Scrum**, com sprints semanais de Quarta a Terça. O cronograma completo, responsáveis e backlog de cada semana estão detalhados no arquivo **[`docs/planejamento-sprints.md`](docs/planejamento-sprints.md)**, e cada sprint tem sua ata em **[`docs/ata-sprints/`](docs/ata-sprints/)**. Datas conforme o Plano de Ensino (FGA0003 — Compiladores 1, T02, 2026/2):

| Entrega | Formulário (prazo) | Apresentação/entrevista |
|---------|---------------------|--------------------------|
| **P1** | até **23/09/2026, 23h59** | **30/09/2026** (equipes 9 a 16, 2ª parte) |
| **P2** | até **04/11/2026, 23h59** | **09/11/2026** (equipes 16 a 9, 1ª parte) |
| **Entrega final / Entrevista (T)** | repositório acessível 15 dias antes | **30/11 ou 02/12/2026** |

**O que cada entrega cobra:**
- **P1** — definição do projeto, linguagem escolhida, planejamento das sprints e o que já foi implementado.
- **P2** — funcionalidades desenvolvidas desde a P1, melhorias e ajustes no planejamento.
- **Entrega final** — repositório acessível 15 dias antes da entrevista presencial, onde o time explica o funcionamento do interpretador e justifica as decisões técnicas tomadas (ver [`docs/decisoes-tecnicas.md`](docs/decisoes-tecnicas.md) e [`docs/problemas-solucoes.md`](docs/problemas-solucoes.md)).

**Fórmula da nota final:**

    NF = (P1 + 2·P2 + 3·T + 4·C) / 10

Aprovação exige **NF ≥ 5,0 e 75% de presença** nas aulas.

## Referências


- Material da disciplina: [COMP1](https://github.com/sergioaafreitas/COMP1)
  (Prof. Sergio A. A. Freitas)
- [GNU Flex](https://westes.github.io/flex/manual/)
- [GNU Bison](https://www.gnu.org/software/bison/manual/)
