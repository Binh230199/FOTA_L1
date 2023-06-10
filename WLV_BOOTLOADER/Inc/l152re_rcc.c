/*
 * l152re_rcc.c
 *
 *  Created on: Mar 17, 2023
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#include "l152re_rcc.h"

void RCC_DeInit(void)
{
	/* Set MSIClockRange, HSITRIM and MSITRIM bits to the reset values */
	RCC->ICSCR |= ((0x00000000UL << RCC_ICSCR_MSICAL_Pos)
			| (0x10UL << RCC_ICSCR_HSITRIM_Pos) | RCC_ICSCR_MSIRANGE_5);
	/* Set MSION bit */
	RCC->CR |= RCC_CR_MSION;

	/* Wait till MSI is ready */
	while ((RCC->CR & RCC_CR_MSIRDY) != RCC_CR_MSIRDY);

	/* Switch SYSCLK to MSI*/
	RCC->CFGR &= ~RCC_CFGR_SW;

	while ((RCC->CFGR & RCC_CFGR_SWS) != 0U);

	/* Reset HSION, HSEON, CSSON & PLLON bits */
	RCC->CR &= ~(RCC_CR_HSION | RCC_CR_HSEON | RCC_CR_CSSON | RCC_CR_PLLON);

	/* Reset HSEBYP bit */
	RCC->CR &= ~ RCC_CR_HSEBYP;

	/* Wait till PLL is not ready */
	while ((RCC->CR & RCC_CR_PLLRDY) != 0U);

	/* Reset CFGR register */
	RCC->CFGR = 0x00UL;

	/* Disable all interrupts */
	RCC->CIR = 0x00UL;

	/* Clear all flags */
	RCC->CIR = RCC_CIR_LSIRDYC | RCC_CIR_LSERDYC | RCC_CIR_HSIRDYC
			| RCC_CIR_HSERDYC | RCC_CIR_PLLRDYC | RCC_CIR_MSIRDYC
			| RCC_CIR_LSECSSC | RCC_CIR_CSSC;
	/* Clear all reset flags */
	RCC->CSR |= RCC_CSR_RMVF;
}
