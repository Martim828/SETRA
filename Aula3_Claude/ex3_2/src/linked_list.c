#include <stdlib.h>
#include "linked_list.h"

/* Private: nobody outside this file knows how the list is built */
typedef struct Node {
    int value;
    struct Node *next;
} Node;

struct LinkedList {
    Node *head;         /* first node (NULL if the list is empty) */
    Node *tail;         /* last node, so that inserting at the end is O(1) */
    size_t size;        /* number of nodes */
};

LinkedList *linked_list_create(void) {
    LinkedList *list = malloc(sizeof *list);

    if (list != NULL) {
        list->head = NULL;
        list->tail = NULL;
        list->size = 0;
    }
    return list;
}

void linked_list_destroy(LinkedList *list) {
    Node *node;

    if (list == NULL) {
        return;
    }
    node = list->head;
    while (node != NULL) {
        Node *next = node->next;
        free(node);
        node = next;
    }
    free(list);
}

int linked_list_insert(LinkedList *list, int value) {
    Node *node;

    if (list == NULL) {
        return LINKED_LIST_ERR_ARG;
    }
    node = malloc(sizeof *node);
    if (node == NULL) {
        return LINKED_LIST_ERR_NOMEM;
    }
    node->value = value;
    node->next = NULL;

    if (list->tail == NULL) {       /* empty list: the node is also the head */
        list->head = node;
    } else {
        list->tail->next = node;
    }
    list->tail = node;
    list->size++;
    return LINKED_LIST_OK;
}

int linked_list_delete(LinkedList *list, int value) {
    Node *prev = NULL;
    Node *node;

    if (list == NULL) {
        return LINKED_LIST_ERR_ARG;
    }
    for (node = list->head; node != NULL; prev = node, node = node->next) {
        if (node->value == value) {
            if (prev == NULL) {             /* deleting the first node */
                list->head = node->next;
            } else {
                prev->next = node->next;
            }
            if (node == list->tail) {       /* deleting the last node */
                list->tail = prev;
            }
            free(node);
            list->size--;
            return LINKED_LIST_OK;
        }
    }
    return LINKED_LIST_NOT_FOUND;
}

void linked_list_print(const LinkedList *list, FILE *out) {
    if (out == NULL) {
        return;
    }
    fputc('[', out);
    if (list != NULL) {
        for (const Node *node = list->head; node != NULL; node = node->next) {
            if (node != list->head) {
                fputs(" -> ", out);
            }
            fprintf(out, "%d", node->value);
        }
    }
    fputs("]\n", out);
}

size_t linked_list_size(const LinkedList *list) {
    return list != NULL ? list->size : 0;
}

int linked_list_contains(const LinkedList *list, int value) {
    if (list == NULL) {
        return 0;
    }
    for (const Node *node = list->head; node != NULL; node = node->next) {
        if (node->value == value) {
            return 1;
        }
    }
    return 0;
}

int linked_list_get(const LinkedList *list, size_t index, int *value) {
    const Node *node;

    if (list == NULL || value == NULL) {
        return LINKED_LIST_ERR_ARG;
    }
    for (node = list->head; node != NULL && index > 0; node = node->next) {
        index--;
    }
    if (node == NULL) {
        return LINKED_LIST_NOT_FOUND;
    }
    *value = node->value;
    return LINKED_LIST_OK;
}
