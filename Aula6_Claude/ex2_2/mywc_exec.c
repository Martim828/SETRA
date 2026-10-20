/* Exercise 2.2 - mywc again, now with fork() + execlp() instead of system().
 *
 * The child runs "wc -w -m" with the file as its stdin and its stdout
 * connected to a pipe; the parent reads the two numbers from the pipe and
 * prints them in the same format as exercise 1.2. */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int input = open(argv[1], O_RDONLY);
    if (input == -1) {
        perror(argv[1]);
        return EXIT_FAILURE;
    }

    int fd[2]; /* fd[0]: read end, fd[1]: write end */
    if (pipe(fd) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        /* child: stdin <- file, stdout -> pipe, then become wc */
        dup2(input, STDIN_FILENO);
        dup2(fd[1], STDOUT_FILENO);
        close(input);
        close(fd[0]);
        close(fd[1]);
        execlp("wc", "wc", "-w", "-m", (char *)NULL);
        perror("execlp wc");
        _exit(127);
    }

    /* parent: wc always prints the words before the characters */
    close(input);
    close(fd[1]);
    FILE *from_wc = fdopen(fd[0], "r");
    long words = 0, chars = 0;
    int got = (from_wc != NULL) ? fscanf(from_wc, "%ld %ld", &words, &chars) : 0;
    if (from_wc != NULL)
        fclose(from_wc);

    int status;
    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        return EXIT_FAILURE;
    }
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0 || got != 2) {
        fprintf(stderr, "wc failed\n");
        return EXIT_FAILURE;
    }

    printf("Characters: %ld\n", chars);
    printf("Words: %ld\n", words);
    return EXIT_SUCCESS;
}
