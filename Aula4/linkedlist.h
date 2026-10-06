#ifndef LINKEDLIST_H
#define LINKEDLIST_H

struct Node {
    int data;
    struct Node* next;
};

void insert_node(struct Node** head_ref, int new_data);
void delete_node(struct Node** head_ref, int deleted_node);
void print_list(struct Node* head_ref);
void free_list(struct Node** head_ref);

#endif // LINKEDLIST_H
