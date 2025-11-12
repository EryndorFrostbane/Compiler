#include <stdio.h>           // printf(), fprintf(), fopen(), fclose()
#include <stdlib.h>          // free()
#include "scanner.h" // token_type, token, get_token()

void print_token(token *token)
{
    printf("Linha %d: ", token->line);

    switch (token->type)
    {
    case T_ID:
        printf("IDENTIFICADOR");
        break;
    case T_INTEIRO:
    case T_REAL:
    case T_SE:
    case T_ENTAO:
    case T_SENAO:
    case T_ENQUANTO:
    case T_REPITA:
    case T_ATE:
    case T_LER:
    case T_MOSTRAR:
        printf("PALAVRA_CHAVE");
        break;
    case T_NUMERO_INT:
        printf("NUMERO_INTEIRO");
        break;
    case T_NUMERO_REAL:
        printf("NUMERO_REAL");
        break;
    case T_SOMA:
        printf("OP_SOMA");
        break;
    case T_SUB:
        printf("OP_SUB");
        break;
    case T_MULT:
        printf("OP_MULT");
        break;
    case T_DIV:
        printf("OP_DIV");
        break;
    case T_E:
        printf("OP_E");
        break;
    case T_OU:
        printf("OP_OU");
        break;
    case T_MENOR:
        printf("OP_MENOR");
        break;
    case T_MENOR_IGUAL:
        printf("OP_MENOR_IGUAL");
        break;
    case T_MAIOR:
        printf("OP_MAIOR");
        break;
    case T_MAIOR_IGUAL:
        printf("OP_MAIOR_IGUAL");
        break;
    case T_IGUAL:
        printf("OP_IGUAL");
        break;
    case T_DIFERENTE:
        printf("OP_DIFERENTE");
        break;
    case T_ATRIBUICAO:
        printf("OP_ATRIBUICAO");
        break;
    case T_PONTO_VIRGULA:
        printf("PONTO_VIRGULA");
        break;
    case T_VIRGULA:
        printf("VIRGULA");
        break;
    case T_ABRE_PARENTESES:
        printf("ABRE_PARENTESES");
        break;
    case T_FECHA_PARENTESES:
        printf("FECHA_PARENTESES");
        break;
    case T_ABRE_CHAVES:
        printf("ABRE_CHAVES");
        break;
    case T_FECHA_CHAVES:
        printf("FECHA_CHAVES");
        break;
    case T_EOF:
        printf("FIM_DE_ARQUIVO");
        break;
    case T_ERRO:
        printf("ERRO_LEXICO");
        break;
    default:
        printf("TOKEN_DESCONHECIDO");
        break;
    }

    // Imprima o lexema, a menos que seja o fim do arquivo
    if (token->type != T_EOF)
        printf(" [%s]\n", token->lexeme);
    else
        printf("\n");
}