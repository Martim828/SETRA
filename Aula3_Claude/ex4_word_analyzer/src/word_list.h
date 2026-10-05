#ifndef WORD_LIST_H
#define WORD_LIST_H

#include <stddef.h>
#include <stdio.h>

/* Return codes */
#define WORD_LIST_OK          0     /* success */
#define WORD_LIST_NOT_FOUND  -1     /* position not in the list */
#define WORD_LIST_ERR_NOMEM  -2     /* out of memory */
#define WORD_LIST_ERR_ARG    -3     /* invalid argument (NULL or empty word) */

/* Opaque list of (word, count) pairs, implemented as a singly linked list
 * that is private to word_list.c. Words are compared byte by byte (the list
 * is case sensitive: converting to lower case is up to the caller). */
typedef struct WordList WordList;

/* Creates an empty list. Returns NULL if there is not enough memory. */
WordList *word_list_create(void);

/* Frees the list, its nodes and the stored words (a NULL list is ignored). */
void word_list_destroy(WordList *list);

/* Counts one more occurrence of word. The list keeps its own copy of the
 * word; new words are added at the end of the list. */
int word_list_add(WordList *list, const char *word);

/* Number of occurrences of word (0 if it is not in the list). */
unsigned long word_list_frequency(const WordList *list, const char *word);

/* Number of distinct words. */
size_t word_list_size(const WordList *list);

/* Total number of occurrences (sum of all counts). */
unsigned long word_list_total(const WordList *list);

/* Sorts the list by decreasing frequency; words with the same frequency are
 * sorted alphabetically. */
void word_list_sort_by_frequency(WordList *list);

/* Gets the entry at position index (0 = first); word and count may be NULL.
 * The returned word belongs to the list. */
int word_list_get(const WordList *list, size_t index,
                  const char **word, unsigned long *count);

/* Prints the entries in list order, one "- word: count" per line. If
 * max_entries > 0, only the first max_entries are printed, followed by a
 * "- ... (N more)" line when some are left out. */
void word_list_print(const WordList *list, FILE *out, size_t max_entries);

#endif /* WORD_LIST_H */
