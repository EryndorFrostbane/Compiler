# Compilador de P-

Compilador para a linguagem P- implementado com Flex e Bison.
Desenvolvido e testado no Arch Linux.

# Instruções

## Compilação do Analisador Léxico

1. Gere o código em C do analisador léxico com o seguinte comando:

```bash
flex scanner.l
```

2. Um arquivo chamado `lex.yy.c` será gerado. Você então deve compilá-lo junto com a aplicação para gerar o analisador:

```bash
gcc lex.yy.c scanner.c -o scanner
```

3. Agora você pode executar o analisador em arquivos P-

```bash
./scanner TestPrograms/test.factorial.p
```


## Compilação do Analisador Sintático

1. Gere o código em C do analisador léxico com o seguinte comando:

```bash
flex scanner.l
```

2. Gere o código em C do analisador sintático com o seguinte comando:

```bash
bison -d parser.y
```

3. Os aquivos `lex.yy.c` e `parser.tab.c` serão gerados. Você então deve compilá-los juntos com a aplicação para gerar o analisador:

```bash
gcc lex.yy.c parser.tab.c util.c parser.c -o parser
```

4. Agora você pode executar o analisador em arquivos P-

```bash
./parser TestPrograms/test.factorial.p
```