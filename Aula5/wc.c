#include <stdio.h>
#include <stdlib.h>

int main(void) {
    printf("Enter the name of the file: ");

    char filename[100];
    scanf("%s", filename);
     
    FILE *file = fopen(filename, "r");
    
    char command[105];
    snprintf(command,sizeof(command), "wc \"%s\"", filename);
    system(command);

    fclose(file);

    return 0;
}
