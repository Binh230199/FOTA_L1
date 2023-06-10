/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "flash.h"
//#include "sim7x00.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
	FOTA_NO_UPDATE = 0x00,
	FOTA_SUCCESS,
	FOTA_BAD_SIGNAL,
	FOTA_DOWNLOAD_ERROR,
	FOTA_FILE_ERROR,
	FOTA_WRITE_ERROR,
	FOTA_CRC_ERROR,
	FOTA_VERSION_UNNEW,
	FOTA_RESET,
} FOTA_State_Update_t;

/*
 * @brief  FOTA Version Structure definition
 */
typedef struct
{
	uint16_t major;
	uint16_t minor;
	uint16_t patch;
} FOTA_Version_t;

/*
 * @brief  FOTA Firmware informations Structure definition
 */
typedef struct
{
	uint32_t size_of_firmware;
	uint32_t current_address;
	uint32_t new_address;
	char name_firmware[60];
} FOTA_Firmawre_Infor_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FOTA_SIZE_OF_APPLICATION(SIZE)	((SIZE) * 1024)
#define FOTA_SIZE_OF_PAGE				(256)
#define FOTA_BOOTLOADER_START_ADDRESS	0x08000000
#define FOTA_OTA_START_ADDRESS			0x08002000
#define FOTA_APP1_START_ADDRESS			0x08007800
#define FOTA_APP2_START_ADDRESS			0x08023800

#define FOTA_FLAG_NEED_UPDATE			0x0803FC00
#define FOTA_CURRENT_ADDRESS			0x0803FC04
#define FOTA_CURRENT_VER_MAJOR_ADDRESS	0x0803FC08
#define FOTA_CURRENT_VER_MINOR_ADDRESS	0x0803FC0C
#define FOTA_CURRENT_VER_PATCH_ADDRESS	0x0803FC10
#define FOTA_NEW_VER_MAJOR_ADDRESS		0x0803FC14
#define FOTA_NEW_VER_MINOR_ADDRESS		0x0803FC18
#define FOTA_NEW_VER_PATCH_ADDRESS		0x0803FC1C

#define FOTA_STATE_UPDATE				0x0803FD00
#define FOTA_FIRMWARE_SIZE				0x0803FD04
#define FOTA_PHONE_REQUEST				0x0803FD08
#define FOTA_FILENAME					0x0803FD14

#define SIM_UART	huart2
#define USART		USART2
#define RX_LEN		580U

typedef enum
{
	RET_OK, RET_ERROR, RET_TIMEOUT, RET_WAIT,
} eSTATUS;

typedef enum
{
	AT_NO_OK, AT_OK, AT_NO_WAIT, AT_WAIT,
} eAT;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CRC_HandleTypeDef hcrc;

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;

/* USER CODE BEGIN PV */
//extern uint8_t rx_buffer[RX_LEN];
FOTA_Firmawre_Infor_t firmware = { 0 };
uint8_t flag_need_update_fw = 0;
FOTA_State_Update_t fota_state = FOTA_NO_UPDATE;
char phone_number_request[13] = { 0 };
FOTA_Version_t g_current_ver = { 0 };
FOTA_Version_t g_new_ver = { 0 };

bool rx_flag = false;
bool is_ok = false;
uint8_t rx_buffer[RX_LEN] = { 0 };
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_CRC_Init(void);
/* USER CODE BEGIN PFP */
static eSTATUS _send_AT_cmd(const char *AT_command, const char *response, uint32_t timeout, eAT is_wait)
{
	eSTATUS answer = RET_WAIT;
	memset(rx_buffer, '\0', RX_LEN);

	HAL_UART_Transmit(&SIM_UART, (uint8_t*) AT_command, strlen(AT_command),
	HAL_MAX_DELAY);
	HAL_UART_Transmit(&SIM_UART, (uint8_t*) "\r\n", strlen("\r\n"),
	HAL_MAX_DELAY);

	if (is_wait == AT_WAIT)
	{
		HAL_Delay(10);    //xxx: 650 -> 10 cho ứng dụng OTA
	}
	uint32_t tick_start = HAL_GetTick();

	while (answer != RET_OK)
	{
		if (HAL_GetTick() - tick_start >= timeout)
		{
			return RET_TIMEOUT;
		}
		if (rx_flag == true)
		{
			rx_flag = false;
			if (strstr((char*) rx_buffer, response) != NULL)
			{
				return RET_OK;
			}
			else if (strstr((char*) rx_buffer, "ERROR") != NULL)
			{
				return RET_ERROR;
			}
		}
	}
	return answer;
}
void sim7x00_rx_comming();
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if (huart->Instance == USART2)
	{
		sim7x00_rx_comming();
	}
}
void ota_enable_send_AT()
{
	DTR_GPIO_Port->BSRR = (uint32_t) DTR_Pin << 16;    //HAL_GPIO_WritePin(DTR_GPIO_Port, DTR_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
}
void ota_disable_send_AT()
{
	HAL_UART_Transmit(&SIM_UART, (uint8_t*) "AT+CSCLK=1\r\n", strlen("AT+CSCLK=1\r\n"), HAL_MAX_DELAY);
	HAL_Delay(100);
	DTR_GPIO_Port->BSRR = (uint32_t) DTR_Pin;    // << 16;    //HAL_GPIO_WritePin(DTR_GPIO_Port, DTR_Pin, GPIO_PIN_RESET);
}

