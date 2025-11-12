#include <stdio.h>
#include "../scanner/scanner.h"
#include "parser.h"

/// @brief Imprime espaços de acordo com a quantidade especificada.
/// @param argc Quantos espaços devem ser impressos.
static void print_spaces(const int amount);

void print_node(token_type token, const char *token_string)
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
    for (int count = 0; count < amount; count++)
        printf(" ");
}

void print_tree(tree_node *tree, const int indentation_level)
{
    while (tree != NULL)
    {
        print_spaces(indentation_level);

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
                print_node(tree->attribute.op, "\0");
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