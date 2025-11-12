#include <stdio.h>
#include "scanner/scanner.h"
#include "parser/parser.h"

/// @brief Variável de depuração do Bison. 0 desativa o debug trace, 1 ativa o debug trace
extern int yydebug;

/// @brief O ponto de entrada do programa.
/// @param argc Número de argumentos passados pela linha de comando.
/// @param argv Array de strings contendo os argumentos da linha de comando.
///             O primeiro elemento (argv[0]) normalmente é o nome do programa.
/// @return O código de saída do programa: 0 em caso de sucesso, diferente de 0 em caso de erro.
int main(int argc, char **argv)
{
    yydebug = 0;
    tree_node *syntaxTree = NULL;

    if (argc < 2)
    {
        fprintf(stderr, "Uso: %s <arquivo_de_entrada>\n", argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin)
    {
        fprintf(stderr, "Nao foi possivel abrir o arquivo %s\n", argv[1]);
        return 1;
    }

    printf("Compilando o arquivo: %s\n", argv[1]);
    printf("-------------------------------------\n");

    syntaxTree = parse();

    if (syntaxTree != NULL)
    {
        printf("\nConstrucao da arvore sintatica finalizada.\n");
        printf("-------------------------------------\n");
        print_tree(syntaxTree, 0);
    }
    else
    {
        printf("\nNao foi possivel construir a arvore sintatica devido a erros.\n");
    }

    fclose(yyin);
    return 0;
}