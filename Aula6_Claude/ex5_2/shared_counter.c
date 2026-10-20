/* Exercise 5.2 - two threads share a counter: one increments it every second
 * and sets a flag; the other polls the flag and prints the counter whenever it
 * changes. The program ends after N_ITERATIONS updates, a value chosen at
 * compile time ("make N_ITERATIONS=10").
 *
 * Both threads read and write the counter and the flag, so every access is
 * done with a mutex locked. Without it there would be a data race: the
 * compiler and the CPU may reorder or cache the accesses, and the printer
 * could see the flag set before the new counter value. */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifndef N_ITERATIONS
#define N_ITERATIONS 5
#endif

#define POLL_PERIOD_MS 10

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static int counter = 0;
static int updated = 0; /* set by the updater, cleared by the printer */

static void *updater(void *arg)
{
    (void)arg;
    for (int i = 1; i <= N_ITERATIONS; i++) {
        sleep(1);
        pthread_mutex_lock(&lock);
        counter++;
        updated = 1;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

static void *printer(void *arg)
{
    (void)arg;
    const struct timespec poll_period = {0, POLL_PERIOD_MS * 1000000L};
    int value = 0;
    while (value < N_ITERATIONS) {
        int changed = 0;
        pthread_mutex_lock(&lock);
        if (updated) {
            updated = 0;
            value = counter;
            changed = 1;
        }
        pthread_mutex_unlock(&lock);

        if (changed)
            printf("Printer: counter = %d\n", value);
        else
            nanosleep(&poll_period, NULL); /* so that polling does not use 100% CPU */
    }
    return NULL;
}

int main(void)
{
    setvbuf(stdout, NULL, _IOLBF, 0);
    printf("Main: counting to %d (N_ITERATIONS), one update per second\n", N_ITERATIONS);

    pthread_t t_updater, t_printer;
    int err = pthread_create(&t_updater, NULL, updater, NULL);
    if (err == 0)
        err = pthread_create(&t_printer, NULL, printer, NULL);
    if (err != 0) {
        fprintf(stderr, "pthread_create: %s\n", strerror(err));
        return EXIT_FAILURE;
    }

    pthread_join(t_updater, NULL);
    pthread_join(t_printer, NULL);
    printf("Main: both threads finished, exiting.\n");
    return EXIT_SUCCESS;
}
