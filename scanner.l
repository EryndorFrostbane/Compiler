%{
#include <stdio.h>

/* Enum para representar os tokens da linguagem P- */
/* Facilita a integracao com um analisador sintatico (como o Bison) no futuro */
typedef enum {
    T_ERRO,
    T_ID,
    
    /* Palavras-chave */
    T_INTEIRO, T_REAL, T_SE, T_ENTAO, T_SENAO, T_ENQUANTO, T_REPITA, T_ATE,
    T_LER, T_MOSTRAR,
    
    /* Numeros */
    T_NUMERO_INT, T_NUMERO_REAL,
    
    /* Operadores */
    T_SOMA, T_SUB, T_MULT, T_DIV,
    T_E, T_OU,
    T_MENOR, T_MENOR_IGUAL, T_MAIOR, T_MAIOR_IGUAL, T_IGUAL, T_DIFERENTE,
    T_ATRIBUICAO,
    
    /* Simbolos e Separadores */
    T_PONTO_VIRGULA, T_VIRGULA,
    T_ABRE_PARENTESES, T_FECHA_PARENTESES,
    T_ABRE_CHAVES, T_FECHA_CHAVES

} TokenType;

/* Variavel global para contar as linhas, util para reportar erros */
int yylineo = 1;

/* Funcao auxiliar para imprimir o nome do token */
void print_token(TokenType token, const char* lexeme);

%}

/* Definicao de estados exclusivos. Usado para tratar comentarios de multiplas linhas,
  que tem uma logica de inicio e fim especifica.
*/
%x COMMENT

/* Definicoes regulares para facilitar a escrita das regras */
digito      [0-9]
letra       [a-zA-Z_]
identificador {letra}({letra}|{digito})*
numero_int  {digito}+
numero_real {digito}+\.{digito}+

%%

"/*"                { BEGIN(COMMENT); /* Entra no estado de comentario */ }
<COMMENT>"*/"        { BEGIN(INITIAL); /* Sai do estado de comentario */ }
<COMMENT>"\n"       { yylineo++; /* Incremente a linha quando encontrar um newline */ }
<COMMENT>.          { /* Ignora qualquer outro caractere dentro do comentario */ }


"inteiro"           { print_token(T_INTEIRO, yytext); /* Palavras-chave */ }
"real"              { print_token(T_REAL, yytext); }
"se"                { print_token(T_SE, yytext); }
"entao"             { print_token(T_ENTAO, yytext); }
"senao"             { print_token(T_SENAO, yytext); }
"enquanto"          { print_token(T_ENQUANTO, yytext); }
"repita"            { print_token(T_REPITA, yytext); }
"ate"               { print_token(T_ATE, yytext); }
"ler"               { print_token(T_LER, yytext); }
"mostrar"           { print_token(T_MOSTRAR, yytext); }

{numero_real}       { print_token(T_NUMERO_REAL, yytext); /* Numeros - A regra do real deve vir antes da do inteiro */ }
{numero_int}        { print_token(T_NUMERO_INT, yytext); }

{identificador}     { print_token(T_ID, yytext); /* Identificador - Deve vir depois das palavras-chave */ }

"&&"                { print_token(T_E, yytext); /* Operadores - Regras de 2 caracteres devem vir antes das de 1 */ }
"||"                { print_token(T_OU, yytext); }
"<="                { print_token(T_MENOR_IGUAL, yytext); }
">="                { print_token(T_MAIOR_IGUAL, yytext); }
"=="                { print_token(T_IGUAL, yytext); }
"!="                { print_token(T_DIFERENTE, yytext); }
"<"                 { print_token(T_MENOR, yytext); }
">"                 { print_token(T_MAIOR, yytext); }
"="                 { print_token(T_ATRIBUICAO, yytext); }
"+"                 { print_token(T_SOMA, yytext); }
"-"                 { print_token(T_SUB, yytext); }
"*"                 { print_token(T_MULT, yytext); }
"/"                 { print_token(T_DIV, yytext); }

