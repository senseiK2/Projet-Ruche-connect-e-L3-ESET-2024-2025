#ifndef _FLASH_H_
#define _FLASH_H_



// clée pour débloquer la flash
#define FLASH_FKEY1 0x45670123
#define FLASH_FKEY2 0xCDEF89AB

#define FLASH_ADDRESS 0x0800F800  // Adresse où écrire en Flash (section FLASH_DATA)


void flashDebloque(void);
void flashBloque(void);
void flashEffacePage(uint32_t page);
void flashEcriture(uint32_t *adresse, uint32_t data1, uint32_t data2);
void testFlash();



#endif
