#include "globals.h"
#include "../scanner/scanner.h"

/* Imprime um token e seu lexema no arquivo stdout */
void printToken(token_type token, const char* tokenString) {
  switch (token) {
    case T_SE:
    case T_ENTAO:
    case T_SENAO:
    case T_REPITA:
    case T_ATE:
    case T_LER:
    case T_MOSTRAR:
    case T_INTEIRO:
    case T_REAL:
      fprintf(stdout, "reserved word: %s\n",tokenString);
      break;
    case T_ATRIBUICAO: fprintf(stdout,"=\n"); break;
    case T_MENOR: fprintf(stdout,"<\n"); break;
    case T_MAIOR: fprintf(stdout, ">\n"); break;
    case T_IGUAL: fprintf(stdout, "==\n"); break;
    case T_MENOR_IGUAL: fprintf(stdout,"<=\n"); break;
    case T_MAIOR_IGUAL: fprintf(stdout,">=\n"); break;
    case T_E: fprintf(stdout,"&&\n"); break;
    case T_DIFERENTE: fprintf(stdout,"!=\n"); break;
    case T_OU: fprintf(stdout,"||\n"); break;
    case T_ABRE_PARENTESES: fprintf(stdout,"(\n"); break;
    case T_FECHA_PARENTESES: fprintf(stdout,")\n"); break;
    case T_PONTO_VIRGULA: fprintf(stdout,";\n"); break;
    case T_SOMA: fprintf(stdout,"+\n"); break;
    case T_SUB: fprintf(stdout,"-\n"); break;
    case T_MULT: fprintf(stdout,"*\n"); break;
    case T_DIV: fprintf(stdout,"/\n"); break;
    case ENDFILE: fprintf(stdout,"EOF\n"); break;
    case T_NUMERO_INT:
    case T_NUMERO_REAL:
      fprintf(stdout, "NUM, val= %s\n",tokenString);
      break;
    case T_ID:
      fprintf(stdout, "ID, name= %s\n",tokenString);
      break;
    case T_ERRO:
      fprintf(stdout, "ERROR: %s\n",tokenString);
      break;
    default: /* nao deve acontecer nunca */
      fprintf(stdout,"Unknown token: %d\n",token);
  }
}

/* Cria um no de declaracao para a construcao da arvore sintatica */
TreeNode * newStmtNode(StmtKind kind) {
  TreeNode *t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(stdout,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0; i<MAXCHILDREN; i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = StmtK;
    t->kind.stmt = kind;
    t->lineno = lineno;
  }
  return t;
}

/* Cria um no de expressao para a construcao da arvore sintatica */
TreeNode *newExpNode(ExpKind kind) {
  TreeNode *t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(stdout,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0; i<MAXCHILDREN; i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ExpK;
    t->kind.exp = kind;
    t->lineno = lineno;
    t->type = Void;
  }
  return t;
}

/* Aloca espaço e faz copia de uma string existente */
char * copyString(char * s) {
  int n;
  char * t;
  if (s == NULL)
    return NULL;
  n = strlen(s)+1;
  t = malloc(n);
  if (t == NULL)
    fprintf(stdout,"Out of memory error at line %d\n",lineno);
  else
    strcpy(t,s);
  return t;
}

/* A variavel indentno eh usada por printTree para armazenar a quantidade de espacos a indentar */
static int indentno = 0;

/* Macros para incrementar e decrementrar a indentacao */
#define INDENT indentno+=2
#define UNINDENT indentno-=2

/* printSpaces faz a indentacao imprimindo espacos */
static void printSpaces(void) {
  int i;
  for (i=0;i<indentno;i++)
    fprintf(stdout," ");
}

/* A funcao printTree imprime e arvore sintatica para o arquivo 
 * stdout usando indentacao para indicar as sub-arvores
 */
void printTree( TreeNode * tree ) {
  int i;
  INDENT;
  while (tree != NULL) {
    printSpaces();
    if (tree->nodekind==StmtK) {
      switch (tree->kind.stmt) {
        case IfK:
          fprintf(stdout,"If\n");
          break;
        case RepeatK:
          fprintf(stdout,"Repeat\n");
          break;
        case WhileK:
          fprintf(stdout,"While\n");
          break;
        case AssignK:
          fprintf(stdout,"Assign to: %s\n",tree->attr.name);
          break;
        case ReadK:
          fprintf(stdout,"Read: %s\n",tree->attr.name);
          break;
        case WriteK:
          fprintf(stdout,"Write\n");
          break;
        case DeclK:
          fprintf(stdout,"Decl: %s\n",tree->attr.name);
          break;
        default:
          fprintf(stdout,"Unknown ExpNode kind\n");
          break;
      }
    }
    else if (tree->nodekind==ExpK) {
      switch (tree->kind.exp) {
        case OpK:
          fprintf(stdout,"Op: ");
          printToken(tree->attr.op,"\0");
          break;
        case ConstK:
          fprintf(stdout,"Const: %d\n",tree->attr.val);
          break;
        case IdK:
          fprintf(stdout,"Id: %s\n",tree->attr.name);
          break;
        default:
          fprintf(stdout,"Unknown ExpNode kind\n");
          break;
      }
    }
    else fprintf(stdout,"Unknown node kind\n");
    for (i=0;i<MAXCHILDREN;i++)
      printTree(tree->child[i]);
    tree = tree->sibling;
  }
  UNINDENT;
}
