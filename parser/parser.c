#include <stdio.h>
#include <stdlib.h>
#include "../scanner/scanner.h"
#include "parser.h"
#include "../semantic/semantics.h" // NOVO: Módulo do analisador semântico

/// @brief Imprime espaços de acordo com a quantidade especificada.
/// @param amount Quantos espaços devem ser impressos.
static void print_spaces(const int amount);

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
    tree_node *semanticTree = NULL;

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

    printf("--- INICIANDO ANÁLISE SINTÁTICA ---\n");
    syntaxTree = parse();


    printf("\n--- ANÁLISE SINTÁTICA CONCLUÍDA ---\n");
    
    // Verifica se a árvore sintática foi gerada
    if (syntaxTree == NULL)
    {
         printf("A análise sintática não gerou a Árvore Sintática Abstrata (AST).\n");
         return 1;
    }


    printf("\n\n=========================================\n");
    printf("        AST ANTES DA ANÁLISE SEMÂNTICA\n");
    printf("=========================================\n");
    print_tree(syntaxTree, 0);


    // 3. FASE DE ANÁLISE SEMÂNTICA
    printf("\n\n=========================================\n");
    printf("        INICIANDO ANALISE SEMANTICA\n");
    printf("=========================================\n");
    
    // O analisador semântico aplica as fases de Tabela de Símbolos e Verificação/Ajuste de Tipos
    semanticTree = apply_semantic_analysis(syntaxTree);

    if (is_error)
    {
        printf("\n--- ANALISE SEMANTICA CONCLUIDA ---\n");
        printf("Erros sementicos encontrados. O processo de compilacao foi interrompido.\n");
        return 1;
    }
    
    printf("\n--- ANALISE SEMANTICA CONCLUIDA ---\n");


    // 4. IMPRESSÃO DA TABELA DE SÍMBOLOS
    printf("\n\n=========================================\n");
    printf("        TABELA DE SIMBOLOS\n");
    printf("=========================================\n");
    print_symbol_table(); // Função implementada em semantics.c

    
    // 5. IMPRESSÃO da AST DEPOIS do ajuste semântico
    printf("\n\n=========================================\n");
    printf("        AST APOS ANALISE SEMANTICA (AJUSTADA)\n");
    printf("=========================================\n");
    print_tree(semanticTree, 0);


    // Fechamento do arquivo e retorno
    fclose(yyin);
    printf("\n\nCompilacao concluida com sucesso (Analise Semantica OK).\n");
    return 0;
}

void print_token(token_type token, const char *token_string)
{
    switch (token)
    {
    case T_SE:
    case T_ENTAO:
    case T_SENAO:
    case T_REPITA:
    case T_ATE:
    case T_LER:
    case T_MOSTRAR:
    case T_INTEIRO:
    case T_REAL:
        printf("reserved word: %s\n", token_string);
        break;
    case T_ATRIBUICAO:
        printf("=\n");
        break;
    case T_MENOR:
        printf("<\n");
        break;
    case T_MAIOR:
        printf(">\n");
        break;
    case T_IGUAL:
        printf("==\n");
        break;
    case T_MENOR_IGUAL:
        printf("<=\n");
        break;
    case T_MAIOR_IGUAL:
        printf(">=\n");
        break;
    case T_E:
        printf("&&\n");
        break;
    case T_DIFERENTE:
        printf("!=\n");
        break;
    case T_OU:
        printf("||\n");
        break;
    case T_ABRE_PARENTESES:
        printf("(\n");
        break;
    case T_FECHA_PARENTESES:
        printf(")\n");
        break;
    case T_PONTO_VIRGULA:
        printf(";\n");
        break;
    case T_SOMA:
        printf("+\n");
        break;
    case T_SUB:
        printf("-\n");
        break;
    case T_MULT:
        printf("*\n");
        break;
    case T_DIV:
        printf("/\n");
        break;
    case T_EOF:
        printf("EOF\n");
        break;
    case T_NUMERO_INT:
    case T_NUMERO_REAL:
        printf("NUM, val= %s\n", token_string);
        break;
    case T_ID:
        printf("ID, name= %s\n", token_string);
        break;
    case T_ERRO:
        printf("ERROR: %s\n", token_string);
        break;
    default: /* nao deve acontecer nunca */
        printf("Unknown token: %d\n", token);
    }
}

tree_node *new_statement_node(statement_kind kind)
{
    tree_node *t = (tree_node *)malloc(sizeof(tree_node));
    int i;
    if (t == NULL)
        printf("Out of memory error at line %d\n", line_number);
    else
    {
        for (i = 0; i < MAXCHILDREN; i++)
            t->child[i] = NULL;
        t->sibling = NULL;
        t->node_kind = STATEMENT_KIND;
        t->kind.stmt = kind;
        t->line_number = line_number;
    }
    return t;
}

tree_node *new_expression_node(expression_kind kind)
{
    tree_node *t = (tree_node *)malloc(sizeof(tree_node));
    int i;
    if (t == NULL)
        printf("Out of memory error at line %d\n", line_number);
    else
    {
        for (i = 0; i < MAXCHILDREN; i++)
            t->child[i] = NULL;
        t->sibling = NULL;
        t->node_kind = EXPRESSION_KIND;
        t->kind.exp = kind;
        t->line_number = line_number;
        t->type = VOID;
    }
    return t;
}

static void print_spaces(const int amount)
{
    for (int amount = 0; amount < amount; amount++)
        printf(" ");
}

void print_tree(tree_node *tree, const int indentation_level)
{
    while (tree != NULL)
    {
        print_spaces(indentation_level);

        printf("L%d: ", tree->line_number);

        if (tree->node_kind == STATEMENT_KIND)
        {
            switch (tree->kind.stmt)
            {
            case IF_STATEMENT:
                printf("If\n");
                break;
            case REPEAT_STATEMENT:
                printf("Repeat\n");
                break;
            case WHILE_STATEMENT:
                printf("While\n");
                break;
            case ASSIGNMENT_STATEMENT:
                printf("Assign to: %s\n", tree->attribute.name);
                break;
            case READ_STATEMENT:
                printf("Read: %s\n", tree->attribute.name);
                break;
            case WRITE_STATEMENT:
                printf("Write\n");
                break;
            case DECLARATION_STATEMENT:
                printf("Decl: %s\n", tree->attribute.name);
                break;
            default:
                printf("Unknown statement node\n");
                break;
            }
        }
        else if (tree->node_kind == EXPRESSION_KIND)
        {
            switch (tree->kind.exp)
            {
            case OPERATION_EXPRESSION:
                printf("Op: ");
                print_token(tree->attribute.op, "\0");
                break;
            case CONSTANT_EXPRESSION:
                switch (tree->type)
                {
                    case INTEGER:
                        printf("Const: %d\n", tree->attribute.int_value);
                        break;
                    case REAL:
                        printf("Const: %f\n", tree->attribute.real_value);
                        break;
                    default:
                        printf("Const (Unknown Type): %d\n", tree->attribute.int_value);
                        break;
                    }
                break;
            case IDENTIFIER_EXPRESSION:
                printf("Id: %s\n", tree->attribute.name);
                break;
            case INT_TO_REAL_EXP: 
                printf("Expression: COERCAO (INT -> REAL)\n");
                break;
            default:
                printf("Unknown expression node\n");
                break;
            }
        }
        else
        {
            printf("Unknown node\n");
        }

        for (int amount = 0; amount < MAXCHILDREN; amount++)
            print_tree(tree->child[amount], indentation_level + 2);

        tree = tree->sibling;
    }
}