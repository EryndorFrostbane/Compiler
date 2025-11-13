#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "semantic.h"

static void check_boolean_condition(semantic_analyzer *analyzer, tree_node *condition_node, int line_number, const char *statement_type)
{
    if (condition_node == NULL)
        return;

    data_type cond_type = get_expression_type_without_init_check(analyzer, condition_node);

    if (cond_type != DT_BOOLEAN && cond_type != DT_VOID)
    {
        report_error(analyzer, line_number,
                     "Condicao do %s deve ser booleana, mas encontrou tipo %s",
                     statement_type,
                     (cond_type == DT_INTEGER) ? "inteiro" : (cond_type == DT_REAL) ? "real"
                                                                                    : "desconhecido");
    }
}

static void print_tree_to_file(FILE *file, tree_node *tree, int indentation_level)
{
    while (tree != NULL)
    {
        for (int i = 0; i < indentation_level; i++)
            fprintf(file, " ");
        fprintf(file, "L%d: ", tree->line_number);

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
            case CONVERSION_EXPRESSION:
                fprintf(file, "Conversion: integer to real\n");
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

            // Para operadores booleanos
            if (node->attribute.op == T_E || node->attribute.op == T_OU)
            {
                if (left_type != DT_BOOLEAN || right_type != DT_BOOLEAN)
                {
                    report_error(analyzer, node->line_number,
                                 "Operador logico requer operandos booleanos");
                }
                return DT_BOOLEAN;
            }

            // Para operadores relacionais
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

            // Para operadores aritméticos
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

data_type get_expression_type_without_init_check(semantic_analyzer *analyzer, tree_node *node)
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
                return DT_VOID;
            }
            return sym->type;
        }
        case CONSTANT_EXPRESSION:
            return (node->type == INTEGER) ? DT_INTEGER : DT_REAL;
        case OPERATION_EXPRESSION:
        {
            data_type left_type = get_expression_type_without_init_check(analyzer, node->child[0]);
            data_type right_type = get_expression_type_without_init_check(analyzer, node->child[1]);

            // Para operadores booleanos
            if (node->attribute.op == T_E || node->attribute.op == T_OU)
            {
                return DT_BOOLEAN;
            }

            // Para operadores relacionais
            if (node->attribute.op == T_MENOR || node->attribute.op == T_MAIOR ||
                node->attribute.op == T_IGUAL || node->attribute.op == T_DIFERENTE ||
                node->attribute.op == T_MENOR_IGUAL || node->attribute.op == T_MAIOR_IGUAL)
            {
                return DT_BOOLEAN;
            }

            // Para operadores aritméticos
            if (left_type == DT_REAL || right_type == DT_REAL)
            {
                return DT_REAL;
            }
            return DT_INTEGER;
        }
        case CONVERSION_EXPRESSION:
            return DT_REAL;
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
    sym->is_initialized = 0; // Inicialmente não inicializada

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

tree_node *create_conversion_node(tree_node *expr_node)
{
    tree_node *convert_node = new_expression_node(CONVERSION_EXPRESSION);
    convert_node->child[0] = expr_node;
    convert_node->type = REAL;
    convert_node->line_number = expr_node->line_number;
    return convert_node;
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
            // Criar nó de conversão explícita
            tree_node *convert_node = create_conversion_node(node->child[0]);
            node->child[0] = convert_node;
        }
        else if (sym->type == DT_INTEGER && expr_type == DT_REAL)
        {
            // Não permitir atribuição de real para inteiro
            report_error(analyzer, node->line_number,
                         "Atribuição incompatível: variável '%s' é \"inteiro\", mas expressão é \"real\".",
                         node->attribute.name);
        }
        else
        {
            report_error(analyzer, node->line_number,
                         "Atribuição incompatível: tipos incompatíveis.");
        }
    }

    // Marcar a variável como inicializada após atribuição válida
    // Verificar se não houve erro antes de marcar como inicializada
    int had_error_before = analyzer->error_count;

    // Se não houve erro novo durante esta atribuição, marcar como inicializada
    if (analyzer->error_count == had_error_before)
    {
        sym->is_initialized = 1;
    }

    return node;
}

