#ifndef CONFIGDATA_H
#define CONFIGDATA_H

#include <stdint.h>

/* Exercise 1.3: maxConnections is hidden inside configData.c. Other modules
 * can only reach it through these interface functions. */
void configInit(void);                          // Init maxConnections to zero
void configSetMaxConnections(uint16_t value);   // Set the value of maxConnections
uint16_t configGetMaxConnections(void);         // Get the value of maxConnections

#endif /* CONFIGDATA_H */
