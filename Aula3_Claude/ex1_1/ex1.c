/*
 * Exercise 1.1 - Keeping local
 *
 * MyRand() generates a random number in [0, 100] and reports how many times it
 * has been called. Global variables are not allowed, so the counter is a
 * 'static' local variable: it is created once, keeps its value between calls,
 * and is only visible inside MyRand().
 *
 * main() also keeps the sum/difference warm-up that uses the cal module
 * (cal.c / cal.h).
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ex1.h"
#include "cal.h"

#define NUM_CALLS 10    /* how many times main() calls MyRand() */

void MyRand(int *value, int *ncalls) {
    static int count = 0;   /* initialised only once, survives between calls */

    count++;
    if (value != NULL) {
        *value = rand() % (MYRAND_MAX + 1);
    }
    if (ncalls != NULL) {
        *ncalls = count;
    }
}

int main(void) {
    int x, y;
    int value, ncalls;
    int errors = 0;

    /* Warm-up: functions from the cal module */
    printf("Enter two integer values: \n");
    if (scanf("%d %d", &x, &y) == 2) {
        printf("The sum is : %d\n", sum(x, y));
        printf("The difference is : %d\n", sub(x, y));
    } else {
        printf("No values read, skipping the sum/difference.\n");
    }

    /* Exercise 1.1: call MyRand() a few times and check every result */
    srand((unsigned)time(NULL));
    for (int i = 1; i <= NUM_CALLS; i++) {
        MyRand(&value, &ncalls);
        printf("Call %2d: value = %3d, ncalls = %2d\n", i, value, ncalls);
        if (value < 0 || value > MYRAND_MAX || ncalls != i) {
            errors++;
        }
    }

    printf("MyRand: %s\n", errors == 0 ? "OK" : "FAILED");
    return errors == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
