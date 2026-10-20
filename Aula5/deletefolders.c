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

    char command[150];
    snprintf(command, sizeof(command), "rmdir -- *\"%s\"**", string);
    //system(command);
    printf("%s\n", command);

    return 0;
}