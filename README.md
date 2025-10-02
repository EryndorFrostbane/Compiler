# Analisador Léxico

Analisador léxico para a linguagem P- implementado com Flex.
Desenvolvido e testado no Arch Linux.

# Instruções

1. Gere o código em C do analisador léxico com o seguinte comando:

```bash
flex scanner.l
```

2. Um arquivo chamado `lex.yy.c` será gerado. Você então deve compilá-lo junto com a aplicação para gerar o analisador:

```bash
gcc lex.yy.c main.c -o scanner
```

3. Agora você pode executar o analisador em arquivos P-

```bash
./scanner TestPrograms/test.factorial.p
```
