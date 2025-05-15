#include <stdint.h>
#include <stm32g031xx.h>
#include <stdio.h>
#include "main.h"
#include "gpio.h"
#include "timer.h"
#include "UART.h"
#include "interruption.h"
#include "hx711.h"
#include "fifo.h"

extern uint32_t ticks;

// === Systick
void SysTick_Handler(void){
    ticks++;
}

// === Initialisation des interruptions
void interruption_init(void) {
    GPIO_Init();

}


void USART1_IRQHandler(void) {
    if (USART1->ISR & USART_ISR_RXNE_RXFNE) {
        char c = USART1->RDR;
        fifo_put(c);  // on stocke dans la FIFO
    }
}




//// === Handler RTC Wake-Up Timer
//void RTC_TAMP_IRQHandler(void)
//{
//    // 1. Vérifier tous les flags possibles
//    uint8_t woken = 0;
//
//    // EXTI20 (WUT)
//    if (EXTI->RPR1 & (1 << 20)) {
//        EXTI->RPR1 = (1 << 20);
//        woken = 1;
//    }
//
//    // RTC Wake-Up Timer
//    if (RTC->MISR & RTC_MISR_WUTMF) {
//        RTC->SCR = RTC_SCR_CWUTF; // Clear RTC flag
//        PWR->SCR |= PWR_SCR_CWUF; // Clear PWR flag
//        UART2_Write_chaine("Réveil RTC réussi!\r\n");
//    }
//
//}


