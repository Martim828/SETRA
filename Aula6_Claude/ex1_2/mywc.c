/* Exercise 1.2 - mywc: prints the number of characters and words of a text
 * file, by running "wc" with system(). */
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "utils.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *file = argv[1];
    if (access(file, R_OK) != 0) {
        perror(file);
        return EXIT_FAILURE;
    }

    /* the name goes to the shell, so it is quoted: names with spaces or
     * characters like ; or $ still work and cannot run other commands */
    char *quoted = shell_quote(file);
    char *command = NULL;
    if (quoted == NULL ||
        asprintf(&command, "printf 'Characters: '; wc -m < %s && "
                           "printf 'Words: '; wc -w < %s",
                 quoted, quoted) == -1) {
        perror("malloc");
        free(quoted);
        return EXIT_FAILURE;
    }

    /* reading from stdin (<) makes wc print only the number, without the name */
    int status = system(command);
    free(command);
    free(quoted);

    if (status == -1) {
        perror("system");
        return EXIT_FAILURE;
    }
    return WIFEXITED(status) ? WEXITSTATUS(status) : EXIT_FAILURE;
}
