#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "modules/graph.h"

int main() {
    Graph *g = create_graph(5);
    reg_airport(g,"CNF","Belo Horizonte");
    reg_airport(g,"BSB","Brasilia");
    reg_airport(g,"GIG","Rio de Janeiro");
    reg_airport(g,"SSA","Salvador");
    reg_airport(g,"GRU","Sao Paulo");

    if (g == NULL) {
        printf("Erro ao criar grafo.\n");
        return 1;
    }

    int op;

    do {
        printf("\n===== SISTEMA DE MALHA AEREA =====\n");
        printf("1 - Cadastrar aeroporto\n");
        printf("2 - Cadastrar voo\n");
        printf("3 - Remover voo\n");
        printf("4 - Listar voos de um aeroporto\n");
        printf("5 - Buscar rotas\n");
        printf("0 - Sair\n");
        printf("Opcao: ");
        scanf("%d", &op);

        switch(op) {

            case 1: {
                char code[4];
                char city[50];

                printf("Codigo do aeroporto: ");
                scanf("%3s", code);

                getchar();

                printf("Cidade: ");
                fgets(city,50,stdin);

                city[strcspn(city,"\n")] = '\0';

                if(reg_airport(g,code,city))
                    printf("Aeroporto cadastrado.\n");
                else
                    printf("Erro ao cadastrar.\n");

                break;
            }

            case 2: {
                char orig[4];
                char dest[4];
                char temp[20];

                char *flight = malloc(20);

                if(flight == NULL){
                    printf("Erro memoria.\n");
                    break;
                }

                printf("Origem: ");
                scanf("%3s",orig);

                printf("Destino: ");
                scanf("%3s",dest);

                printf("Numero do voo: ");
                scanf("%19s",temp);

                strcpy(flight,temp);

                if(reg_flight(g,orig,dest,flight))
                    printf("Voo cadastrado.\n");
                else{
                    printf("Erro cadastro.\n");
                    free(flight);
                }

                break;
            }

            case 3: {
                char flight[20];

                printf("Numero do voo: ");
                scanf("%19s",flight);

                if(rem_flight(g,flight))
                    printf("Voo removido.\n");
                else
                    printf("Voo nao encontrado.\n");

                break;
            }

            case 4: {
                char orig[4];

                printf("Codigo aeroporto: ");
                scanf("%3s",orig);

                list_flights(g,orig);

                break;
            }

            case 5: {
                char orig[4];
                char dest[4];

                printf("Origem: ");
                scanf("%3s",orig);

                printf("Destino: ");
                scanf("%3s",dest);

                list_routes(g,orig,dest);

                break;
            }

            case 0:
                printf("Encerrando...\n");
                break;

            default:
                printf("Opcao invalida.\n");
        }

    } while(op != 0);

    free_graph(g);

    return 0;
}