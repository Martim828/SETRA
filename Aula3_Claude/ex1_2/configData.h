#ifndef CONFIGDATA_H
#define CONFIGDATA_H

#include <stdint.h>

/* Exercise 1.2: maxConnections is shared with the other modules through
 * 'extern'. This is only a declaration; the variable itself (the storage) is
 * defined once, in configData.c. */
extern uint16_t maxConnections;

void configInit(void); // Init maxConnections to zero (call it before using the variable)

#endif /* CONFIGDATA_H */
