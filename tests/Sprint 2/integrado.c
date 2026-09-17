/*
   Teste integrado: junta tokens de todas as categorias
   do scanner em um programa completo.
*/

typedef struct Divisao {
    int dividendo;
    int divisor;
    int quociente;
    int resto;
} Divisao;

union Valor {
    int inteiro;
    float real;
};

enum Status { OK, ERRO };

const double EPSILON = 0.000001;
static int contador = 0;
extern int limiteGlobal;
volatile short contadorCurto = 0;

int dividir(int a, int b, Divisao *saida) {
    if (b == 0) {
        return ERRO;
    }

    saida->dividendo = a;
    saida->divisor = b;
    saida->quociente = a / b;
    saida->resto = a % b; // resto da divisao inteira

    contador++;
    return OK;
}

int ehPar(int n) {
    return (n % 2) == 0;
}

int main(void) {
    int numeros[5] = {10, 7, 0, 33, 128};
    unsigned int i = 0;
    signed int delta = -1;
    long total = 0;
    float media = 0.0;
    double mediaExata = 0.0;
    char inicial = 'D';
    char titulo[] = "Relatorio de divisoes";

    for (i = 0; i < 5; i++) {
        Divisao d;
        int status = dividir(numeros[i], 3, &d);

        if (status != OK) {
            continue;
        }

        total += d.quociente;

        /* ajuste conforme o resto da divisao */
        if (ehPar(d.resto) && d.resto <= 2) {
            total -= 1;
        } else if (d.resto == 1 || d.resto >= 2) {
            total += 2;
        }
    }

    media = total / 5.0;
    mediaExata = total / 5.0 + delta % 2;

    while (total > 100) {
        total /= 2;
        total %= 100;
    }

    do {
        total <<= 1;
        contadorCurto++;
    } while (total < 10);

    switch (contador % 3) {
        case 0:
            media = media / 2;
            break;
        case 1:
            media *= 1.5;
            break;
        default:
            media = 0.0;
            break;
    }

    total &= 255;
    total |= 1;
    total ^= 2;
    total >>= 1;
    total = ~total;
    total = (total << 2) >> 1;
    total--;

    if (!(mediaExata > EPSILON)) {
        mediaExata = 0.0;
    }

    if (inicial == 'D' && titulo[0] != 'x') {
        total = total + limiteGlobal;
    }

    return (int) (total / sizeof(int));
}
