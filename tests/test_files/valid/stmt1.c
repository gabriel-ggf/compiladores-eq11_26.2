// Bloco simples e blocos aninhados
{}
{
    int a = 10;
    a = a + 5;
    {
        int b = 20;
        b = b * 2;
    }
}

// Condicionais: if e if-else
if (1) x = 1;

if (x > 0) {
    x = x + 1;
}

if (x >= 10) {
    x = 10;
} else {
    x = 0;
}

if (x == 1) {
    y = 10;
} else if (x == 2) {
    y = 20;
} else {
    y = 30;
}

// Loops: while
while (x > 0) x--;

while (x < 10) {
    x = x + 1;
}

// Loops: for
for (i = 0; i < 10; i++) {
    y = y + 1;
}

for (int j = 0; j < 5; j++) {
    y = y + j;
}

for (; x < 20; ) {
    x = x + 2;
}

// Comandos de salto e retorno
for (;;) {
    if (x > 100) {
        break;
    } else {
        continue;
    }
}

while (1) {
    break;
}

return;
return 0;
return (x + y);
