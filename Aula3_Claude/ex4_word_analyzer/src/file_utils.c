#include <string.h>
#include "file_utils.h"

#define READ_CHUNK 4096     /* bytes read from the file at a time */

/* Role of a character in a word */
typedef enum {
    CHAR_SEPARATOR,         /* ends the current word: space, punctuation, symbol */
    CHAR_WORD,              /* letter or digit */
    CHAR_JOINER             /* ' or -: part of the word only between two letters */
} CharClass;

/* State of one analysis. The text is fed one byte at a time, so the same code
 * handles files (read in blocks) and strings. */
typedef struct {
    TextStats *stats;
    WordList *words;
    int error;                          /* first error found */
    unsigned long last_char;            /* last character seen */

    unsigned char utf8[4];              /* UTF-8 sequence being decoded */
    int utf8_len;                       /* bytes received so far */
    int utf8_need;                      /* bytes of the whole sequence */

    char word[FILE_UTILS_MAX_WORD + 1]; /* current word (UTF-8, lower case) */
    size_t word_len;                    /* bytes used in word[] */
    int in_word;                        /* inside a word? */
    int word_full;                      /* the word did not fit (truncated) */
    char joiner;                        /* pending ' or - (0 if none) */
} Analyzer;

/* ---------------------------------------------------------------------- */
/* Characters                                                              */
/* ---------------------------------------------------------------------- */

static CharClass classify(unsigned long c) {
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
        return CHAR_WORD;
    }
    if (c == '\'' || c == '-' || c == 0x2019 || c == 0x2010 || c == 0x2011) {
        return CHAR_JOINER;                     /* ' - and the typographic ’ ‐ ‑ */
    }
    if (c < 0xC0) {
        /* rest of ASCII and the Latin-1 symbols (NBSP « » ° ...), except
         * the letters ª µ º */
        return (c == 0xAA || c == 0xB5 || c == 0xBA) ? CHAR_WORD : CHAR_SEPARATOR;
    }
    if (c == 0xD7 || c == 0xF7 ||               /* × ÷ */
        (c >= 0x2000 && c <= 0x2BFF) ||         /* punctuation — “ ” …, symbols € → */
        (c >= 0x1F000 && c <= 0x1FFFF) ||       /* emoji */
        c == 0xFEFF) {                          /* byte order mark */
        return CHAR_SEPARATOR;
    }
    return CHAR_WORD;                   /* accented letters, Greek, Cyrillic, ... */
}

/* Lower case for A-Z and for the Latin-1 capitals À..Þ (Ç -> ç, Ã -> ã, ...) */
static unsigned long to_lower(unsigned long c) {
    if (c >= 'A' && c <= 'Z') {
        return c + ('a' - 'A');
    }
    if (c >= 0xC0 && c <= 0xDE && c != 0xD7) {
        return c + 0x20;
    }
    return c;
}

/* ---------------------------------------------------------------------- */
/* Words                                                                   */
/* ---------------------------------------------------------------------- */

/* Appends character c, encoded in UTF-8, to the current word. If it does not
 * fit, the word is truncated: c and the rest of the word are dropped. */
static void word_append(Analyzer *a, unsigned long c) {
    unsigned char bytes[4];
    size_t n;

    if (c < 0x80) {
        bytes[0] = (unsigned char)c;
        n = 1;
    } else if (c < 0x800) {
        bytes[0] = (unsigned char)(0xC0 | (c >> 6));
        bytes[1] = (unsigned char)(0x80 | (c & 0x3F));
        n = 2;
    } else if (c < 0x10000) {
        bytes[0] = (unsigned char)(0xE0 | (c >> 12));
        bytes[1] = (unsigned char)(0x80 | ((c >> 6) & 0x3F));
        bytes[2] = (unsigned char)(0x80 | (c & 0x3F));
        n = 3;
    } else {
        bytes[0] = (unsigned char)(0xF0 | (c >> 18));
        bytes[1] = (unsigned char)(0x80 | ((c >> 12) & 0x3F));
        bytes[2] = (unsigned char)(0x80 | ((c >> 6) & 0x3F));
        bytes[3] = (unsigned char)(0x80 | (c & 0x3F));
        n = 4;
    }

    if (a->word_full || a->word_len + n > FILE_UTILS_MAX_WORD) {
        a->word_full = 1;
        return;
    }
    memcpy(a->word + a->word_len, bytes, n);
    a->word_len += n;
}

/* Ends the current word, if there is one: counts it and stores it */
static void word_end(Analyzer *a) {
    if (a->in_word) {
        a->stats->words++;
        if (a->words != NULL && a->error == FILE_UTILS_OK) {
            a->word[a->word_len] = '\0';
            if (word_list_add(a->words, a->word) != WORD_LIST_OK) {
                a->error = FILE_UTILS_ERR_NOMEM;
            }
        }
    }
    a->in_word = 0;
    a->word_len = 0;
    a->word_full = 0;
    a->joiner = 0;
}

/* Processes one (already decoded) character */
static void analyze_char(Analyzer *a, unsigned long c) {
    a->stats->characters++;
    if (c == '\n') {
        a->stats->lines++;
    }
    a->last_char = c;

    switch (classify(c)) {
    case CHAR_WORD:
        if (a->joiner != 0) {           /* the pending ' or - is inside the word */
            word_append(a, (unsigned long)a->joiner);
            a->joiner = 0;
        }
        word_append(a, to_lower(c));
        a->in_word = 1;
        break;
    case CHAR_JOINER:
        if (a->in_word && a->joiner == 0) {
            /* keep it pending: it only belongs to the word if a letter follows */
            a->joiner = (c == '-' || c == 0x2010 || c == 0x2011) ? '-' : '\'';
        } else {
            word_end(a);
        }
        break;
    case CHAR_SEPARATOR:
    default:
        word_end(a);
        break;
    }
}

