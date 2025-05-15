#ifndef RTC_H
#define RTC_H

#include <stdint.h>

void GPIO_SleepStatusInit(void);
void RTC_TAMP_IRQHandler(void);
void RTC_Init(void);
void enter_low_power_standby_mode(uint32_t sleepTime);

#endif
