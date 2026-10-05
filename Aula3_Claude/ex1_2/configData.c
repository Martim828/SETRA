#include "configData.h"

/* Definition of the shared variable: one single copy for the whole program,
 * accessible from every module that includes configData.h. */
uint16_t maxConnections;

void configInit(void) {
    maxConnections = 0;
}
