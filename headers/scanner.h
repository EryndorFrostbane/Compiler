#ifndef SCANNER_H
#define SCANNER_H

/// @brief Representa todos os possíveis tipos de tokens da linguagem P-
typedef enum token_type
{
    T_EOF,  // Fim do arquivo P-
    T_ERRO,
    T_ID,

    /* Palavras-chave */
    T_INTEIRO,
    T_REAL,
    T_SE,
    T_ENTAO,
    T_SENAO,
    T_ENQUANTO,
    T_REPITA,
    T_ATE,
    T_LER,
    T_MOSTRAR,

    /* Numeros */
    T_NUMERO_INT,
    T_NUMERO_REAL,

    /* Operadores */
    T_SOMA,
    T_SUB,
    T_MULT,
    T_DIV,
    T_E,
    T_OU,
    T_MENOR,
    T_MENOR_IGUAL,
    T_MAIOR,
    T_MAIOR_IGUAL,
    T_IGUAL,
    T_DIFERENTE,
    T_ATRIBUICAO,

    /* Simbolos e Separadores */
    T_PONTO_VIRGULA,
    T_VIRGULA,
    T_ABRE_PARENTESES,
    T_FECHA_PARENTESES,
    T_ABRE_CHAVES,
    T_FECHA_CHAVES

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

#endif // SCANNER_H
