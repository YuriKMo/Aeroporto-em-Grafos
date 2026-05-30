#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <graph.h>

Graph *create_graph(unsigned int initial_cap) {
    Graph *g = (Graph *) malloc(sizeof(Graph));

    if (g == NULL) {
        printf("[ERRO]: Ocorreu um problema na alocação da memória para o grafo.");
        return NULL;
    }

    g->airports_array = (Airport *) malloc(initial_cap * sizeof(Airport));
    g->rows = (Node **) malloc(initial_cap * sizeof(Node*));
    g->columns = (Node **) malloc(initial_cap * sizeof(Node*));

    if (g->airports_array == NULL || g->rows == NULL || g->columns == NULL) {
        printf("[ERRO]: Ocorreu um problema na alocação da memória para os vetores.");
        
        free(g->airports_array);
        free(g->rows);
        free(g->columns);
        free(g);
        return NULL;
    }

    for (int i = 0; i < initial_cap; i++) {
        g->rows[i] = NULL;
        g->columns[i] = NULL;
    }

    g->array_max = initial_cap;
    g->qtd_airports = 0;

    return g;
}

int reg_airport(Graph *g, char *code, char *city) {
    if (g->qtd_airports == g->array_max) {
        int new_cap = g->array_max * 2;

        Airport *temp_airports = (Airport *) realloc(g->airports_array, new_cap * sizeof(Airport));
        Node **temp_rows = (Node **) realloc(g->rows, new_cap * sizeof(Node*));
        Node **temp_columns = (Node **) realloc(g->columns, new_cap * sizeof(Node*));

        if (temp_airports == NULL || temp_rows == NULL || temp_columns == NULL) return 0;

        g->airports_array = temp_airports;
        g->rows = temp_rows;
        g->columns = temp_columns;

        for (int i = g->array_max; i < new_cap; i++) {
            g->rows[i] = NULL;
            g->columns[i] = NULL;
        }

        g->array_max = new_cap;
    }

    strcpy(g->airports_array[g->qtd_airports].code, code);
    strcpy(g->airports_array[g->qtd_airports].city, city);
    g->qtd_airports++;
    return 1;
}

int search_idx(Graph *g, char *code) {
    for (int i = 0; i < g->qtd_airports; i++) {
        if (strcmp(g->airports_array[i].code, code) == 0) return i;
    }
 
    return -1;
}

int reg_flight(Graph *g, char *orig_code, char *dest_code, void *info) {
    int row = search_idx(g, orig_code);
    int column = search_idx(g, dest_code);

    if (row == -1 || column == -1) return 0;

    Node *flight = (Node *) malloc(sizeof(Node));

    if (flight == NULL) return 0;

    flight->row = row;
    flight->column = column;
    flight->info = info;

    flight->right = g->rows[row];
    g->rows[row] = flight;

    flight->down = g->columns[column];
    g->columns[column] = flight;

    return 1;
}

int rem_flight(Graph *g, char *flight_number) {
    // Percorre todas as linhas (aeroportos de origem) da matriz
    for (int i = 0; i < g->qtd_airports; i++) {
        Node *previous = NULL;

        // 'current' é o nó que está agora. Começa no nó da linha 'i'.
        Node *current = g->rows[i];
        
        // Anda para a direita, passando por todos os voos da linha atual
        while (current != NULL) {
            // Verifica se achou o alvo. O cast (char *) ta aí pq o info é genérico.
            if (strcmp((char *)current->info, flight_number) == 0) {

                // DESCONEXÃO DA LISTA HORIZONTAL (LINHA)
                if (previous == NULL) {
                    // Se for o primeiro nó da lista, o vetor principal passa a apontar para o segundo nó
                    g->rows[i] = current->right;
                } else {
                    // Se for um nó intermediário/final, o nó anterior pula o atual e aponta para o próximo
                    previous->right = current->right;
                }

                // DESCONEXÃO DA LISTA VERTICAL (COLUNA)

                // Obtém o índice da coluna onde o nó alvo está inserido para buscá-lo na vertical
                int col_idx = current->column;

                // Ponteiros auxiliares para percorrer a lista encadeada vertical correspondente
                Node *prev_col = NULL;
                Node *curr_col = g->columns[col_idx];

                // Percorre a coluna de cima para baixo
                while (curr_col != NULL) {
                    if (curr_col == current) {
                        if (prev_col == NULL) {
                            // Atualiza o vetor principal de colunas se for o primeiro nó
                            g->columns[col_idx] = curr_col->down;
                        } else {
                            // Nó de cima aponta para o nó de baixo
                            prev_col->down = curr_col->down;
                        }

                        // Nó desvinculado da lista vertical; interrompe a busca nesta coluna
                        break;
                    }

                    prev_col = curr_col;
                    curr_col = curr_col->down;
                }

                // O nó agora está totalmente isolado das listas da matriz.
                // A memória pode ser liberada com segurança sem causar vazamentos ou ponteiros órfãos.
                free(current);
                return 1;
            }

            previous = current;
            current = current->right;
        }   
    }

    // Retorna 0 (falha) se iterou por toda a matriz esparsa e não encontrou a string correspondente
    return 0;
}

