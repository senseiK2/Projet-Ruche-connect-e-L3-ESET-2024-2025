#include <stdint.h>
#include <stm32g031xx.h>
#include <stdio.h>
#include "main.h"
#include "hx711.h"
#include "uart.h"
#include "flash.h"


void flashDebloque(void) {
	while ((FLASH->SR & FLASH_SR_BSY1));     // Attend que la Flash ne soit plus occupée (bit BSY = 0)
	if ((FLASH->CR & FLASH_CR_LOCK)){       // Vérifie si la Flash est verrouillée (bit LOCK = 1)
		FLASH->KEYR = FLASH_FKEY1;            // Écriture de la première clé pour déverrouiller la Flash
		FLASH->KEYR = FLASH_FKEY2;            // Écriture de la deuxième clé pour déverrouiller la Flash
	}
}

void flashBloque(void) {
	FLASH->CR |= FLASH_CR_LOCK; // Active le bit LOCK pour verrouiller la Flash
}

void flashEffacePage(uint32_t adresse) {
    // Vérifier que l'adresse est bien alignée sur une page de 2 Ko
    if (adresse % 0x800 != 0) return;

    uint32_t page = (adresse - 0x08000000) / 0x800; // Calcul du numéro de page

    flashDebloque(); // Déverrouiller la Flash

    // Configurer l'effacement
    FLASH->CR &= ~FLASH_CR_PNB_Msk;  // Effacer les bits PNB
    FLASH->CR |= (page << FLASH_CR_PNB_Pos) & FLASH_CR_PNB_Msk; // Sélectionner la page
    FLASH->CR |= FLASH_CR_PER;  // Activer l’effacement de page
    FLASH->CR |= FLASH_CR_STRT; // Lancer l’opération

    while (FLASH->SR & FLASH_SR_BSY1); // Attendre que l'effacement soit terminé

    // Vérifier les erreurs
    if (FLASH->SR & FLASH_SR_PROGERR) FLASH->SR |= FLASH_SR_PROGERR;
    if (FLASH->SR & FLASH_SR_WRPERR) FLASH->SR |= FLASH_SR_WRPERR;
    if (FLASH->SR & FLASH_SR_EOP) FLASH->SR |= FLASH_SR_EOP; // Effacer le flag de fin d’opération

    FLASH->CR &= ~FLASH_CR_PER; // Désactiver le mode d’effacement
    flashBloque(); // Reverrouiller la Flash
}



void flashEcriture(uint32_t *adresse, uint32_t data1, uint32_t data2) {
    // Vérifier que l'adresse est alignée sur 64 bits
    if ((uint32_t)adresse % 8 != 0) return;

    flashDebloque(); // Déverrouiller la Flash

    FLASH->CR |= FLASH_CR_PG; // Activer le mode programmation

    *(__IO uint32_t *)adresse = data1;        // Première partie du double-mot
    *(__IO uint32_t *)(adresse + 1) = data2;  // Seconde partie du double-mot

    while (FLASH->SR & FLASH_SR_BSY1); // Attendre la fin de l’écriture

    // Vérifier les erreurs
    if (FLASH->SR & FLASH_SR_PROGERR) FLASH->SR |= FLASH_SR_PROGERR;
    if (FLASH->SR & FLASH_SR_WRPERR) FLASH->SR |= FLASH_SR_WRPERR;
    if (FLASH->SR & FLASH_SR_EOP) FLASH->SR |= FLASH_SR_EOP; // Effacer le flag de fin d’opération

    FLASH->CR &= ~FLASH_CR_PG; // Désactiver le mode programmation
    flashBloque(); // Reverrouiller la Flash
}

void testFlash(uint32_t valeur_a_ecrire1, uint32_t valeur_a_ecrire2) {
    uint32_t valeur_lue1, valeur_lue2;
    char buffer[100];

    // 1️ Effacer la page Flash
    flashEffacePage(FLASH_ADDRESS);
    UART2_Write_chaine("Page Flash effacée à l'adresse 0x0800F800\n");

    // 2️ Écrire les valeurs définies par l'utilisateur
    flashEcriture((uint32_t *)FLASH_ADDRESS, valeur_a_ecrire1, valeur_a_ecrire2);
    sprintf(buffer, "Valeurs écrites en Flash : 0x%08X 0x%08X\n", valeur_a_ecrire1, valeur_a_ecrire2);
    UART2_Write_chaine(buffer);

    // 3️ Lire les valeurs après écriture
    valeur_lue1 = *(__IO uint32_t *)FLASH_ADDRESS;
    valeur_lue2 = *(__IO uint32_t *)(FLASH_ADDRESS + 4);

    // 4️ Affichage des valeurs lues
    sprintf(buffer, "Valeurs lues en Flash : 0x%08X 0x%08X\n", valeur_lue1, valeur_lue2);
    UART2_Write_chaine(buffer);

    // 5️ Vérifier si l'écriture est correcte
    if (valeur_lue1 == valeur_a_ecrire1 && valeur_lue2 == valeur_a_ecrire2) {
        UART2_Write_chaine("Test réussi : écriture et lecture OK !\n");
    } else {
        UART2_Write_chaine("Test échoué : la Flash ne contient pas les bonnes valeurs !\n");
    }
}

