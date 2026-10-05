#include <stdlib.h>
#include <string.h>
#include "word_list.h"

typedef struct WordNode {
    char *word;                 /* own copy of the word */
    unsigned long count;        /* number of occurrences */
    struct WordNode *next;
} WordNode;

struct WordList {
    WordNode *head;             /* first node (NULL if the list is empty) */
    size_t size;                /* number of distinct words */
    unsigned long total;        /* sum of all counts */
};

/* strdup() is POSIX, not standard C, so the copy is done by hand */
static char *copy_string(const char *s) {
    size_t len = strlen(s) + 1;
    char *copy = malloc(len);

    if (copy != NULL) {
        memcpy(copy, s, len);
    }
    return copy;
}

WordList *word_list_create(void) {
    WordList *list = malloc(sizeof *list);

    if (list != NULL) {
        list->head = NULL;
        list->size = 0;
        list->total = 0;
    }
    return list;
}

void word_list_destroy(WordList *list) {
    WordNode *node;

    if (list == NULL) {
        return;
    }
    node = list->head;
    while (node != NULL) {
        WordNode *next = node->next;
        free(node->word);
        free(node);
        node = next;
    }
    free(list);
}

int word_list_add(WordList *list, const char *word) {
    WordNode **link;
    WordNode *node;

    if (list == NULL || word == NULL || *word == '\0') {
        return WORD_LIST_ERR_ARG;
    }

    /* Look for the word. If it is not found, the loop ends with 'link'
     * pointing to the 'next' field of the last node (or to 'head'), which is
     * exactly where the new node must be hooked. */
    for (link = &list->head; *link != NULL; link = &(*link)->next) {
        if (strcmp((*link)->word, word) == 0) {
            (*link)->count++;
            list->total++;
            return WORD_LIST_OK;
        }
    }

    node = malloc(sizeof *node);
    if (node == NULL) {
        return WORD_LIST_ERR_NOMEM;
    }
    node->word = copy_string(word);
    if (node->word == NULL) {
        free(node);
        return WORD_LIST_ERR_NOMEM;
    }
    node->count = 1;
    node->next = NULL;
    *link = node;

    list->size++;
    list->total++;
    return WORD_LIST_OK;
}

unsigned long word_list_frequency(const WordList *list, const char *word) {
    if (list == NULL || word == NULL) {
        return 0;
    }
    for (const WordNode *node = list->head; node != NULL; node = node->next) {
        if (strcmp(node->word, word) == 0) {
            return node->count;
        }
    }
    return 0;
}

size_t word_list_size(const WordList *list) {
    return list != NULL ? list->size : 0;
}

unsigned long word_list_total(const WordList *list) {
    return list != NULL ? list->total : 0;
}

/* ---------------------------------------------------------------------- */
/* Sorting: merge sort, which suits singly linked lists (O(n log n), no    */
/* extra memory, only the 'next' links are changed)                        */
/* ---------------------------------------------------------------------- */

/* Order of the report: higher count first, then alphabetical order */
static int comes_before(const WordNode *a, const WordNode *b) {
    if (a->count != b->count) {
        return a->count > b->count;
    }
    return strcmp(a->word, b->word) <= 0;
}

/* Merges two sorted lists into a single sorted list */
static WordNode *merge(WordNode *a, WordNode *b) {
    WordNode first;                 /* dummy node: first.next is the result */
    WordNode *last = &first;

    first.next = NULL;
    while (a != NULL && b != NULL) {
        if (comes_before(a, b)) {
            last->next = a;
            a = a->next;
        } else {
            last->next = b;
            b = b->next;
        }
        last = last->next;
    }
    last->next = (a != NULL) ? a : b;   /* append what is left */
    return first.next;
}

static WordNode *merge_sort(WordNode *head) {
    WordNode *slow, *fast, *second;

    if (head == NULL || head->next == NULL) {
        return head;
    }

    /* Split in two halves: 'fast' advances two nodes for each one of 'slow',
     * so 'slow' stops at the middle */
    slow = head;
    fast = head->next;
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }
    second = slow->next;
    slow->next = NULL;

    return merge(merge_sort(head), merge_sort(second));
}

void word_list_sort_by_frequency(WordList *list) {
    if (list != NULL) {
        list->head = merge_sort(list->head);
    }
}

int word_list_get(const WordList *list, size_t index,
                  const char **word, unsigned long *count) {
    const WordNode *node;

    if (list == NULL) {
        return WORD_LIST_ERR_ARG;
    }
    for (node = list->head; node != NULL && index > 0; node = node->next) {
        index--;
    }
    if (node == NULL) {
        return WORD_LIST_NOT_FOUND;
    }
    if (word != NULL) {
        *word = node->word;
    }
    if (count != NULL) {
        *count = node->count;
    }
    return WORD_LIST_OK;
}

void word_list_print(const WordList *list, FILE *out, size_t max_entries) {
    size_t printed = 0;

    if (list == NULL || out == NULL) {
        return;
    }
    for (const WordNode *node = list->head; node != NULL; node = node->next) {
        if (max_entries > 0 && printed == max_entries) {
            fprintf(out, "- ... (%lu more)\n", (unsigned long)(list->size - printed));
            break;
        }
        fprintf(out, "- %s: %lu\n", node->word, node->count);
        printed++;
    }
}
