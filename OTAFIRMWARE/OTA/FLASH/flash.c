/*
 * flash.c
 *
 *  Created on: Jun 22, 2022
 *      Author: This PC
 */

#include "flash.h"

/**
 * @brief Unlock the flash.
 * @retval None
 */
void flash_unlock()
{
	HAL_FLASH_Unlock();
}
/**
 * @brief Lock the flash.
 * @retval None
 */
void flash_lock()
{
	HAL_FLASH_Lock();
}
/**
 * @brief Erase a page.
 * @param pageAddress: start address of page
 * @retval None
 */
void flash_erase(uint32_t pageAddress)
{
	while (__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY));
	FLASH->PECR |= FLASH_PECR_ERASE;
	FLASH->PECR |= FLASH_PECR_PROG;
	while (__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY));
	*(__IO uint32_t*) (uint32_t) (pageAddress & ~(FLASH_PAGE_SIZE - 1)) =
			0x00000000;
	CLEAR_BIT(FLASH->PECR, FLASH_PECR_PROG);
	CLEAR_BIT(FLASH->PECR, FLASH_PECR_ERASE);
}
/**
 * @brief Write a value in integer type
 * @param address: address want to write the value
 * @param iData: value in integer type
 * @retval None
 */
void flash_write_int(uint32_t address, int32_t iData)
{
	while (__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY));    //quá trình ghi dữ liệu, tương ứng trong hàm HAL_FLASH_Program
	*(__IO uint32_t*) address = iData;    //dữ liệu ở đây là HALFWORD, WORD là uint32_t
	while (__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY));
}
/**
 * @brief Write an array in char type
 * @param address: address want to write the value
 * @param arrData: data array
 * @param length: length of arrData
 * @retval None
 */
void flash_write_array(uint32_t address, char *arrData, uint16_t length)
{
	uint32_t *pt = (uint32_t*) arrData;
	for (uint16_t i = 0; i < (length + 1) / 4; i++)
	{
		while (__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY));
		*(__IO uint32_t*) (address + 4 * i) = *pt;
		pt++;
	}
	while (__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY));
	CLEAR_BIT(FLASH->PECR, FLASH_PECR_PROG);
	CLEAR_BIT(FLASH->PECR, FLASH_PECR_ERASE);
}
/**
 * @brief Read value in an address
 * @param address: address want to read the value
 * @retval value in address
 */
uint32_t flash_read_int(uint32_t address)
{
	uint32_t *val = (uint32_t*) address;
	return *val;
}
/**
 * @brief Read array value start at an address
 * @param address: address want to read the value
 * @param arrData: buffer to store the data
 * @param length: data length want to read
 * @retval None
 */
void flash_read_array(uint32_t address, char *arrData, uint16_t length)
{
	uint16_t *pt = (uint16_t*) arrData;

	for (uint16_t i = 0; i < (length + 1) / 2; i++)
	{
		*pt = *(__IO uint16_t*) (address + 2 * i);
		pt++;
	}
}
