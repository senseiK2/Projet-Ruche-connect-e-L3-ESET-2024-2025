#ifndef HX711_H_
#define HX711_H_
#include <stdint.h>

#define FLASH_TARE_ADDRESS 0x0800F800  // Adresse Flash pour stocker la TARE


typedef struct
{
  uint32_t       TARE;
  float       coeff;
  //uint8_t       lock;
}hx711_t;




void hx711_lock(hx711_t *hx711);
void hx711_unlock(hx711_t *hx711);
void hx711_init(hx711_t *hx711);
int32_t hx711_lecture(hx711_t *hx711);
int32_t hx711_lecture_moyenne(hx711_t *hx711, uint16_t nb_echantillons);
void hx711_tare(hx711_t *hx711);
void hx711_calibration(hx711_t *hx711);
float hx711_poid(hx711_t *hx711);
void hx711_load_tare(hx711_t *hx711);
void hx711_check_for_tare(hx711_t *hx711);



#endif
