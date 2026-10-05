/*
 * Exercise 1.2 - Sharing variables across modules
 *
 * main() initialises maxConnections with configInit() and then reads and
 * modifies it directly (the variable is 'extern' in configData.h).
 */
#include <stdio.h>
#include <stdlib.h>
#include "configData.h"

/* Prints maxConnections and compares it with the expected value.
 * Returns 1 if the value is wrong, 0 otherwise. */
static int check(const char *step, unsigned expected) {
    int ok = (maxConnections == expected);

    printf("%-34s maxConnections = %5u  [%s]\n",
           step, (unsigned)maxConnections, ok ? "OK" : "FAIL");
    return ok ? 0 : 1;
}

int main(void) {
    int errors = 0;

    configInit();
    errors += check("after configInit():", 0);

    maxConnections = 100;                       /* direct write */
    errors += check("after maxConnections = 100:", 100);

    maxConnections += 50;                       /* direct read-modify-write */
    errors += check("after maxConnections += 50:", 150);

    maxConnections = UINT16_MAX;
    errors += check("after maxConnections = UINT16_MAX:", 65535);

    /* Direct access cannot be validated: the value silently wraps around */
    maxConnections++;
    errors += check("after maxConnections++:", 0);

    maxConnections = 20;
    configInit();
    errors += check("after configInit() again:", 0);

    printf("%s\n", errors == 0 ? "All checks passed." : "Some checks FAILED.");
    return errors == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
