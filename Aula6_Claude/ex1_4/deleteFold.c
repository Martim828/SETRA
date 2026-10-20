/* Exercise 1.4 - deleteFold: "deleteFold Group 10" removes the folders
 * Group1, Group2, ..., Group10, by running "rm -rf" with system().
 * With -i ("deleteFold -i Group 10") it asks before removing each folder.
 *
 * rmdir only removes empty folders and has no option to remove their
 * contents, so a folder that may contain files is removed with "rm -r". */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "utils.h"

static void usage(const char *prog)
{
    fprintf(stderr, "Usage: %s [-i] <name> <n>   (1 <= n <= 99)\n"
                    "  -i  ask for confirmation before removing each folder\n",
            prog);
}

int main(int argc, char *argv[])
{
    int interactive = 0;
    int opt;
    while ((opt = getopt(argc, argv, "i")) != -1) {
        if (opt != 'i') {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
        interactive = 1;
    }

    int n;
    if (argc - optind != 2 || argv[optind][0] == '\0' ||
        parse_count(argv[optind + 1], &n) != 0) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    const char *name = argv[optind];

    int failures = 0;
    for (int i = 1; i <= n; i++) {
        char *folder = NULL;
        if (asprintf(&folder, "%s%d", name, i) == -1) {
            perror("malloc");
            return EXIT_FAILURE;
        }

        /* lstat (not stat): a symbolic link to a folder is not followed */
        struct stat st;
        if (lstat(folder, &st) != 0 || !S_ISDIR(st.st_mode)) {
            printf("Skipping %s: no such folder\n", folder);
            free(folder);
            continue;
        }

        if (interactive) {
            char *question = NULL;
            if (asprintf(&question, "Remove folder %s?", folder) == -1) {
                perror("malloc");
                return EXIT_FAILURE;
            }
            int yes = ask_yes_no(question);
            free(question);
            if (!yes) {
                printf("Kept %s\n", folder);
                free(folder);
                continue;
            }
        }

        char *quoted = shell_quote(folder);
        char *command = NULL;
        if (quoted == NULL || asprintf(&command, "rm -rf -- %s", quoted) == -1) {
            perror("malloc");
            return EXIT_FAILURE;
        }

        printf("Removing %s\n", folder);
        fflush(stdout);
        if (system(command) != 0)
            failures++;

        free(command);
        free(quoted);
        free(folder);
    }

    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