bool ota_write_firmware()
{
	/* Kiểm tra kích thước firmware có nằm trong khoảng giới hạn của vùng nhớ? */
	if ((firmware.size_of_firmware > 0) && (firmware.size_of_firmware < FOTA_SIZE_OF_APPLICATION(112)))
	{
		/* Mở khoá FLASH để thao tác với FLASH */
		HAL_FLASH_Unlock();
		/* Xoá vùng nhớ FLASH */
		for (int page = 0; page < 448; page++)    //TODO: de y truong hop, khi + 1 thi lo firmware gan 110k thi no co xoa sang vung tiep theo khong???
		{
			flash_erase((uint32_t) firmware.new_address + page * FOTA_SIZE_OF_PAGE);
		}
		/* Chọn ổ nhớ F để đọc dữ liệu ra */
		_send_AT_cmd("AT+FSCD=F:", "OK", 3000, AT_NO_WAIT);

		uint32_t rest_size_of_firmware = firmware.size_of_firmware;
		uint32_t byte_to_write = 0;
		uint16_t page_index = 0;
		char buffer[100] = { 0 };
		uint16_t byte = 0;
		while (rest_size_of_firmware > 0)
		{
			memset(buffer, '\0', 100);
			if (rest_size_of_firmware >= 512)    //512
			{
				sprintf(buffer, "AT+CFTRANTX=\"F:/%s\",%ld,%ld", firmware.name_firmware, byte_to_write, (uint32_t) 512);
				byte = 512;
				byte_to_write += 512;
				rest_size_of_firmware -= 512;
			}
			else
			{
				sprintf(buffer, "AT+CFTRANTX=\"F:/%s\",%ld,%ld", firmware.name_firmware, byte_to_write,
						(uint32_t) rest_size_of_firmware);
				byte = rest_size_of_firmware;
				byte_to_write += rest_size_of_firmware;
				rest_size_of_firmware -= rest_size_of_firmware;
			}
			if (RET_OK == _send_AT_cmd(buffer, "+CFTRANTX:", 3000, AT_WAIT))
			{
				/* 2. WRITE */
				flash_write_array((uint32_t) firmware.new_address + (page_index++ * (FOTA_SIZE_OF_PAGE * 2)),
						(char*) rx_buffer + 23, byte);
			}
		}
		/* Khoá bộ nhớ FLASH */
		HAL_FLASH_Lock();
		sprintf(buffer, "AT+FSDEL=\"/%s\"", firmware.name_firmware);
		_send_AT_cmd(buffer, "OK", 3000, AT_NO_WAIT);
		return true;
	}
	char buffer[100] = { 0 };
	sprintf(buffer, "AT+FSDEL=\"/%s\"", firmware.name_firmware);
	_send_AT_cmd(buffer, "OK", 3000, AT_NO_WAIT);
	return false;
}

