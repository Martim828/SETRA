/*
 * Theme 4 - Final project: Word Analyzer
 *
 * Usage: word_analyzer <file> [N]
 *
 * Prints the number of lines, words and characters of the file, followed by
 * the frequency of each word, from the most to the least frequent (only the
 * N most frequent words if N is given).
 *
 *   main.c          command line and report
 *   file_utils.c/h  reads the file and splits it into lines, words, characters
 *   word_list.c/h   linked list with the (word, count) pairs
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "file_utils.h"
#include "word_list.h"

static const char *plural(unsigned long n, const char *one, const char *many) {
    return n == 1 ? one : many;
}

int main(int argc, char *argv[]) {
    TextStats stats;
    WordList *words;
    size_t max_words = 0;           /* 0 = all the words */
    int rc;

    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s <file> [N]\n"
                        "  N: show only the N most frequent words\n", argv[0]);
        return EXIT_FAILURE;
    }
    if (argc == 3) {
        char *end;
        long n;

        errno = 0;
        n = strtol(argv[2], &end, 10);
        if (errno != 0 || end == argv[2] || *end != '\0' || n < 1) {
            fprintf(stderr, "%s: N must be a positive integer, not '%s'\n", argv[0], argv[2]);
            return EXIT_FAILURE;
        }
        max_words = (size_t)n;
    }

    words = word_list_create();
    if (words == NULL) {
        fprintf(stderr, "%s: out of memory\n", argv[0]);
        return EXIT_FAILURE;
    }

    rc = file_utils_analyze_file(argv[1], &stats, words);
    if (rc != FILE_UTILS_OK) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], argv[1], file_utils_strerror(rc));
        word_list_destroy(words);
        return EXIT_FAILURE;
    }

    word_list_sort_by_frequency(words);

    printf("%lu %s\n", stats.lines, plural(stats.lines, "line", "lines"));
    printf("%lu %s\n", stats.words, plural(stats.words, "word", "words"));
    printf("%lu %s\n", stats.characters, plural(stats.characters, "character", "characters"));
    printf("Word frequency:\n");
    word_list_print(words, stdout, max_words);

    word_list_destroy(words);
    return EXIT_SUCCESS;
}
