#ifndef UTILS_H
#define UTILS_H

/* Helpers shared by the exercises of parts 1 and 2. */

/* Returns a malloc'ed copy of s between single quotes, safe to paste into a
 * /bin/sh command line (each ' becomes '\''). Returns NULL if malloc fails. */
char *shell_quote(const char *s);

/* Reads a folder count between 1 and 99 from text.
 * Returns 0 on success, -1 if text is not a number in that range. */
int parse_count(const char *text, int *n);

/* Prints "question [y/N] " and reads one line from stdin.
 * Returns 1 if the answer starts with 'y' or 'Y', 0 otherwise (EOF = no). */
int ask_yes_no(const char *question);

/* Runs a program as a child process: fork() + execvp(argv[0], argv) + waitpid().
 * argv must end with NULL. Returns the program's exit status, or -1 if it
 * could not be started or was killed by a signal. */
int run_command(const char *const argv[]);

#endif
