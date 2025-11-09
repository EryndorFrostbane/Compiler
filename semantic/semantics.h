// semantics.h

#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "../parser/parser.h" // Inclui a definição de tree_node e symbol_entry

// Protótipos de Funções da Tabela de Símbolos
void st_insert(char *name, exp_type type, int line);
symbol_entry *st_lookup(char *name);
void print_symbol_table(void);
int get_memory_offset(void);

// Funções do Analisador Semântico
void build_symbol_table(tree_node *tree);
void check_types(tree_node *tree);
tree_node *apply_semantic_analysis(tree_node *syntaxTree);

#endif // SEMANTICS_H