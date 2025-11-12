#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "semantic.h"

// Função auxiliar para imprimir árvore em arquivo
static void print_tree_to_file(FILE *file, tree_node *tree, int indentation_level)
{
    while (tree != NULL)
    {
        for (int i = 0; i < indentation_level; i++)
            fprintf(file, " ");

        if (tree->node_kind == STATEMENT_KIND)
        {
            switch (tree->kind.stmt)
            {
            case IF_STATEMENT:
                fprintf(file, "If\n");
                break;
            case REPEAT_STATEMENT:
                fprintf(file, "Repeat\n");
                break;
            case WHILE_STATEMENT:
                fprintf(file, "While\n");
                break;
            case ASSIGNMENT_STATEMENT:
                fprintf(file, "Assign to: %s\n", tree->attribute.name);
                break;
            case READ_STATEMENT:
                fprintf(file, "Read: %s\n", tree->attribute.name);
                break;
            case WRITE_STATEMENT:
                fprintf(file, "Write\n");
                break;
            case DECLARATION_STATEMENT:
                fprintf(file, "Decl: %s\n", tree->attribute.name);
                break;
            default:
                fprintf(file, "Unknown statement node\n");
                break;
            }
        }
        else if (tree->node_kind == EXPRESSION_KIND)
        {
            switch (tree->kind.exp)
            {
            case OPERATION_EXPRESSION:
                fprintf(file, "Op: ");
                switch (tree->attribute.op)
                {
                case T_E:
                    fprintf(file, "&&\n");
                    break;
                case T_OU:
                    fprintf(file, "||\n");
                    break;
                case T_MENOR:
                    fprintf(file, "<\n");
                    break;
                case T_MAIOR:
                    fprintf(file, ">\n");
                    break;
                case T_IGUAL:
                    fprintf(file, "==\n");
                    break;
                case T_DIFERENTE:
                    fprintf(file, "!=\n");
                    break;
                case T_MENOR_IGUAL:
                    fprintf(file, "<=\n");
                    break;
                case T_MAIOR_IGUAL:
                    fprintf(file, ">=\n");
                    break;
                case T_SOMA:
                    fprintf(file, "+\n");
                    break;
                case T_SUB:
                    fprintf(file, "-\n");
                    break;
                case T_MULT:
                    fprintf(file, "*\n");
                    break;
                case T_DIV:
                    fprintf(file, "/\n");
                    break;
                default:
                    fprintf(file, "unknown\n");
                    break;
                }
                break;
            case CONSTANT_EXPRESSION:
                if (tree->type == INTEGER)
                {
                    fprintf(file, "Const: %d\n", tree->attribute.int_value);
                }
                else if (tree->type == REAL)
                {
                    fprintf(file, "Const: %f\n", tree->attribute.real_value);
                }
                else
                {
                    fprintf(file, "Const (Unknown Type)\n");
                }
                break;
            case IDENTIFIER_EXPRESSION:
                fprintf(file, "Id: %s\n", tree->attribute.name);
                break;
            default:
                fprintf(file, "Unknown expression node\n");
                break;
            }
        }
        else
        {
            fprintf(file, "Unknown node\n");
        }

        for (int i = 0; i < MAXCHILDREN; i++)
        {
            print_tree_to_file(file, tree->child[i], indentation_level + 2);
        }

        tree = tree->sibling;
    }
}

semantic_analyzer *create_semantic_analyzer(tree_node *syntax_tree)
{
    semantic_analyzer *analyzer = (semantic_analyzer *)malloc(sizeof(semantic_analyzer));
    analyzer->table.count = 0;
    analyzer->table.next_address = 0;
    analyzer->error_count = 0;
    analyzer->original_tree = syntax_tree;
    analyzer->adjusted_tree = NULL;
    return analyzer;
}

