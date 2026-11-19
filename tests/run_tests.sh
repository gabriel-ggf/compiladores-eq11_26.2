#!/usr/bin/env bash
# =============================================================================
# run_tests.sh — Runner de testes automatizados do interpretador
#
# Uso:
#   ./tests/run_tests.sh [--build] [--verbose]
#
# Opcoes:
#   --build     Rebuilda o projeto com CMake antes de rodar os testes
#   --verbose   Imprime o diff completo quando um teste falha
#
# Tipos de teste:
#   - tests/e2e/valid/   : arquivos .lang com saida esperada em .expected
#                          Passa quando stdout == .expected (sem erros em stderr)
#   - tests/e2e/invalid/ : arquivos .lang com .expected contendo uma das tags:
#                            ERRO_SEMANTICO  — espera "Erro Semantico" no stderr
#                            ERRO_SINTATICO  — espera "[Erro Sintatico]" no stderr
#                            ERRO_RUNTIME    — espera exit code != 0
#   - tests/unit/        : executavel teste_nucleo (AST + interpretador)
# =============================================================================

set -euo pipefail


RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
BOLD='\033[1m'
RESET='\033[0m'

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
INTERP="$ROOT/build/interpretador"
UNIT_BIN="$ROOT/build/teste_nucleo"

BUILD=0
VERBOSE=0
for arg in "$@"; do
    case "$arg" in
        --build)   BUILD=1 ;;
        --verbose) VERBOSE=1 ;;
        *)
            echo "Uso: $0 [--build] [--verbose]"
            exit 1
            ;;
    esac
done


if [ "$BUILD" -eq 1 ]; then
    echo -e "${CYAN}${BOLD}==> Compilando projeto...${RESET}"
    cmake -S "$ROOT" -B "$ROOT/build" -DCMAKE_BUILD_TYPE=Debug > /dev/null 2>&1
    cmake --build "$ROOT/build" 2>&1 | tail -5
    echo ""
fi

if [ ! -x "$INTERP" ]; then
    echo -e "${RED}ERRO: interpretador nao encontrado em $INTERP${RESET}"
    echo -e "Execute com ${BOLD}--build${RESET} ou rode 'cmake --build build' manualmente."
    exit 1
fi


PASS=0
FAIL=0
SKIP=0

TMPOUT="$(mktemp)"
TMPERR="$(mktemp)"
trap 'rm -f "$TMPOUT" "$TMPERR"' EXIT

print_header() {
    echo -e "${BOLD}${CYAN}$1${RESET}"
    echo -e "${CYAN}$(printf '%.0s─' {1..60})${RESET}"
}

pass() {
    local name="$1"
    echo -e "  ${GREEN}✓${RESET} $name"
    PASS=$((PASS + 1))
}

fail() {
    local name="$1"
    local reason="$2"
    echo -e "  ${RED}✗${RESET} $name"
    echo -e "    ${RED}↳ $reason${RESET}"
    FAIL=$((FAIL + 1))
}

skip_test() {
    local name="$1"
    local reason="$2"
    echo -e "  ${YELLOW}~${RESET} $name  ${YELLOW}(ignorado: $reason)${RESET}"
    SKIP=$((SKIP + 1))
}


