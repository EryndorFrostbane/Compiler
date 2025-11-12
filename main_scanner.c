#include <stdio.h>   // printf(), fprintf(), fopen(), fclose()
#include <stdlib.h>  // free()
#include "scanner/scanner.h" // token_type, token, get_token()

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