tree_node *adjust_operation(semantic_analyzer *analyzer, tree_node *node)
{
    // Não chamar get_expression_type aqui - isso causa processamento duplicado
    // A verificação de tipos já foi feita em adjust_expression

    // Apenas ajustar conversões de tipo se necessário
    data_type left_type = get_expression_type_without_init_check(analyzer, node->child[0]);
    data_type right_type = get_expression_type_without_init_check(analyzer, node->child[1]);

    if (left_type == DT_VOID || right_type == DT_VOID)
    {
        return node; // Já reportou erro
    }

    // Para operadores aritméticos, ajustar tipos mistos
    if (node->attribute.op == T_SOMA || node->attribute.op == T_SUB ||
        node->attribute.op == T_MULT || node->attribute.op == T_DIV)
    {

        if (left_type != right_type)
        {
            if (left_type == DT_INTEGER && right_type == DT_REAL)
            {
                // Converter left para real
                tree_node *convert_node = create_conversion_node(node->child[0]);
                node->child[0] = convert_node;
            }
            else if (left_type == DT_REAL && right_type == DT_INTEGER)
            {
                // Converter right para real
                tree_node *convert_node = create_conversion_node(node->child[1]);
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

    // Se este nó já foi processado, retornar imediatamente
    if (node->processed)
        return node;

    if (node->node_kind == EXPRESSION_KIND)
    {
        switch (node->kind.exp)
        {
        case IDENTIFIER_EXPRESSION:
        {
            // Verificar inicialização apenas uma vez aqui
            symbol *sym = find_symbol(analyzer, node->attribute.name);
            if (sym != NULL && !sym->is_initialized)
            {
                // Não verificar inicialização em contextos booleanos (será verificado separadamente)
                // Mas para outros contextos, reportar erro
                report_error(analyzer, node->line_number, "Variavel '%s' nao inicializada", node->attribute.name);
            }
            break;
        }
        case OPERATION_EXPRESSION:
            node = adjust_operation(analyzer, node);
            break;
        }
    }

    // Marcar como processado antes de processar os filhos
    node->processed = 1;

    for (int i = 0; i < MAXCHILDREN; i++)
    {
        if (node->child[i] != NULL)
        {
            adjust_expression(analyzer, node->child[i]);
        }
    }

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

    // Primeiro processar o nó atual
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
            else
            {
                // Marcar a variável como inicializada após leitura
                sym->is_initialized = 1;
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
                             "Condição deve ser booleana");
            }
            break;
        }
        }
    }

    // Depois processar os filhos
    for (int i = 0; i < MAXCHILDREN; i++)
    {
        node->child[i] = adjust_tree(analyzer, node->child[i]);
    }

    // Finalmente processar os irmãos (em ordem sequencial)
    node->sibling = adjust_tree(analyzer, node->sibling);
    return node;
}

void analyze_semantics(semantic_analyzer *analyzer)
{
    // Primeiro processar declarações para construir a tabela de símbolos
    process_declarations(analyzer, analyzer->original_tree);

    // Depois ajustar a árvore com verificações semânticas - usando processamento sequencial
    analyzer->adjusted_tree = adjust_tree_sequential(analyzer, analyzer->original_tree);
}

