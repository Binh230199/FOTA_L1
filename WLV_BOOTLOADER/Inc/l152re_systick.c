/*
 * l152re_systick.c
 *
 *  Created on: Mar 16, 2023
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#include "l152re_systick.h"

volatile uint32_t uwTick = 0u;
uint32_t uwTickFreq = 1u;

void SysTick_Init(void)
{
	SysTick->LOAD = 2096;
	SysTick->VAL = 0;
	SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk
			| SysTick_CTRL_CLKSOURCE_Msk);
}

void SysTick_DelayMillis(uint32_t ms)
{
	for (uint32_t i = 0; i < ms; i++)
	{
		/* Read bit COUNTFLAG */
		while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
				!= SysTick_CTRL_COUNTFLAG_Msk);
	}
}

uint32_t SysTick_GetTick(void)
{
	return uwTick;
}

void SysTick_Handler(void)
{
	uwTick++;
}
