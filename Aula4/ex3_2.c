#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"


int main() {
    // criar os nós inicais da lista ligada
    struct Node* head = NULL;
    struct Node* second = NULL;
    struct Node* third = NULL;

    
    // Allocar memória para os nós
    head = (struct Node*)malloc(sizeof(struct Node));
    second = (struct Node*)malloc(sizeof(struct Node));
    third = (struct Node*)malloc(sizeof(struct Node));

    // Verificar a alocação de memória
    if (head == NULL || second == NULL || third == NULL) {
        printf("Erro ao alocar memória.\n");
        return 1;
    }

    head->data = 1; // atribuir dados ao primeiro nó
    head->next = second; // fazer o próximo do primeiro nó apontar para o segundo nó
    
    second->data = 2; // atribuir dados ao segundo nó
    second->next = third; // fazer o próximo do segundo nó apontar para o terceiro nó

    third->data = 3; // atribuir dados ao terceiro nó
    third->next = NULL; // fazer o próximo do terceiro nó apontar para NULL

    print_list(head); // imprimir a lista ligada

    insert_node(&head, 20); // inserir um novo nó com valor 20 no início da lista
    insert_node(&head, 30); // inserir um novo nó com valor 30 no início da lista

    print_list(head); // imprimir a lista ligada após as inserções

    delete_node(&head, 2); // deletar o nó com valor 2 da lista
    delete_node(&head, 30); // deletar o nó com valor 3 da lista

    print_list(head); // imprimir a lista ligada após as deleções

    free_list(&head); // liberar a memória alocada para a lista ligada

    return 0;
}