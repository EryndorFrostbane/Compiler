#include "semantics.h"
#include <stdio.h>

#define SIZE_INT 4 // Inteiro: 4 bytes
#define SIZE_REAL 8 // Real: 8 bytes

static symbol_entry *hashTable[211];
static int offset = 0; // Contador de endereços de memória, começa em 0

// Função auxiliar simples de hash
static int hash(char *key) {
    int temp = 0;
    for (int i = 0; key[i] != '\0'; i++)
        temp = ((temp << 4) + key[i]) % 211;
    return temp;
}

// Insere uma variável na tabela de símbolos
void st_insert(char *name, exp_type type, int line) {
    int h = hash(name);
    symbol_entry *l = hashTable[h];

    // Verifica se a variável já foi declarada
    while ((l != NULL) && (strcmp(name, l->name) != 0))
        l = l->next;

    if (l == NULL) {
        // Variável não encontrada, insere
        l = (symbol_entry *)malloc(sizeof(symbol_entry));
        l->name = strdup(name);
        l->type = type;
        
        // Define o endereço de memória
        l->memory_address = offset;
        offset += (type == INTEGER) ? SIZE_INT : SIZE_REAL;
        
        l->next = hashTable[h];
        hashTable[h] = l;
    } else {
        // Erro: Variável já declarada
        fprintf(stderr, "Erro semantico na linha %d: Variavel '%s' ja foi declarada.\n", line, name);
        is_error = 1;
    }
}

// Busca uma variável na tabela de símbolos
symbol_entry *st_lookup(char *name) {
    int h = hash(name);
    symbol_entry *l = hashTable[h];
    while ((l != NULL) && (strcmp(name, l->name) != 0))
        l = l->next;
    return l;
}

// Função para atravessar a AST e construir a tabela
static void traverse_build(tree_node *tree) {
    if (tree == NULL) return;

    // Processamento do nó: Encontrar Declarações
    if (tree->node_kind == STATEMENT_KIND) {
        if (tree->kind.stmt == DECLARATION_STATEMENT) {
            // Supondo que você ajustará o parser.y para criar esse nó
            // ou processar a id_list durante a fase de verificação
            // A forma mais limpa é criar nós de Declaração.
            st_insert(tree->attribute.name, tree->type, tree->line_number);
        }
    }

    // Travessia recursiva para filhos e irmãos
    for (int i = 0; i < MAXCHILDREN; i++)
        traverse_build(tree->child[i]);
    
    traverse_build(tree->sibling);
}

void build_symbol_table(tree_node *tree) {
    traverse_build(tree);
    printf("Memory Offset Final: %d bytes\n", offset);
    print_symbol_table();
}

tree_node *new_int_to_real_exp_node(tree_node *child) {
    tree_node *t = new_expression_node(INT_TO_REAL_EXP);
    if (t != NULL) {
        t->child[0] = child;
        t->type = REAL;
        t->line_number = child->line_number;
    }
    return t;
}

// Tenta coerção de tipo (somente int -> real é permitido)
// Retorna um nó com o ajuste, ou o nó original.
tree_node *coerce_expression(tree_node *exp, exp_type target_type) {
    if (exp == NULL || exp->type == target_type)
        return exp;

    if (exp->type == INTEGER && target_type == REAL) {
        // Ajuste: Inserir nó de conversão
        printf("Aviso semantico na linha %d: Convertendo INTEGER para REAL.\n", exp->line_number);
        return new_int_to_real_exp_node(exp);
    }
    return exp; // Sem ajuste ou coerção impossível (ex: real -> int)
}

