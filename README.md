# Analisador Léxico

Analisador léxico para a linguagem P- implementado com Flex.

# Instruções

1. Gere o código em C do analisador com o seguinte comando:

```bash
flex scanner.l
```

2. Um arquivo chamado `lex.yy.c` será gerado. Você então deve compilá-lo para gerar o analisador:

```bash
# Talvez seja necessário adicionar a opção -lfl também.
gcc lex.yy.c -o scanner
```

3. Agora você pode executar o analisador em arquivos P-

```bash
./scanner TestPrograms/test.factorial.p
```
