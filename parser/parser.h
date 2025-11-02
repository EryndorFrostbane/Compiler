
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

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define MAXRESERVED 8

extern FILE *source;
extern FILE *listing;

extern int lineno;
extern int Error;

/*************************************************/
/******* Arvore sintatica para o parser  ********/
/*************************************************/

typedef enum node_kind
{
    StmtK,
    ExpK
} node_kind;
typedef enum stmt_kind
{
    IfK,
    RepeatK,
    WhileK,
    AssignK,
    ReadK,
    WriteK,
    DeclK,
} stmt_kind;
typedef enum exp_kind
{
    OpK,
    ConstK,
    IdK
} exp_kind;

typedef enum exp_type
{
    Void,
    Integer,
    Boolean
} exp_type;

#define MAXCHILDREN 3

typedef struct tree_node
{
    struct tree_node *child[MAXCHILDREN];
    struct tree_node *sibling;
    int lineno;
    node_kind nodekind;
    union
    {
        stmt_kind stmt;
        exp_kind exp;
    } kind;
    union
    {
        token_type op;
        int val;
        double real_val;
        char *name;
    } attr;
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
tree_node *new_statement_node(stmt_kind kind);

/* Cria um no de expressao para construcao da arvore sintatica */
tree_node *new_expression_node(exp_kind);

/* Imprime a arvore sintatica usando indentacao para indicar as subarvores  */
void print_tree(tree_node *);

/* A funcao parse retorna a arvore sintatica construida */
tree_node * parse(void);

#endif