/* Exercise 1.3 - createFold: "createFold Group 10" creates the folders
 * Group1, Group2, ..., Group10, by running "mkdir" with system(). */
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

int main(int argc, char *argv[])
{
    int n;
    if (argc != 3 || argv[1][0] == '\0' || parse_count(argv[2], &n) != 0) {
        fprintf(stderr, "Usage: %s <name> <n>   (1 <= n <= 99)\n", argv[0]);
        return EXIT_FAILURE;
    }

    int failures = 0;
    for (int i = 1; i <= n; i++) {
        char *folder = NULL;
        char *quoted = NULL;
        char *command = NULL;

        if (asprintf(&folder, "%s%d", argv[1], i) == -1 ||
            (quoted = shell_quote(folder)) == NULL ||
            asprintf(&command, "mkdir -p -- %s", quoted) == -1) {
            perror("malloc");
            return EXIT_FAILURE;
        }

        /* -p: no error if the folder already exists;
         * --: a name starting with '-' is not taken as an option */
        printf("Creating %s\n", folder);
        fflush(stdout);
        if (system(command) != 0)
            failures++;

        free(command);
        free(quoted);
        free(folder);
    }

    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
