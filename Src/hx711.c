#include <stdint.h>
#include <stm32g031xx.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "hx711.h"
#include "gpio.h"
#include "timer.h"
#include "uart.h"
#include "fifo.h"


//void hx711_lock(hx711_t *hx711) {
//	while (hx711->lock){
//		TIM2_Delay(1);
//	}
//	hx711->lock = 1;
//}

//void hx711_unlock(hx711_t *hx711) {
//	hx711->lock = 0;
//}

void hx711_init(hx711_t *hx711) {
	//hx711->lock = 0;

	//hx711_lock(hx711);

	/* Activer l'horloge pour GPIOB */
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN;

	/*Activer la pin D3*/
	GPIOB->MODER &= ~(GPIO_MODER_MODE1_Msk);
	GPIOB->MODER |= INPUT_MODE << GPIO_MODER_MODE1_Pos;
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD1_Msk);

	/*Activer la pin D2*/
	GPIOA->MODER &= ~(GPIO_MODER_MODE15_Msk);
	GPIOA->MODER |= OUTPUT_MODE << GPIO_MODER_MODE15_Pos;
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD15_Msk);

	//hx711_unlock(hx711);
}

int32_t hx711_lecture(hx711_t *hx711){
	uint32_t data = 0;  // Variable pour stocker les données lues
	uint8_t bit = 24;   // Nombre de bits à lire

	// Attendre que DT passe à LOW (données prêtes)
	while (GPIOB->IDR & GPIO_IDR_ID1){
		TIM2_Delay(1);
	}

	//Mettre D2 à 0
	GPIOA->ODR &= ~GPIO_ODR_OD15;

	for (int i = 0; i < bit; i++) {

		//Mettre D2 à 1 donc front montant
	    GPIOA->ODR |= GPIO_ODR_OD15;
	    TIM2_Delay(40); // Délai pour stabiliser le signal

	    //Mettre D2 à 0
        GPIOA->ODR &= ~GPIO_ODR_OD15;
        TIM2_Delay(40); // Délai pour stabiliser le signal

	    //décalage des bits sur la gauche
	    data = data << 1;

	    //Si la Pin D3 est à 1 la copié dans data
	    if (GPIOB->IDR & GPIO_IDR_ID1) {
	    	data |= 1;
	    }
	}


	//Envoyer une 25ème impulsion
	GPIOA->ODR |= GPIO_ODR_OD15;  // Mettre D2 à 1
	TIM2_Delay(40);             // Délai pour stabiliser le signal
	GPIOA->ODR &= ~GPIO_ODR_OD15; // Mettre D2 à 0
	TIM2_Delay(40);             // Délai pour stabiliser le signal

	return data; // Retourner les données brutes lues
}


int32_t hx711_lecture_moyenne(hx711_t *hx711, uint16_t nb_echantillons){
	//hx711_lock(hx711);
	int64_t total = 0;
	int32_t lecture = 0;

	for(uint16_t i=0; i<nb_echantillons; i++){
		lecture = hx711_lecture(hx711);
		total += lecture;
		TIM2_Delay(10);
	}
	int32_t moyenne = (int32_t)(total/nb_echantillons);
	//hx711_unlock(hx711);
	return moyenne;
}

void hx711_tare(hx711_t *hx711) {
    UART2_Write_chaine("\nAttente de la nouvelle TARE...\r\n");

    // Mesurer la tare
    uint32_t TARE = hx711_lecture_moyenne(hx711, 20);
    hx711->TARE = TARE;

    // Écriture en Flash (seulement sur appui bouton)
    flashEffacePage(FLASH_TARE_ADDRESS); // Effacer la page avant écriture
    flashEcriture((uint32_t *)FLASH_TARE_ADDRESS, TARE, 0xFFFFFFFF); // Sauvegarde la TARE

    UART2_Write_chaine("Nouvelle TARE enregistrée en Flash !\r\n");
}


void hx711_calibration(hx711_t *hx711) {

	hx711->coeff = 11024;
}

float hx711_poid(hx711_t *hx711) {
	uint32_t poid = hx711_lecture_moyenne(hx711, 20);
	if(poid  > hx711->TARE){
		poid -= hx711->TARE;
		poid = (poid / hx711->coeff) *1000;
	}
	else{
		poid = 0;
	}

	return poid;
}

void hx711_load_tare(hx711_t *hx711) {
    uint32_t tareLue = *(__IO uint32_t *)FLASH_TARE_ADDRESS;

    // Vérifier si une TARE valide existe dans la Flash
    if (tareLue != 0xFFFFFFFF) {
        hx711->TARE = tareLue;
        UART2_Write_chaine("TARE récupérée en Flash : ");
    } else {
        hx711->TARE = 0; // Valeur par défaut
        UART2_Write_chaine("Aucune TARE trouvée, utilisation de la valeur par défaut.\r\n");
    }

    // Afficher la valeur récupérée
    char buffer[50];
    sprintf(buffer, "%lu\r\n", hx711->TARE);
    UART2_Write_chaine(buffer);
}


void hx711_check_for_tare(hx711_t *hx711) {
    char bloc[512];
    uint16_t bloc_index = 0;
    uint32_t start = SYSTICK_Get();

    while ((SYSTICK_Get() - start) < 5000 && bloc_index < sizeof(bloc) - 1) {
        char c;
        if (fifo_get(&c)) {
            bloc[bloc_index++] = c;
        }
    }
    bloc[bloc_index] = '\0';

    if (strstr(bloc, "RX: \"01\"") != NULL) {
        UART2_Write_chaine("🟢 TARE demandée\r\n");
        hx711_tare(hx711);
    }
}
