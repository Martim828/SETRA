/* Exercise 2.1 - myPS again, now with execlp() instead of system().
 *
 * exec replaces this program with ps: no shell is started, and nothing after
 * the call runs unless exec fails. */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    /* execlp searches PATH for "ps"; argv[0] = "ps", argv[1] = "aux" */
    execlp("ps", "ps", "aux", (char *)NULL);

    perror("execlp ps");
    return EXIT_FAILURE;
}
