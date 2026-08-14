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

Atualizem este arquivo conforme a estratégia de testes for definida.
