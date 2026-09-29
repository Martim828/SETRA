#include <stdio.h>
#include <stdlib.h>

#include "ex1.h"
#include "cal.h"


int sum(int x, int y) {
    return x + y;
}

int sub(int x, int y) {
    return x - y;
}

void MyRand(int *value, int *ncalls) {
    *value = rand() % 100 + 1;
    (*ncalls)++;
}


int main() {
    int x, y;
    printf("Enter two integer values: \n");
    scanf("%d %d", &x, &y);
    printf("The sum is : %d\n", sum(x,y));
    printf("The difference is : %d\n", sub(x,y));
    printf("Numero aleatorio de 1 a 100: %d\n", MyRand(&x, &y));
}

