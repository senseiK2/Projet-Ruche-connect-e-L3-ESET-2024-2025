#ifndef __STANDBY_H
#define __STANDBY_H

#include <stdint.h>

void RTC_LSI_Init(void);
void RTC_Wakeup_Set(uint16_t seconds);
void Standby_Enter(void);
uint8_t Standby_Check_Wakeup(void);

#endif
