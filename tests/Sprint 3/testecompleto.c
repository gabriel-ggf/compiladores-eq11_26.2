int main() {
    int x = 10;
    int y = 20;
    float resultado = 30.5;

    // Soma
    x += y;

    if (x >= 30) {
        resultado = resultado + x;
    }

    while (x > 0) {
        x--;
    }

    return x;
}