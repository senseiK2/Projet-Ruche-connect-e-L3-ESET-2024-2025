#include <stdint.h>
#include <stm32g031xx.h>
#include <stdio.h>
#include "main.h"
#include "hx711.h"
#include "gpio.h"
#include "timer.h"
#include "uart.h"
#include "DS18B20.h"

uint8_t DS18B20_reset(GPIO_TypeDef *GPIO, uint8_t PIN) {
	GPIO->OTYPER |= (1 << PIN);

	GPIO->ODR &= ~(1 << PIN);  // Tirer la ligne à LOW (Reset)
    TIM2_Delay(479);           // Attendre 480 µs minimum

    GPIO->ODR |= (1 << PIN);   // Relâcher la ligne (HIGH)
    TIM2_Delay(69);            // Attendre avant de lire la réponse du capteur

    if (!(GPIO->IDR & (1 << PIN))) { // Vérifier si le capteur a répondu LOW
        TIM2_Delay(409);        // Attendre la fin de la réponse (total 480 µs)
        return 1;               // Capteur présent
    } else {
        TIM2_Delay(409);        // Fin du délai si capteur absent
        return 0;               // Capteur absent
    }
}



void DS18B20_WBit(GPIO_TypeDef *GPIO, uint8_t PIN, uint8_t Bit) {
    if (Bit == 1) {
        // Écrire un 1 :
        GPIO->ODR &= ~(1 << PIN);      // Tirer la ligne à LOW
        TIM2_Delay(9);               // Maintenir LOW pendant 10 µs
        GPIO->ODR |= (1 << PIN);      // Relâcher la ligne (HIGH)
        TIM2_Delay(55);               // Compléter le slot de 60 µs
    } else {
        // Écrire un 0 :
        GPIO->ODR &= ~(1 << PIN);     // Tirer la ligne à LOW
        TIM2_Delay(64);               // Maintenir LOW pendant 60 µs
        GPIO->ODR |= (1 << PIN);      // Relâcher la ligne (HIGH)
        TIM2_Delay(5);
    }

}


void DS18B20_WOctet(GPIO_TypeDef *GPIO, uint8_t PIN, uint8_t Octet) {
	GPIO->OTYPER &= ~(1 << PIN);
	for (uint8_t i = 0; i < 8; i++) {
        uint8_t bit = (Octet >> i) & 0x01;  // Extraire le bit (LSB d'abord)
        DS18B20_WBit(GPIO, PIN, bit);       // Écrire le bit
    }
}




uint8_t DS18B20_RBit(GPIO_TypeDef *GPIO, uint8_t PIN) {
	uint8_t bit;

    GPIO->ODR &= ~(1 << PIN);   // LOW
    TIM2_Delay(3);                   // Maintenir LOW pendant 10 µs
    GPIO->ODR |= (1 << PIN);
    TIM2_Delay(9);                   // Attendre 15 µs pour la stabilisation des données (tRDV)

    if (GPIO->IDR & (1 << PIN)) {
        bit = 1;
    } else {
        bit = 0;
    }

    TIM2_Delay(52);                   // Pause avant le prochain bit (tSLOT = 60-120 µs)

    return bit;
}


uint8_t DS18B20_ROctet(GPIO_TypeDef *GPIO, uint8_t PIN) {
	GPIO->OTYPER |= (1 << PIN);

	uint8_t octet = 0;

    for (uint8_t i = 0; i < 8; i++) {
        uint8_t bit = DS18B20_RBit(GPIO, PIN);
        octet |= (bit << i);
    }

    return octet;
}

float DS1820_Lecture(GPIO_TypeDef *GPIO, uint8_t broche) {
    uint8_t presence = DS18B20_reset(GPIO, broche);
    uint8_t tentative = 0;

    // Vérifier la présence de la sonde
    while (!presence && tentative < 10) {
        UART2_Write_chaine("Sonde introuvable\r\n");
        presence = DS18B20_reset(GPIO, broche);
        tentative++;
    }
    if (!presence) {
        return 404; // Erreur si la sonde n'est pas détectée
    }

    // Commande pour convertir la température
    DS18B20_WOctet(GPIO, broche, 0xCC); // SKIP ROM
    DS18B20_WOctet(GPIO, broche, 0x44); // CONVERT T


    SYSTICK_Delay(750);

    // Réinitialisation pour lire le Scratchpad
    if (!DS18B20_reset(GPIO, broche)) {
        return 404; // Erreur si la sonde n'est pas détectée
    }

    // Lecture du Scratchpad
    DS18B20_WOctet(GPIO, broche, 0xCC); // SKIP ROM
    DS18B20_WOctet(GPIO, broche, 0xBE); // READ SCRATCHPAD

    // Lire les 2 octets de température
    uint8_t Temp_LSB = DS18B20_ROctet(GPIO, broche);
    uint8_t Temp_MSB = DS18B20_ROctet(GPIO, broche);

    // Combinaison des 16 bits
    int16_t raw_temp = (Temp_MSB << 8) | Temp_LSB;

    // Conversion en température en °C
    float temperature = raw_temp * 0.0625;

    return temperature;
}

