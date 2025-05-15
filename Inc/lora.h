#ifndef LORA_H
#define LORA_H

#include "main.h"
#include "uart.h"
#include <string.h>

void send_AT_command(void);
void send(const char *cmd);
void send_message(const char *cmd);
void lora_read_downlink(void);
void lora_wake(void);
void lora_sleep(void);
#endif
