/*
 * l152re_eeprom.c
 *
 *  Created on: Mar 16, 2023
 *      Author: Binh Hoang - binhhv.23.1.99@gmail.com
 */

#include "l152re_eeprom.h"
#include "l152re_systick.h"
static void EEPROM_WaitFlagBSYIsReset(void)
{
	uint32_t tickStart = SysTick_GetTick();
	uint32_t tickElapsed;
	/* Polling bit BSY to be reset */
	while ((FLASH->SR & FLASH_SR_BSY_Msk) == FLASH_SR_BSY_Msk)
	{
		tickElapsed = SysTick_GetTick() - tickStart;
		if (tickElapsed > 1000)
		{
			break;
		}
	}
}

void EEPROM_Unlock(void)
{
	/* Read bit PELOCK */
	if ((FLASH->PECR & FLASH_PECR_PELOCK_Msk) == FLASH_PECR_PELOCK_Msk)
	{
		/* Write PEKEY1 and PEKEY2 to FLASH_PEKEYR */
		FLASH->PEKEYR = FLASH_PEKEY_1;
		FLASH->PEKEYR = FLASH_PEKEY_2;
	}
}

void EEPROM_Lock(void)
{
	/* Set PELOCK bit to lock EEPROM data */
	FLASH->PECR |= FLASH_PECR_PELOCK_Msk;
}

void EEPROM_EraseWord(uint32_t address)
{
	if (IS_VALID_EEPROM_ADDRESS(address))
	{
		*(volatile uint32_t*) address = 0x00000000UL;
	}
}

void EEPROM_EraseByte(uint32_t address)
{
	if (IS_VALID_EEPROM_ADDRESS(address))
	{
		*(volatile uint8_t*) address = 0x00U;
	}
}

void EEPROM_ProgramWord(uint32_t address, uint32_t wordData)
{
	FLASH->PECR &= ~FLASH_PECR_FTDW_Msk;

	/* Polling bit BSY to be reset */
	EEPROM_WaitFlagBSYIsReset();    //while ((FLASH->SR & FLASH_SR_BSY_Msk) == FLASH_SR_BSY_Msk);

	/* Write a word by writing 2 words to the address */
	if (IS_VALID_EEPROM_ADDRESS(address))
	{
		*(volatile uint32_t*) address = wordData;
	}

	/* Polling bit BSY to be reset */
	EEPROM_WaitFlagBSYIsReset();    //while ((FLASH->SR & FLASH_SR_BSY_Msk) == FLASH_SR_BSY_Msk);
}

void EEPROM_ProgramByte(uint32_t address, uint8_t byteData)
{
	FLASH->PECR &= ~FLASH_PECR_FTDW_Msk;

	/* Write a byte to the address */
	if (IS_VALID_EEPROM_ADDRESS(address))
	{
		if (byteData != 0)
		{
			*(volatile uint8_t*) address = byteData;
		}
	}

	/* Polling bit BSY to be reset */
	EEPROM_WaitFlagBSYIsReset();    //while ((FLASH->SR & FLASH_SR_BSY_Msk) == FLASH_SR_BSY_Msk);
}

/*
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */

