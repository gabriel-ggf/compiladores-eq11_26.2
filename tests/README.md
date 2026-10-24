# Testes

Ainda não há testes automatizados. Ideia inicial para quando a linguagem
tiver mais recursos:

- Um programa de exemplo (`.lang`) por caso de teste, com a saída
  esperada em um arquivo `.expected` ao lado.
- Um script (`run_tests.sh` ou similar) que roda o interpretador em cada
  exemplo e compara a saída com o `.expected`, reportando falhas.
- Futuramente, migrar para um framework como
  [Catch2](https://github.com/catchorg/Catch2) ou
  [GoogleTest](https://github.com/google/googletest) para testar
  unidades internas (lexer, parser, interpreter) isoladamente.

## Arquivos de teste manual (`tests/test_files/`)

- `valid/`:
  - `expr1.c`: expressões aritméticas e lógicas.
  - `var_dec1.c`: declaração de variáveis.
  - `stmt1.c`: comandos, blocos `{ }`, condicionais (`if`, `if-else`), laços (`while`, `for`) e comandos de salto (`return`, `break`, `continue`).
- `invalid/`:
  - `expr2.c`: erros em expressões e divisões por zero.
  - `var_dec2.c`: erros em declarações.
  - `stmt2.c`: erros de sintaxe em comandos e controle de fluxo.

Como executar:
```bash
./parser tests/test_files/valid/stmt1.c
./parser tests/test_files/invalid/stmt2.c
```

Atualizem este arquivo conforme a estratégia de testes for definida.
