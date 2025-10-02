%option noyywrap

%{
#include <stdio.h>  // fprintf()
#include <string.h> // strdup()
#include "headers/scanner.h"  // token_type, token, get_token()

/*
 * A macro YY_DECL é usada para redefinir a assinatura da função do analisador léxico.
 * Por padrão, ela é "int yylex(void)".
 * Alteramos para que ela se chame "get_token" e retorne um "token" ao invés de um "int".
 * O Flex gerará o código C com esta nova assinatura.
 */
#undef YY_DECL
#define YY_DECL token get_token(void)

/* Variável global para contar as linhas, útil para reportar erros */
int yylineo = 1;

%}

/*
 * Definição de estado exclusivo.
 * Usado para tratar comentários de múltiplas linhas.
 */
%x COMMENT

/* Definições regulares para facilitar a escrita das regras */
digito      [0-9]
letra       [a-zA-Z_]
identificador {letra}({letra}|{digito})*
numero_int  {digito}+
numero_real {digito}+\.{digito}+

%%

"/*"                { BEGIN(COMMENT); /* Entra no estado de comentário */ }
<COMMENT>"*/"       { BEGIN(INITIAL); /* Sai do estado de comentário */ }
<COMMENT>"\n"       { yylineo++; /* Incrementa a linha dentro do comentário */ }
<COMMENT>.          { /* Ignora qualquer outro caractere dentro do comentário */ }


"inteiro"           { token t = {T_INTEIRO, strdup(yytext), yylineo}; return t; }
"real"              { token t = {T_REAL, strdup(yytext), yylineo}; return t; }
"se"                { token t = {T_SE, strdup(yytext), yylineo}; return t; }
"entao"             { token t = {T_ENTAO, strdup(yytext), yylineo}; return t; }
"senao"             { token t = {T_SENAO, strdup(yytext), yylineo}; return t; }
"enquanto"          { token t = {T_ENQUANTO, strdup(yytext), yylineo}; return t; }
"repita"            { token t = {T_REPITA, strdup(yytext), yylineo}; return t; }
"ate"               { token t = {T_ATE, strdup(yytext), yylineo}; return t; }
"ler"               { token t = {T_LER, strdup(yytext), yylineo}; return t; }
"mostrar"           { token t = {T_MOSTRAR, strdup(yytext), yylineo}; return t; }

{numero_real}       { token t = {T_NUMERO_REAL, strdup(yytext), yylineo}; return t; }
{numero_int}        { token t = {T_NUMERO_INT, strdup(yytext), yylineo}; return t; }

{identificador}     { token t = {T_ID, strdup(yytext), yylineo}; return t; }

"&&"                { token t = {T_E, strdup(yytext), yylineo}; return t; }
"||"                { token t = {T_OU, strdup(yytext), yylineo}; return t; }
"<="                { token t = {T_MENOR_IGUAL, strdup(yytext), yylineo}; return t; }
">="                { token t = {T_MAIOR_IGUAL, strdup(yytext), yylineo}; return t; }
"=="                { token t = {T_IGUAL, strdup(yytext), yylineo}; return t; }
"!="                { token t = {T_DIFERENTE, strdup(yytext), yylineo}; return t; }
"<"                 { token t = {T_MENOR, strdup(yytext), yylineo}; return t; }
">"                 { token t = {T_MAIOR, strdup(yytext), yylineo}; return t; }
"="                 { token t = {T_ATRIBUICAO, strdup(yytext), yylineo}; return t; }
"+"                 { token t = {T_SOMA, strdup(yytext), yylineo}; return t; }
"-"                 { token t = {T_SUB, strdup(yytext), yylineo}; return t; }
"*"                 { token t = {T_MULT, strdup(yytext), yylineo}; return t; }
"/"                 { token t = {T_DIV, strdup(yytext), yylineo}; return t; }

";"                 { token t = {T_PONTO_VIRGULA, strdup(yytext), yylineo}; return t; }
","                 { token t = {T_VIRGULA, strdup(yytext), yylineo}; return t; }
"("                 { token t = {T_ABRE_PARENTESES, strdup(yytext), yylineo}; return t; }
")"                 { token t = {T_FECHA_PARENTESES, strdup(yytext), yylineo}; return t; }
"{"                 { token t = {T_ABRE_CHAVES, strdup(yytext), yylineo}; return t; }
"}"                 { token t = {T_FECHA_CHAVES, strdup(yytext), yylineo}; return t; }


"\n"                { yylineo++; /* Ignora, mas incrementa o contador de linha */ }
[ \t\r]+            { /* Ignora outros espaços em branco */ }

.                   {
                      fprintf(stderr, "Erro lexico na linha %d: Caractere inesperado '%s'\n", yylineo, yytext);
                      token t = {T_ERRO, strdup(yytext), yylineo};
                      return t;
                    }

<<EOF>>             {
                      // Retorna um token especial para Fim de Arquivo (End of File)
                      token t = {T_EOF, strdup(""), yylineo};
                      return t;
                    }
%%
