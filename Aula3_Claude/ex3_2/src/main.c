/*
 * Exercise 3.2 - Linked list in modular files
 *
 * Same steps as exercise 3.1, but using the encapsulated linked_list module.
 * The unit tests are in test/test_linked_list.c (make test).
 */
#include <stdio.h>
#include <stdlib.h>
#include "linked_list.h"

/* Prints the list after one step */
static void show(const char *step, const LinkedList *list) {
    printf("%-26s (size %lu) ", step, (unsigned long)linked_list_size(list));
    linked_list_print(list, stdout);
}

int main(void) {
    LinkedList *list = linked_list_create();

    if (list == NULL) {
        fprintf(stderr, "out of memory\n");
        return EXIT_FAILURE;
    }
    show("empty list", list);

    for (int v = 1; v <= 5; v++) {
        if (linked_list_insert(list, v) != LINKED_LIST_OK) {
            fprintf(stderr, "out of memory\n");
            linked_list_destroy(list);
            return EXIT_FAILURE;
        }
    }
    show("insert 1..5", list);

    linked_list_delete(list, 1);
    show("delete 1 (head)", list);

    linked_list_delete(list, 3);
    show("delete 3 (middle)", list);

    linked_list_delete(list, 5);
    show("delete 5 (tail)", list);

    if (linked_list_delete(list, 42) == LINKED_LIST_NOT_FOUND) {
        show("delete 42 -> not found", list);
    }

    linked_list_insert(list, 6);
    show("insert 6", list);

    printf("contains 4? %s, contains 5? %s\n",
           linked_list_contains(list, 4) ? "yes" : "no",
           linked_list_contains(list, 5) ? "yes" : "no");

    linked_list_destroy(list);
    return EXIT_SUCCESS;
}
