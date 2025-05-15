#ifndef INTERRUPT_H_
#define INTERRUPT_H_

void SysTick_Handler(void);

// Initialisation des interruptions : bouton, RTC (WUT)
void interruption_init(void);

// Handler bouton TARE (PA8)
void EXTI4_15_IRQHandler(void);

// Handler pour le RTC wake-up timer (via RTC_TAMP_IRQn)
void RTC_TAMP_IRQHandler(void);

#endif
