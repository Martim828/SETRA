#ifndef CONFIGDATA_H
#define CONFIGDATA_H
#include <stdint.h>
#include <stdio.h>


extern uint16_t maxConnections;

void restoremaxConnections(uint16_t *maxConnections);
void configInit(void); // Init maxConnections to zero
void configSetMaxConnections(uint16_t value);// Set the value of maxConnections
uint16_t configGetMaxConnections(void); // Get the value of maxConnections

#endif /* CONFIGDATA_H */