/* ---------------------------------------------------------------------- */
/* UTF-8 decoding                                                          */
/* ---------------------------------------------------------------------- */

/* Length of the UTF-8 sequence that starts with byte b, or 0 if b cannot
 * start one (continuation byte 10xxxxxx or invalid byte) */
static int utf8_length(unsigned char b) {
    if (b < 0x80) {
        return 1;                       /* 0xxxxxxx: ASCII */
    }
    if (b >= 0xC2 && b <= 0xDF) {
        return 2;                       /* 110xxxxx 10xxxxxx */
    }
    if (b >= 0xE0 && b <= 0xEF) {
        return 3;                       /* 1110xxxx 10xxxxxx 10xxxxxx */
    }
    if (b >= 0xF0 && b <= 0xF4) {
        return 4;                       /* 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
    }
    return 0;
}

/* The pending bytes are not valid UTF-8: take each one as a Latin-1 character */
static void utf8_flush(Analyzer *a) {
    for (int i = 0; i < a->utf8_len; i++) {
        analyze_char(a, a->utf8[i]);
    }
    a->utf8_len = 0;
}

/* Processes one byte of the text */
static void analyze_byte(Analyzer *a, unsigned char b) {
    int len;

    if (a->utf8_len > 0) {                      /* inside a multi-byte sequence */
        if ((b & 0xC0) == 0x80) {               /* continuation byte */
            a->utf8[a->utf8_len++] = b;
            if (a->utf8_len == a->utf8_need) {
                /* bits of the first byte (7 - need of them), then 6 bits from
                 * each continuation byte */
                unsigned long c = a->utf8[0] & (0x7F >> a->utf8_need);
                for (int i = 1; i < a->utf8_need; i++) {
                    c = (c << 6) | (a->utf8[i] & 0x3F);
                }
                a->utf8_len = 0;
                analyze_char(a, c);
            }
            return;
        }
        utf8_flush(a);                          /* interrupted sequence */
    }

    len = utf8_length(b);
    if (len <= 1) {
        analyze_char(a, b);                     /* ASCII, or a Latin-1 byte */
    } else {
        a->utf8[0] = b;
        a->utf8_len = 1;
        a->utf8_need = len;
    }
}

/* ---------------------------------------------------------------------- */
/* Analysis                                                                */
/* ---------------------------------------------------------------------- */

static void analyzer_init(Analyzer *a, TextStats *stats, WordList *words) {
    memset(a, 0, sizeof *a);
    a->stats = stats;
    a->words = words;
    a->error = FILE_UTILS_OK;

    stats->lines = 0;
    stats->words = 0;
    stats->characters = 0;
}

static int analyzer_finish(Analyzer *a) {
    utf8_flush(a);                      /* incomplete sequence at the very end */
    word_end(a);
    if (a->stats->characters > 0 && a->last_char != '\n') {
        a->stats->lines++;              /* last line without '\n' */
    }
    return a->error;
}

int file_utils_analyze_stream(FILE *stream, TextStats *stats, WordList *words) {
    Analyzer a;
    unsigned char buffer[READ_CHUNK];
    size_t n;

    if (stream == NULL || stats == NULL) {
        return FILE_UTILS_ERR_ARG;
    }
    analyzer_init(&a, stats, words);

    while ((n = fread(buffer, 1, sizeof buffer, stream)) > 0) {
        for (size_t i = 0; i < n; i++) {
            analyze_byte(&a, buffer[i]);
        }
    }
    if (ferror(stream)) {
        return FILE_UTILS_ERR_READ;
    }
    return analyzer_finish(&a);
}

int file_utils_analyze_file(const char *path, TextStats *stats, WordList *words) {
    FILE *file;
    int rc;

    if (path == NULL || stats == NULL) {
        return FILE_UTILS_ERR_ARG;
    }
    /* binary mode: on Windows '\r' is not removed, so the counts are the same
     * on every system (and the same as 'wc') */
    file = fopen(path, "rb");
    if (file == NULL) {
        return FILE_UTILS_ERR_OPEN;
    }
    rc = file_utils_analyze_stream(file, stats, words);
    fclose(file);
    return rc;
}

int file_utils_analyze_string(const char *text, TextStats *stats, WordList *words) {
    Analyzer a;

    if (text == NULL || stats == NULL) {
        return FILE_UTILS_ERR_ARG;
    }
    analyzer_init(&a, stats, words);
    for (; *text != '\0'; text++) {
        analyze_byte(&a, (unsigned char)*text);
    }
    return analyzer_finish(&a);
}

const char *file_utils_strerror(int code) {
    switch (code) {
    case FILE_UTILS_OK:
        return "success";
    case FILE_UTILS_ERR_ARG:
        return "invalid argument";
    case FILE_UTILS_ERR_OPEN:
        return "cannot open file";
    case FILE_UTILS_ERR_READ:
        return "read error";
    case FILE_UTILS_ERR_NOMEM:
        return "out of memory";
    default:
        return "unknown error";
    }
}
