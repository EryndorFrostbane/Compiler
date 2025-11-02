#ifndef _PARSE_H_
#define _PARSE_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "../scanner/scanner.h"

#ifndef YYPARSER
#define ENDFILE 0
#endif

#define MAXRESERVED 8

extern FILE *source;
extern FILE *listing;

extern int line_number;
extern int is_error;

/*************************************************/
/******* Arvore sintatica para o parser  ********/
/*************************************************/

typedef enum node_kind
{
    STATEMENT_KIND,
    EXPRESSION_KIND
} node_kind;
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
typedef enum expression_kind
{
    OPERATION_EXPRESSION,
    CONSTANT_EXPRESSION,
    IDENTIFIER_EXPRESSION
} expression_kind;

typedef enum exp_type
{
    VOID,
    INTEGER,
    REAL
} exp_type;

#define MAXCHILDREN 3

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
} tree_node;

/*****************************************************/
/*********** Flags para rastreamento    ************/
/*****************************************************/

extern int EchoSource;
extern int TraceScan;
extern int TraceParse;

/* Variavel global para armazenar o lexema do token */
extern char *tokenString;

/* Imprime o token e seu lexema */
void print_token(token_type token_type, const char *lexeme);

/* Cria um no de declaracao para construcao da arvore sintatica */
tree_node *new_statement_node(statement_kind kind);

/* Cria um no de expressao para construcao da arvore sintatica */
tree_node *new_expression_node(expression_kind);

/* Imprime a arvore sintatica usando indentacao para indicar as subarvores  */
void print_tree(tree_node *, const int);

/* A funcao parse retorna a arvore sintatica construida */
tree_node * parse(void);

#endif