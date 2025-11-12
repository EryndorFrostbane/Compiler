#include <stdio.h>
#include "parser/parser.h"
#include "semantic/semantic.h"

/// @brief Variável de depuração do Bison. 0 desativa o debug trace, 1 ativa o debug trace
extern int yydebug;

int main(int argc, char **argv)
{
    yydebug = 0;
    
    if (argc < 2)
    {
        fprintf(stderr, "Uso: %s <arquivo_de_entrada>\n", argv[0]);
        return 1;
    }
    
    yyin = fopen(argv[1], "r");
    if (!yyin)
    {
        fprintf(stderr, "Não foi possível abrir o arquivo %s\n", argv[1]);
        return 1;
    }
    
    printf("Compilando o arquivo: %s\n", argv[1]);
    printf("-------------------------------------\n");
    
    tree_node *syntaxTree = parse();

    if (syntaxTree != NULL)
    {
        printf("\nConstrução da árvore sintática finalizada.\n");
        printf("-------------------------------------\n");

        // Análise semântica
        semantic_analyzer *analyzer = create_semantic_analyzer(syntaxTree);
        analyze_semantics(analyzer);

        // Gerar relatório
        char report_filename[256];
        snprintf(report_filename, sizeof(report_filename), "%s_semantic_report.txt", argv[1]);
        generate_report(analyzer, report_filename);

        printf("\n-------------------------------------\n");
        printf("Análise semântica concluída. Relatório salvo em: %s\n", report_filename);
    }
    else
    {
        printf("\nNão foi possível construir a árvore sintática devido a erros.\n");
    }

    fclose(yyin);
    return 0;
}