E2E_VALID_DIR="$SCRIPT_DIR/e2e/valid"
if [ -d "$E2E_VALID_DIR" ]; then
    print_header "Testes e2e — entradas validas"

    for lang_file in "$E2E_VALID_DIR"/*.lang; do
        [ -f "$lang_file" ] || continue
        base="${lang_file%.lang}"
        expected_file="${base}.expected"
        name="$(basename "$lang_file")"

        if [ ! -f "$expected_file" ]; then
            skip_test "$name" "sem .expected"
            continue
        fi

        set +e
        "$INTERP" "$lang_file" > "$TMPOUT" 2> "$TMPERR"
        exit_code=$?
        set -e

        stderr_content="$(cat "$TMPERR")"

        if [ -n "$stderr_content" ]; then
            fail "$name" "stderr nao esperado: $(head -1 "$TMPERR")"
            if [ "$VERBOSE" -eq 1 ]; then
                echo -e "    ${YELLOW}--- stderr ---${RESET}"
                cat "$TMPERR" | sed 's/^/    /'
            fi
            continue
        fi

        if [ "$exit_code" -ne 0 ]; then
            fail "$name" "exit code inesperado: $exit_code"
            continue
        fi

        if diff -q "$TMPOUT" "$expected_file" > /dev/null 2>&1; then
            pass "$name"
        else
            fail "$name" "saida diferente do esperado"
            if [ "$VERBOSE" -eq 1 ]; then
                echo -e "    ${YELLOW}--- diff (esperado vs obtido) ---${RESET}"
                diff "$expected_file" "$TMPOUT" | sed 's/^/    /' || true
            fi
        fi
    done
    echo ""
fi

E2E_INVALID_DIR="$SCRIPT_DIR/e2e/invalid"
if [ -d "$E2E_INVALID_DIR" ]; then
    print_header "Testes e2e — entradas invalidas"

    for lang_file in "$E2E_INVALID_DIR"/*.lang; do
        [ -f "$lang_file" ] || continue
        base="${lang_file%.lang}"
        expected_file="${base}.expected"
        name="$(basename "$lang_file")"

        if [ ! -f "$expected_file" ]; then
            skip_test "$name" "sem .expected"
            continue
        fi

        set +e
        "$INTERP" "$lang_file" > "$TMPOUT" 2> "$TMPERR"
        exit_code=$?
        set -e

        stderr_content="$(cat "$TMPERR")"
        ok=1

        while IFS= read -r tag; do
            tag="$(echo "$tag" | tr -d '[:space:]')"
            [ -z "$tag" ] && continue
            case "$tag" in
                ERRO_SEMANTICO)
                    if ! echo "$stderr_content" | grep -q "Erro Semantico"; then
                        fail "$name" "esperado 'Erro Semantico' no stderr, nao encontrado"
                        ok=0
                        break
                    fi
                    ;;
                ERRO_SINTATICO)
                    if ! echo "$stderr_content" | grep -q "\[Erro Sintatico\]"; then
                        fail "$name" "esperado '[Erro Sintatico]' no stderr, nao encontrado"
                        ok=0
                        break
                    fi
                    ;;
                ERRO_RUNTIME)
                    if [ "$exit_code" -eq 0 ]; then
                        fail "$name" "esperado exit code != 0 (erro de runtime), obtido 0"
                        ok=0
                        break
                    fi
                    ;;
                *)
                    skip_test "$name" "tag desconhecida no .expected: '$tag'"
                    ok=-1
                    break
                    ;;
            esac
        done < "$expected_file"

        if [ "$ok" -eq 1 ]; then
            pass "$name"
        elif [ "$ok" -eq -1 ]; then
            : # skip ja foi impresso
        fi
        # ok=0 → fail ja foi impresso

        if [ "$VERBOSE" -eq 1 ] && [ "$ok" -eq 0 ]; then
            echo -e "    ${YELLOW}--- stderr obtido ---${RESET}"
            cat "$TMPERR" | sed 's/^/    /' || true
        fi
    done
    echo ""
fi

print_header "Teste unitario — nucleo (AST + interpretador)"

if [ ! -x "$UNIT_BIN" ]; then
    skip_test "teste_nucleo" "binario nao encontrado ($UNIT_BIN)"
else
    EXPECTED_UNIT="30
60
15"
    set +e
    actual="$("$UNIT_BIN" 2>"$TMPERR")"
    unit_exit=$?
    set -e

    if [ "$unit_exit" -ne 0 ]; then
        fail "teste_nucleo" "exit code $unit_exit"
        if [ "$VERBOSE" -eq 1 ]; then
            cat "$TMPERR" | sed 's/^/    /' || true
        fi
    elif [ "$actual" = "$EXPECTED_UNIT" ]; then
        pass "teste_nucleo"
    else
        fail "teste_nucleo" "saida diferente do esperado"
        if [ "$VERBOSE" -eq 1 ]; then
            echo -e "    ${YELLOW}esperado:${RESET}"
            echo "$EXPECTED_UNIT" | sed 's/^/      /'
            echo -e "    ${YELLOW}obtido:${RESET}"
            echo "$actual" | sed 's/^/      /'
        fi
    fi
fi
echo ""
TOTAL=$((PASS + FAIL + SKIP))
echo -e "${BOLD}$(printf '%.0s═' {1..60})${RESET}"
echo -e "${BOLD}Resultado: ${GREEN}$PASS passou${RESET}  ${RED}$FAIL falhou${RESET}  ${YELLOW}$SKIP ignorado${RESET}  (total: $TOTAL)"
echo -e "${BOLD}$(printf '%.0s═' {1..60})${RESET}"

if [ "$FAIL" -gt 0 ]; then
    exit 1
fi
exit 0
