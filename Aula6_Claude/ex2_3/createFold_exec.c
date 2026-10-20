/* Exercise 2.3 (a) - createFold again, now running "mkdir" with
 * fork() + execvp() + waitpid() (see run_command() in common/utils.c).
 *
 * exec gives the folder name directly to mkdir as one argument, with no shell
 * in between, so it does not need to be quoted. */
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
        if (asprintf(&folder, "%s%d", argv[1], i) == -1) {
            perror("malloc");
            return EXIT_FAILURE;
        }

        printf("Creating %s\n", folder);
        const char *mkdir_argv[] = {"mkdir", "-p", "--", folder, NULL};
        if (run_command(mkdir_argv) != 0)
            failures++;

        free(folder);
    }

    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
