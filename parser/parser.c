#include <stdio.h>
#include "../scanner/scanner.h"
#include "parser.h"

extern int yydebug;

int main(int argc, char **argv)
{
    TreeNode *syntaxTree;

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

    fprintf(stdout, "Compilando o arquivo: %s\n", argv[1]);
    fprintf(stdout, "-------------------------------------\n");

    yydebug = 0; // 0 desativa o debug trace, 1 ativa o debug trace
    syntaxTree = parse();

    if (syntaxTree != NULL)
    {
        fprintf(stdout, "\nConstrucao da arvore sintatica finalizada.\n");
        fprintf(stdout, "-------------------------------------\n");
        printTree(syntaxTree);
    }
    else
    {
        fprintf(stdout, "\nNao foi possivel construir a arvore sintatica devido a erros.\n");
    }

    fclose(yyin);
    return 0;
}