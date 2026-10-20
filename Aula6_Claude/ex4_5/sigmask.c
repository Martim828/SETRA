/* Exercise 4.5 - sa_mask: SIGUSR1 increments a counter, and its handler
 * takes HANDLER_SECONDS to finish. SIGUSR2 is in the sa_mask of the SIGUSR1
 * handler, so a SIGUSR2 that arrives meanwhile is held back (pending) and
 * its handler only runs after the SIGUSR1 handler has finished.
 *
 *   ./sigmask &                       ./sigmask --no-mask &
 *   kill -USR1 <PID>; kill -USR2 <PID>
 *
 * With --no-mask, sa_mask is empty and the SIGUSR2 handler interrupts the
 * SIGUSR1 handler in the middle. Stop it with "kill <PID>". */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define HANDLER_SECONDS 3

static volatile sig_atomic_t counter = 0;

/* Signal handlers can only call async-signal-safe functions, which leaves
 * out printf(); these two write text and numbers with write() instead. */
static void say(const char *text)
{
    ssize_t ignored = write(STDOUT_FILENO, text, strlen(text));
    (void)ignored;
}

static void say_number(long n)
{
    char digits[24];
    int i = sizeof digits;
    int negative = n < 0;
    unsigned long u = negative ? 0UL - (unsigned long)n : (unsigned long)n;
    do {
        digits[--i] = (char)('0' + u % 10);
        u /= 10;
    } while (u != 0);
    if (negative)
        digits[--i] = '-';
    ssize_t ignored = write(STDOUT_FILENO, digits + i, sizeof digits - (size_t)i);
    (void)ignored;
}

static void on_sigusr1(int sig)
{
    (void)sig;
    counter++;
    say("SIGUSR1 handler: start, counter = ");
    say_number(counter);
    say("\n");

    /* sleep() is async-signal-safe; it is restarted with the time left so the
     * handler lasts HANDLER_SECONDS even if another handler interrupts it */
    unsigned left = HANDLER_SECONDS;
    while (left > 0)
        left = sleep(left);

    say("SIGUSR1 handler: end\n");
}

static void on_sigusr2(int sig)
{
    (void)sig;
    say("SIGUSR2 handler: runs now (counter = ");
    say_number(counter);
    say(")\n");
}

int main(int argc, char *argv[])
{
    int use_mask = 1;
    if (argc == 2 && strcmp(argv[1], "--no-mask") == 0) {
        use_mask = 0;
    } else if (argc != 1) {
        fprintf(stderr, "Usage: %s [--no-mask]\n", argv[0]);
        return EXIT_FAILURE;
    }

    struct sigaction sa1;
    memset(&sa1, 0, sizeof sa1);
    sa1.sa_handler = on_sigusr1;
    sigemptyset(&sa1.sa_mask);
    if (use_mask)
        sigaddset(&sa1.sa_mask, SIGUSR2); /* blocked while on_sigusr1 runs */
    sa1.sa_flags = SA_RESTART;

    struct sigaction sa2;
    memset(&sa2, 0, sizeof sa2);
    sa2.sa_handler = on_sigusr2;
    sigemptyset(&sa2.sa_mask);
    sa2.sa_flags = SA_RESTART;

    if (sigaction(SIGUSR1, &sa1, NULL) == -1 || sigaction(SIGUSR2, &sa2, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    printf("PID %d: SIGUSR2 is %s while the SIGUSR1 handler runs (%d s)\n",
           getpid(), use_mask ? "blocked" : "NOT blocked", HANDLER_SECONDS);
    fflush(stdout);

    for (;;)
        pause();
}