data_type get_expression_type(semantic_analyzer *analyzer, tree_node *node)
{
    if (node == NULL)
        return DT_VOID;

    if (node->node_kind == EXPRESSION_KIND)
    {
        switch (node->kind.exp)
        {
        case IDENTIFIER_EXPRESSION:
        {
            symbol *sym = find_symbol(analyzer, node->attribute.name);
            if (sym == NULL)
            {
                report_error(analyzer, node->line_number, "Variavel '%s' nao declarada", node->attribute.name);
                return DT_VOID;
            }
            return sym->type;
        }
        case CONSTANT_EXPRESSION:
            return (node->type == INTEGER) ? DT_INTEGER : DT_REAL;
        case OPERATION_EXPRESSION:
        {
            data_type left_type = get_expression_type(analyzer, node->child[0]);
            data_type right_type = get_expression_type(analyzer, node->child[1]);

            // Para operadores booleanos, verificar operandos e retornar booleano
            if (node->attribute.op == T_E || node->attribute.op == T_OU)
            {
                if (left_type != DT_BOOLEAN || right_type != DT_BOOLEAN)
                {
                    report_error(analyzer, node->line_number,
                                 "Operador logico requer operandos booleanos");
                }
                return DT_BOOLEAN;
            }

            // Para operadores relacionais, verificar operandos numéricos e retornar booleano
            if (node->attribute.op == T_MENOR || node->attribute.op == T_MAIOR ||
                node->attribute.op == T_IGUAL || node->attribute.op == T_DIFERENTE ||
                node->attribute.op == T_MENOR_IGUAL || node->attribute.op == T_MAIOR_IGUAL)
            {

                if ((left_type != DT_INTEGER && left_type != DT_REAL) ||
                    (right_type != DT_INTEGER && right_type != DT_REAL))
                {
                    report_error(analyzer, node->line_number,
                                 "Operador relacional requer operandos numericos");
                }
                return DT_BOOLEAN;
            }

            // Para operadores aritméticos, determinar o tipo resultante
            if (left_type == DT_REAL || right_type == DT_REAL)
            {
                return DT_REAL;
            }
            return DT_INTEGER;
        }
        }
    }
    return DT_VOID;
}

void add_symbol(semantic_analyzer *analyzer, const char *name, data_type type, int line)
{
    if (find_symbol(analyzer, name) != NULL)
    {
        report_error(analyzer, line, "Variavel '%s' ja declarada", name);
        return;
    }

    if (analyzer->table.count >= MAX_SYMBOLS)
    {
        report_error(analyzer, line, "Tabela de simbolos cheia");
        return;
    }

    symbol *sym = &analyzer->table.symbols[analyzer->table.count++];
    sym->name = strdup(name);
    sym->type = type;
    sym->declared_line = line;

    sym->memory_address = analyzer->table.next_address;
    sym->size = (type == DT_INTEGER) ? 4 : 8;
    analyzer->table.next_address += sym->size;
}

symbol *find_symbol(semantic_analyzer *analyzer, const char *name)
{
    for (int i = 0; i < analyzer->table.count; i++)
    {
        if (strcmp(analyzer->table.symbols[i].name, name) == 0)
        {
            return &analyzer->table.symbols[i];
        }
    }
    return NULL;
}

void report_error(semantic_analyzer *analyzer, int line, const char *format, ...)
{
    if (analyzer->error_count >= MAX_ERRORS)
        return;

    va_list args;
    va_start(args, format);
    semantic_error *error = &analyzer->errors[analyzer->error_count++];
    error->line = line;
    vsnprintf(error->message, sizeof(error->message), format, args);
    va_end(args);
}

tree_node *adjust_assignment(semantic_analyzer *analyzer, tree_node *node)
{
    symbol *sym = find_symbol(analyzer, node->attribute.name);
    if (sym == NULL)
    {
        report_error(analyzer, node->line_number, "Variavel '%s' nao declarada", node->attribute.name);
        return node;
    }

    data_type expr_type = get_expression_type(analyzer, node->child[0]);

    if (expr_type == DT_VOID)
    {
        return node; // Já reportou erro
    }

    // Verificar compatibilidade de tipos
    if (expr_type != sym->type)
    {
        // Permitir atribuição de inteiro para real (conversão implícita)
        if (sym->type == DT_REAL && expr_type == DT_INTEGER)
        {
            // Conversão implícita de inteiro para real
            tree_node *convert_node = new_expression_node(OPERATION_EXPRESSION);
            convert_node->attribute.op = T_ATRIBUICAO; // Marcador de conversão
            convert_node->child[0] = node->child[0];
            convert_node->type = REAL;
            convert_node->line_number = node->line_number;
            node->child[0] = convert_node;
        }
        else if (sym->type == DT_INTEGER && expr_type == DT_REAL)
        {
            // Não permitir atribuição de real para inteiro sem conversão explícita
            report_error(analyzer, node->line_number,
                         "Atribuicao incompativel: variavel '%s' eh inteiro, expressao eh real",
                         node->attribute.name);
        }
        else
        {
            report_error(analyzer, node->line_number,
                         "Atribuicao incompativel: tipos incompativeis");
        }
    }
    return node;
}

