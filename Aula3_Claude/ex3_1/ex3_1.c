/*
 * Exercise 3.1 - Basic linked list
 *
 * Singly linked list of integers with separate insert, delete and print
 * functions. The list is just a pointer to its first node (the head); the
 * functions that may change the head receive its address (Node **).
 */
#include <stdio.h>
#include <stdlib.h>

/* Return codes */
#define LIST_OK          0      /* success */
#define LIST_NOT_FOUND  -1      /* value not in the list */
#define LIST_ERR_NOMEM  -2      /* out of memory */

#define COUNT(array) ((int)(sizeof(array) / sizeof((array)[0])))

typedef struct Node {
    int value;
    struct Node *next;
} Node;

/* Inserts value at the end of the list. */
int list_insert(Node **head, int value) {
    Node *node = malloc(sizeof *node);

    if (node == NULL) {
        return LIST_ERR_NOMEM;
    }
    node->value = value;
    node->next = NULL;

    while (*head != NULL) {         /* walk to the link that holds NULL... */
        head = &(*head)->next;
    }
    *head = node;                   /* ...and hook the new node there */
    return LIST_OK;
}

/* Deletes the first node that holds value. */
int list_delete(Node **head, int value) {
    while (*head != NULL) {
        if ((*head)->value == value) {
            Node *victim = *head;
            *head = victim->next;   /* unlink (also works for the first node) */
            free(victim);
            return LIST_OK;
        }
        head = &(*head)->next;
    }
    return LIST_NOT_FOUND;
}

/* Prints the list, e.g. "[1 -> 2 -> 3]". */
void list_print(const Node *head) {
    printf("[");
    for (const Node *node = head; node != NULL; node = node->next) {
        if (node != head) {
            printf(" -> ");
        }
        printf("%d", node->value);
    }
    printf("]\n");
}

/* Frees every node and leaves the list empty (head = NULL). */
void list_free(Node **head) {
    while (*head != NULL) {
        Node *next = (*head)->next;
        free(*head);
        *head = next;
    }
}

/* ---------------------------------------------------------------------- */
/* Test helpers                                                            */
/* ---------------------------------------------------------------------- */

/* Returns 1 if the list holds exactly the n values in expected[]. */
static int list_equals(const Node *head, const int *expected, int n) {
    int i = 0;

    for (; head != NULL; head = head->next, i++) {
        if (i >= n || head->value != expected[i]) {
            return 0;
        }
    }
    return i == n;
}

/* Prints the list after one step and checks its contents.
 * Returns 1 on failure, 0 on success. */
static int check(const char *step, int rc, int expected_rc,
                 const Node *head, const int *expected, int n) {
    int ok = (rc == expected_rc) && list_equals(head, expected, n);

    printf("[%s] %-26s", ok ? " OK " : "FAIL", step);
    list_print(head);
    return ok ? 0 : 1;
}

int main(void) {
    static const int after_insert[]   = {1, 2, 3, 4, 5};
    static const int after_del_head[] = {2, 3, 4, 5};
    static const int after_del_mid[]  = {2, 4, 5};
    static const int after_del_tail[] = {2, 4};
    static const int after_reinsert[] = {2, 4, 6, 4};
    static const int after_del_dup[]  = {2, 6, 4};
    Node *list = NULL;
    int errors = 0;
    int rc = LIST_OK;

    errors += check("empty list", LIST_OK, LIST_OK, list, NULL, 0);

    for (int v = 1; v <= 5 && rc == LIST_OK; v++) {
        rc = list_insert(&list, v);
    }
    errors += check("insert 1..5", rc, LIST_OK, list, after_insert, COUNT(after_insert));

    rc = list_delete(&list, 1);
    errors += check("delete 1 (head)", rc, LIST_OK, list, after_del_head, COUNT(after_del_head));

    rc = list_delete(&list, 3);
    errors += check("delete 3 (middle)", rc, LIST_OK, list, after_del_mid, COUNT(after_del_mid));

    rc = list_delete(&list, 5);
    errors += check("delete 5 (tail)", rc, LIST_OK, list, after_del_tail, COUNT(after_del_tail));

    rc = list_delete(&list, 42);
    errors += check("delete 42 (not in list)", rc, LIST_NOT_FOUND, list, after_del_tail, COUNT(after_del_tail));

    rc = list_insert(&list, 6);
    if (rc == LIST_OK) {
        rc = list_insert(&list, 4);
    }
    errors += check("insert 6, insert 4", rc, LIST_OK, list, after_reinsert, COUNT(after_reinsert));

    rc = list_delete(&list, 4);
    errors += check("delete 4 (first one only)", rc, LIST_OK, list, after_del_dup, COUNT(after_del_dup));

    list_delete(&list, 2);
    list_delete(&list, 6);
    rc = list_delete(&list, 4);
    errors += check("delete 2, 6, 4", rc, LIST_OK, list, NULL, 0);

    rc = list_delete(&list, 4);
    errors += check("delete from empty list", rc, LIST_NOT_FOUND, list, NULL, 0);

    list_insert(&list, 7);
    list_insert(&list, 8);
    list_free(&list);
    errors += check("insert 7, 8 and free", LIST_OK, LIST_OK, list, NULL, 0);

    printf("%s\n", errors == 0 ? "All checks passed." : "Some checks FAILED.");
    return errors == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
