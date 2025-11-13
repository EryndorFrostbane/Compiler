#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>

/// @brief Representa todos os possíveis tipos de tokens da linguagem P-
typedef enum token_type
{
    /* Palavras-chave */
    T_INTEIRO = 258,
    T_REAL,
    T_SE,
    T_ENTAO,
    T_SENAO,
    T_ENQUANTO,
    T_REPITA,
    T_ATE,
    T_LER,
    T_MOSTRAR,
    T_ID,
    
    /* Numeros */
    T_NUMERO_INT,
    T_NUMERO_REAL,
    
    /* Operadores */
    T_E,
    T_OU,
    T_MENOR_IGUAL,
    T_MAIOR_IGUAL,
    T_IGUAL,
    T_DIFERENTE,
    T_MENOR,
    T_MAIOR,
    T_SOMA,
    T_SUB,
    T_MULT,
    T_DIV,
    T_ATRIBUICAO,
    
    /* Simbolos e Separadores */
    T_ABRE_PARENTESES,
    T_FECHA_PARENTESES,
    T_ABRE_CHAVES,
    T_FECHA_CHAVES,
    T_PONTO_VIRGULA,
    T_VIRGULA,
    
    /* Finalizadores */
    T_EOF = 0, // Fim do arquivo P-
    T_ERRO = 1,
} token_type;

/// @brief Armazena as informações completas de um token.
typedef struct token
{
    token_type type;    // O tipo do token.
    char *lexeme;       // O lexema.
    int line;           // A linha onde o lexema foi encontrado.
} token;

/// @brief Variável global do Flex para o arquivo de entrada.
extern FILE *yyin;

/// @brief Função de processamento gerado pelo Flex.
/// @attention O corpo desta função está em "lex.yy.c", que é gerado pelo Flex como definido em "scanner.l".
/// @return O token atual a ser processado.
extern token get_token(void);

/// @brief Imprime as informações de um token de forma organizada.
/// @param token O token a ser impresso.
void print_token(token *token);

/*
 * Constantes internas que o Bison espera que existam.
 * Quando suprimimos a geracao do enum do Bison usando YYTOKENTYPE,
 * precisamos definir estas constantes manualmente.
 */
#ifndef YYEMPTY
#define YYEMPTY -2
#endif
#ifndef YYEOF
#define YYEOF 0
#endif
#ifndef YYerror
#define YYerror 256
#endif
#ifndef YYUNDEF
#define YYUNDEF -1
#endif

#endif // SCANNER_H
