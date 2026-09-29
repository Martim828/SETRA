#include <stdio.h>
#include "configData.h"

int main(void) {
   restoremaxConnections(&maxConnections);
   printf("Max connections after restore: %d\n", maxConnections);
    maxConnections = 100;
   printf("Max connections after update: %d\n", maxConnections);
   configInit();
   printf("Max connections after init: %d\n", maxConnections);
   configSetMaxConnections(50);
   printf("Max connections after set: %d\n", configGetMaxConnections());
   int a = configGetMaxConnections();
   printf("Max connections retrieved: %d\n", a); 
    return 0;
}