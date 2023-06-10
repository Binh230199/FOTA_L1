/*
 * main.h
 *
 *  Created on: Mar 16, 2023
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdint.h>

#define BOOT_START_ADDR 	0x08000000U
#define BOOT_MAX_SIZE		(32U)
#define BOOT_END_ADDR		((BOOT_START_ADDR) + ((BOOT_MAX_SIZE) * 1024))

#define APP_1_START_ADDR 	BOOT_END_ADDR
#define APP_1_MAX_SIZE		(240U)
#define APP_1_END_ADDR		((APP_1_START_ADDR) + ((APP_1_MAX_SIZE) * 1024))

#define APP_2_START_ADDR 	APP_1_END_ADDR
#define APP_2_MAX_SIZE		(APP_1_MAX_SIZE)
#define APP_2_END_ADDR		((APP_2_START_ADDR) + ((APP_2_MAX_SIZE) * 1024))

#define NUMBER_FLASH_PAGES	((APP_1_MAX_SIZE) / (256U))

/* Next variable's EEPROM address = Prev variable's EEPROM address + sizeof(Prev variable's EEPROM address) */
#define CURRENT_ADDR	(FLASH_EEPROM_BASE)
#define CURRENT_MAJOR 	(CURRENT_ADDR + 4)
#define CURRENT_MINOR 	(CURRENT_MAJOR + 1)
#define CURRENT_PATCH 	(CURRENT_MINOR + 1)

void System_ClockConfig(void);
void DeInit(void);
void gotoFirmware(uint32_t address);

#endif /* MAIN_H_ */
