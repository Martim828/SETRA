/* Exercise 3.1 - fork(): the parent and the child say who they are, their
 * own PID and the PID of the other one, and that they are finishing. */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    printf("Process PID %d is going to fork\n", getpid());

    /* fork() copies the stdout buffer too: without this, a message still in
     * the buffer would be printed twice (once by each process) */
    fflush(stdout);

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        /* fork() returns 0 in the child */
        printf("Child:  my PID is %d, my parent's PID is %d\n", getpid(), getppid());
        printf("Process PID %d is finishing now\n", getpid());
        return EXIT_SUCCESS;
    }

    /* ... and the child's PID in the parent */
    printf("Parent: my PID is %d, my child's PID is %d\n", getpid(), pid);
    printf("Process PID %d is finishing now\n", getpid());
    return EXIT_SUCCESS;
}