static void traverse_check(tree_node *tree) {
    if (tree == NULL) return;

    // 1. Travessia recursiva (Pós-ordem: verifica filhos primeiro)
    for (int i = 0; i < MAXCHILDREN; i++)
        traverse_check(tree->child[i]);

    // 2. Processamento do nó atual (Verificação de Tipos)

    if (tree->node_kind == EXPRESSION_KIND) {
        if (tree->kind.exp == IDENTIFIER_EXPRESSION) {
            symbol_entry *entry = st_lookup(tree->attribute.name);
            if (entry == NULL) {
                // Erro: Variável não declarada
                fprintf(stderr, "Erro semantico na linha %d: Variavel '%s' nao declarada.\n", tree->line_number, tree->attribute.name);
                is_error = 1;
                tree->type = VOID;
            } else {
                // Atribui o tipo da tabela ao nó
                tree->type = entry->type;
            }
        } 
        else if (tree->kind.exp == OPERATION_EXPRESSION) {
            exp_type left_type = tree->child[0]->type;
            exp_type right_type = tree->child[1]->type;

            switch (tree->attribute.op) {
                case T_SOMA:
                case T_SUB:
                case T_MULT:
                case T_DIV:
                    // Verificação: Operandos devem ser int ou real
                    if (left_type == VOID || right_type == VOID) {
                        tree->type = VOID;
                        break;
                    }
                    
                    // Ajuste: Coerção de Int para Real se mista
                    if (left_type == REAL && right_type == INTEGER) {
                        tree->child[1] = coerce_expression(tree->child[1], REAL);
                        tree->type = REAL;
                    } else if (left_type == INTEGER && right_type == REAL) {
                        tree->child[0] = coerce_expression(tree->child[0], REAL);
                        tree->type = REAL;
                    } else if (left_type == REAL && right_type == REAL) {
                        tree->type = REAL;
                    } else { // Ambos INTEGER
                        tree->type = INTEGER;
                    }
                    break;

                case T_MENOR:
                case T_MAIOR:
                case T_MENOR_IGUAL:
                case T_MAIOR_IGUAL:
                case T_IGUAL:
                case T_DIFERENTE:
                    // Verificação: Operandos devem ser int ou real
                    if (left_type == VOID || right_type == VOID) {
                         tree->type = VOID;
                         break;
                    }
                    // Coerção se mista (Int+Real)
                    if (left_type != right_type) {
                        exp_type result_type = (left_type == REAL || right_type == REAL) ? REAL : INTEGER;
                        tree->child[0] = coerce_expression(tree->child[0], result_type);
                        tree->child[1] = coerce_expression(tree->child[1], result_type);
                    }
                    // Resultado: Booleano (representado por INTEGER, 0 ou 1)
                    tree->type = INTEGER; 
                    break;

                case T_E:
                case T_OU:
                    // Verificação: Operandos devem ser booleanos (INTEGER)
                    if (left_type != INTEGER || right_type != INTEGER) {
                        fprintf(stderr, "Erro semantico na linha %d: Operadores logicos demandam tipo boolean (integer).\n", tree->line_number);
                        is_error = 1;
                        tree->type = VOID;
                        break;
                    }
                    // Resultado: Booleano (INTEGER)
                    tree->type = INTEGER;
                    break;
            }
        }
    }
    // Processamento de Comandos (Statements)
    else if (tree->node_kind == STATEMENT_KIND) {
        if (tree->kind.stmt == IF_STATEMENT || tree->kind.stmt == WHILE_STATEMENT || tree->kind.stmt == REPEAT_STATEMENT) {
            // Verificação: Condição deve ser booleana (INTEGER)
            if (tree->child[0]->type != INTEGER) {
                fprintf(stderr, "Erro semantico na linha %d: Operacoes condicionais devem ser do tipo boolean (integer).\n", tree->line_number);
                is_error = 1;
            }
        }
        else if (tree->kind.stmt == ASSIGNMENT_STATEMENT) {
            // Verificação: Tipo da variável à esquerda
            symbol_entry *var_entry = st_lookup(tree->attribute.name);
            if (var_entry != NULL) {
                exp_type target_type = var_entry->type;
                exp_type source_type = tree->child[0]->type;
                
                // Ajuste: Coerção do lado direito
                if (source_type != VOID) {
                    tree->child[0] = coerce_expression(tree->child[0], target_type);
                    if (tree->child[0]->type != target_type) {
                        // Se a coerção for impossível (ex: real para int), é um erro semântico
                         fprintf(stderr, "Erro semantico na linha %d: Nao pode converter variavel de tipo %d para variavel '%s' de tipo %d.\n", tree->line_number, source_type, tree->attribute.name, target_type);
                         is_error = 1;
                    }
                }
            }
        }
    }

    // 3. Travessia para o irmão
    traverse_check(tree->sibling);
}

void check_types(tree_node *tree) {
    traverse_check(tree);
}

void print_symbol_table(void)
{
    printf("+---------------------------------------------------+\n");
    printf("| NOME DO ID | TIPO | ENDEREÇO DE MEMORIA (OFFSET) |\n");
    printf("+---------------------------------------------------+\n");

    for (int i = 0; i < 211; i++)
    {
        symbol_entry *entry = hashTable[i];
        while (entry != NULL)
        {
            // Determina a string do tipo
            const char *type_str;
            switch (entry->type)
            {
            case INTEGER:
                type_str = "INTEIRO";
                break;
            case REAL:
                type_str = "REAL";
                break;
            default:
                type_str = "VOID/UNKNOWN";
                break;
            }

            printf("| %-10s | %-4s | %-28d |\n", 
                   entry->name, 
                   type_str, 
                   entry->memory_address);

            entry = entry->next;
        }
    }
    printf("+---------------------------------------------------+\n");
}

// Função principal para o analisador semântico
tree_node *apply_semantic_analysis(tree_node *syntaxTree) {
    // 1. Construir Tabela de Símbolos
    build_symbol_table(syntaxTree);

    // Se houve erro na fase 1 (declarações duplicadas, etc.), para.
    if (is_error) return NULL; 

    // 2. Verificar e Ajustar Tipos (Construir a AST Semântica)
    check_types(syntaxTree);
    
    // Retorna a AST ajustada
    return syntaxTree; 
}