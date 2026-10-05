#ifndef EX1_H
#define EX1_H

#define MYRAND_MAX 100  /* MyRand() returns values in [0, MYRAND_MAX] */

/* Generates a random number in [0, MYRAND_MAX] and stores in *ncalls how many
 * times MyRand() has been called, including the present call. */
void MyRand(int *value, int *ncalls);

#endif /* EX1_H */
