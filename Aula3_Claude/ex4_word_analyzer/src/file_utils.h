#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stdio.h>
#include "word_list.h"

/* Return codes */
#define FILE_UTILS_OK          0    /* success */
#define FILE_UTILS_ERR_ARG    -1    /* invalid argument (NULL pointer) */
#define FILE_UTILS_ERR_OPEN   -2    /* the file could not be opened */
#define FILE_UTILS_ERR_READ   -3    /* error while reading */
#define FILE_UTILS_ERR_NOMEM  -4    /* out of memory while storing the words */

/* Longest word stored in the word list, in bytes. Longer words are still
 * counted, but they are stored truncated. */
#define FILE_UTILS_MAX_WORD   64

typedef struct {
    unsigned long lines;
    unsigned long words;
    unsigned long characters;
} TextStats;

/*
 * The text is read as UTF-8 (plain ASCII is also valid UTF-8). Bytes that are
 * not valid UTF-8 are taken as Latin-1 (ISO-8859-1), so files saved in that
 * encoding also work.
 *
 *  - character: one Unicode character, so "ç" counts as 1 although it takes 2
 *               bytes. Spaces, tabs, '\r' and '\n' also count (like 'wc -m');
 *  - line:      text ended by '\n'; a last line without '\n' also counts;
 *  - word:      sequence of letters (accented ones included) and digits; an
 *               apostrophe or hyphen between two letters belongs to the word
 *               ("don't", "real-time"). Words are stored in lower case (A-Z
 *               and accented capitals such as "Ç"), so "The" and "the" are
 *               the same word.
 *
 * If 'words' is NULL only the counters are computed.
 */

/* Analyzes the file at 'path' */
int file_utils_analyze_file(const char *path, TextStats *stats, WordList *words);

/* Analyzes everything that can be read from an open stream */
int file_utils_analyze_stream(FILE *stream, TextStats *stats, WordList *words);

/* Analyzes a NUL-terminated string (handy for unit tests) */
int file_utils_analyze_string(const char *text, TextStats *stats, WordList *words);

/* Short description of a FILE_UTILS_* return code */
const char *file_utils_strerror(int code);

#endif /* FILE_UTILS_H */
