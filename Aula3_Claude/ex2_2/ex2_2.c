/*
 * Exercise 2.2 - Swap with pointers / pointer-to-pointer
 *
 * swap_int()     swaps two int values, receiving pointers to them (int *).
 * swap_int_ptr() swaps two pointers, receiving pointers to them (int **):
 *                the pointers exchange targets, the int values do not move.
 */
#include <stdio.h>
#include <stdlib.h>

/* Swaps the integers pointed to by a and b. */
void swap_int(int *a, int *b) {
    int tmp;

    if (a == NULL || b == NULL) {
        return;
    }
    tmp = *a;
    *a = *b;
    *b = tmp;
}

/* Swaps the pointers pointed to by a and b. */
void swap_int_ptr(int **a, int **b) {
    int *tmp;

    if (a == NULL || b == NULL) {
        return;
    }
    tmp = *a;
    *a = *b;
    *b = tmp;
}

/* Prints the result of one check. Returns 1 on failure, 0 on success. */
static int check(const char *what, int ok) {
    printf("  %-42s [%s]\n", what, ok ? "OK" : "FAIL");
    return ok ? 0 : 1;
}

int main(void) {
    int x = 3, y = 7;
    int *px = &x;
    int *py = &y;
    int errors = 0;

    /* 1) Swap two values */
    printf("swap_int(&x, &y)\n");
    printf("  before: x = %d, y = %d\n", x, y);
    swap_int(&x, &y);
    printf("  after:  x = %d, y = %d\n", x, y);
    errors += check("x and y exchanged their values", x == 7 && y == 3);

    swap_int(&x, &x);
    errors += check("swapping x with itself keeps its value", x == 7);

    /* 2) Swap two pointers: the pointers move, the values stay */
    printf("swap_int_ptr(&px, &py)\n");
    printf("  before: px -> %s (%d), py -> %s (%d)\n",
           px == &x ? "x" : "y", *px, py == &x ? "x" : "y", *py);
    swap_int_ptr(&px, &py);
    printf("  after:  px -> %s (%d), py -> %s (%d)\n",
           px == &x ? "x" : "y", *px, py == &x ? "x" : "y", *py);
    errors += check("px now points to y and py to x", px == &y && py == &x);
    errors += check("x and y themselves did not change", x == 7 && y == 3);

    /* 3) NULL arguments are ignored */
    swap_int(NULL, &x);
    swap_int_ptr(&px, NULL);
    errors += check("NULL arguments are ignored", x == 7 && px == &y);

    printf("%s\n", errors == 0 ? "All checks passed." : "Some checks FAILED.");
    return errors == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
