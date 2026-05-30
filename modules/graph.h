#ifndef GRAPH_H
#define GRAPH_H

// Estrutura para guardar os dados do Aeroporto (Vértice)
typedef struct {
    char code[4]; // Ex: "CNF"
    char city[50]; // Ex: "Belo Horizonte"
} Airport;

// Estrutura do Nó da Matriz Esparsa (Aresta/Voo)
typedef struct Node {
    unsigned int row; // Índice do aeroporto de origem no vetor
    unsigned int column; // Índice do aeroporto de destino no vetor
    void *info; // Dado genérico (ex: número do voo alocado dinamicamente)

    struct Node* right;  // Aponta para o próximo voo saindo da MESMA origem (direita)
    struct Node* down; // Aponta para o próximo voo chegando no MESMO destino (baixo)
} Node;

typedef struct {
    Airport *airports_array;
    int array_max;        // Tamanho máximo atual do vetor
    int qtd_airports;          // Quantidade de aeroportos cadastrados no momento

    // Matriz Esparsa: Vetores de ponteiros para as linhas e colunas
    Node** rows;  // Array dinâmico de ponteiros (início de cada linha)
    Node** columns; // Array dinâmico de ponteiros (início de cada coluna)
} Graph;

/**
 * @brief Inicializa a estrutura do Grafo (matriz esparsa) na memória.
 *
 * Cria a matriz base do sistema, alocando o vetor de aeroportos e os 
 * vetores de ponteiros para as linhas e colunas (cabeçalhos).
 *
 * @param initial_cap Capacidade inicial do vetor dinâmico de aeroportos.
 * @return Graph* Ponteiro para a estrutura criada ou NULL em caso de erro de alocação.
 */
Graph *create_graph(unsigned int initial_cap);

/**
 * @brief Cadastra um novo aeroporto no grafo.
 *
 * Adiciona o código e a cidade ao vetor dinâmico. Caso o vetor atinja sua 
 * capacidade máxima, realiza o redimensionamento seguro (realloc) da matriz, 
 * expandindo os dados e os painéis de linhas/colunas simultaneamente.
 *
 * @param g    Ponteiro para a estrutura do Grafo.
 * @param code String com o código de identificação do aeroporto (ex: "BSB").
 * @param city String com o nome da cidade onde o aeroporto está localizado.
 * @return int Retorna 1 se o aeroporto foi cadastrado com sucesso, ou 0 em caso de erro.
 */
int reg_airport(Graph *g, char *code, char *city);

/**
 * @brief Registra um novo voo conectando dois aeroportos na matriz esparsa.
 *
 * Instancia um nó único na matriz esparsa cruzando a linha do aeroporto de 
 * origem com a coluna do aeroporto de destino, inserindo-o no início das 
 * listas encadeadas.
 *
 * @param g         Ponteiro para a estrutura do Grafo.
 * @param orig_code String com o código do aeroporto de origem.
 * @param dest_code String com o código do aeroporto de destino.
 * @param info      Ponteiro genérico para os dados do voo (ex: número do voo alocado).
 * @return int      Retorna 1 (sucesso) ou 0 (origem/destino inexistentes ou erro de memória).
 */
int reg_flight(Graph *g, char *orig_code, char *dest_code, void *info);

/**
 * @brief Remove um voo específico da matriz esparsa.
 *
 * @param g             Ponteiro para a estrutura do Grafo.
 * @param flight_number String contendo o número do voo a ser removido.
 * @return int Retorna 1 (sucesso) ou 0 (falha/não encontrado).
 */
int rem_flight(Graph *g, char *flight_number);

/**
 * @brief Lista todos os voos diretos partindo de um aeroporto específico.
 *
 * Imprime as informações dos voos ao percorrer horizontalmente a lista 
 * encadeada da linha correspondente ao aeroporto de origem.
 *
 * @param g         Ponteiro para a estrutura do Grafo.
 * @param orig_code String com o código do aeroporto de origem.
 */
void list_flights(Graph *g, char *orig_code);

/**
 * @brief Busca e lista voos (diretos ou com conexões) entre dois aeroportos.
 *
 * Avalia as possibilidades de rotas na malha do grafo partindo da origem 
 * até alcançar o destino desejado.
 *
 * @param g         Ponteiro para a estrutura do Grafo.
 * @param orig_code String com o código do aeroporto de origem.
 * @param dest_code String com o código do aeroporto de destino.
 */
void list_routes(Graph *g, char *orig_code, char *dest_code);

/**
 * @brief Libera completamente a memória utilizada pelo grafo.
 *
 * Previne vazamentos de memória (memory leaks) varrendo toda a matriz ortogonal 
 * para aplicar `free` em cada nó individual de voo, seguido da liberação dos 
 * vetores de controle dinâmicos e da estrutura raiz do grafo.
 *
 * @param g Ponteiro para a estrutura do Grafo a ser destruída.
 */
void free_graph(Graph *g);

#endif