tree_node *adjust_tree_sequential(semantic_analyzer *analyzer, tree_node *node)
{
    tree_node *current = node;
    while (current != NULL)
    {
        // Processar o nó atual
        if (current->node_kind == STATEMENT_KIND)
        {
            switch (current->kind.stmt)
            {
            case ASSIGNMENT_STATEMENT:
                current = adjust_assignment(analyzer, current);
                break;
            case READ_STATEMENT:
            {
                symbol *sym = find_symbol(analyzer, current->attribute.name);
                if (sym == NULL)
                {
                    report_error(analyzer, current->line_number,
                                 "Variavel '%s' nao declarada", current->attribute.name);
                }
                else if (sym->type != DT_INTEGER && sym->type != DT_REAL)
                {
                    report_error(analyzer, current->line_number,
                                 "Leitura so permitida para variaveis numericas");
                }
                else
                {
                    sym->is_initialized = 1;
                }
                break;
            }
            case WRITE_STATEMENT:
            {
                // A expressão será ajustada pelo loop de filhos abaixo
                // Verificar se a expressão é numérica
                data_type expr_type = get_expression_type_without_init_check(analyzer, current->child[0]);
                if (expr_type != DT_INTEGER && expr_type != DT_REAL && expr_type != DT_VOID)
                {
                    report_error(analyzer, current->line_number,
                                 "Escrita so permitida para expressoes numericas");
                }
                break;
            }
            case IF_STATEMENT:
            {
                // Verificar se a condição é booleana
                check_boolean_condition(analyzer, current->child[0], current->line_number, "se");
                break;
            }
            case WHILE_STATEMENT:
            {
                // Verificar se a condição é booleana
                check_boolean_condition(analyzer, current->child[0], current->line_number, "enquanto");
                break;
            }
            case REPEAT_STATEMENT:
            {
                // Verificar se a condição é booleana
                check_boolean_condition(analyzer, current->child[1], current->line_number, "repita");
                break;
            }
            }
        }

        // Processar os filhos (expressões) do nó atual - APENAS UMA VEZ
        for (int i = 0; i < MAXCHILDREN; i++)
        {
            if (current->child[i] != NULL)
            {
                adjust_expression(analyzer, current->child[i]);
            }
        }

        // Processar o próximo irmão
        current = current->sibling;
    }
    return node;
}

void generate_report(semantic_analyzer *analyzer, const char *filename)
{
    // Imprimir no console
    printf("=== RELATÓRIO DE ANÁLISE SEMÂNTICA ===\n\n");

    printf("1. ÁRVORE SINTÁTICA ORIGINAL:\n");
    printf("----------------------------------------\n");
    print_tree(analyzer->original_tree, 0);

    printf("\n2. ÁRVORE APOS AJUSTES SEMÂNTICOS:\n");
    printf("----------------------------------------\n");
    print_tree(analyzer->adjusted_tree, 0);

    printf("\n3. TABELA DE SÍMBOLOS:\n");
    printf("----------------------------------------\n");
    printf("%-15s %-10s %-10s %-10s %-12s\n", "Nome", "Tipo", "Endereço", "Tamanho", "Inicializada");
    printf("----------------------------------------\n");
    for (int i = 0; i < analyzer->table.count; i++)
    {
        symbol *sym = &analyzer->table.symbols[i];
        printf("%-15s %-10s %-10d %-10d %-12s\n",
               sym->name,
               (sym->type == DT_INTEGER) ? "inteiro" : "real",
               sym->memory_address,
               sym->size,
               sym->is_initialized ? "sim" : "nao");
    }

    printf("\n4. ERROS SEMÂNTICOS:\n");
    printf("----------------------------------------\n");
    if (analyzer->error_count == 0)
    {
        printf("Nenhum erro semântico encontrado.\n");
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
        fprintf(stderr, "Erro ao criar arquivo de relatório: %s\n", filename);
        return;
    }

    fprintf(report, "=== RELATÓRIO DE ANÁLISE SEMÂNTICA ===\n\n");

    fprintf(report, "1. ÁRVORE SINTÁTICA ORIGINAL:\n");
    fprintf(report, "----------------------------------------\n");
    print_tree_to_file(report, analyzer->original_tree, 0);

    fprintf(report, "\n2. ÁRVORE APÓS AJUSTES SEMÂNTICOS:\n");
    fprintf(report, "----------------------------------------\n");
    print_tree_to_file(report, analyzer->adjusted_tree, 0);

    fprintf(report, "\n3. TABELA DE SÍMBOLOS:\n");
    fprintf(report, "----------------------------------------\n");
    fprintf(report, "%-15s %-10s %-10s %-10s\n", "Nome", "Tipo", "Endereço", "Tamanho");
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

    fprintf(report, "\n4. ERROS SEMÂNTICOS:\n");
    fprintf(report, "----------------------------------------\n");
    if (analyzer->error_count == 0)
    {
        fprintf(report, "Nenhum erro semântico encontrado.\n");
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