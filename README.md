# Compiladores — Equipe 11 (2026.2)

Interpretador desenvolvido para a disciplina de **Compiladores** (curso de
Engenharia de Software), seguindo a proposta e o material de apoio do
professor: [github.com/sergioaafreitas/COMP1](https://github.com/sergioaafreitas/COMP1).

## Equipe

| Nome | Matrícula | GitHub |
|------|-----------|--------|
|      |           |        |
|      |           |        |
|      |           |        |
|      |           |        |
|      |           |        |

_(preencham a tabela acima com os membros do grupo — até 5 integrantes,
conforme as regras da disciplina)_

## O que é este projeto

Um **interpretador** para uma linguagem simples, criada pelo próprio
time. Diferente de um compilador (que traduz o código fonte para outra
linguagem/código de máquina), o interpretador lê o programa fonte e o
**executa diretamente**.

O pipeline é o clássico de compiladores/interpretadores:

```
código fonte (.lang)
        │
        ▼
  Análise Léxica      (Flex)      →  tokens
        │
        ▼
  Análise Sintática   (Bison)     →  Árvore Sintática Abstrata (AST)
        │
        ▼
  Interpretação       (C++)       →  execução direta da AST
```

A linguagem atual (ponto de partida) suporta:

- Expressões aritméticas: `+ - * /`, com precedência e parênteses
- Variáveis (todas numéricas, tipo `double`): `x = 10;`
- Comando de saída: `print x + 1;`
- Comentários de linha: `// comentário`

Isso é só o esqueleto inicial — a evolução da linguagem (estruturas de
controle, funções, tipos, etc.) é planejada em
[`docs/planejamento-sprints.md`](docs/planejamento-sprints.md).

## Estrutura do repositório

```
.
├── CMakeLists.txt          # build (CMake + módulos FindFLEX/FindBISON)
├── src/
│   ├── main.cpp            # ponto de entrada
│   ├── lexer/
│   │   └── scanner.l       # analisador léxico (Flex)
│   ├── parser/
│   │   └── parser.y        # analisador sintático (Bison) — monta a AST
│   ├── ast/
│   │   └── AST.hpp         # definição dos nós da AST
│   └── interpreter/
│       ├── Interpreter.hpp
│       └── Interpreter.cpp # percorre a AST e executa o programa
├── examples/
│   └── exemplo1.lang       # programa de exemplo na linguagem
├── tests/                  # testes (ver tests/README.md)
└── docs/
    ├── decisoes-tecnicas.md      # justificativa das escolhas técnicas
    ├── planejamento-sprints.md   # backlog e planejamento por sprint
    └── problemas-solucoes.md     # log de problemas e soluções (dev log)
```

## Como compilar e rodar

### Pré-requisitos

- CMake >= 3.16
- Um compilador C++17 (g++ ou clang++)
- Flex
- Bison

No Ubuntu/Debian:

```bash
sudo apt install build-essential cmake flex bison
```

### Windows

O projeto precisa rodar em Windows, mas **a forma recomendada é via
WSL (Windows Subsystem for Linux)**, não compilando nativamente com
MSVC — assim o time usa o mesmo ambiente (Flex/Bison/g++/CMake via apt)
em qualquer sistema operacional, evitando divergências de build entre
Windows/Linux/macOS.

1. Instalar o WSL (PowerShell como administrador):
   ```powershell
   wsl --install
   ```
   (por padrão instala Ubuntu; reinicie a máquina se for pedido)
2. Abrir o terminal Ubuntu/WSL e seguir os mesmos passos do Ubuntu/Debian
   acima (`sudo apt install build-essential cmake flex bison`) e o
   restante deste guia normalmente dentro do WSL.
3. O código do repositório pode continuar em `C:\...` e ser acessado de
   dentro do WSL em `/mnt/c/...`, ou ser clonado direto dentro do
   filesystem do WSL (mais rápido para compilar) — qualquer uma das duas
   funciona.

### Build

```bash
cmake -S . -B build
cmake --build build
```

### Executar

```bash
./build/interpretador examples/exemplo1.lang
```

Saída esperada:

```
30
60
15
```

## Metodologia e entregas

O time segue **Scrum**, com sprints semanais (ver
[`docs/planejamento-sprints.md`](docs/planejamento-sprints.md)). As
entregas da disciplina são:

- **P1** — definição do projeto, linguagem escolhida, planejamento das
  sprints e o que já foi implementado.
- **P2** — funcionalidades desenvolvidas desde a P1, melhorias e ajustes
  no planejamento.
- **Entrega final** — repositório acessível 15 dias antes da entrevista
  presencial, onde o time explica o funcionamento do interpretador e
  justifica as decisões técnicas tomadas (ver
  [`docs/decisoes-tecnicas.md`](docs/decisoes-tecnicas.md) e
  [`docs/problemas-solucoes.md`](docs/problemas-solucoes.md)).

## Referências

- Material da disciplina: [COMP1](https://github.com/sergioaafreitas/COMP1)
  (Prof. Sergio A. A. Freitas)
- [GNU Flex](https://westes.github.io/flex/manual/)
- [GNU Bison](https://www.gnu.org/software/bison/manual/)
