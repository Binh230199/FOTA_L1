/*
 * l152re_systick.h
 *
 *  Created on: Mar 16, 2023
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#ifndef L152RE_SYSTICK_H_
#define L152RE_SYSTICK_H_

#include "stm32l1xx.h"

#define MSI	2097000U

void SysTick_Init(void);
void SysTick_Handler(void);
void SysTick_DelayMillis(uint32_t ms);
uint32_t SysTick_GetTick(void);

#endif /* L152RE_SYSTICK_H_ */