tree_node *adjust_operation(semantic_analyzer *analyzer, tree_node *node)
{
    data_type left_type = get_expression_type(analyzer, node->child[0]);
    data_type right_type = get_expression_type(analyzer, node->child[1]);

    if (left_type == DT_VOID || right_type == DT_VOID)
    {
        return node; // Já reportou erro
    }

    // Operadores booleanos já foram verificados em get_expression_type
    // Operadores relacionais já foram verificados em get_expression_type

    // Para operadores aritméticos, ajustar tipos mistos
    if (node->attribute.op == T_SOMA || node->attribute.op == T_SUB ||
        node->attribute.op == T_MULT || node->attribute.op == T_DIV)
    {

        if (left_type != right_type)
        {
            if (left_type == DT_INTEGER && right_type == DT_REAL)
            {
                // Converter left para real
                tree_node *convert_node = new_expression_node(OPERATION_EXPRESSION);
                convert_node->attribute.op = T_ATRIBUICAO;
                convert_node->child[0] = node->child[0];
                convert_node->type = REAL;
                convert_node->line_number = node->line_number;
                node->child[0] = convert_node;
            }
            else if (left_type == DT_REAL && right_type == DT_INTEGER)
            {
                // Converter right para real
                tree_node *convert_node = new_expression_node(OPERATION_EXPRESSION);
                convert_node->attribute.op = T_ATRIBUICAO;
                convert_node->child[0] = node->child[1];
                convert_node->type = REAL;
                convert_node->line_number = node->line_number;
                node->child[1] = convert_node;
            }
        }
    }

    return node;
}

tree_node *adjust_expression(semantic_analyzer *analyzer, tree_node *node)
{
    if (node == NULL)
        return NULL;

    if (node->node_kind == EXPRESSION_KIND &&
        node->kind.exp == OPERATION_EXPRESSION)
    {
        node = adjust_operation(analyzer, node);
    }

    for (int i = 0; i < MAXCHILDREN; i++)
    {
        node->child[i] = adjust_expression(analyzer, node->child[i]);
    }

    node->sibling = adjust_expression(analyzer, node->sibling);
    return node;
}

void process_declarations(semantic_analyzer *analyzer, tree_node *node)
{
    while (node != NULL)
    {
        if (node->node_kind == STATEMENT_KIND &&
            node->kind.stmt == DECLARATION_STATEMENT)
        {
            // Converter exp_type para data_type
            data_type type;
            if (node->type == INTEGER)
            {
                type = DT_INTEGER;
            }
            else if (node->type == REAL)
            {
                type = DT_REAL;
            }
            else
            {
                type = DT_VOID;
            }
            add_symbol(analyzer, node->attribute.name, type, node->line_number);
        }

        for (int i = 0; i < MAXCHILDREN; i++)
        {
            process_declarations(analyzer, node->child[i]);
        }
        node = node->sibling;
    }
}

tree_node *adjust_tree(semantic_analyzer *analyzer, tree_node *node)
{
    if (node == NULL)
        return NULL;

    // Primeiro processar os filhos
    for (int i = 0; i < MAXCHILDREN; i++)
    {
        node->child[i] = adjust_tree(analyzer, node->child[i]);
    }

    // Depois processar o nó atual
    if (node->node_kind == STATEMENT_KIND)
    {
        switch (node->kind.stmt)
        {
        case ASSIGNMENT_STATEMENT:
            node = adjust_assignment(analyzer, node);
            break;
        case READ_STATEMENT:
        {
            symbol *sym = find_symbol(analyzer, node->attribute.name);
            if (sym == NULL)
            {
                report_error(analyzer, node->line_number,
                             "Variavel '%s' nao declarada", node->attribute.name);
            }
            else if (sym->type != DT_INTEGER && sym->type != DT_REAL)
            {
                report_error(analyzer, node->line_number,
                             "Leitura so permitida para variaveis numericas");
            }
            break;
        }
        case WRITE_STATEMENT:
        {
            data_type expr_type = get_expression_type(analyzer, node->child[0]);
            if (expr_type != DT_INTEGER && expr_type != DT_REAL && expr_type != DT_VOID)
            {
                report_error(analyzer, node->line_number,
                             "Escrita so permitida para expressoes numericas");
            }
            break;
        }
        case IF_STATEMENT:
        case WHILE_STATEMENT:
        case REPEAT_STATEMENT:
        {
            data_type cond_type = get_expression_type(analyzer, node->child[0]);
            if (cond_type != DT_BOOLEAN && cond_type != DT_VOID)
            {
                report_error(analyzer, node->line_number,
                             "Condicao deve ser booleana");
            }
            break;
        }
        }
    }

    // Processar irmãos
    node->sibling = adjust_tree(analyzer, node->sibling);
    return node;
}

