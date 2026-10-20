/* Exercise 4.1 - catches SIGINT, SIGHUP, SIGUSR1 and SIGCONT with sigaction()
 * and prints which signal arrived.
 *
 *   ./catch_signals &        (prints its PID)
 *   kill -USR1 <PID>         kill -HUP <PID>        kill -CONT <PID>
 *   CTRL+C sends SIGINT when the program runs in the foreground.
 *   SIGINT is caught, so stop it with "kill <PID>" (SIGTERM) or "kill -KILL <PID>". */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void handler(int sig)
{
    const char *msg;
    switch (sig) {
    case SIGINT:  msg = "Received SIGINT\n";  break;
    case SIGHUP:  msg = "Received SIGHUP\n";  break;
    case SIGUSR1: msg = "Received SIGUSR1\n"; break;
    case SIGCONT: msg = "Received SIGCONT\n"; break;
    default:      msg = "Received an unexpected signal\n"; break;
    }
    /* printf is not async-signal-safe (it may be interrupted in the middle of
     * another printf); write() is */
    ssize_t ignored = write(STDOUT_FILENO, msg, strlen(msg));
    (void)ignored;
}

int main(void)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof sa);
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART; /* interrupted syscalls continue after the handler */

    const int signals[] = {SIGINT, SIGHUP, SIGUSR1, SIGCONT};
    for (size_t i = 0; i < sizeof signals / sizeof signals[0]; i++) {
        if (sigaction(signals[i], &sa, NULL) == -1) {
            perror("sigaction");
            return EXIT_FAILURE;
        }
    }

    printf("PID %d waiting for SIGINT, SIGHUP, SIGUSR1 or SIGCONT\n", getpid());
    fflush(stdout);

    for (;;)
        pause(); /* sleeps until a signal is handled */
}
