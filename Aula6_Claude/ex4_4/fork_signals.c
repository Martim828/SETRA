/* Exercise 4.4 - the parent sends SIGUSR1 to its child ten times, one per
 * second, with kill(); the child finishes after receiving the ten signals. */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define N_SIGNALS 10

static volatile sig_atomic_t received = 0;

static void on_sigusr1(int sig)
{
    (void)sig;
    received++;
}

int main(void)
{
    /* The handler is installed and SIGUSR1 is blocked BEFORE fork(), and the
     * child inherits both. Otherwise the parent could send the first signal
     * before the child had installed its handler, and the default action
     * of SIGUSR1 would kill the child. A blocked signal stays pending until
     * the child is ready for it. */
    struct sigaction sa;
    memset(&sa, 0, sizeof sa);
    sa.sa_handler = on_sigusr1;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    sigset_t usr1, old_mask;
    sigemptyset(&usr1);
    sigaddset(&usr1, SIGUSR1);
    sigprocmask(SIG_BLOCK, &usr1, &old_mask);

    setvbuf(stdout, NULL, _IOLBF, 0);

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        printf("Child  (PID %d): waiting for %d SIGUSR1\n", getpid(), N_SIGNALS);
        int shown = 0;
        while (shown < N_SIGNALS) {
            /* unblocks SIGUSR1 and waits for it, in one atomic step */
            while (received == shown)
                sigsuspend(&old_mask);
            while (shown < received) {
                shown++;
                printf("Child  (PID %d): received SIGUSR1 #%d\n", getpid(), shown);
            }
        }
        printf("Child  (PID %d): got all %d signals, finishing\n", getpid(), N_SIGNALS);
        return EXIT_SUCCESS;
    }

    printf("Parent (PID %d): child is PID %d\n", getpid(), pid);
    for (int i = 1; i <= N_SIGNALS; i++) {
        sleep(1);
        printf("Parent (PID %d): sending SIGUSR1 #%d\n", getpid(), i);
        if (kill(pid, SIGUSR1) == -1) {
            perror("kill");
            break;
        }
    }

    /* wait for the child, so that it is not left as a zombie and its last
     * messages come out before the shell prompt */
    int status;
    waitpid(pid, &status, 0);
    printf("Parent (PID %d): child finished, finishing\n", getpid());
    return EXIT_SUCCESS;
}