void analyze_semantics(semantic_analyzer *analyzer)
{
    // Primeiro processar declarações para construir a tabela de símbolos
    process_declarations(analyzer, analyzer->original_tree);

    // Depois ajustar a árvore com verificações semânticas
    analyzer->adjusted_tree = adjust_tree(analyzer, analyzer->original_tree);
}

void generate_report(semantic_analyzer *analyzer, const char *filename)
{
    // Imprimir no console
    printf("=== RELATORIO DE ANALISE SEMANTICA ===\n\n");

    printf("1. ARVORE SINTATICA ORIGINAL:\n");
    printf("----------------------------------------\n");
    print_tree(analyzer->original_tree, 0);

    printf("\n2. ARVORE APOS AJUSTES SEMANTICOS:\n");
    printf("----------------------------------------\n");
    print_tree(analyzer->adjusted_tree, 0);

    printf("\n3. TABELA DE SIMBOLOS:\n");
    printf("----------------------------------------\n");
    printf("%-15s %-10s %-10s %-10s\n", "Nome", "Tipo", "Endereco", "Tamanho");
    printf("----------------------------------------\n");
    for (int i = 0; i < analyzer->table.count; i++)
    {
        symbol *sym = &analyzer->table.symbols[i];
        printf("%-15s %-10s %-10d %-10d\n",
               sym->name,
               (sym->type == DT_INTEGER) ? "inteiro" : "real",
               sym->memory_address,
               sym->size);
    }

    printf("\n4. ERROS SEMANTICOS:\n");
    printf("----------------------------------------\n");
    if (analyzer->error_count == 0)
    {
        printf("Nenhum erro semantico encontrado.\n");
    }
    else
    {
        for (int i = 0; i < analyzer->error_count; i++)
        {
            printf("Linha %d: %s\n",
                   analyzer->errors[i].line,
                   analyzer->errors[i].message);
        }
    }

    // Também salvar em arquivo
    FILE *report = fopen(filename, "w");
    if (!report)
    {
        fprintf(stderr, "Erro ao criar arquivo de relatorio: %s\n", filename);
        return;
    }

    fprintf(report, "=== RELATORIO DE ANALISE SEMANTICA ===\n\n");

    fprintf(report, "1. ARVORE SINTATICA ORIGINAL:\n");
    fprintf(report, "----------------------------------------\n");
    print_tree_to_file(report, analyzer->original_tree, 0);

    fprintf(report, "\n2. ARVORE APOS AJUSTES SEMANTICOS:\n");
    fprintf(report, "----------------------------------------\n");
    print_tree_to_file(report, analyzer->adjusted_tree, 0);

    fprintf(report, "\n3. TABELA DE SIMBOLOS:\n");
    fprintf(report, "----------------------------------------\n");
    fprintf(report, "%-15s %-10s %-10s %-10s\n", "Nome", "Tipo", "Endereco", "Tamanho");
    fprintf(report, "----------------------------------------\n");
    for (int i = 0; i < analyzer->table.count; i++)
    {
        symbol *sym = &analyzer->table.symbols[i];
        fprintf(report, "%-15s %-10s %-10d %-10d\n",
                sym->name,
                (sym->type == DT_INTEGER) ? "inteiro" : "real",
                sym->memory_address,
                sym->size);
    }

    fprintf(report, "\n4. ERROS SEMANTICOS:\n");
    fprintf(report, "----------------------------------------\n");
    if (analyzer->error_count == 0)
    {
        fprintf(report, "Nenhum erro semantico encontrado.\n");
    }
    else
    {
        for (int i = 0; i < analyzer->error_count; i++)
        {
            fprintf(report, "Linha %d: %s\n",
                    analyzer->errors[i].line,
                    analyzer->errors[i].message);
        }
    }

    fclose(report);
}