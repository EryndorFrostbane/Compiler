#include <stdio.h>
#include "../scanner/scanner.h"
#include "parser.h"

extern int yydebug;

int main(int argc, char **argv)
{
    tree_node *syntaxTree;

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

    yydebug = 0; // 0 desativa o debug trace, 1 ativa o debug trace
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

/* Imprime um token e seu lexema no arquivo stdout */
void print_token(token_type token, const char *tokenString)
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
        printf("reserved word: %s\n", tokenString);
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
    case ENDFILE:
        printf("EOF\n");
        break;
    case T_NUMERO_INT:
    case T_NUMERO_REAL:
        printf("NUM, val= %s\n", tokenString);
        break;
    case T_ID:
        printf("ID, name= %s\n", tokenString);
        break;
    case T_ERRO:
        printf("ERROR: %s\n", tokenString);
        break;
    default: /* nao deve acontecer nunca */
        printf("Unknown token: %d\n", token);
    }
}

/* Cria um no de declaracao para a construcao da arvore sintatica */
tree_node *new_statement_node(stmt_kind kind)
{
    tree_node *t = (tree_node *)malloc(sizeof(tree_node));
    int i;
    if (t == NULL)
        printf("Out of memory error at line %d\n", lineno);
    else
    {
        for (i = 0; i < MAXCHILDREN; i++)
            t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = StmtK;
        t->kind.stmt = kind;
        t->lineno = lineno;
    }
    return t;
}

/* Cria um no de expressao para a construcao da arvore sintatica */
tree_node *new_expression_node(exp_kind kind)
{
    tree_node *t = (tree_node *)malloc(sizeof(tree_node));
    int i;
    if (t == NULL)
        printf("Out of memory error at line %d\n", lineno);
    else
    {
        for (i = 0; i < MAXCHILDREN; i++)
            t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = ExpK;
        t->kind.exp = kind;
        t->lineno = lineno;
        t->type = Void;
    }
    return t;
}

/* print_spaces faz a indentacao imprimindo espacos */
static void print_spaces(const int indentation_level)
{
    for (int amount = 0; amount < indentation_level; amount++)
        printf(" ");
}

/* A funcao print_tree imprime e arvore sintatica para o arquivo
 * stdout usando indentacao para indicar as sub-arvores
 */
void print_tree(tree_node *tree, const int indentation_level)
{
    while (tree != NULL)
    {
        print_spaces(indentation_level);

        if (tree->nodekind == StmtK)
        {
            switch (tree->kind.stmt)
            {
            case IfK:
                printf("If\n");
                break;
            case RepeatK:
                printf("Repeat\n");
                break;
            case WhileK:
                printf("While\n");
                break;
            case AssignK:
                printf("Assign to: %s\n", tree->attr.name);
                break;
            case ReadK:
                printf("Read: %s\n", tree->attr.name);
                break;
            case WriteK:
                printf("Write\n");
                break;
            case DeclK:
                printf("Decl: %s\n", tree->attr.name);
                break;
            default:
                printf("Unknown statement node\n");
                break;
            }
        }
        else if (tree->nodekind == ExpK)
        {
            switch (tree->kind.exp)
            {
            case OpK:
                printf("Op: ");
                print_token(tree->attr.op, "\0");
                break;
            case ConstK:
                printf("Const: %d\n", tree->attr.val);
                break;
            case IdK:
                printf("Id: %s\n", tree->attr.name);
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