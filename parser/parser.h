#ifndef _PARSE_H_
#define _PARSE_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "../scanner/scanner.h"

/// @brief Variável global para armazenar a linha atual.
extern int line_number;

/// @brief Variável global para definir se ocorreu um erro.
extern int is_error;

/// @brief Os possíveis tipos de nó da árvore sintática.
typedef enum node_kind
{
    STATEMENT_KIND,
    EXPRESSION_KIND
} node_kind;

/// @brief Os possíveis tipos de declarações.
typedef enum statement_kind
{
    IF_STATEMENT,
    REPEAT_STATEMENT,
    WHILE_STATEMENT,
    ASSIGNMENT_STATEMENT,
    READ_STATEMENT,
    WRITE_STATEMENT,
    DECLARATION_STATEMENT,
} statement_kind;

/// @brief Os possíveis tipos de expressão.
typedef enum expression_kind
{
    OPERATION_EXPRESSION,
    CONSTANT_EXPRESSION,
    IDENTIFIER_EXPRESSION,
    CONVERSION_EXPRESSION
} expression_kind;

/// @brief Os possíveis tipos de variáveis.
typedef enum exp_type
{
    VOID,
    INTEGER,
    REAL,
    PROCESSED_MARKER = 999 // Marcador para nós já processados
} exp_type;

#define MAXCHILDREN 3

/// @brief Um nó da árvore sintática.
typedef struct tree_node
{
    struct tree_node *child[MAXCHILDREN];
    struct tree_node *sibling;
    int line_number;
    node_kind node_kind;
    union kind
    {
        statement_kind stmt;
        expression_kind exp;
    } kind;
    union attribute
    {
        token_type op;
        int int_value;
        double real_value;
        char *name;
    } attribute;
    exp_type type;
    int processed; //c 0 = não processado, 1 = processado
} tree_node;

/// @brief Variável global para armazenar o lexema do token.
extern char *token_string;

/// @brief Imprime um token e seu lexema.
/// @param token_type O tipo do token.
/// @param lexeme O lexema.
void print_node(token_type token_type, const char *lexeme);

/// @brief Cria um nó de declaração para construção da árvore sintática.
/// @param kind O tipo da declaração.
/// @return Um nó da árvore sintática.
tree_node *new_statement_node(statement_kind kind);

/// @brief Cria um nó de expressão para construção da árvore sintática.
/// @param kind O tipo da expressão.
/// @return Um nó da árvore sintática.
tree_node *new_expression_node(expression_kind kind);

/// @brief Imprime a árvore sintática
/// @param tree Um nó da árvore sintática.
/// @param intentation_level O nível de indentação do nó atual.
void print_tree(tree_node *tree, const int indentation_level);

/// @brief Processa um programa P- e retorna sua árvore sintática.
/// @return O nó raíz da árvore sintática.
tree_node * parse(void);

#endif