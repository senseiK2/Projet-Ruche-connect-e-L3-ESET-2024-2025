#include <stm32g031xx.h>
#include "standby.h"
#include "uart.h"
#include "timer.h"


void RTC_LSI_Init(void)
{
    UART2_Write_chaine("Init RTC (LSI)...\r\n");

    // 1. Activer l'horloge PWR et autoriser l'accès aux registres backup
    RCC->APBENR1 |= RCC_APBENR1_PWREN;
    PWR->CR1 |= PWR_CR1_DBP;

    // 2. Activer LSI et attendre qu'il soit prêt
    RCC->CSR |= RCC_CSR_LSION;
    UART2_Write_chaine("Attente LSI ready...\r\n");
    while (!(RCC->CSR & RCC_CSR_LSIRDY)) {
        // Ajouter un timeout pour éviter un blocage infini
        static uint32_t timeout = 0xFFFF;
        if (--timeout == 0) {
            UART2_Write_chaine("Erreur: Timeout LSI!\r\n");
            return;
        }
    }
    UART2_Write_chaine("LSI stable.\r\n");

    // 3. Désactiver le RTC avant de reconfigurer
    RCC->BDCR &= ~RCC_BDCR_RTCEN;

    // 4. Sélectionner LSI comme horloge RTC
    RCC->BDCR &= ~RCC_BDCR_RTCSEL_Msk;
    RCC->BDCR |= RCC_BDCR_RTCSEL_1;  // LSI (0b10)
    RCC->BDCR |= RCC_BDCR_RTCEN;

    // Petite pause après activation
    TIM2_Delay(5000);

    UART2_Write_chaine("RTC activé sur LSI.\r\n");

    // 5. Déverrouiller les registres RTC
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    // 9. Verrouiller les registres
    RTC->WPR = 0xFF;
    UART2_Write_chaine("RTC initialisé avec succès.\r\n");
}

void RTC_Wakeup_Set(uint16_t seconds)
{
    UART2_Write_chaine("Configuration Wake-up Timer RTC...\r\n");

    // 1. Autoriser l'accès aux registres backup
    RCC->APBENR1 |= RCC_APBENR1_PWREN;
    PWR->CR1 |= PWR_CR1_DBP;

    // 2. Déverrouiller les registres RTC
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    // 3. Désactiver le Wake-Up Timer
    RTC->CR &= ~RTC_CR_WUTE;
    UART2_Write_chaine("Demande désactivation WUT...\r\n");

    // Attente avec timeout
    uint32_t timeout = 100000; // ~1s à 48MHz
    while ((RTC->CR & RTC_CR_WUTE) && timeout--);
    UART2_Write_chaine("WUT désactivé.\r\n");

    if (!timeout) {
        UART2_Write_chaine("Erreur: Timeout désactivation WUTE!\r\n");
        return;
    }

    // 4. Attendre que le WUT soit configurable (WUTWF=1)
    UART2_Write_chaine("Attente WUTWF = 1...\r\n");
    timeout = 100000;
    while (!(RTC->ICSR & RTC_ICSR_WUTWF) && timeout--) {
    }

    if (!timeout) {
        UART2_Write_chaine("Erreur: Timeout WUTWF!\r\n");

        // Debug: Afficher l'état du RTC
        char debug[100];
        sprintf(debug, "RTC->ICSR=0x%08lX, RTC->CR=0x%08lX\r\n", RTC->ICSR, RTC->CR);
        UART2_Write_chaine(debug);
        return;
    }
    UART2_Write_chaine("WUTWF = 1, configuration possible.\r\n");

    // 5. Configurer le prescaler
    RTC->CR &= ~RTC_CR_WUCKSEL_Msk;
    RTC->CR |= (0b100 << RTC_CR_WUCKSEL_Pos);  // LSI/16 (2kHz)
    UART2_Write_chaine("Prescaler = LSI / 16.\r\n");

    // 6. Configurer la valeur de Wake-Up
    RTC->WUTR = (seconds * 2000) - 1;  // Pour LSI/16 (2kHz)
    UART2_Write_chaine("WUTR mis à jour.\r\n");

    // 7. Activer le Wake-Up Timer
    RTC->CR |= RTC_CR_WUTE;
    UART2_Write_chaine("WUT activé.\r\n");

    // 8. Verrouiller les registres
    RTC->WPR = 0xFF;
    UART2_Write_chaine("Configuration WUT terminée.\r\n");
}

void Standby_Enter(void)
{
    UART2_Write_chaine("Préparation à l'entrée en veille...\r\n");

    PWR->SCR |= PWR_SCR_CSBF | PWR_SCR_CWUF;
    PWR->CR3 &= ~PWR_CR3_RRS; //RAM pas conserver

    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    PWR->CR1 &= ~PWR_CR1_LPMS;
    PWR->CR1 |= (0b110 << PWR_CR1_LPMS_Pos);

    UART2_Write_chaine("Entrée en Standby (WFI)...\r\n");
    __WFI();
}

uint8_t Standby_Check_Wakeup(void)
{
    if (PWR->SR1 & PWR_SR1_SBF) {
        UART2_Write_chaine("Réveil depuis le mode Standby détecté.\r\n");
        return 1;
    }
    return 0;
}
