#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{

    char string[100];
    int n;
    printf("Enter a number between 1 and 99: ");
    scanf("%d", &n);
    printf("Enter a string: ");
    scanf("%s", string);

    printf("%d", n);

    for (int i = 1; i <= n; i++)
    {
        char command[150];
        snprintf(command, sizeof(command), "mkdir -p \"%s%d\"", string, i);
        printf("%s\n", command);
        system(command);
    }
    char command[150];
    snprintf(command, sizeof(command), "mkdir -p \"%s%d\"", string, n);
    system(command);

    return 0;
}