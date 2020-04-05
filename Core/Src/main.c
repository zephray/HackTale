/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

LCD_HandleTypeDef hlcd;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_LCD_Init(void);
static void MX_RTC_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void delay() {
	volatile int x = 1000000;
	while (x--);
}

#define LCD_SEG_THOUSANDS (24)
#define LCD_SEG_DEG (18)
#define LCD_SEG_C (2*32+18)
#define LCD_SEG_F (1*32+18)
#define LCD_SEG_DP1 (1*32+21)
#define LCD_SEG_DP2 (1*32+5)
#define LCD_SEG_DP3 (1*32+24)
#define LCD_SEG_NEG (1*32+25)

void lcd_disp_seg(int seg, int on) {
	uint32_t com = (seg / 32) * 2;
	seg = seg % 32;
	HAL_LCD_Write(&hlcd, com, ~(1ul << seg), on << seg);
}

// number -1 means clear
void lcd_disp_digit(int position, int number) {
	// masks are not in the lut, masks would always be the same as digit 8
	uint32_t digit_lut[3*10*3] = {
		// 0
		(1 << 15),
		(1 << 15) | (1 << 6) | (1 << 10),
		(1 << 15) | (1 << 10),
		// 1
		0,
		(1 << 6) | (1 << 10),
		0,
		// 2
		(1 << 10) | (1 << 15),
		(1 << 6),
		(1 << 15) | (1 << 10),
		// 3
		(1 << 10) | (1 << 15),
		(1 << 6) | (1 << 10),
		(1 << 10),
		// 4
		(1 << 10),
		(1 << 15) | (1 << 6) | (1 << 10),
		0,
		// 5
		(1 << 10) | (1 << 15),
		(1 << 15) | (1 << 10),
		(1 << 10),
		// 6
		(1 << 10) | (1 << 15),
		(1 << 15) | (1 << 10),
		(1 << 15) | (1 << 10),
		// 7
		(1 << 15),
		(1 << 6) | (1 << 10),
		0,
		// 8
		(1 << 10) | (1 << 15),
		(1 << 15) | (1 << 6) | (1 << 10),
		(1 << 15) | (1 << 10),
		// 9
		(1 << 10) | (1 << 15),
		(1 << 15) | (1 << 6) | (1 << 10),
		(1 << 10),
		// 0
		(1 << 6) | (1 << 5),
		(1 << 22) | (1 << 23),
		(1 << 6) | (1 << 23),
		// 1
		0,
		(1 << 22) | (1 << 23),
		0,
		// 2
		(1 << 6) | (1 << 5) | (1 << 23),
		(1 << 22),
		(1 << 23),
		// 3
		(1 << 6) | (1 << 23),
		(1 << 22) | (1 << 23),
		(1 << 23),
		// 4
		(1 << 23),
		(1 << 22) | (1 << 23),
		(1 << 6),
		// 5
		(1 << 6) | (1 << 23),
		(1 << 23),
		(1 << 6) | (1 << 23),
		// 6
		(1 << 6) | (1 << 5) | (1 << 23),
		(1 << 23),
		(1 << 6) | (1 << 23),
		// 7
		(1 << 6),
		(1 << 22) | (1 << 23),
		0,
		// 8
		(1 << 6) | (1 << 5) | (1 << 23),
		(1 << 22) | (1 << 23),
		(1 << 6) | (1 << 23),
		// 9
		(1 << 6) | (1 << 23),
		(1 << 22) | (1 << 23),
		(1 << 6) | (1 << 23),
		// 0
		(1 << 22) | (1 << 21),
		(1 << 19) | (1 << 20),
		(1 << 20) | (1 << 22),
		// 1
		0,
		(1 << 19) | (1 << 20),
		0,
		// 2
		(1 << 22) | (1 << 20) | (1 << 21),
		(1 << 19),
		(1 << 20),
		// 3
		(1 << 22) | (1 << 20),
		(1 << 19) | (1 << 20),
		(1 << 20),
		// 4
		(1 << 20),
		(1 << 19) | (1 << 20),
		(1 << 22),
		// 5
		(1 << 22) | (1 << 20),
		(1 << 20),
		(1 << 20) | (1 << 22),
		// 6
		(1 << 22) | (1 << 20) | (1 << 21),
		(1 << 20),
		(1 << 20) | (1 << 22),
		// 7
		(1 << 22),
		(1 << 19) | (1 << 20),
		0,
		// 8
		(1 << 22) | (1 << 20) | (1 << 21),
		(1 << 19) | (1 << 20),
		(1 << 20) | (1 << 22),
		// 9
		(1 << 22) | (1 << 20),
		(1 << 19) | (1 << 20),
		(1 << 20) | (1 << 22),
	};

	if (number != -1) {
		// set
		HAL_LCD_Write(&hlcd, 0, ~digit_lut[position*30+8*3+0],
				digit_lut[position*30+number*3+0]);
		HAL_LCD_Write(&hlcd, 2, ~digit_lut[position*30+8*3+1],
				digit_lut[position*30+number*3+1]);
		HAL_LCD_Write(&hlcd, 4, ~digit_lut[position*30+8*3+2],
				digit_lut[position*30+number*3+2]);
	}
	else {
		// clear
		HAL_LCD_Write(&hlcd, 0, ~digit_lut[position*30+8*3+0], 0);
		HAL_LCD_Write(&hlcd, 2, ~digit_lut[position*30+8*3+1], 0);
		HAL_LCD_Write(&hlcd, 4, ~digit_lut[position*30+8*3+2], 0);
	}

}

