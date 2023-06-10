/*
 * l152re_eeprom.h
 *
 *  Created on: Mar 16, 2023
 *      Author: Binh Hoang - binhhv.23.1.99@gmail.com
 */

#ifndef L152RE_EEPROM_H_
#define L152RE_EEPROM_H_

#include "stm32l1xx.h"

#define EEPROM_BANK_1_BASE		(0x08080000UL)
#define EEPROM_BANK_1_END		(0x08081FFFUL)
#define EEPROM_BANK_2_BASE		(0x08082000UL)
#define EEPROM_BANK_2_END		(0x08083FFFUL)
#define FLASH_PEKEY_1			(0x89ABCDEFU)
#define FLASH_PEKEY_2			(0x02030405U)

#define IS_VALID_EEPROM_ADDRESS(ADDR)	((((ADDR) >= EEPROM_BANK_1_BASE) && ((ADDR) <= EEPROM_BANK_2_END)))

void EEPROM_Unlock(void);
void EEPROM_Lock(void);
void EEPROM_EraseWord(uint32_t address);
void EEPROM_EraseByte(uint32_t address);
void EEPROM_ProgramWord(uint32_t address, uint32_t wordData);
void EEPROM_ProgramByte(uint32_t address, uint8_t byteData);

#endif /* L152RE_EEPROM_H_ */
