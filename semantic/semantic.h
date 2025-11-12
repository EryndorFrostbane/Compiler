#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "../parser/parser.h"

#define MAX_SYMBOLS 1000
#define MAX_ERRORS 100

typedef enum data_type
{
    DT_INTEGER,
    DT_REAL,
    DT_BOOLEAN,
    DT_VOID
} data_type;

typedef struct symbol
{
    char *name;
    data_type type;
    int declared_line;
    int memory_address;
    int size;
} symbol;

typedef struct symbol_table
{
    symbol symbols[MAX_SYMBOLS];
    int count;
    int next_address;
} symbol_table;

typedef struct semantic_error
{
    int line;
    char message[256];
} semantic_error;

typedef struct semantic_analyzer
{
    symbol_table table;
    semantic_error errors[MAX_ERRORS];
    int error_count;
    tree_node *original_tree;
    tree_node *adjusted_tree;
} semantic_analyzer;

// Funções principais
semantic_analyzer *create_semantic_analyzer(tree_node *syntax_tree);
void analyze_semantics(semantic_analyzer *analyzer);
void generate_report(semantic_analyzer *analyzer, const char *filename);

// Funções auxiliares
data_type get_expression_type(semantic_analyzer *analyzer, tree_node *node);
void add_symbol(semantic_analyzer *analyzer, const char *name, data_type type, int line);
symbol *find_symbol(semantic_analyzer *analyzer, const char *name);
void report_error(semantic_analyzer *analyzer, int line, const char *format, ...);

// Funções de ajuste da árvore
tree_node *create_conversion_node(tree_node *expr_node);
tree_node *adjust_assignment(semantic_analyzer *analyzer, tree_node *node);
tree_node *adjust_operation(semantic_analyzer *analyzer, tree_node *node);
tree_node *adjust_expression(semantic_analyzer *analyzer, tree_node *node);
tree_node *adjust_tree(semantic_analyzer *analyzer, tree_node *node);

#endif