void lcd_disp_number(int num) {
	int thousands = num / 1000;
	int hundreds = num % 1000 / 100;
	int tens = num % 100 / 10;
	int ones = num % 10;
	if (thousands)
		lcd_disp_seg(LCD_SEG_THOUSANDS, 1);
	else
		lcd_disp_seg(LCD_SEG_THOUSANDS, 0);

	if ((thousands != 0) || (hundreds != 0))
		lcd_disp_digit(0, hundreds);
	else
		lcd_disp_digit(0, -1);

	if ((thousands != 0) || (hundreds != 0) || (tens != 0))
		lcd_disp_digit(1, tens);
	else
		lcd_disp_digit(1, -1);

	lcd_disp_digit(2, ones);
}

uint16_t tmp_read(uint8_t addr) {
	uint8_t startAddr = addr; // Temperature Register
	uint8_t buffer[2];

	if (HAL_I2C_Master_Transmit(&hi2c1, 0x48 << 1, &startAddr, 1, HAL_MAX_DELAY) != HAL_OK)
		return 0;
	if (HAL_I2C_Master_Receive(&hi2c1, 0x48 << 1, buffer, 2, HAL_MAX_DELAY) != HAL_OK)
		return 0;

	uint16_t temp = (buffer[0] << 8) | (buffer[1] & 0xff);

	return temp;
}

void tmp_write(uint8_t addr, uint16_t value) {
	//uint8_t startAddr = addr; // Temperature Register
	uint8_t buffer[3];
	buffer[0] = addr;
	buffer[1] = (value >> 8) & 0xff;
	buffer[2] = value & 0xff;

	if (HAL_I2C_Master_Transmit(&hi2c1, 0x48 << 1, buffer, 3, HAL_MAX_DELAY) != HAL_OK)
		return;
}

int temp_conv(uint16_t temp) {
	int16_t raw = (int16_t)temp;
	raw = raw >> 4; // 12 bit, sign extension
	float ft = (float)raw * 0.0625f * 10.0f;
	int it = (int)ft;
	return it;
}

void disp_temp(int it) {
	lcd_disp_number(abs(it));
	lcd_disp_seg(LCD_SEG_NEG, (it < 0) ? 1 : 0);
	HAL_LCD_UpdateDisplayRequest(&hlcd);
}

void send_temp(int it) {
	char buf[32];
	int len = snprintf(buf, 32, "%d.%d\n", it / 10, abs(it % 10));
	if (len >= 32) len = 31; // truncated
	CDC_Transmit_FS(buf, len);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	int16_t it = temp_conv(tmp_read(0x00));
	disp_temp(it);
	send_temp(it);
}

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
  MX_I2C1_Init();
  MX_LCD_Init();
  MX_USB_DEVICE_Init();
  MX_RTC_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

	HAL_LCD_Clear(&hlcd);
	lcd_disp_seg(LCD_SEG_DEG, 1);
	lcd_disp_seg(LCD_SEG_C, 1);
	lcd_disp_seg(LCD_SEG_DP1, 1);

	uint16_t val = tmp_read(0x00);
	printf("0x%x", val);
	tmp_write(0x01, 0x6040);
	val = tmp_read(0x01);
	printf("0x%x", val);

	/* Start Channel1 */
	if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK)
	{
		// error?
	}

  __HAL_SYSCFG_USBPULLUP_ENABLE();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  //asm("wfe");
	  /*for (int i = 0; i < 1000; i++) {
		  lcd_disp_number(i);
		  HAL_LCD_UpdateDisplayRequest(&hlcd);
		  delay();
	  }*/
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_LCD;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.LCDClockSelection = RCC_RTCCLKSOURCE_LSE;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief LCD Initialization Function
  * @param None
  * @retval None
  */
static void MX_LCD_Init(void)
{

  /* USER CODE BEGIN LCD_Init 0 */

  /* USER CODE END LCD_Init 0 */

  /* USER CODE BEGIN LCD_Init 1 */

  /* USER CODE END LCD_Init 1 */
  hlcd.Instance = LCD;
  hlcd.Init.Prescaler = LCD_PRESCALER_1;
  hlcd.Init.Divider = LCD_DIVIDER_16;
  hlcd.Init.Duty = LCD_DUTY_1_3;
  hlcd.Init.Bias = LCD_BIAS_1_2;
  hlcd.Init.VoltageSource = LCD_VOLTAGESOURCE_INTERNAL;
  hlcd.Init.Contrast = LCD_CONTRASTLEVEL_0;
  hlcd.Init.DeadTime = LCD_DEADTIME_5;
  hlcd.Init.PulseOnDuration = LCD_PULSEONDURATION_3;
  hlcd.Init.MuxSegment = LCD_MUXSEGMENT_DISABLE;
  hlcd.Init.BlinkMode = LCD_BLINKMODE_OFF;
  hlcd.Init.BlinkFrequency = LCD_BLINKFREQUENCY_DIV8;
  if (HAL_LCD_Init(&hlcd) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LCD_Init 2 */

  /* USER CODE END LCD_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only 
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 31999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
