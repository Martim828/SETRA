/* Exercise 4.3 - the timer of exercise 4.2, but every other signal is ignored
 * instead of terminating it.
 *
 *   SIGINT  (CTRL+C, kill -INT <PID>)  reset to 0 and keep counting
 *   SIGHUP  (kill -HUP <PID>)          suspend the counting
 *   SIGCONT (kill -CONT <PID>)         resume the counting
 *   anything else (kill -USR1, kill -TERM, CTRL+\, ...) is ignored
 *
 * SIGKILL and SIGSTOP cannot be caught, blocked or ignored, so the only way
 * to stop it is "kill -KILL <PID>".
 *
 * The handler only sets flags; the main loop does the printing, since
 * printf() must not be called from a signal handler. */
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static volatile sig_atomic_t reset_requested = 0;
static volatile sig_atomic_t counting = 1;

static void handler(int sig)
{
    switch (sig) {
    case SIGINT:
        reset_requested = 1;
        counting = 1;
        break;
    case SIGHUP:
        counting = 0;
        break;
    case SIGCONT:
        counting = 1;
        break;
    default:
        break;
    }
}

/* Sleeps one second. Returns 1 if the second went by, or 0 if a signal reset
 * or suspended the counting in the meantime. */
static int wait_one_second(void)
{
    struct timespec left = {1, 0};
    /* a handled signal interrupts nanosleep (EINTR), which saves the time
     * still missing in "left" */
    while (nanosleep(&left, &left) == -1) {
        if (errno != EINTR || reset_requested || !counting)
            return 0;
    }
    return 1;
}

int main(void)
{
    sigset_t handled; /* the three signals used by the timer */
    sigemptyset(&handled);
    const int signals[] = {SIGINT, SIGHUP, SIGCONT};
    for (size_t i = 0; i < sizeof signals / sizeof signals[0]; i++)
        sigaddset(&handled, signals[i]);

    /* 1. Every signal that can be ignored gets SIG_IGN. sigaction() fails
     *    (EINVAL) for SIGKILL, SIGSTOP and the real-time signals that glibc
     *    keeps for itself, and those are just skipped. The signals sent by
     *    the CPU when the program itself crashes (SIGSEGV, SIGFPE, ...) keep
     *    their default action: ignoring them would make a crash loop forever. */
    struct sigaction ignore;
    memset(&ignore, 0, sizeof ignore);
    ignore.sa_handler = SIG_IGN;
    sigemptyset(&ignore.sa_mask);
    for (int sig = 1; sig < NSIG; sig++) {
        if (sig == SIGSEGV || sig == SIGBUS || sig == SIGFPE || sig == SIGILL ||
            sig == SIGTRAP || sig == SIGABRT || sig == SIGSYS)
            continue;
        sigaction(sig, &ignore, NULL);
    }

    /* 2. The three timer signals get the handler. sa_mask holds the signals
     *    blocked while the handler runs: with all three in it, one handler
     *    can never be interrupted by another of them. */
    struct sigaction sa;
    memset(&sa, 0, sizeof sa);
    sa.sa_handler = handler;
    sa.sa_mask = handled;
    sa.sa_flags = 0;
    for (size_t i = 0; i < sizeof signals / sizeof signals[0]; i++) {
        if (sigaction(signals[i], &sa, NULL) == -1) {
            perror("sigaction");
            return EXIT_FAILURE;
        }
    }

    /* one line at a time, even when the output goes to a pipe or file */
    setvbuf(stdout, NULL, _IOLBF, 0);
    printf("Timer PID %d: SIGINT = reset, SIGHUP = suspend, SIGCONT = resume, "
           "other signals ignored\n", getpid());

    unsigned seconds = 0;
    int was_counting = 1;
    printf("%u s\n", seconds);

    for (;;) {
        /* Signals are blocked while the flags are checked and are let in
         * again by sigsuspend(), which also starts waiting in the same atomic
         * step. With "if (...) pause();" a signal arriving between the test
         * and pause() would be missed and the program would hang. */
        sigset_t old_mask;
        sigprocmask(SIG_BLOCK, &handled, &old_mask);
        while (!counting && !reset_requested && !was_counting)
            sigsuspend(&old_mask);
        int reset = reset_requested;
        int now_counting = counting;
        reset_requested = 0;
        sigprocmask(SIG_SETMASK, &old_mask, NULL);

        if (reset) {
            seconds = 0;
            printf("SIGINT: reset, counting again from 0\n");
        } else if (now_counting && !was_counting) {
            printf("SIGCONT: counting resumed at %u s\n", seconds);
        } else if (!now_counting && was_counting) {
            printf("SIGHUP: counting suspended at %u s\n", seconds);
        }
        was_counting = now_counting;

        if (now_counting && wait_one_second()) {
            seconds++;
            printf("%u s\n", seconds);
        }
    }
}
