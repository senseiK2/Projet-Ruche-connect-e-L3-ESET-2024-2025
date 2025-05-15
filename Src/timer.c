#include <stdint.h>
#include "stm32g031xx.h"
#include "timer.h"

uint32_t SystemCoreClock = 16000000;
uint32_t ticks = 0;

void SYSTICK_Init(void){
	SysTick_Config(SystemCoreClock / 1000);
}

void SYSTICK_Delay(uint32_t Delay)
{
	uint32_t tickstart = SYSTICK_Get();
	uint32_t wait = Delay;

	while((SYSTICK_Get() - tickstart) < wait);
}

uint32_t SYSTICK_Get(void){
	return ticks;
}

void TIM2_Init(void){
	RCC->APBENR1 |= RCC_APBENR1_TIM2EN;
	TIM2->CR1 |= TIM_CR1_CEN;
	TIM2->PSC = 15;
}

void TIM2_Delay(uint32_t delay){
	TIM2->ARR = delay;
	TIM2->CNT = 0;
	TIM2->SR &= ~TIM_SR_UIF_Msk;
	while(!(TIM2->SR & TIM_SR_UIF));
}

