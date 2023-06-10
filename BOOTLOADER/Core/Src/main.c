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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FOTA_FLAG_NEED_UPDATE			0x0803FC00
#define FOTA_CURRENT_ADDRESS			0x0803FC04
#define FOTA_CURRENT_VER_MAJOR_ADDRESS	0x0803FC08
#define FOTA_CURRENT_VER_MINOR_ADDRESS	0x0803FC0C
#define FOTA_CURRENT_VER_PATCH_ADDRESS	0x0803FC10
#define FOTA_OTA_START_ADDRESS			0x08002000
#define FOTA_APP1_START_ADDRESS			0x08007800
#define FOTA_STATE_UPDATE				0x0803FD00
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
uint32_t flash_read_int(uint32_t address)
{
	uint32_t *val = (uint32_t*) address;
	return *val;
}
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
void flash_write_int(uint32_t address, int32_t iData)
{
	while (__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY));    //quá trình ghi dữ liệu, tương ứng trong hàm HAL_FLASH_Program
	*(__IO uint32_t*) address = iData;    //dữ liệu ở đây là HALFWORD, WORD là uint32_t
	while (__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY));
}
void fota_goto_firmware(uint32_t address)
{
	HAL_RCC_DeInit();    //Tắt hết ngoại vi xoá hết c�? ngắt

	HAL_DeInit();			//Xoá hết các Pending, đồng th�?i tắt System Tick

	SCB->SHCSR &= ~( SCB_SHCSR_USGFAULTENA_Msk |
	SCB_SHCSR_BUSFAULTENA_Msk |
	SCB_SHCSR_MEMFAULTENA_Msk);

	__set_MSP(*((__IO uint32_t*) address));

	uint32_t jump_address = *((__IO uint32_t*) (address + 4));
	void (*reset_handler)(void) = (void*) jump_address;
	reset_handler();
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
	/* USER CODE BEGIN 2 */
	uint32_t firmware_address_to_jump = flash_read_int(
	FOTA_CURRENT_ADDRESS);
	/* Ở lần chạy đầu tiên, chưa có gì trong flash nên cần phải cài đặt địa chỉ
	 * cho lần chạy đầu tiên là địa chỉ bắt đầu của vùng nhớ thứ nhất.
	 */
	if (firmware_address_to_jump == 0x00000000)
	{
		firmware_address_to_jump = FOTA_APP1_START_ADDRESS;
		HAL_FLASH_Unlock();
		flash_erase(FOTA_FLAG_NEED_UPDATE);
		flash_write_int(FOTA_CURRENT_ADDRESS, firmware_address_to_jump);
		flash_write_int(FOTA_CURRENT_VER_MAJOR_ADDRESS, 0x00);
		flash_write_int(FOTA_CURRENT_VER_MINOR_ADDRESS, 0x00);
		flash_write_int(FOTA_CURRENT_VER_PATCH_ADDRESS, 0x00);
		flash_erase(FOTA_STATE_UPDATE);
		flash_write_int(FOTA_STATE_UPDATE, 0);
		HAL_FLASH_Lock();
	}
	/* Nếu có cờ yêu cầu cập nhật thì nhảy đến địa chỉ của chương trình OTA
	 * Nếu không có thì nhảy đến địa chỉ đọc được từ bộ nhớ flash hoặc là
	 * địa chỉ vùng 1 ở lần chạy đầu tiên.
	 */
	uint8_t flag_need_update_fw = flash_read_int(FOTA_FLAG_NEED_UPDATE);
	if (flag_need_update_fw == 0x00000001)
	{
		firmware_address_to_jump = FOTA_OTA_START_ADDRESS;
	}

	fota_goto_firmware(firmware_address_to_jump);
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
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = 0;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

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

