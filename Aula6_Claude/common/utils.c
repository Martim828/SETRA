#include "utils.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

char *shell_quote(const char *s)
{
    size_t quotes = 0;
    for (const char *p = s; *p != '\0'; p++)
        if (*p == '\'')
            quotes++;

    /* each ' grows to 4 characters, plus the two outer quotes and the '\0' */
    char *out = malloc(strlen(s) + 3 * quotes + 3);
    if (out == NULL)
        return NULL;

    char *q = out;
    *q++ = '\'';
    for (const char *p = s; *p != '\0'; p++) {
        if (*p == '\'') {
            memcpy(q, "'\\''", 4);
            q += 4;
        } else {
            *q++ = *p;
        }
    }
    *q++ = '\'';
    *q = '\0';
    return out;
}

int parse_count(const char *text, int *n)
{
    char *end;
    errno = 0;
    long value = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || value < 1 || value > 99)
        return -1;
    *n = (int)value;
    return 0;
}

int ask_yes_no(const char *question)
{
    printf("%s [y/N] ", question);
    fflush(stdout);

    char line[64];
    if (fgets(line, sizeof line, stdin) == NULL) {
        printf("\n");
        return 0;
    }
    /* throw away the rest of a very long answer */
    if (strchr(line, '\n') == NULL) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
            ;
    }
    return line[0] == 'y' || line[0] == 'Y';
}

int run_command(const char *const argv[])
{
    /* otherwise anything still in our stdout buffer would be printed by the
     * child too, since fork() copies the buffer */
    fflush(stdout);

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    }
    if (pid == 0) {
        /* execvp does not change the strings; its prototype is just older than const */
        execvp(argv[0], (char *const *)argv);
        perror(argv[0]); /* exec only returns if it failed */
        _exit(127);
    }

    int status;
    while (waitpid(pid, &status, 0) == -1) {
        if (errno != EINTR) {
            perror("waitpid");
            return -1;
        }
    }
    return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}
