/*
 * Exercise 2.3 - String reversal with pointer arithmetic
 *
 * str_reverse() reverses a string in place using only pointers (no indexes).
 *
 * Usage: ex2_3 [text ...]   reverses and prints each argument; without
 *                           arguments it runs a set of checks.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 64

/* Reverses the NUL-terminated string s in place. */
void str_reverse(char *s) {
    char *end;

    if (s == NULL || *s == '\0') {
        return;             /* nothing to reverse (and s - 1 would be invalid) */
    }

    end = s;
    while (*(end + 1) != '\0') {    /* move end to the last character */
        end++;
    }

    while (s < end) {               /* swap the ends and walk to the middle */
        char tmp = *s;
        *s++ = *end;
        *end-- = tmp;
    }
}

/* Reverses a copy of input and compares it with expected.
 * Returns 1 on failure, 0 on success. */
static int check(const char *input, const char *expected) {
    char buffer[MAX_LEN];   /* string literals are read-only: work on a copy */
    int ok;

    if (strlen(input) >= sizeof buffer) {
        return 1;
    }
    strcpy(buffer, input);
    str_reverse(buffer);
    ok = (strcmp(buffer, expected) == 0);
    printf("  \"%s\" -> \"%s\"  [%s]\n", input, buffer, ok ? "OK" : "FAIL");
    return ok ? 0 : 1;
}

int main(int argc, char *argv[]) {
    static const struct {
        const char *input;
        const char *expected;
    } cases[] = {
        { "",              ""              },
        { "a",             "a"             },
        { "ab",            "ba"            },
        { "abc",           "cba"           },
        { "racecar",       "racecar"       },
        { "Hello, world!", "!dlrow ,olleH" },
        { "SETR 2026",     "6202 RTES"     },
    };
    char text[] = "pointer arithmetic";
    int errors = 0;

    if (argc > 1) {     /* the argv strings can be modified */
        for (int i = 1; i < argc; i++) {
            str_reverse(argv[i]);
            printf("%s\n", argv[i]);
        }
        return EXIT_SUCCESS;
    }

    printf("str_reverse() checks:\n");
    for (size_t i = 0; i < sizeof cases / sizeof cases[0]; i++) {
        errors += check(cases[i].input, cases[i].expected);
    }

    /* Reversing twice gives back the original string */
    str_reverse(text);
    str_reverse(text);
    if (strcmp(text, "pointer arithmetic") != 0) {
        printf("  reversing twice changed the string\n");
        errors++;
    }

    str_reverse(NULL);  /* must not crash */

    printf("%s\n", errors == 0 ? "All checks passed." : "Some checks FAILED.");
    return errors == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
