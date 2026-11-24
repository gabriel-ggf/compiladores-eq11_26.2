# Testes Automatizados

Este diretório contém a suíte de testes automatizados do interpretador, cobrindo testes de ponta a ponta (*end-to-end*) e testes unitários do núcleo em C.

---

## Como Executar os Testes

Para rodar todos os testes de uma vez, execute o runner a partir da raiz do repositório:

```bash
./tests/run_tests.sh
```

### Opções Disponíveis

- **Recompilar antes de testar (`--build`):**
  Compila o projeto via CMake antes de executar a suíte.
  ```bash
  ./tests/run_tests.sh --build
  ```

- **Exibir detalhes de falhas (`--verbose`):**
  Em caso de falha, exibe o `diff` completo entre a saída obtida e a esperada, ou o conteúdo do `stderr`.
  ```bash
  ./tests/run_tests.sh --verbose
  ```

---

## Estrutura do Diretório

```text
tests/
├── run_tests.sh       # Script principal de automação dos testes
├── e2e/               # Testes de ponta a ponta (execução via interpretador)
│   ├── valid/         # Programas válidos com saída esperada
│   └── invalid/       # Cenários de erro (sintático, semântico e runtime)
├── unit/              # Testes unitários de módulos isolados
│   └── teste_ast_interpretador.c
└── README.md          # Esta documentação
```

---

## Categorias de Teste

### 1. Testes Válidos (`tests/e2e/valid/`)
Cada caso de teste é composto por um par de arquivos:
- `nome.lang`: código-fonte na linguagem do projeto.
- `nome.expected`: saída exata esperada no `stdout`.

O teste passa se o programa rodar com sucesso (`exit code 0`), sem mensagens no `stderr` e com saída idêntica ao `.expected`.

### 2. Testes Inválidos (`tests/e2e/invalid/`)
Destinados a validar a captura e tratamento de erros:
- `nome.lang`: código com erro intencional.
- `nome.expected`: tag do tipo de erro esperado:
  - `ERRO_SEMANTICO`: espera mensagem `"Erro Semantico"` no `stderr`.
  - `ERRO_SINTATICO`: espera mensagem `"[Erro Sintatico]"` no `stderr`.
  - `ERRO_RUNTIME`: espera interrupção com `exit code != 0`.

### 3. Testes Unitários (`tests/unit/`)
Testam diretamente as estruturas internas em C (AST, tabela de símbolos e interpretador) sem passar pelo scanner/parser, compilados como binários independentes (ex: `teste_nucleo`).

---

## Como Adicionar um Novo Teste

1. **Para um teste válido:**
   - Crie `tests/e2e/valid/XX_nome_do_teste.lang` com o código a ser executado.
   - Crie `tests/e2e/valid/XX_nome_do_teste.expected` contendo o que deve ser impresso no terminal.

2. **Para um teste de erro:**
   - Crie `tests/e2e/invalid/XX_nome_do_teste.lang` com o código inválido.
   - Crie `tests/e2e/invalid/XX_nome_do_teste.expected` contendo a tag correspondente (`ERRO_SEMANTICO`, `ERRO_SINTATICO` ou `ERRO_RUNTIME`).

O script `run_tests.sh` descobre automaticamente os novos arquivos e os inclui na execução.
