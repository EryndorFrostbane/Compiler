/*
  Programa para ler um numero do usuario, calcular seu fatorial
  e exibir o resultado.
*/
{
  inteiro numero;
  inteiro fatorial;
  inteiro acumulador;

  ler(numero);

  fatorial = 1;
  acumulador = 1;

  enquanto (acumulador <= numero) {
    fatorial = fatorial * acumulador;
    acumulador = acumulador + 1;
  }

  mostrar(fatorial);
}