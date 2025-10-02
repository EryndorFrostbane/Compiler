#include <stdio.h>      // printf(), fprintf(), fopen(), fclose()
#include <stdlib.h>     // free()
#include "scanner.h" 

/// @brief Imprime as informações de um token de forma organizada.
/// @param token O token a ser impresso.
void print_token(token *token);

/// @brief O ponto de entrada do programa.
/// @param argc Número de argumentos passados pela linha de comando.
/// @param argv Array de strings contendo os argumentos da linha de comando.
///             O primeiro elemento (argv[0]) normalmente é o nome do programa.
/// @return O código de saída do programa: 0 em caso de sucesso, diferente de 0 em caso de erro.
int main(int argc, char **argv)
{
    // Verifica se um arquivo foi fornecido na linha de comando
    if (argc < 2)
    {
        fprintf(stderr, "Uso: %s <arquivo_de_entrada>\n", argv[0]);
        return 1;
    }

    // Abre o arquivo P-
    yyin = fopen(argv[1], "r"); // yyin é uma variável global definida pelo Flex.
    if (!yyin)
    {
        fprintf(stderr, "Nao foi possivel abrir o arquivo %s\n", argv[1]);
        return 1;
    }

    token current_token;

    // Colete tokens até encontrar o fim do arquivo
    do
    {
        current_token = get_token();
        print_token(&current_token);

        // Libere a memória alocada por strdup() dentro do Flex
        free(current_token.lexeme);
    } while (current_token.type != T_EOF);

    // Fecha o arquivo P-
    fclose(yyin);

    return 0;
}

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