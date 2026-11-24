# Planejamento de Sprints

Metodologia sugerida pela disciplina: **Scrum**, com sprints semanais (Quarta a Terça).
Preencham esta tabela a cada sprint — ela é usada nas entregas **P1** e **P2**.

Apresentações e Prazos:
- **P1:** Prazo 23/09 | Apresentação 30/09
- **P2:** Prazo 04/11 | Apresentação 09/11
- **Entrevista Final (T):** Semana de 30/11

## Histórico de Versões

| Versão | Data | Descrição | Autor(es) |
|:------:|:----:|-----------|-----------|
| `0.1.0` | 26/08/2026 | Criação do planejamento inicial e distribuição de papéis. | Equipe 11 |
| `0.2.0` | 01/09/2026 | Ajuste no calendário das sprints. | Equipe 11 |
| `0.3.0` | 02/09/2026 | Ajuste para ciclo de Quarta a Terça. | Equipe 11 |
| `0.4.0` | 30/09/2026 | Reorganizando as sprints. | Equipe 11 |
| `0.5.0` | 24/11/2026 | Backlog atualizado com o que já foi concluído. | Equipe 11 |

## Backlog geral

- [ ] Definir a gramática completa da linguagem (EBNF) — a gramática existe em `src/parser/parser.y`, mas ainda não foi escrita em EBNF na documentação
- [x] Scanner: tokens básicos (números, identificadores, operadores)
- [x] Parser: expressões aritméticas
- [x] AST + interpretador: expressões aritméticas
- [x] Variáveis e atribuição — incluindo atribuição composta (`+=`, `-=`, `*=`, `/=`, `%=`) e `++`/`--`
- [x] Comando `print` — `print(...)` e `printf(...)` como funções embutidas
- [x] Estruturas de controle (`if`/`else`, laços) — `if`/`else`, `while`, `do...while`, `for`, `break` e `continue`
- [ ] Funções / escopos — só a função `main` é aceita; não há funções do usuário nem escopos aninhados
- [x] Tipos de dados (inteiro, ponto flutuante, string, booleano) — `int`, `float`/`double`, `char` e `char *` implementados; não há tipo booleano (comparações devolvem inteiro)
- [x] Tratamento de erros léxicos/sintáticos/semânticos amigável — as três fases reportam erro, com recuperação até o próximo `;` (mensagens de execução ainda estão em inglês)
- [x] Testes automatizados (ver `tests/`) — suíte em `tests/run_tests.sh` rodando no CI a cada push/PR
- [ ] Documentação final + preparação para a entrevista — em andamento
      

---

## Sprint 1 — 26/08 a 01/09

| Item | Responsável | Status |
|------|-------------|--------|
| Implementação | Código: Gabriel e João Merlin | |
| Testes | Teste: Levi e Arthur | |
| Documentação da Sprint | Doc: Vitor | |

---

## Sprint 2 — 02/09 a 08/09

| Item | Responsável | Status |
|------|-------------|--------|
| Implementação | Código: Vitor e Gabriel | |
| Testes | Teste: João Merlin e Levi | |
| Documentação da Sprint | Doc: Arthur | |

---

## Sprint 3 — 09/09 a 07/10

| Item | Responsável | Status |
|------|-------------|--------|
| Implementação | Código: Gabriel e Vitor | |
| Testes | Teste: Arthur e João Merlin | |
| Documentação da Sprint | Doc: Levi | |

---

*Pausa nas Sprints nas semanas de 23/09 e 30/09.*
*Apresentação P1: 30 de setembro.*

---

## Sprint 4 — 07/10 a 13/10

| Item | Responsável | Status |
|------|-------------|--------|
| Implementação:  | Código: João Merlin e Levi | |
| Testes | Teste: Arthur e Vitor | |
| Documentação da Sprint | Doc: Gabriel | |

---

## Sprint 5 — 14/10 a 20/10

| Item | Responsável | Status |
|------|-------------|--------|
| Implementação | Código: Gabriel e João Merlin | |
| Testes:  | Teste: Levi e Arthur | |
| Documentação da Sprint | Doc: Vitor | |

---

## Sprint 6 — 21/10 a 27/10

| Item | Responsável | Status |
|------|-------------|--------|
| Implementação | Código: Vitor e Gabriel | |
| Testes | Teste: João Merlin e Levi | |
| Documentação da Sprint | Doc: Arthur | |

---

## Sprint 7 — 28/10 a 03/11

| Item | Responsável | Status |
|------|-------------|--------|
| Implementação | Código: Arthur e Vitor | |
| Testes  | Teste: Gabriel e João Merlin | |
| Documentação da Sprint (Entrega P2) | Doc: Levi | |

---

## Sprint 8 — 04/11 a 10/11

| Item | Responsável | Status |
|------|-------------|--------|
| Implementação  | Código: Levi e Arthur | |
| Testes  | Teste: Vitor e Gabriel | |
| Documentação da Sprint | Doc: João Merlin | |

---

*Pausa na semana de 11/11.*
*Apresentação P2: 09 de novembro.*

---

## Sprint 9 — 18/11 a 24/11

| Item | Responsável | Status |
|------|-------------|--------|
| Implementação  | Código: João Merlin e Levi | |
| Testes  | Teste: Arthur e Vitor | |
| Documentação da Sprint | Doc: Gabriel | |

---

## Sprint 10 — 25/11 a 01/12

| Item | Responsável | Status |
|------|-------------|--------|
| Implementação | Código: Gabriel e João Merlin | |
| Testes | Teste: Levi e Arthur | |
| Documentação | Doc: Vitor | |

