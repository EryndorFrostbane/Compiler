#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "scanner.h"

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

typedef enum NodeKind
{
   StmtK,
   ExpK
} NodeKind;
typedef enum StmtKind
{
   IfK,
   RepeatK,
   WhileK,
   AssignK,
   ReadK,
   WriteK
} StmtKind;
typedef enum ExpKind
{
   OpK,
   ConstK,
   IdK
} ExpKind;

typedef enum ExpType
{
   Void,
   Integer,
   Boolean
} ExpType;

#define MAXCHILDREN 3

typedef struct TreeNode
{
   struct TreeNode *child[MAXCHILDREN];
   struct TreeNode *sibling;
   int lineno;
   NodeKind nodekind;
   union
   {
      StmtKind stmt;
      ExpKind exp;
   } kind;
   union
   {
      token_type op;
      int val;
      double real_val;
      char *name;
   } attr;
   ExpType type;
} TreeNode;

/*****************************************************/
/*********** Flags para rastreamento    ************/
/*****************************************************/

extern int EchoSource;
extern int TraceScan;
extern int TraceParse;

/* Variavel global para armazenar o lexema do token */
extern char *tokenString;

/* Imprime o token e seu lexema */
void printToken(token_type token_type, const char *lexeme);

/* Cria um no de declaracao para construcao da arvore sintatica */
TreeNode *newStmtNode(StmtKind kind);

/* Cria um no de expressao para construcao da arvore sintatica */
TreeNode *newExpNode(ExpKind);

/* Aloca espaco e faz copia de uma string */
char *copyString(char *);

/* Imprime a arvore sintatica usando indentacao para indicar as subarvores  */
void printTree(TreeNode *);

#endif