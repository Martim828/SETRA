#include "configData.h"
#include <stdio.h>
#include <stdint.h>

uint16_t maxConnections;
void restoremaxConnections(uint16_t *maxConnections) {
    *maxConnections = 0;
}

void configSetMaxConnections(uint16_t value) {
    maxConnections = value;
}

void configInit(void) {
    maxConnections = 0;
}

uint16_t configGetMaxConnections(void) {
    return maxConnections;
}
