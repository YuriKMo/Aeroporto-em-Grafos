#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

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

    if (row == -1 && column == -1) {
        printf("[ERRO] Ambos os aeroportos de origem ('%s') e destino ('%s') nao foram encontrados no sistema.\n", orig_code, dest_code);
        return 0;
    } else if (row == -1) {
        printf("[ERRO] Aeroporto de origem ('%s') nao encontrado no sistema.\n", orig_code);
        return 0;
    } else if (column == -1) {
        printf("[ERRO] Aeroporto de destino ('%s') nao encontrado no sistema.\n", dest_code);
        return 0;
    }

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
                free(current->info);
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

void list_flights(Graph *g, char *orig_code) {
    int row = search_idx(g, orig_code);

    if (row == -1) {
        printf("[ERRO] Aeroporto de origem '%s' nao encontrado no sistema.\n", orig_code);
        return; 
    }

    Node *current = g->rows[row];

    if (current == NULL) {
        printf("Nenhum voo cadastrado partindo de %s.\n", orig_code);
        return;
    }

    printf("=== Voos Diretos partindo de %s ===\n", orig_code);

    while (current != NULL) {
        char *flight_num = (char *)current->info;

        int col_idx = current->column;
        char *dest_code = g->airports_array[col_idx].code;

        printf(" -> Voo: %s | Destino: %s\n", flight_num, dest_code);


        current = current->right;
    }
    printf("======================================\n");   
}

void search_path(Graph *g, int row, int column, int *visited, int *path, int steps) {
    path[steps] = row;
    visited[row] = 1;

    if (row == column) {
        printf(" Rota: ");

        for (int i = 0; i <= steps; i++) {
            printf("%s", g->airports_array[path[i]].code);

            if (i < steps) {
                printf(" -> ");
            }
        }

        printf("\n");
    } else {
        Node *current = g->rows[row];

        while (current != NULL) {
            // TODO
        }
    }
}

void list_routes(Graph *g, char *orig_code, char *dest_code) {
    int row = search_idx(g, orig_code);
    int column = search_idx(g, dest_code);

    if (row == -1 && column == -1) {
        printf("[ERRO] Ambos os aeroportos de origem ('%s') e destino ('%s') nao foram encontrados no sistema.\n", orig_code, dest_code);
        return;
    } else if (row == -1) {
        printf("[ERRO] Aeroporto de origem ('%s') nao encontrado no sistema.\n", orig_code);
        return;
    } else if (column == -1) {
        printf("[ERRO] Aeroporto de destino ('%s') nao encontrado no sistema.\n", dest_code);
        return;
    }

    int *visited = (int *) calloc(g->qtd_airports, sizeof(int));
    int *path = (int *) malloc(g->qtd_airports * sizeof(int));

    printf("=== Rotas encontradas de %s para %s ===\n", orig_code, dest_code);

    search_path(g, row, column, visited, path, 0);

    printf("=========================================\n");

    free(visited);
    free(path);
}

void free_graph(Graph *g) {
    if (g == NULL)
        return;


    for (int i = 0; i < g->qtd_airports; i++) { // Percorre todas as linhas da matriz esparsa.

        // Começa no primeiro nó da linha atual.
        Node *current = g->rows[i];

        // Percorre horizontalmente todos os nós da linha.
        while (current != NULL) {

            // Guarda temporariamente o nó atual
            Node *temp = current;

            // Avança para o próximo nó antes de liberar
            current = current->right;

            // Libera a memória associada às informações
            free(temp->info);

            // Libera o próprio nó da matriz esparsa.
            free(temp);
        }
    }

    // Libera o vetor dinâmico contendo os aeroportos.
    free(g->airports_array);

    // Libera os vetores de ponteiros utilizados para controlar linhas e colunas.
    free(g->rows);
    free(g->columns);

    // Libera o grafo.
    free(g);
}