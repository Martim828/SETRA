/* Exercise 5.1 - the main thread creates two threads and waits for both with
 * pthread_join(). Thread 1 works for 2 s and thread 2 for 3 s. */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct thread_info {
    int id;
    unsigned seconds; /* how long the thread "works" */
};

static void *thread_main(void *arg)
{
    const struct thread_info *info = arg;
    printf("Thread %d starting\n", info->id);
    sleep(info->seconds);
    printf("Thread %d finishing\n", info->id);
    return NULL; /* same as pthread_exit(NULL) */
}

int main(void)
{
    /* one line at a time, even when the output goes to a pipe or file */
    setvbuf(stdout, NULL, _IOLBF, 0);

    struct thread_info info[2] = {{1, 2}, {2, 3}};
    pthread_t threads[2];

    printf("Main thread: creating two threads named 1 and 2\n");
    for (int i = 0; i < 2; i++) {
        /* each thread gets its own struct: passing &i would be a bug, since
         * i changes before the thread reads it */
        int err = pthread_create(&threads[i], NULL, thread_main, &info[i]);
        if (err != 0) {
            fprintf(stderr, "pthread_create: %s\n", strerror(err));
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < 2; i++)
        pthread_join(threads[i], NULL);

    printf("Main thread: both threads finished, exiting.\n");
    return EXIT_SUCCESS;
}
