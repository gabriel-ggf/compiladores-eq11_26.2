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

A linguagem atual (ponto de partida) suporta:

- Expressões aritméticas: `+ - * /`, com precedência e parênteses
- Variáveis (todas numéricas, tipo `double`): `x = 10;`
- Comando de saída: `print x + 1;`
- Comentários de linha: `// comentário`

## Estrutura do repositório

    .
    ├── CMakeLists.txt
    ├── src/
    │   ├── main.c
    │   ├── lexer/
    │   │   └── scanner.l
    │   ├── parser/
    │   │   └── parser.y
    │   ├── ast/
    │   │   ├── ast.h
    │   │   └── ast.c
    │   └── interpreter/
    │       ├── interpreter.h
    │       └── interpreter.c
    ├── examples/
    │   └── exemplo1.lang
    ├── tests/
    └── docs/
        ├── decisoes-tecnicas.md
        ├── planejamento-sprints.md
        └── problemas-solucoes.md

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

### Executar

    ./build/interpretador examples/exemplo1.lang

Saída esperada:

    30
    60
    15

## Metodologia e entregas

O time segue **Scrum**, com sprints semanais de Quarta a Terça. O cronograma completo, responsáveis e backlog de cada semana estão detalhados no arquivo **[`docs/planejamento-sprints.md`](docs/planejamento-sprints.md)**. Datas conforme o Plano de Ensino (FGA0003 — Compiladores 1, T02, 2026/2):

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
