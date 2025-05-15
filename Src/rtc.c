#include <stdint.h>
#include <stm32g031xx.h>
#include "rtc.h"



void GPIO_SleepStatusInit(void) {
    // 1. Activer l'accès au périphérique PWR
    RCC->APBENR1 |= RCC_APBENR1_PWREN;

    // 2. Activer l'horloge GPIOB (nécessaire pour accéder à GPIOB->MODER)
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;

    // 3. Mettre PB5 (D2) en entrée
    GPIOB->MODER &= ~(3 << (5 * 2));  // Clear MODER5 → mode entrée (00)

    // 4. Activer le pull-up sur PB5 pendant la veille
    PWR->PUCRB |= (1 << 5);           // Pull-up sur PB5

    // 5. Autoriser l’application des pull-up/pull-down en Standby
    PWR->CR3 |= PWR_CR3_APC;
}




void RTC_TAMP_IRQHandler(void) {

    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;


    RTC->SCR |= RTC_SCR_CWUTF;

    RTC->WPR = 0xFF;
}


void RTC_Init(void) {
    RCC->APBENR1 |= RCC_APBENR1_PWREN;
    RCC->APBENR1 |= RCC_APBENR1_RTCAPBEN;

    PWR->CR1 |= 1 << PWR_CR1_DBP_Pos;
    RCC->BDCR |= 1 << RCC_BDCR_RTCEN_Pos;
    RCC->BDCR |= (2 << RCC_BDCR_RTCSEL_Pos);
    RCC->CSR |= RCC_CSR_LSION;

    while(!(RCC->CSR & RCC_CSR_LSIRDY));

    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    RTC->CR &= ~RTC_CR_WUTE;  // Désactiver Wakeup Timer
    RTC->CR &= ~RTC_CR_WUTIE;  // Désactiver temporairement l'interruption

    RTC->WPR = 0xFF;
}

void enter_low_power_standby_mode(uint32_t sleepTime) {
    PWR->SCR |= PWR_SCR_CWUF;

    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    PWR->CR1 |= (0b011 << PWR_CR1_LPMS_Pos);

    while (!(RTC->ICSR & RTC_ICSR_WUTWF));

    RTC->WUTR = sleepTime;

    RTC->SCR |= RTC_SCR_CWUTF;
    RTC->CR &= ~(7 << RTC_CR_WUCKSEL_Pos);
    RTC->CR |= (4 << RTC_CR_WUCKSEL_Pos);

    RTC->CR |= RTC_CR_WUTE | RTC_CR_WUTIE;

    RTC->WPR = 0xFF;

    // Entrer en mode veille
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    NVIC_EnableIRQ(RTC_TAMP_IRQn);
    __WFI();

}
