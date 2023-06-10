/*
 * sim7x00.c
 *
 *  Created on: Jun 22, 2022
 *      Author: This PC
 */

#include "sim7x00.h"

//bool rx_flag = false;
//bool is_ok = false;
//uint8_t rx_buffer[RX_LEN] = { 0 };

//static eSTATUS _send_AT_cmd(char *AT_command, char *response, uint32_t timeout,
//		eAT is_wait)
//{
//	eSTATUS answer = RET_WAIT;
//	uint32_t tick_start = 0;
//	memset(rx_buffer, '\0', RX_LEN);
//
//	HAL_UART_Transmit(&SIM_UART, (uint8_t*) AT_command, strlen(AT_command),
//	HAL_MAX_DELAY);
//	HAL_UART_Transmit(&SIM_UART, (uint8_t*) "\r\n", strlen("\r\n"),
//	HAL_MAX_DELAY);
//
//	if (is_wait == AT_WAIT)
//	{
//		HAL_Delay(650);
//	}
//	tick_start = HAL_GetTick();
//
//	while (answer != RET_OK)
//	{
//		if (HAL_GetTick() - tick_start >= timeout)
//		{
//			answer = RET_TIMEOUT;
//			break;
//		}
//		if (rx_flag == true)
//		{
//			if (strstr((char*) rx_buffer, response) != NULL)
//			{
//				answer = RET_OK;
//				break;
//			}
//			else if (strstr((char*) rx_buffer, "ERROR") != NULL)
//			{
//				answer = RET_ERROR;
//				break;
//			}
//		}
//	}
//	rx_flag = false;
//	return answer;
//}
//static eSTATUS _send_AT_cmd(char *AT_command, char *response, uint32_t timeout,
//		eAT is_wait)
//{
//	eSTATUS answer = RET_WAIT;
//	memset(rx_buffer, '\0', RX_LEN);
//
//	HAL_UART_Transmit(&SIM_UART, (uint8_t*) AT_command, strlen(AT_command),
//	HAL_MAX_DELAY);
//	HAL_UART_Transmit(&SIM_UART, (uint8_t*) "\r\n", strlen("\r\n"),
//	HAL_MAX_DELAY);
//
//	if (is_wait == AT_WAIT)
//	{
//		HAL_Delay(50);    //xxx: 650 -> 50 cho ứng dụng OTA
//	}
//	uint32_t tick_start = HAL_GetTick();
//
//	while (answer != RET_OK)
//	{
//		if (HAL_GetTick() - tick_start >= timeout)
//		{
//			return RET_TIMEOUT;
//		}
//		if (rx_flag == true)
//		{
//			rx_flag = false;
//			if (strstr((char*) rx_buffer, response) != NULL)
//			{
//				return RET_OK;
//			}
//			else if (strstr((char*) rx_buffer, "ERROR") != NULL)
//			{
//				return RET_ERROR;
//			}
//		}
//	}
//	return answer;
//}
//void sim7x00_rx_comming()
//{
//	rx_flag = true;
//	HAL_UARTEx_ReceiveToIdle_DMA(&SIM_UART, rx_buffer, RX_LEN);
//	__HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
//}
//
//char* sim7x00_get_rx_buffer_data()
//{
//	return (char*) rx_buffer;
//}
//
//eSTATUS sim7x00_select_F_disk()
//{
//	return _send_AT_cmd("AT+FSCD=F:", "OK", 3000, AT_NO_WAIT);
//}
//
///*
// * sizeof(p_data) must be > 512 byte
// */
//eSTATUS sim7x00_read_data_from_F_disk(char *filename, uint32_t start_pos_read,
//		uint32_t size, char *p_data)
//{
//	char buffer[80] = { 0 };
//	sprintf(buffer, "AT+CFTRANTX=\"F:/%s\",%ld,%ld", filename, start_pos_read,
//			size);
//	char number[6] = { 0 };
//	sprintf(number, "%ld", size);
//
//	eSTATUS stt = _send_AT_cmd(buffer, number, 4000, AT_WAIT);
//	if (RET_OK == stt)
//	{
//		HAL_Delay(50);
//		memcpy(p_data, (char*) rx_buffer + 23, size);
//
//	}
//	return stt;
//}
//eSTATUS sim7x00_delete_file_on_disk(char *fileName)
//{
//	char buffer[50] = { 0 };
//	sprintf(buffer, "AT+FSDEL=\"/%s\"", fileName);
//	return _send_AT_cmd(buffer, "OK", 3000, AT_NO_WAIT);
//}

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
 */
