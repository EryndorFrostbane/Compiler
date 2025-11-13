{
    inteiro a, b;

    ler(a);
    ler(b);
    ler(c); /* erro semântico */
    ler(a + b); /* erro sintático */

    /* Válidos */

    mostrar(a + b);
    mostrar(a - b);
    mostrar(a * b);
    mostrar(a / b);

    /* Inválidos */

    mostrar(a < b);
    mostrar(a > b);
    mostrar(a == b);
    mostrar(a >= b);
    mostrar(a <= b);
    mostrar(a != b);
    mostrar(a && b);
    mostrar(a || b);
}