bool ota_check_CRC(uint32_t address)
{
	uint32_t crc = HAL_CRC_Calculate(&hcrc, (uint32_t*) address, firmware.size_of_firmware / 4);
	if (crc == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_USART2_UART_Init();
	MX_CRC_Init();
	/* USER CODE BEGIN 2 */
	ota_enable_send_AT();

	HAL_UARTEx_ReceiveToIdle_DMA(&SIM_UART, rx_buffer, RX_LEN);
	__HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);

	flag_need_update_fw = flash_read_int(FOTA_FLAG_NEED_UPDATE);
	if (flag_need_update_fw == 1)
	{
		firmware.current_address = flash_read_int(FOTA_CURRENT_ADDRESS);
		g_current_ver.major = flash_read_int(
		FOTA_CURRENT_VER_MAJOR_ADDRESS);
		g_current_ver.minor = flash_read_int(
		FOTA_CURRENT_VER_MINOR_ADDRESS);
		g_current_ver.patch = flash_read_int(
		FOTA_CURRENT_VER_PATCH_ADDRESS);
		firmware.size_of_firmware = flash_read_int(FOTA_FIRMWARE_SIZE);
		flash_read_array(FOTA_FILENAME, firmware.name_firmware, 60);
		flash_read_array(FOTA_PHONE_REQUEST, phone_number_request, 12);

		if (firmware.current_address == FOTA_APP1_START_ADDRESS)
		{
			firmware.new_address = FOTA_APP2_START_ADDRESS;
		}
		else if (firmware.current_address == FOTA_APP2_START_ADDRESS)
		{
			firmware.new_address = FOTA_APP1_START_ADDRESS;
		}
		if (ota_write_firmware() == true) /* Write OK */
		{
			HAL_Delay(2000);
			/* Calculate CRC-32 */
			if (ota_check_CRC(firmware.new_address))
			{
				fota_state = FOTA_SUCCESS;
			}
			else
			{
				fota_state = FOTA_CRC_ERROR;
			}
		}
		else /* Write fail */
		{
			fota_state = FOTA_WRITE_ERROR;
		}
		ota_disable_send_AT();
	}

	switch (fota_state)
	{
		case FOTA_NO_UPDATE:
			break;
		case FOTA_SUCCESS:
			g_new_ver.major = flash_read_int(
			FOTA_NEW_VER_MAJOR_ADDRESS);
			g_new_ver.minor = flash_read_int(
			FOTA_NEW_VER_MINOR_ADDRESS);
			g_new_ver.patch = flash_read_int(
			FOTA_NEW_VER_PATCH_ADDRESS);
			firmware.current_address = firmware.new_address;

			HAL_FLASH_Unlock();
			flash_erase(FOTA_FLAG_NEED_UPDATE);
			flash_write_int(FOTA_FLAG_NEED_UPDATE, 0x00000000);
			flash_write_int(FOTA_CURRENT_ADDRESS, firmware.current_address);
			flash_write_int(FOTA_CURRENT_VER_MAJOR_ADDRESS, g_new_ver.major);
			flash_write_int(FOTA_CURRENT_VER_MINOR_ADDRESS, g_new_ver.minor);
			flash_write_int(FOTA_CURRENT_VER_PATCH_ADDRESS, g_new_ver.patch);

			flash_erase(FOTA_STATE_UPDATE);
			flash_write_int(FOTA_STATE_UPDATE, fota_state);
			flash_write_array(FOTA_PHONE_REQUEST, phone_number_request, 12);
			HAL_FLASH_Lock();
			break;
		case FOTA_FILE_ERROR:
			HAL_FLASH_Unlock();
			flash_erase(FOTA_FLAG_NEED_UPDATE);
			flash_write_int(FOTA_FLAG_NEED_UPDATE, 0x00000000);
			flash_write_int(FOTA_CURRENT_ADDRESS, firmware.current_address);
			flash_write_int(FOTA_CURRENT_VER_MAJOR_ADDRESS, g_current_ver.major);
			flash_write_int(FOTA_CURRENT_VER_MINOR_ADDRESS, g_current_ver.minor);
			flash_write_int(FOTA_CURRENT_VER_PATCH_ADDRESS, g_current_ver.patch);
			flash_erase(FOTA_STATE_UPDATE);
			flash_write_int(FOTA_STATE_UPDATE, fota_state);
			flash_write_array(FOTA_PHONE_REQUEST, phone_number_request, 12);
			HAL_FLASH_Lock();
			break;
		case FOTA_WRITE_ERROR:
			HAL_FLASH_Unlock();
			flash_erase(FOTA_FLAG_NEED_UPDATE);
			flash_write_int(FOTA_FLAG_NEED_UPDATE, 0x00000000);
			flash_write_int(FOTA_CURRENT_ADDRESS, firmware.current_address);
			flash_write_int(FOTA_CURRENT_VER_MAJOR_ADDRESS, g_current_ver.major);
			flash_write_int(FOTA_CURRENT_VER_MINOR_ADDRESS, g_current_ver.minor);
			flash_write_int(FOTA_CURRENT_VER_PATCH_ADDRESS, g_current_ver.patch);
			flash_erase(FOTA_STATE_UPDATE);
			flash_write_int(FOTA_STATE_UPDATE, fota_state);
			flash_write_array(FOTA_PHONE_REQUEST, phone_number_request, 12);
			HAL_FLASH_Lock();
			break;
		case FOTA_CRC_ERROR:
			HAL_FLASH_Unlock();
			flash_erase(FOTA_FLAG_NEED_UPDATE);
			flash_write_int(FOTA_FLAG_NEED_UPDATE, 0x00000000);
			flash_write_int(FOTA_CURRENT_ADDRESS, firmware.current_address);
			flash_write_int(FOTA_CURRENT_VER_MAJOR_ADDRESS, g_current_ver.major);
			flash_write_int(FOTA_CURRENT_VER_MINOR_ADDRESS, g_current_ver.minor);
			flash_write_int(FOTA_CURRENT_VER_PATCH_ADDRESS, g_current_ver.patch);
			flash_erase(FOTA_STATE_UPDATE);
			flash_write_int(FOTA_STATE_UPDATE, fota_state);
			flash_write_array(FOTA_PHONE_REQUEST, phone_number_request, 12);
			HAL_FLASH_Lock();
			break;
		default:
			break;
	}
	NVIC_SystemReset();
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
	RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief CRC Initialization Function
 * @param None
 * @retval None
 */
static void MX_CRC_Init(void)
{

	/* USER CODE BEGIN CRC_Init 0 */

	/* USER CODE END CRC_Init 0 */

	/* USER CODE BEGIN CRC_Init 1 */

	/* USER CODE END CRC_Init 1 */
	hcrc.Instance = CRC;
	if (HAL_CRC_Init(&hcrc) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN CRC_Init 2 */

	/* USER CODE END CRC_Init 2 */

}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void)
{

	/* USER CODE BEGIN USART2_Init 0 */

	/* USER CODE END USART2_Init 0 */

	/* USER CODE BEGIN USART2_Init 1 */

	/* USER CODE END USART2_Init 1 */
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART2_Init 2 */

	/* USER CODE END USART2_Init 2 */

}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void)
{

	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA1_Channel6_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(DTR_GPIO_Port, DTR_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : DTR_Pin */
	GPIO_InitStruct.Pin = DTR_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(DTR_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void sim7x00_rx_comming()
{
	rx_flag = true;
	HAL_UARTEx_ReceiveToIdle_DMA(&SIM_UART, rx_buffer, RX_LEN);
	__HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
}

char* sim7x00_get_rx_buffer_data()
{
	return (char*) rx_buffer;
}

//eSTATUS sim7x00_select_F_disk()
//{
//	return _send_AT_cmd("AT+FSCD=F:", "OK", 3000, AT_NO_WAIT);
//}
//
///*
// * sizeof(p_data) must be > 512 byte
// */
//eSTATUS sim7x00_read_data_from_F_disk(char *filename, uint32_t start_pos_read,
//		uint32_t size, uint16_t *flash_index)
//{
//	char buffer[80] = { 0 };
//	sprintf(buffer, "AT+CFTRANTX=\"F:/%s\",%ld,%ld", filename, start_pos_read,
//			size);
//	char number[6] = { 0 };
//	sprintf(number, "%ld", size);
//
//	eSTATUS stt = _send_AT_cmd(buffer, number, 3000, AT_WAIT);
//	HAL_Delay(50);
//	if (RET_OK == stt)
//	{
//		/* 2. WRITE */
//		flash_write_array(
//				(uint32_t) firmware.new_address
//						+ ((*flash_index)++ * FOTA_SIZE_OF_PAGE),
//				(char*) rx_buffer + 23,
//				FOTA_SIZE_OF_PAGE);
//		flash_write_array(
//				(uint32_t) firmware.new_address
//						+ ((*flash_index)++ * FOTA_SIZE_OF_PAGE),
//				(char*) rx_buffer + 23 + FOTA_SIZE_OF_PAGE,
//				FOTA_SIZE_OF_PAGE);
//	}
//	return stt;
//}
//eSTATUS sim7x00_delete_file_on_disk(char *fileName)
//{
//	char buffer[50] = { 0 };
//	sprintf(buffer, "AT+FSDEL=\"/%s\"", fileName);
//	return _send_AT_cmd(buffer, "OK", 3000, AT_NO_WAIT);
//}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
		NVIC_SystemReset();
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

