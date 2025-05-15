#ifndef FIFO_H
#define FIFO_H

#include <stdint.h>

void fifo_put(char c);
int fifo_get(char *c);
void fifo_clear(void);

#endif
