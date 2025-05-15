#include <stdint.h>
#include <stm32g031xx.h>
#include <stdio.h>
#include "main.h"
#include "gpio.h"
#include "timer.h"
#include "hx711.h"
#include "uart.h"
#include "flash.h"
#include "interruption.h"
#include "DS18B20.h"
#include "lora.h"
#include "fifo.h"
#include "rtc.h"

hx711_t test;

int main(void)
{
    // === Initialisation des périphériques ===
    GPIO_Init();
    SYSTICK_Init();
    TIM2_Init();
    UART_Init();
    hx711_init(&test);
    hx711_load_tare(&test);
    hx711_calibration(&test);
    interruption_init();

    // === Initialisation du RTC ===
    RTC_Init();

    // === Démarrage LoRa ===
    send_AT_command();

    uint8_t last_temp = -1;

    while (1) {
        UART2_Write_chaine(" Réveil STM32\r\n");

        // Réveil LoRa (si mode AUTOON actif)
        lora_wake();

        // Lire le poids
        uint32_t poid = hx711_poid(&test);
        char buffer_poid[20];
        sprintf(buffer_poid, "%ld\r\n", poid);

        // Lire la température
        int8_t temp = (int8_t)DS1820_Lecture(GPIOA, 0);
        if (temp != 85) last_temp = temp;
        else temp = last_temp;

        char buffer_temp[20];
        sprintf(buffer_temp, "%d\r\n", temp);

        // Affichage UART
        UART2_Write_chaine(buffer_poid);
        UART2_Write_chaine(buffer_temp);

        // Envoi LoRa
        char buffer_lora[100];
        sprintf(buffer_lora, "Poids: %ld g, Temp: %ld C", poid, temp);
        send_message(buffer_lora);

        // Rendormir le module LoRa
        lora_sleep();

        // Gestion tare auto
        hx711_check_for_tare(&test);

        // Petit délai d’observation
        TIM2_Delay(5000);

        // Préparer D2 (PB5) pour rester à 3.3V pendant la veille
        GPIO_SleepStatusInit();

        // Entrer en veille pendant 30 secondes
        enter_low_power_standby_mode(600);
    }
}
