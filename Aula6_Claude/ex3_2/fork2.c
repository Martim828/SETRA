/* Exercise 3.2 - the child works for WORK_SECONDS seconds. Every message shows
 * the time since the start, so the order of events is easy to follow.
 *
 *   fork2             the parent waits for the child with waitpid(), so it
 *                     always finishes after the child
 *   fork2 --no-wait   the parent does not wait: it finishes first and the
 *                     child is left an orphan (its parent PID changes) */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define WORK_SECONDS 3

static struct timespec start;

/* printf with "[time since start]" in front, written and flushed at once */
static void log_msg(const char *fmt, ...)
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    double elapsed = (double)(now.tv_sec - start.tv_sec) +
                     (double)(now.tv_nsec - start.tv_nsec) / 1e9;

    va_list args;
    va_start(args, fmt);
    printf("[%4.1f s] ", elapsed);
    vprintf(fmt, args);
    va_end(args);
    fflush(stdout);
}

int main(int argc, char *argv[])
{
    int wait_for_child = 1;
    if (argc == 2 && strcmp(argv[1], "--no-wait") == 0) {
        wait_for_child = 0;
    } else if (argc != 1) {
        fprintf(stderr, "Usage: %s [--no-wait]\n", argv[0]);
        return EXIT_FAILURE;
    }

    clock_gettime(CLOCK_MONOTONIC, &start);
    log_msg("Parent (PID %d) starting\n", getpid());

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        log_msg("Child (PID %d, parent %d) starting, working for %d s\n",
                getpid(), getppid(), WORK_SECONDS);
        for (int i = 1; i <= WORK_SECONDS; i++) {
            sleep(1); /* stands for some real work */
            log_msg("Child working... %d/%d (my parent is now PID %d)\n",
                    i, WORK_SECONDS, getppid());
        }
        log_msg("Process PID %d is finishing now\n", getpid());
        return EXIT_SUCCESS;
    }

    log_msg("Parent (PID %d) created child PID %d\n", getpid(), pid);

    if (wait_for_child) {
        log_msg("Parent waiting for child %d\n", pid);
        int status;
        if (waitpid(pid, &status, 0) == -1)
            perror("waitpid");
        else if (WIFEXITED(status))
            log_msg("Parent: child %d exited with status %d\n", pid, WEXITSTATUS(status));
    }

    log_msg("Process PID %d is finishing now\n", getpid());
    return EXIT_SUCCESS;
}
