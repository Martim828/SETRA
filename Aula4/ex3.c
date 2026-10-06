#include <stdio.h>
#include <stdlib.h>

// definir a estrtutura para uma lista ligada

struct Node {
    int data;
    struct Node* next;
};

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