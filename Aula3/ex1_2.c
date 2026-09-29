#include <stdio.h>
#include "configData.h"

int main(void) {
   restoremaxConnections(&maxConnections);
   printf("Max connections after restore: %d\n", maxConnections);
    maxConnections = 100;
   printf("Max connections after update: %d\n", maxConnections); 
    return 0;
}