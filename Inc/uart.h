#ifndef UART_H
#define UART_H

#include "stm32g031xx.h"

void UART_Init(void);
void UART2_Write(uint8_t ch);
char UART2_Read(void);
void UART2_Write_chaine(const char *str);

void UART1_Write(uint8_t ch);
char UART1_Read(void);
void UART1_Write_chaine(const char *str);
void USART1_IRQHandler(void);
#endif
