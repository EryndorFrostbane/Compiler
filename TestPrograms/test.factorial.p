{
  /*
    Programa para ler um numero do usuario, calcular seu fatorial
    e exibir o resultado.
  */

  inteiro numero;
  inteiro fatorial;
  inteiro i;

  ler(numero);

  fatorial = 1;
  i = 1;

  enquanto (i <= numero) {
    fatorial = fatorial * i;
    i = i + 1;
  }

  mostrar(fatorial);
}
