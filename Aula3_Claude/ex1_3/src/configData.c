#include "configData.h"

/* 'static' at file scope gives the variable internal linkage: the name is not
 * visible outside this file, so other modules cannot read or write it (an
 * 'extern uint16_t maxConnections;' elsewhere fails at link time). */
static uint16_t maxConnections;

void configInit(void) {
    maxConnections = 0;
}

void configSetMaxConnections(uint16_t value) {
    /* Every write goes through here, so this is the single place where the
     * value could be validated (e.g. against a maximum) if needed. */
    maxConnections = value;
}

uint16_t configGetMaxConnections(void) {
    return maxConnections;
}
