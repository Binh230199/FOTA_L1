/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Binh Hoang - binhhv.23.1.99@gmail.com
 * @brief          : Main program body of bootloader
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include "main.h"
#include "l152re_eeprom.h"
#include "l152re_systick.h"
#include "l152re_rcc.h"

int main(void)
{
	System_ClockConfig();
	SysTick_Init();

	uint32_t currentAddress = *(volatile uint32_t*) CURRENT_ADDR;

	if (currentAddress != APP_1_START_ADDR && currentAddress != APP_2_START_ADDR)
	{
		currentAddress = APP_1_START_ADDR;
		EEPROM_Unlock();
		EEPROM_EraseWord(CURRENT_ADDR);
		EEPROM_EraseByte(CURRENT_MAJOR);
		EEPROM_EraseByte(CURRENT_MINOR);
		EEPROM_EraseByte(CURRENT_PATCH);

		EEPROM_ProgramWord(CURRENT_ADDR, currentAddress);
		EEPROM_ProgramByte(CURRENT_MAJOR, 1u);
		EEPROM_ProgramByte(CURRENT_MINOR, 0u);
		EEPROM_ProgramByte(CURRENT_PATCH, 0u);
		EEPROM_Lock();
	}

	gotoFirmware(currentAddress);
	/* Loop forever */
	while (1)
	{

	}
}

void System_ClockConfig(void)
{
	RCC->CR |= RCC_CR_MSION;

	while ((RCC->CR & RCC_CR_MSIRDY) != RCC_CR_MSIRDY);

	RCC->APB1ENR |= RCC_APB1ENR_PWREN;

	RCC->AHBENR |= RCC_AHBENR_FLITFEN;

	PWR->CR |= (1 << 11);
	PWR->CR &= ~(1 << 12);

	FLASH->ACR &= ~FLASH_ACR_LATENCY;
}

void DeInit(void)
{
	RCC->APB1RSTR = 0xFFFFFFFFU;
	RCC->APB1RSTR = 0x00000000U;

	RCC->APB2RSTR = 0xFFFFFFFFU;
	RCC->APB2RSTR = 0x00000000U;

	RCC->AHBRSTR = 0xFFFFFFFFU;
	RCC->AHBRSTR = 0x00000000U;
}

void gotoFirmware(uint32_t address)
{
	RCC_DeInit();
	DeInit();

	SCB->SHCSR &= ~( SCB_SHCSR_USGFAULTENA_Msk |
	SCB_SHCSR_BUSFAULTENA_Msk |
	SCB_SHCSR_MEMFAULTENA_Msk);

	__set_MSP(*((volatile uint32_t*) address));

	uint32_t jump_address = *((__IO uint32_t*) (address + 4));
	void (*reset_handler)(void) = (void*) jump_address;
	reset_handler();
}
