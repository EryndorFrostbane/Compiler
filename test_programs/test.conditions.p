{
    inteiro a, b;
    a = 10;
    b = 5;


    /* Condições válidas */

    se (a > b) entao
    {
        mostrar(a);
    }

    enquanto (a == b)
    {
        mostrar(b);
    }

    repita
    {
        mostrar(a + b);
    } ate (a < b);


    /* Condições inválidas */

    se (a) entao        /* erro semântico */
    {
        mostrar(a);
    }

    enquanto (a = b)    /* erro semântico */
    {
        mostrar(b);
    }

    repita
    {
        mostrar(a + b);
    } ate (b);          /* erro semântico */
}