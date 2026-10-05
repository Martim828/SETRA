/*
 * Exercise 2.1 - Dynamic array statistics
 *
 * random_array_stats() allocates an array of N integers, fills it with random
 * values in [1, M] and returns the array together with the average, maximum
 * and minimum of its elements.
 *
 * Usage: ex2_1 [N] [M] [seed]       (defaults: N = 10, M = 100, seed = time)
 */
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DEFAULT_N    10
#define DEFAULT_M    100
#define MAX_PRINTED  20     /* print at most this many elements */

/* Allocates an array of n integers with random values in [1, m] and computes
 * the average (*avg), maximum (*max) and minimum (*min) of its elements.
 * Returns the array, which the caller must release with free(), or NULL if
 * n < 1, m < 1, an output pointer is NULL or there is not enough memory.
 * Note: rand() only goes up to RAND_MAX (32767 with MinGW), so values above
 * RAND_MAX + 1 never appear. */
int *random_array_stats(int n, int m, double *avg, int *max, int *min) {
    int *array;
    long long sum = 0;

    if (n < 1 || m < 1 || avg == NULL || max == NULL || min == NULL) {
        return NULL;
    }

    /* I. memory for n integers */
    array = malloc((size_t)n * sizeof *array);
    if (array == NULL) {
        return NULL;
    }

    /* II. random values in [1, m] and III. statistics, in a single pass */
    for (int i = 0; i < n; i++) {
        array[i] = rand() % m + 1;
        sum += array[i];
        if (i == 0 || array[i] > *max) {
            *max = array[i];
        }
        if (i == 0 || array[i] < *min) {
            *min = array[i];
        }
    }
    *avg = (double)sum / n;

    return array;
}

/* Checks the array and the statistics independently of random_array_stats().
 * Returns the number of problems found. */
static int verify(const int *array, int n, int m, double avg, int max, int min) {
    int errors = 0;
    int max_found = 0, min_found = 0;
    double sum = 0.0, diff;

    for (const int *p = array; p < array + n; p++) {
        if (*p < 1 || *p > m) {
            printf("  element %d is outside [1, %d]\n", *p, m);
            errors++;
        }
        if (*p > max || *p < min) {
            printf("  element %d is outside [min, max]\n", *p);
            errors++;
        }
        max_found |= (*p == max);
        min_found |= (*p == min);
        sum += *p;
    }
    if (!max_found || !min_found) {
        printf("  max or min is not an element of the array\n");
        errors++;
    }
    diff = sum / n - avg;
    if (diff > 1e-9 || diff < -1e-9) {
        printf("  wrong average (expected %f)\n", sum / n);
        errors++;
    }
    return errors;
}

/* Converts a command-line argument to int. Returns 1 on success, 0 otherwise. */
static int parse_int(const char *text, int *value) {
    char *end;
    long v;

    errno = 0;
    v = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || v < INT_MIN || v > INT_MAX) {
        return 0;
    }
    *value = (int)v;
    return 1;
}

int main(int argc, char *argv[]) {
    int n = DEFAULT_N, m = DEFAULT_M, seed = (int)time(NULL);
    int max, min;
    double avg;
    int *array;
    int errors;

    if (argc > 4 || (argc > 1 && !parse_int(argv[1], &n))
                 || (argc > 2 && !parse_int(argv[2], &m))
                 || (argc > 3 && !parse_int(argv[3], &seed))) {
        fprintf(stderr, "Usage: %s [N] [M] [seed]\n", argv[0]);
        return EXIT_FAILURE;
    }
    srand((unsigned)seed);

    array = random_array_stats(n, m, &avg, &max, &min);
    if (array == NULL) {
        fprintf(stderr, "random_array_stats(%d, %d) failed: N and M must be >= 1\n", n, m);
        return EXIT_FAILURE;
    }

    printf("N = %d, M = %d, seed = %d\n", n, m, seed);
    printf("Array:");
    for (int i = 0; i < n && i < MAX_PRINTED; i++) {
        printf(" %d", array[i]);
    }
    if (n > MAX_PRINTED) {
        printf(" ... (%d more)", n - MAX_PRINTED);
    }
    printf("\nAverage = %.3f\nMax     = %d\nMin     = %d\n", avg, max, min);

    errors = verify(array, n, m, avg, max, min);
    free(array);

    /* Invalid sizes must be rejected */
    if (random_array_stats(0, m, &avg, &max, &min) != NULL ||
        random_array_stats(n, 0, &avg, &max, &min) != NULL) {
        printf("  invalid N or M was not rejected\n");
        errors++;
    }

    printf("Check: %s\n", errors == 0 ? "OK" : "FAILED");
    return errors == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
