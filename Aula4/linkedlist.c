#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"

void insert_node(struct Node** head_ref, int new_data) {
    // alocar memória para o novo nó
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
    
    // verificar a alocação de memória
    if (new_node == NULL) {
        printf("Erro ao alocar memória.\n");
        return;
    }

    // atribuir dados ao novo nó
    new_node->data = new_data;
    
    // fazer o próximo do novo nó apontar para o nó atual da cabeça
    new_node->next = (*head_ref);
    
    // mover a cabeça para apontar para o novo nó
    (*head_ref) = new_node;
}

void delete_node(struct Node** head_ref, int deleted_node) {
    struct Node* temp = *head_ref;
    struct Node* prev = NULL;

    // se o nó a ser deletado é a cabeça
    if (temp != NULL && temp->data == deleted_node) {
        *head_ref = temp->next; // mudar a cabeça
        free(temp); // liberar memória
        return;
    }

    // procurar pelo nó a ser deletado
    while (temp != NULL && temp->data != deleted_node) {
        prev = temp;
        temp = temp->next;
    }

    // se o nó não foi encontrado
    if (temp == NULL) return;

    // desconectar o nó da lista ligada
    prev->next = temp->next;

    free(temp); // liberar memória
}

void print_list(struct Node* head_ref) {
    struct Node* temp = head_ref;
    while (temp != NULL) {
        printf("%d -> ", temp->data);
        temp = temp->next;
    }
    printf("NULL\n");


}

void free_list(struct Node** head_ref) {
    struct Node* temp = *head_ref;
    while (temp != NULL) {
        struct Node* next = temp->next;
        free(temp);
        temp = next;
    }
    *head_ref = NULL;   // evita dangling pointer
}