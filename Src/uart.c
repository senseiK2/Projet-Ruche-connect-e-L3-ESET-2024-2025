#include "uart.h"
#include "main.h"
#include "stm32g031xx.h"

void UART_Init(void) {
    // Activer l'horloge pour GPIOA et USART2
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    RCC->APBENR1 |= RCC_APBENR1_USART2EN;
    RCC->APBENR2 |= RCC_APBENR2_USART1EN;

    // Configurer PA2 (TX) en mode alternatif AF1
    GPIOA->MODER &= ~(GPIO_MODER_MODE2_Msk);
    GPIOA->MODER |= (0b10 << GPIO_MODER_MODE2_Pos); // Mode alternatif
    GPIOA->AFR[0] |= (0b0001 << GPIO_AFRL_AFSEL2_Pos); // AF1 pour PA2

    // Configurer PA3 (RX) en mode alternatif AF1
    GPIOA->MODER &= ~(GPIO_MODER_MODE3_Msk);
    GPIOA->MODER |= (0b10 << GPIO_MODER_MODE3_Pos); // Mode alternatif
    GPIOA->AFR[0] |= (0b0001 << GPIO_AFRL_AFSEL3_Pos); // AF1 pour PA3

    GPIOA->MODER &= ~(GPIO_MODER_MODE9_Msk);
    GPIOA->MODER |= (0b10 << GPIO_MODER_MODE9_Pos);
    GPIOA->AFR[1] &= ~(GPIO_AFRH_AFSEL9_Msk);
    GPIOA->AFR[1] |= (1 << GPIO_AFRH_AFSEL9_Pos);

    GPIOA->MODER &= ~(GPIO_MODER_MODE10_Msk);
    GPIOA->MODER |= (0b10 << GPIO_MODER_MODE10_Pos);
    GPIOA->AFR[1] &= ~(GPIO_AFRH_AFSEL10_Msk);
    GPIOA->AFR[1] |= (1 << GPIO_AFRH_AFSEL10_Pos);

    // Configurer le baud rate (115200 pour 16 MHz)
    USART2->BRR = 138;
    USART1->BRR = SystemCoreClock / 9600;

    // Activer TX, RX et l'USART
    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;

    // Activer l'interruption de réception pour USART1
    USART1->CR1 |= USART_CR1_RXNEIE_RXFNEIE;  // RX interruption enable
    NVIC_EnableIRQ(USART1_IRQn);             // Activer dans le NVIC
}

void UART2_Write(uint8_t ch) {
    // Attendre que le registre TDR soit prêt
    while (!(USART2->ISR & USART_ISR_TXE_TXFNF));
    USART2->TDR = ch;
}

char UART2_Read(void) {
    // Attendre qu'un caractère soit reçu
    while (!(USART2->ISR & USART_ISR_RXNE_RXFNE));
    return USART2->RDR;
}

void UART2_Write_chaine(const char *str) {
    // Envoyer une chaîne de caractères
    while (*str) {
        UART2_Write(*str);
        str++;
    }
 }

void UART1_Write(uint8_t ch) {
    while (!(USART1->ISR & USART_ISR_TXE_TXFNF));
    USART1->TDR = ch;
}

char UART_Read1(void) {
    while (!(USART1->ISR & USART_ISR_RXNE_RXFNE));
    return USART1->RDR;
}

void UART_Write_chaine1(const char *str) {
    while (*str) {
        UART1_Write(*str);
        str++;
    }
}
