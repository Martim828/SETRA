#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stddef.h>
#include <stdio.h>

/* Return codes */
#define LINKED_LIST_OK          0   /* success */
#define LINKED_LIST_NOT_FOUND  -1   /* value or position not in the list */
#define LINKED_LIST_ERR_NOMEM  -2   /* out of memory */
#define LINKED_LIST_ERR_ARG    -3   /* invalid argument (NULL pointer) */

/* Opaque type: the node and list structures are defined only in
 * linked_list.c, so other modules can only use the list through the
 * functions below and cannot break its internal links. */
typedef struct LinkedList LinkedList;

/* Creates an empty list. Returns NULL if there is not enough memory. */
LinkedList *linked_list_create(void);

/* Frees the list and all its nodes (a NULL list is ignored). */
void linked_list_destroy(LinkedList *list);

/* Inserts value at the end of the list. */
int linked_list_insert(LinkedList *list, int value);

/* Deletes the first node that holds value. */
int linked_list_delete(LinkedList *list, int value);

/* Prints the list to out, e.g. "[1 -> 2 -> 3]" (a NULL list prints "[]"). */
void linked_list_print(const LinkedList *list, FILE *out);

/* Number of elements (0 for a NULL list). */
size_t linked_list_size(const LinkedList *list);

/* Returns 1 if value is in the list, 0 otherwise. */
int linked_list_contains(const LinkedList *list, int value);

/* Stores in *value the element at position index (0 = first). */
int linked_list_get(const LinkedList *list, size_t index, int *value);

#endif /* LINKED_LIST_H */
