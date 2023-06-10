/*
 * flash.h
 *
 *  Created on: Jun 22, 2022
 *      Author: This PC
 */

#ifndef FLASH_FLASH_H_
#define FLASH_FLASH_H_

#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "stm32l1xx_hal.h"

void flash_lock();
void flash_unlock();
void flash_erase(uint32_t pageAddress);
void flash_write_int(uint32_t address, int32_t iData);
void flash_write_array(uint32_t address, char *arrData, uint16_t length);
uint32_t flash_read_int(uint32_t address);
void flash_read_array(uint32_t address, char *arrData, uint16_t length);

#endif /* FLASH_FLASH_H_ */
