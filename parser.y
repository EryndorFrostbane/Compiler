
%{
#define YYPARSER /* Distingue a saida do Yacc de outros arquivos de codigo */

#include "globals.h"
#include "util.h"
#include "parse.h"

#define YYSTYPE TreeNode *
static char * savedName; /* Para usar nas atribuicoes (assignments) */
static int savedLineNo;  /* Temporario para salvar o numero da linha */
static TreeNode * savedTree; /* Armazena a arvore sintatica para retornar posteriormente */

static int yylex(void); /* Remover aviso de declaracao implicita da funcao */
int yyerror(char *); /* Remover aviso de declaracao implicita da funcao */

%}

%token T_INTEIRO T_REAL
%token T_SE T_ENTAO T_SENAO
%token T_ENQUANTO T_REPITA T_ATE
%token T_LER T_MOSTRAR
%token T_ID T_NUMERO_INT T_NUMERO_REAL

%token T_E T_OU
%token T_MENOR_IGUAL T_MAIOR_IGUAL T_IGUAL T_DIFERENTE T_MENOR T_MAIOR
%token T_SOMA T_SUB T_MULT T_DIV T_ATRIBUICAO
%token T_ABRE_PARENTESES T_FECHA_PARENTESES T_ABRE_CHAVES T_FECHA_CHAVES
%token T_PONTO_VIRGULA T_VIRGULA
%token T_EOF T_ERRO


%nonassoc T_SENAO 

%% /* Gramatica da linguagem P */

program     : decl_list stmt_seq T_EOF
                 { 
                   savedTree = $2; 
                 } 
            ;

decl_list   : decl_list decl { $$ = $1; }
            | /* vazio */ { $$ = NULL; } 
            ;

decl        : T_INTEIRO id_list T_PONTO_VIRGULA { $$ = NULL; }
            | T_REAL id_list T_PONTO_VIRGULA { $$ = NULL; }
            ;

id_list     : T_ID { $$ = NULL; }
            | id_list T_VIRGULA T_ID { $$ = NULL; }
            ;

stmt_seq    : stmt_seq T_PONTO_VIRGULA stmt
                 { YYSTYPE t = $1;
                   if (t != NULL)
                   { while (t->sibling != NULL)
                        t = t->sibling;
                     t->sibling = $3;
                     $$ = $1; 
                   }
                   else $$ = $3;
                 }
            | stmt  { $$ = $1; }
            ;

stmt        : if_stmt { $$ = $1; }
            | repeat_stmt { $$ = $1; }
            | while_stmt { $$ = $1; }
            | assign_stmt { $$ = $1; }
            | read_stmt { $$ = $1; }
            | write_stmt { $$ = $1; }
            | block_stmt { $$ = $1; }
            | error  { $$ = NULL; }
            ;

block_stmt  : T_ABRE_CHAVES stmt_seq T_FECHA_CHAVES 
		 { 
		   $$ = $2; 
		 }
	    ;

if_stmt     : T_SE exp T_ENTAO command
                 { 
                   $$ = newStmtNode(IfK);
                   $$->child[0] = $2;
                   $$->child[1] = $4;
                 }
            |
            T_SE exp T_ENTAO command T_SENAO command
                 { 
                   $$ = newStmtNode(IfK);
                   $$->child[0] = $2;
                   $$->child[1] = $4;
                   $$->child[2] = $6;
                 }
            ;

repeat_stmt : T_REPITA command T_ATE exp
                 { $$ = newStmtNode(RepeatK);
                   $$->child[0] = $2; /* command */
                   $$->child[1] = $4; /* exp */
                 }
            ;

while_stmt  : T_ENQUANTO T_ABRE_PARENTESES exp T_FECHA_PARENTESES command
                 { 
                   $$ = newStmtNode(WhileK); 
                   $$->child[0] = $3; 
                   $$->child[1] = $5; 
                 }
            ;

command     : stmt { $$ = $1; }
	    ;

assign_stmt : T_ID { savedName = copyString(tokenString);
                     savedLineNo = lineno;
                   }
              T_ATRIBUICAO exp
                 { $$ = newStmtNode(AssignK);
                   $$->child[0] = $4;
                   $$->attr.name = savedName;
                   $$->lineno = savedLineNo;
                 }
            ;

read_stmt   : T_LER T_ABRE_PARENTESES T_ID T_FECHA_PARENTESES
                 { $$ = newStmtNode(ReadK);
                   $$->attr.name =
                     copyString(tokenString);
                 }
            ;

write_stmt  : T_MOSTRAR T_ABRE_PARENTESES exp T_FECHA_PARENTESES
                 { $$ = newStmtNode(WriteK);
                   $$->child[0] = $3;
                 }
            ;

exp         : exp T_OU log_and_exp
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = T_OU;
                 }
            | log_and_exp { $$ = $1; }
            ;

log_and_exp : log_and_exp T_E rel_exp
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = T_E;
                 }
            | rel_exp { $$ = $1; }
            ;

rel_exp     : arith_exp T_MENOR arith_exp 
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = T_MENOR;
                 }
            | arith_exp T_MENOR_IGUAL arith_exp
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = T_MENOR_IGUAL;
                 }
            | arith_exp T_MAIOR arith_exp
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = T_MAIOR;
                 }
            | arith_exp T_MAIOR_IGUAL arith_exp
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = T_MAIOR_IGUAL;
                 }
            | arith_exp T_IGUAL arith_exp
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = T_IGUAL;
                 }
            | arith_exp T_DIFERENTE arith_exp
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = T_DIFERENTE;
                 }
            | arith_exp { $$ = $1; }
            ;

arith_exp   : arith_exp T_SOMA term 
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = T_SOMA;
                 }
            | arith_exp T_SUB term
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = T_SUB;
                 } 
            | term { $$ = $1; }
            ;

term        : term T_MULT factor 
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = T_MULT;
                 }
            | term T_DIV factor
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = T_DIV;
                 }
            | factor { $$ = $1; }
            ;

factor      : T_ABRE_PARENTESES exp T_FECHA_PARENTESES
                 { $$ = $2; }
            | T_NUMERO_INT
                 { $$ = newExpNode(ConstK);
                   $$->attr.val = atoi(tokenString);
                 }
            | T_NUMERO_REAL /* Incluído T_NUMERO_REAL, assumindo que val ou um campo real é usado */
                 { $$ = newExpNode(ConstK);
                   $$->attr.real_val = atof(tokenString);
                 }
            | T_ID 
                 { $$ = newExpNode(IdK);
                   $$->attr.name =
                         copyString(tokenString);
                 }
            | T_ERRO { $$ = NULL; }
            ;

%%

int yyerror(char * message)
{ fprintf(listing,"Syntax error at line %d: %s\n",lineno,message);
  fprintf(listing,"Current token: ");
  printToken(yychar,tokenString);
  Error = TRUE;
  return 0;
}

static int yylex(void)
{ return getToken(); }

TreeNode * parse(void)
{ yyparse();
  return savedTree;
}

