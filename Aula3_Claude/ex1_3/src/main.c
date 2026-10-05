/*
 * Exercise 1.3 - Specifying interfaces / encapsulation
 *
 * Same checks as in exercise 1.2, but maxConnections is now hidden inside the
 * configData module and main() can only use the interface functions.
 */
#include <stdio.h>
#include <stdlib.h>
#include "configData.h"

/* Prints the value returned by configGetMaxConnections() and compares it with
 * the expected one. Returns 1 if the value is wrong, 0 otherwise. */
static int check(const char *step, unsigned expected) {
    unsigned value = configGetMaxConnections();
    int ok = (value == expected);

    printf("%-36s maxConnections = %5u  [%s]\n", step, value, ok ? "OK" : "FAIL");
    return ok ? 0 : 1;
}

int main(void) {
    int errors = 0;

    configInit();
    errors += check("after configInit():", 0);

    configSetMaxConnections(100);
    errors += check("after configSetMaxConnections(100):", 100);

    configSetMaxConnections((uint16_t)(configGetMaxConnections() + 50));
    errors += check("after get() + 50:", 150);

    configSetMaxConnections(UINT16_MAX);
    errors += check("after set(UINT16_MAX):", 65535);

    configInit();
    errors += check("after configInit() again:", 0);

    /* maxConnections = 10;   <- no longer compiles: 'maxConnections' undeclared */

    printf("%s\n", errors == 0 ? "All checks passed." : "Some checks FAILED.");
    return errors == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
