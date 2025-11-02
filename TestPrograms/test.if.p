{
    inteiro a;
    inteiro b;
    inteiro resultado;

    a = 10;
    b = 20;
    resultado = 0;


    /* Teste do Else Pendente */

    se (a == 10) entao
        se (b < 15) entao
            resultado = a + b;  /* Este bloco nao deve ser executado */
        senao
            resultado = a - b;  /* ESTE BLOCO DEVE ser executado */


    /* Deve exibir -10 */
    mostrar(resultado);


    /* Teste do Else Não-Pendente */

    se (a == 10) entao {
        se (b > 15) entao
            resultado = a + b;  /* ESTE BLOCO DEVE ser executado */
    }
    senao
        resultado = a - b;    /* Este bloco nao deve ser executado */


    /* Deve exibir 30 */
    mostrar(resultado);
}