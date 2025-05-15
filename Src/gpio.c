#include <stdint.h>
#include <stm32g031xx.h>
#include "main.h"
#include "gpio.h"
#include "timer.h"

void GPIO_Init(void){

	RCC->IOPENR |= RCC_IOPENR_GPIOCEN;

	// Boutton pour le TARE
	GPIOA->MODER &= ~(GPIO_MODER_MODE8_Msk);
	GPIOA->MODER |= INPUT_MODE << GPIO_MODER_MODE8_Pos;

	GPIOC->MODER &= ~(GPIO_MODER_MODE6_Msk);
	GPIOC->MODER |= OUTPUT_MODE << GPIO_MODER_MODE6_Pos;

	GPIO_MODE(GPIOA, 0, OUTPUT_MODE);
	//GPIOA->PUPDR |= 1 << GPIO_PUPDR_PUPD0_Pos;

}


void GPIO_MODE(GPIO_TypeDef *GPIO, uint8_t PIN, uint8_t MODE) {
    // Effacer les bits MODER pour configurer la broche
    GPIO->MODER &= ~(0b11 << (PIN * 2));

    if (MODE == OUTPUT_MODE) { // Mode sortie
        GPIO->MODER |= (0b01 << (PIN * 2)); // Configurer la broche en sortie
        GPIO->OTYPER |= (1 << PIN);       // Activer le mode Open-Drain
    } else if (MODE == INPUT_MODE) { // Mode entrée
        GPIO->MODER &= ~(0b11 << (PIN * 2)); // Configurer la broche en entrée
        GPIO->OTYPER &= ~(1 << PIN);        // Désactiver Open-Drain (inutile en entrée)
    }
}



