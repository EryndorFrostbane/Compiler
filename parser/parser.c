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

    fprintf(stdout, "Compilando o arquivo: %s\n", argv[1]);
    fprintf(stdout, "-------------------------------------\n");

    yydebug = 0; // 0 desativa o debug trace, 1 ativa o debug trace
    syntaxTree = parse();

    if (syntaxTree != NULL)
    {
        fprintf(stdout, "\nConstrucao da arvore sintatica finalizada.\n");
        fprintf(stdout, "-------------------------------------\n");
        print_tree(syntaxTree);
    }
    else
    {
        fprintf(stdout, "\nNao foi possivel construir a arvore sintatica devido a erros.\n");
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
        fprintf(stdout, "reserved word: %s\n", tokenString);
        break;
    case T_ATRIBUICAO:
        fprintf(stdout, "=\n");
        break;
    case T_MENOR:
        fprintf(stdout, "<\n");
        break;
    case T_MAIOR:
        fprintf(stdout, ">\n");
        break;
    case T_IGUAL:
        fprintf(stdout, "==\n");
        break;
    case T_MENOR_IGUAL:
        fprintf(stdout, "<=\n");
        break;
    case T_MAIOR_IGUAL:
        fprintf(stdout, ">=\n");
        break;
    case T_E:
        fprintf(stdout, "&&\n");
        break;
    case T_DIFERENTE:
        fprintf(stdout, "!=\n");
        break;
    case T_OU:
        fprintf(stdout, "||\n");
        break;
    case T_ABRE_PARENTESES:
        fprintf(stdout, "(\n");
        break;
    case T_FECHA_PARENTESES:
        fprintf(stdout, ")\n");
        break;
    case T_PONTO_VIRGULA:
        fprintf(stdout, ";\n");
        break;
    case T_SOMA:
        fprintf(stdout, "+\n");
        break;
    case T_SUB:
        fprintf(stdout, "-\n");
        break;
    case T_MULT:
        fprintf(stdout, "*\n");
        break;
    case T_DIV:
        fprintf(stdout, "/\n");
        break;
    case ENDFILE:
        fprintf(stdout, "EOF\n");
        break;
    case T_NUMERO_INT:
    case T_NUMERO_REAL:
        fprintf(stdout, "NUM, val= %s\n", tokenString);
        break;
    case T_ID:
        fprintf(stdout, "ID, name= %s\n", tokenString);
        break;
    case T_ERRO:
        fprintf(stdout, "ERROR: %s\n", tokenString);
        break;
    default: /* nao deve acontecer nunca */
        fprintf(stdout, "Unknown token: %d\n", token);
    }
}

/* Cria um no de declaracao para a construcao da arvore sintatica */
tree_node *new_statement_node(stmt_kind kind)
{
    tree_node *t = (tree_node *)malloc(sizeof(tree_node));
    int i;
    if (t == NULL)
        fprintf(stdout, "Out of memory error at line %d\n", lineno);
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
        fprintf(stdout, "Out of memory error at line %d\n", lineno);
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

/* A variavel indentno eh usada por print_tree para armazenar a quantidade de espacos a indentar */
static int indentno = 0;

/* Macros para incrementar e decrementrar a indentacao */
#define INDENT indentno += 2
#define UNINDENT indentno -= 2

/* print_spaces faz a indentacao imprimindo espacos */
static void print_spaces(void)
{
    int i;
    for (i = 0; i < indentno; i++)
        fprintf(stdout, " ");
}

/* A funcao print_tree imprime e arvore sintatica para o arquivo
 * stdout usando indentacao para indicar as sub-arvores
 */
void print_tree(tree_node *tree)
{
    int i;
    INDENT;
    while (tree != NULL)
    {
        print_spaces();
        if (tree->nodekind == StmtK)
        {
            switch (tree->kind.stmt)
            {
            case IfK:
                fprintf(stdout, "If\n");
                break;
            case RepeatK:
                fprintf(stdout, "Repeat\n");
                break;
            case WhileK:
                fprintf(stdout, "While\n");
                break;
            case AssignK:
                fprintf(stdout, "Assign to: %s\n", tree->attr.name);
                break;
            case ReadK:
                fprintf(stdout, "Read: %s\n", tree->attr.name);
                break;
            case WriteK:
                fprintf(stdout, "Write\n");
                break;
            case DeclK:
                fprintf(stdout, "Decl: %s\n", tree->attr.name);
                break;
            default:
                fprintf(stdout, "Unknown ExpNode kind\n");
                break;
            }
        }
        else if (tree->nodekind == ExpK)
        {
            switch (tree->kind.exp)
            {
            case OpK:
                fprintf(stdout, "Op: ");
                print_token(tree->attr.op, "\0");
                break;
            case ConstK:
                fprintf(stdout, "Const: %d\n", tree->attr.val);
                break;
            case IdK:
                fprintf(stdout, "Id: %s\n", tree->attr.name);
                break;
            default:
                fprintf(stdout, "Unknown ExpNode kind\n");
                break;
            }
        }
        else
            fprintf(stdout, "Unknown node kind\n");
        for (i = 0; i < MAXCHILDREN; i++)
            print_tree(tree->child[i]);
        tree = tree->sibling;
    }
    UNINDENT;
}