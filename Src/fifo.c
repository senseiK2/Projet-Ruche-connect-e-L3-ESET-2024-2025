#include "fifo.h"

#define RX_FIFO_SIZE 256
static char fifo_buffer[RX_FIFO_SIZE];
static uint8_t fifo_head = 0;
static uint8_t fifo_tail = 0;

void fifo_put(char c) {
    uint8_t next = (fifo_head + 1) % RX_FIFO_SIZE;
    if (next != fifo_tail) {
        fifo_buffer[fifo_head] = c;
        fifo_head = next;
    }
    // sinon : FIFO pleine, on ignore
}

int fifo_get(char *c) {
    if (fifo_head == fifo_tail) return 0;  // vide
    *c = fifo_buffer[fifo_tail];
    fifo_tail = (fifo_tail + 1) % RX_FIFO_SIZE;
    return 1;
}

void fifo_clear(void) {
    fifo_head = fifo_tail = 0;
}
