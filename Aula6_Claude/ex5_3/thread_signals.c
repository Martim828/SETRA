/* Exercise 5.3 - two threads wait for a signal. The main thread sends SIGUSR1
 * to each of them with pthread_kill(), one second apart, and finishes after
 * both have finished (pthread_join).
 *
 * A signal sent with pthread_kill() is delivered to that thread only: the
 * handler runs in that thread, which then wakes up. Signal handlers are shared
 * by the whole process, so the handler is installed once, before the threads
 * are created. */
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* one copy per thread: the handler marks the thread it interrupted */
static _Thread_local volatile sig_atomic_t received_signal = 0;

static void handler(int sig)
{
    received_signal = sig;
}

static void *thread_main(void *arg)
{
    int id = *(const int *)arg;

    /* SIGUSR1 is blocked in this thread (the mask is inherited from main).
     * sigsuspend() unblocks it and waits, in one atomic step, like a pause()
     * without its race: with "while (!received_signal) pause();", a signal
     * arriving after the test but before pause() would be missed and the
     * thread would wait forever. */
    sigset_t wait_mask;
    pthread_sigmask(SIG_BLOCK, NULL, &wait_mask); /* current mask */
    sigdelset(&wait_mask, SIGUSR1);

    printf("Thread %d: waiting for a signal\n", id);
    while (received_signal == 0)
        sigsuspend(&wait_mask);

    printf("Thread %d: received signal %d (%s), terminating\n",
           id, (int)received_signal, strsignal(received_signal));
    return NULL;
}

int main(void)
{
    setvbuf(stdout, NULL, _IOLBF, 0);

    struct sigaction sa;
    memset(&sa, 0, sizeof sa);
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    /* Block SIGUSR1 before creating the threads: they inherit the mask, so
     * a signal sent before a thread reaches sigsuspend() stays pending. The
     * main thread keeps it blocked, so a "kill -USR1 <PID>" sent to the
     * process from outside also goes to one of the waiting threads. */
    sigset_t usr1;
    sigemptyset(&usr1);
    sigaddset(&usr1, SIGUSR1);
    pthread_sigmask(SIG_BLOCK, &usr1, NULL);

    int ids[2] = {1, 2};
    pthread_t threads[2];
    printf("Main: creating threads 1 and 2\n");
    for (int i = 0; i < 2; i++) {
        int err = pthread_create(&threads[i], NULL, thread_main, &ids[i]);
        if (err != 0) {
            fprintf(stderr, "pthread_create: %s\n", strerror(err));
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < 2; i++) {
        sleep(1);
        printf("Main: sending SIGUSR1 to thread %d\n", ids[i]);
        int err = pthread_kill(threads[i], SIGUSR1);
        if (err != 0)
            fprintf(stderr, "pthread_kill: %s\n", strerror(err));
    }

    for (int i = 0; i < 2; i++)
        pthread_join(threads[i], NULL);
    printf("Main: both threads terminated, exiting.\n");
    return EXIT_SUCCESS;
}
