#include <stdio.h>
#include "parser/parser.h"
#include "semantic/semantic.h"

extern int yydebug;

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

        // Análise semântica
        semantic_analyzer *analyzer = create_semantic_analyzer(syntaxTree);
        analyze_semantics(analyzer);

        // Gerar relatório
        char report_filename[256];
        snprintf(report_filename, sizeof(report_filename), "%s_semantic_report.txt", argv[1]);
        generate_report(analyzer, report_filename);

        printf("\n-------------------------------------\n");
        printf("Analise semantica concluida. Relatorio salvo em: %s\n", report_filename);
    }
    else
    {
        printf("\nNao foi possivel construir a arvore sintatica devido a erros.\n");
    }

    fclose(yyin);
    return 0;
}