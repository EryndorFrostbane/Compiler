%{
#define YYPARSER /* Distingue a saida do Yacc de outros arquivos de codigo */

#include "headers/globals.h"
#include "headers/parser.h"

#define YYSTYPE TreeNode *
#define YYDEBUG 1

/* Variaveis globais usadas pelo parser */
static char * savedName;
static int savedLineNo;
static TreeNode * savedTree;

/* Definicao da variavel global para o lexema do token */
char * tokenString;
int lineno;
int Error;

/* Prototipos */
static int yylex(void);
static int yyerror(char *);

%}

%code requires {
  #include "headers/scanner.h" // Inclui os tokens usados no analisador léxico
  #include "headers/globals.h" // Inclui funções para construção da árvore sintática

  /* Redefine YYTOKENTYPE para usar token_type. Isso suprime a geracao do enum padrao do Bison. */
  #define YYTOKENTYPE token_type
}

/* --- Definicoes de Tokens (sem alteracoes) --- */
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
%token T_ERRO

%nonassoc "then"
%nonassoc T_SENAO 

%% /* --- Gramatica (sem alteracoes) --- */

program     : decl_list optional_stmt_seq
                 {
                   savedTree = $2;
                 }
            ;

optional_stmt_seq : stmt_seq { $$ = $1; }
                  | /* empty */ { $$ = NULL; }
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

stmt_seq    : stmt_seq stmt
                 { YYSTYPE t = $1;
                   if (t != NULL)
                   { while (t->sibling != NULL)
                        t = t->sibling;
                     t->sibling = $2;
                     $$ = $1;
                   }
                   else $$ = $2;
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
            | /* empty */ { $$ = NULL; }
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
            | T_SE exp T_ENTAO command T_SENAO command
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
              T_ATRIBUICAO exp T_PONTO_VIRGULA /* << ADD SEMICOLON */
                 { $$ = newStmtNode(AssignK);
                   if ($$)
                   {
                       $$->child[0] = $4;
                       $$->attr.name = savedName;
                       $$->lineno = savedLineNo;
                   }
                 }
            ;

read_stmt   : T_LER T_ABRE_PARENTESES T_ID { savedName = copyString(tokenString);
                                                                savedLineNo = lineno;
                                                              }
                                                              T_FECHA_PARENTESES T_PONTO_VIRGULA /* << ADD SEMICOLON */
                 { $$ = newStmtNode(ReadK);
                   if ($$) $$->attr.name = savedName;
                 }
            ;

write_stmt  : T_MOSTRAR T_ABRE_PARENTESES exp T_FECHA_PARENTESES T_PONTO_VIRGULA /* << ADD SEMICOLON */
                 { $$ = newStmtNode(WriteK);
                   if ($$) $$->child[0] = $3;
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
            | T_NUMERO_REAL
                 { $$ = newExpNode(ConstK);
                   $$->attr.real_val = atof(tokenString);
                 }
            | T_ID 
                 { $$ = newExpNode(IdK);
                   $$->attr.name = copyString(tokenString);
                 }
            | T_ERRO { $$ = NULL; }
            ;

%% /* --- Funcoes Auxiliares --- */

int yyerror(char * message)
{ 
  fprintf(stderr,"Syntax error at line %d: %s\n",lineno,message);
  fprintf(stderr,"Current token: %s\n", tokenString);
  Error = TRUE;
  return 0;
}

/*
 * Chama get_token() do seu analisador léxico, copia os dados
 * para as variáveis globais que o analisador sintático espera (lineno, tokenString)
 * e retorna apenas o tipo do token, como o Bison espera.
 */
static int yylex(void)
{
  /* Variavel estatica para nao perder o ponteiro entre chamadas */
  static char* lexeme_to_free = NULL;

  /* Libera a memoria do lexema anterior, se houver */
  if (lexeme_to_free != NULL)
  {
      free(lexeme_to_free);
      lexeme_to_free = NULL;
  }
  
  token current_token = get_token();
  
  /* Guarda o ponteiro do lexema para liberar na proxima chamada */
  lexeme_to_free = current_token.lexeme;
  
  /* Copia as informacoes do token para as variaveis globais do parser */
  lineno = current_token.line;
  tokenString = current_token.lexeme;

  /* Retorna o tipo do token para o parser */
  return current_token.type;
}


TreeNode * parse(void)
{ 
  yyparse();
  return savedTree;
}