";"                 { print_token(T_PONTO_VIRGULA, yytext); /* Simbolos e Separadores */ }
","                 { print_token(T_VIRGULA, yytext); }
"("                 { print_token(T_ABRE_PARENTESES, yytext); }
")"                 { print_token(T_FECHA_PARENTESES, yytext); }
"{"                 { print_token(T_ABRE_CHAVES, yytext); }
"}"                 { print_token(T_FECHA_CHAVES, yytext); }


"\n"                { yylineo++; /* Ignora, mas incrementa o contador de linha */ }
[ \t\r]+            { /* Ignora outros espacos em branco */ }

.                   { fprintf(stderr, "Erro lexico na linha %d: Caractere inesperado '%s'\n", yylineo, yytext); /* Regra para qualquer outro caractere (erro léxico) */ }

%%

int yywrap(void) {
    return 1;
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            fprintf(stderr, "Nao foi possivel abrir o arquivo %s\n", argv[1]);
            return 1;
        }
        yyin = file;
    }

    /* Função principal gerada pelo Flex que lê a entrada e encontra o próximo token */
    yylex();

    return 0;
}

/* Implementacao da funcao auxiliar para imprimir tokens */
void print_token(TokenType token, const char* lexeme) {
    printf("Linha %d: ", yylineo);
    switch (token) {
        case T_ID: printf("IDENTIFICADOR"); break;
        case T_INTEIRO: printf("PALAVRA_CHAVE"); break;
        case T_REAL: printf("PALAVRA_CHAVE"); break;
        case T_SE: printf("PALAVRA_CHAVE"); break;
        case T_ENTAO: printf("PALAVRA_CHAVE"); break;
        case T_SENAO: printf("PALAVRA_CHAVE"); break;
        case T_ENQUANTO: printf("PALAVRA_CHAVE"); break;
        case T_REPITA: printf("PALAVRA_CHAVE"); break;
        case T_ATE: printf("PALAVRA_CHAVE"); break;
        case T_LER: printf("PALAVRA_CHAVE"); break;
        case T_MOSTRAR: printf("PALAVRA_CHAVE"); break;
        case T_NUMERO_INT: printf("NUMERO_INTEIRO"); break;
        case T_NUMERO_REAL: printf("NUMERO_REAL"); break;
        case T_SOMA: printf("OP_SOMA"); break;
        case T_SUB: printf("OP_SUB"); break;
        case T_MULT: printf("OP_MULT"); break;
        case T_DIV: printf("OP_DIV"); break;
        case T_E: printf("OP_E"); break;
        case T_OU: printf("OP_OU"); break;
        case T_MENOR: printf("OP_MENOR"); break;
        case T_MENOR_IGUAL: printf("OP_MENOR_IGUAL"); break;
        case T_MAIOR: printf("OP_MAIOR"); break;
        case T_MAIOR_IGUAL: printf("OP_MAIOR_IGUAL"); break;
        case T_IGUAL: printf("OP_IGUAL"); break;
        case T_DIFERENTE: printf("OP_DIFERENTE"); break;
        case T_ATRIBUICAO: printf("OP_ATRIBUICAO"); break;
        case T_PONTO_VIRGULA: printf("PONTO_VIRGULA"); break;
        case T_VIRGULA: printf("VIRGULA"); break;
        case T_ABRE_PARENTESES: printf("ABRE_PARENTESES"); break;
        case T_FECHA_PARENTESES: printf("FECHA_PARENTESES"); break;
        case T_ABRE_CHAVES: printf("ABRE_CHAVES"); break;
        case T_FECHA_CHAVES: printf("FECHA_CHAVES"); break;
        default: printf("TOKEN_DESCONHECIDO"); break;
    }
    printf(" [%s]\n", lexeme);
}
