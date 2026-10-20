/* Exercise 1.1 - myPS: runs "ps aux" with system(). */
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(void)
{
    /* system() starts "/bin/sh -c 'ps aux'" and waits for it to finish */
    int status = system("ps aux");
    if (status == -1) {
        perror("system");
        return EXIT_FAILURE;
    }

    /* the return value is a wait() status: return the exit code of ps */
    return WIFEXITED(status) ? WEXITSTATUS(status) : EXIT_FAILURE;
}
