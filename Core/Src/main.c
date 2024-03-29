/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : STM32L476 Nucleo-64 Workbench Tests @ WUT Mchtr
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020 STMicroelectronics.<br>
  * Copyright (c) 2021 Mateusz Szumilas @ WUT Faculty of Mechatronics.<br>
  * All rights reserved.
  *
  * This software component is licensed under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/

//<<<<<<< HEAD
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "lcd.h"
#include <flick.h>
#include <lsm6ds33_reg.h>
#include <main.h>
#include <math.h>
#include <stdio.h>
#include <stm32_adafruit_lcd.h>
#include <stm32_hal_legacy.h>
#include <stm32l476xx.h>
#include <stm32l4xx_hal_def.h>
#include <stm32l4xx_hal_flash.h>
#include <stm32l4xx_hal_gpio.h>
#include <stm32l4xx_hal_i2c.h>
#include <stm32l4xx_hal_i2c_ex.h>
#include <stm32l4xx_hal_pwr_ex.h>
#include <stm32l4xx_hal_rcc.h>
#include <stm32l4xx_hal_rcc_ex.h>
#include <stm32l4xx_hal_spi.h>
#include <stm32l4xx_hal_tim.h>
#include <stm32l4xx_hal_tim_ex.h>
#include <stm32l4xx_hal_uart.h>
#include <stm32l4xx_hal_uart_ex.h>
#include <sys/_stdint.h>

#include "stepper.h"
#include "magneto.h"
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
I2C_HandleTypeDef hi2c2;

SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

I2C_HandleTypeDef *hi2cflick = &hi2c1;

SPI_HandleTypeDef *hnucleo_Spi = &hspi2;

int pulse_cnt;
int servo_speed=50;
uint8_t rotation_cnt=0;
uint8_t position = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM2_Init(void);
static void MX_SPI2_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C2_Init(void);
/* USER CODE BEGIN PFP */

void StInit(void);
void StMenu(void);
void StAzimuth(void);
void StRange(void);
void StEdit (void);
void StSpeed (void);
void flick_set_value(uint16_t *value, uint16_t min_value, uint16_t max_value);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void (*state_array[])() = {StInit, StMenu, StAzimuth,StRange,StEdit,StSpeed};
/* an enumerated type used for indexing the state_array */
typedef enum {ST_INIT, ST_MENU, ST_AZIMUTH, ST_RANGE, ST_EDIT,ST_SPEED} state_name_t;

state_name_t current_state;
state_name_t last_state;
state_name_t select_mode = 2;


uint8_t opt;
uint8_t last_opt;

uint16_t set_angle_1;
uint16_t set_angle_2;

uint32_t gesture, touch;
airwheel_data_t airwheel;

//stepper
STEPPER stepper;

void Pulse_Counter(void)
{
	if (pulse_cnt > 0) pulse_cnt--;
	else
	{
		HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_2);
		__HAL_TIM_SET_COUNTER(&htim2, 2000);
	}
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
  current_state = ST_INIT;
  last_state = ST_INIT;
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_TIM4_Init();
  MX_TIM2_Init();
  MX_SPI2_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */

  /* Magneto */
  /* cfg magneto */

  /*wpisanie funkcji cfg*/
  cfg_magneto(&hi2c2);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  state_array[current_state]();

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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_I2C1
                              |RCC_PERIPHCLK_I2C2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
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
  hi2c1.Init.Timing = 0x00702991;
  hi2c1.Init.OwnAddress1 = 132;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x00702991;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 7;
  hspi2.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

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
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 80;
  htim2.Init.CounterMode = TIM_COUNTERMODE_DOWN;
  htim2.Init.Period = 2000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 1000;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 8000;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_GATED;
  sSlaveConfig.InputTrigger = TIM_TS_TI2FP2;
  sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_FALLING;
  sSlaveConfig.TriggerFilter = 15;
  if (HAL_TIM_SlaveConfigSynchro(&htim4, &sSlaveConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_ENABLE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

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
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LCD_DC_Pin|LCD_RST_Pin|LCD_CS_Pin|FLICK_TS_Pin
                          |FLICK_RESET_Pin|MOT_REF_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|MOT_DIR1_Pin|MOT_RESET_Pin|MOT_E_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, MOT_MODE1_Pin|MOT_MODE2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_DC_Pin LCD_CS_Pin */
  GPIO_InitStruct.Pin = LCD_DC_Pin|LCD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_RST_Pin */
  GPIO_InitStruct.Pin = LCD_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LCD_RST_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD2_Pin MOT_DIR1_Pin MOT_RESET_Pin MOT_E_Pin */
  GPIO_InitStruct.Pin = LD2_Pin|MOT_DIR1_Pin|MOT_RESET_Pin|MOT_E_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : FLICK_TS_Pin */
  GPIO_InitStruct.Pin = FLICK_TS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(FLICK_TS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : FLICK_RESET_Pin MOT_REF_Pin */
  GPIO_InitStruct.Pin = FLICK_RESET_Pin|MOT_REF_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : MOT_MODE1_Pin MOT_MODE2_Pin */
  GPIO_InitStruct.Pin = MOT_MODE1_Pin|MOT_MODE2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void StInit(void)
{
	/* Stepstick */
	  HAL_TIM_Base_Start(&htim4);

	  HAL_GPIO_WritePin(MOT_RESET_GPIO_Port, MOT_RESET_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(MOT_MODE1_GPIO_Port, MOT_MODE1_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(MOT_MODE2_GPIO_Port, MOT_MODE2_Pin, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(MOT_RESET_GPIO_Port, MOT_RESET_Pin, GPIO_PIN_SET);

	  /* LCD */
	  BSP_LCD_Init();
	  BSP_LCD_DisplayOn();

	  /* Sensors */
	  HAL_I2C_Init(&hi2c1);
	  HAL_I2C_Init(&hi2c2);

	  /* Flick */
	  flick_reset();
	  flick_set_param(0x90, 0x20, 0x20);


  stepper = stepper_inizialize(
			GPIOC, GPIO_PIN_7,
			GPIOA, GPIO_PIN_8,
			GPIOA, GPIO_PIN_9,
			GPIOA, GPIO_PIN_10,
			GPIOB, GPIO_PIN_4,
			GPIOB, GPIO_PIN_6,
			&htim2, TIM_CHANNEL_2);


	  /* IMU */
	  uint8_t i2c2_buf[10];
	  HAL_I2C_Mem_Read(&hi2c2, ACC_GYRO_ADDR, WHO_AM_I, I2C_MEMADD_SIZE_8BIT, i2c2_buf, 1, 1);
	  HAL_I2C_Mem_Read(&hi2c2, 0x1e<<1, 0x0f, I2C_MEMADD_SIZE_8BIT, i2c2_buf+1, 1, 1);

	  /* cfg gyro */
	  i2c2_buf[0] = 0x38;		// enable X,Y,Z axes
	  HAL_I2C_Mem_Write(&hi2c2, ACC_GYRO_ADDR, CTRL10_C, I2C_MEMADD_SIZE_8BIT, i2c2_buf, 1, 1);
	  i2c2_buf[0] = 0x14;		// 13 Hz ODR, 500 dps
	  HAL_I2C_Mem_Write(&hi2c2, ACC_GYRO_ADDR, CTRL2_G, I2C_MEMADD_SIZE_8BIT, i2c2_buf, 1, 1);

	  // cfg acc
	  i2c2_buf[0] = 0x38;		// enable X,Y,Z axes
	  HAL_I2C_Mem_Write(&hi2c2, ACC_GYRO_ADDR, CTRL9_XL, I2C_MEMADD_SIZE_8BIT, i2c2_buf, 1, 1);
	  i2c2_buf[0] = 0x10;		// 13 Hz ODR, +/- 2 g
	  HAL_I2C_Mem_Write(&hi2c2, ACC_GYRO_ADDR, CTRL1_XL, I2C_MEMADD_SIZE_8BIT, i2c2_buf, 1, 1);

	  current_state = ST_MENU;
}

void StMenu(void)
{
	if(current_state != last_state)
	{
		char str[40];
		BSP_LCD_Clear(LCD_COLOR_WHITE);

		sprintf(str, "MENU");
		BSP_LCD_DisplayStringAt(0, 1, (uint8_t *) str, CENTER_MODE);

		sprintf(str, "Wybierz opcje:");
		BSP_LCD_DisplayStringAt(0, 15, (uint8_t *) str, CENTER_MODE);

		sprintf(str, "<- Wybor trybu");
		BSP_LCD_DisplayStringAt(0, 70, (uint8_t *) str, LEFT_MODE);

		sprintf(str, "Ustaw predkosc ->");
		BSP_LCD_DisplayStringAt(0, 90, (uint8_t *) str, RIGHT_MODE);

		sprintf(str, "Aktywuj");
		BSP_LCD_DisplayStringAt(0, 147, (uint8_t *) str, CENTER_MODE);

		last_state = current_state;
	}

	// Odczyt gestu flick
	flick_poll_data(&gesture, &touch, &airwheel);
	HAL_Delay(10);
	flick_interaction_t interaction=flick_get_interaction(gesture,touch,airwheel);

	// Przejście do odpowiedniego stanus

	if(interaction == FLICK_TOUCH_LEWO)
	{
		current_state = ST_EDIT;
	}
	else if(interaction == FLICK_TOUCH_PRAWO)
	{
		current_state = ST_SPEED;
	}
	else if(interaction == FLICK_TOUCH_DOL)
	{
		current_state = select_mode;
	}
}
void StAzimuth(void)
{
	char str[40];
	uint8_t angle = 0;
	uint8_t last_angle = 0;

	// odczyt z magnetometru
	// wywolanie funkcji kierunek
	angle = kierunek_kompas(&hi2c2);	//0-255

	flick_poll_data(&gesture, &touch, &airwheel);

	if(current_state != last_state)
	{
		BSP_LCD_Clear(LCD_COLOR_WHITE);

		sprintf(str, "Utrzymanie azymutu");
		BSP_LCD_DisplayStringAt(0, 10, (uint8_t *) str, CENTER_MODE);
		BSP_LCD_DisplayStringAt(0, 20, "N", CENTER_MODE);

		sprintf(str, "Powrot do MENU");
		BSP_LCD_DisplayStringAt(0, 147, (uint8_t *) str, CENTER_MODE);
		last_state = current_state;
	}

	//uint8_t set_angle_1 = 3.14/3;	// kat testowy
	if (set_angle_1 != angle)
	{
		LCD_PrintDirection(set_angle_1, set_angle_1, 64, 80, 50, LCD_COLOR_BLACK); // zadany azymut
	}

	//angle = 3.14/4;	// kat testowy
	LCD_PrintDirection(angle, last_angle, 64, 80, 50, LCD_COLOR_RED); //aktualny azymut

	last_angle = angle;

	// dodać sterowanie silnikiem

	uint8_t set_position = angle - set_angle_1;
	if(!stepper.is_working)
	{
		stepper_set_destination(&stepper,set_position,2);
		stepper_proceed(&stepper);
	}

	// Przejść do stanu Menu
	flick_interaction_t interaction = flick_get_interaction(gesture,touch,airwheel);

	if(interaction == FLICK_TOUCH_SRODEK)
	{
		current_state = ST_MENU;
	}
}
void StRange(void)
{
	char str[40];

	uint8_t angle;
	uint8_t last_angle = 0;

	angle = kierunek_kompas(&hi2c2);	//0-255

	// odczyt z magnetometru

	flick_poll_data(&gesture, &touch, &airwheel);
	if(current_state != last_state)
	{
		BSP_LCD_Clear(LCD_COLOR_WHITE);

		sprintf(str, "Zakres");
		BSP_LCD_DisplayStringAt(0, 10, (uint8_t *) str, CENTER_MODE);

		sprintf(str, "Powrot do MENU");
		BSP_LCD_DisplayStringAt(0, 147, (uint8_t *) str, CENTER_MODE);
		last_state = current_state;
	}

	//angle = 0;	// kat testowy
	LCD_PrintDirection(angle, last_angle, 64, 80, 50, LCD_COLOR_RED); //aktualny azymut

	//zadany przez użytkownika dolny ogranicznik
	//uint8_t set_angle_1 = 0;	// kat testowy
	if (set_angle_1 != angle)
	{
		LCD_PrintDirection(set_angle_1, set_angle_1, 64, 80, 50, LCD_COLOR_BLACK); //ogranicznik 1
	}
	//zadany przez użytkownika gorny ogranicznik
	//uint8_t set_angle_2 = 0;	// kat testowy

	if (set_angle_2 != angle)
	{
		LCD_PrintDirection(set_angle_2, set_angle_2, 64, 80, 50, LCD_COLOR_BLACK); // ogranicznik 2
	}

	//sterowanie silnikiem
	uint8_t set_angle = set_angle_1;
	if(set_angle_1 == angle) angle = set_angle_2;
	if(set_angle_2 == angle) angle = set_angle_1;
	uint8_t set_position = angle - set_angle;

	if(!stepper.is_working)
	{
		stepper_set_destination(&stepper,set_position,2);
		stepper_proceed(&stepper);
	}

	// Przejść do stanu Menu
	flick_interaction_t interaction = flick_get_interaction(gesture,touch,airwheel);

	if(interaction == FLICK_TOUCH_SRODEK)
	{
		current_state = ST_MENU;
	}
}
void StEdit (void)
{
	char str[40];

	flick_poll_data(&gesture, &touch, &airwheel);
	if(current_state != last_state)
	{

		opt = 0;
		last_opt = 1;
		last_state = current_state;
	}

	if(opt == 0)
	{
		if(last_opt != opt)
		{
			BSP_LCD_Clear(LCD_COLOR_WHITE);
			sprintf(str, "Wybierz tryb");
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t *) str, CENTER_MODE);

			sprintf(str, "Zatwierdz");
			BSP_LCD_DisplayStringAt(0, 147, (uint8_t *) str, CENTER_MODE);
		}

		uint8_t tmp = select_mode*10;
		flick_set_value(&tmp, ST_AZIMUTH*10, ST_RANGE*10);
		select_mode = tmp/10;

		if(select_mode == ST_AZIMUTH)
		{
			sprintf(str, "     Azymut    ");
			BSP_LCD_DisplayStringAt(0, 60, (uint8_t *) str, CENTER_MODE);
		}
		else if(select_mode == ST_RANGE)
		{
			sprintf(str, "     Zakres     ");
			BSP_LCD_DisplayStringAt(0, 60, (uint8_t *) str, CENTER_MODE);
		}

		last_opt = opt;
	}

	else if(opt == 1)
	{
		if(last_opt != opt)
		{
			BSP_LCD_Clear(LCD_COLOR_WHITE);
			sprintf(str, "Podaj dol wart");
			BSP_LCD_DisplayStringAt(0, 10, (uint8_t *) str, CENTER_MODE);

			sprintf(str, "Zatwierdz");
			BSP_LCD_DisplayStringAt(0, 147, (uint8_t *) str, CENTER_MODE);
		}

		flick_set_value(&set_angle_1, 0, 255);

		sprintf(str, "   %02d   ", set_angle_1);
		BSP_LCD_DisplayStringAt(0, 60, (uint8_t *) str, CENTER_MODE);
		last_opt = opt;
	}

	else if(opt == 2)
	{
		if(select_mode == ST_RANGE)
		{
			if(last_opt != opt)
			{
				BSP_LCD_Clear(LCD_COLOR_WHITE);
				sprintf(str, "Podaj gorna wart");
				BSP_LCD_DisplayStringAt(0, 10, (uint8_t *) str, CENTER_MODE);

				sprintf(str, "Zatwierdz");
				BSP_LCD_DisplayStringAt(0, 147, (uint8_t *) str, CENTER_MODE);
			}

			flick_set_value(&set_angle_2, 0, 255);

			sprintf(str, "   %02d   ", set_angle_2);
			BSP_LCD_DisplayStringAt(0, 60, (uint8_t *) str, CENTER_MODE);

			last_opt = opt;
		}
		else opt++;
	}

	else
	{
		current_state = ST_MENU;
	}
	HAL_Delay(500);

	// przejscie do kolejnej opcji
	flick_interaction_t interaction = flick_get_interaction(gesture,touch,airwheel);
	if(interaction == FLICK_TOUCH_SRODEK)
	{
		opt++;
	}
}
void StSpeed (void)
{
	char str[40];
	flick_poll_data(&gesture, &touch, &airwheel);
	if(current_state != last_state)
	{
		BSP_LCD_Clear(LCD_COLOR_WHITE);

		sprintf(str, "Podaj predkosc siln");
		BSP_LCD_DisplayStringAt(0, 10, (uint8_t *) str, CENTER_MODE);

		sprintf(str, "Zatwierdz");
		BSP_LCD_DisplayStringAt(0, 147, (uint8_t *) str, CENTER_MODE);
		last_state = current_state;
	}

	flick_set_speed(&servo_speed, airwheel, &rotation_cnt);
	stepper_set_speed(&stepper, servo_speed);
	// odczyt z flick i zmiana predkosci

	//wyswietlenie nastawionej predkosci
	sprintf(str, "   %02d   ", servo_speed);
	BSP_LCD_DisplayStringAt(0, 60, (uint8_t *) str, CENTER_MODE);

	// zatwierdz kliknieciem
	flick_interaction_t interaction = flick_get_interaction(gesture,touch,airwheel);
	if(interaction == FLICK_TOUCH_SRODEK)
	{
		current_state = ST_MENU;
	}
}
void flick_set_value(uint16_t *value, uint16_t min_value, uint16_t max_value)
{
	flick_poll_data(&gesture, &touch, &airwheel);
	flick_interaction_t interaction = flick_get_interaction(gesture,touch,airwheel);
	if(interaction == FLICK_TOUCH_LEWO)
	{
		*value = *value - 10;
		if(*value < min_value) *value = min_value;
	}
	else if(interaction == FLICK_TOUCH_PRAWO)
	{
		*value = *value + 10;
		if(*value > max_value) *value = max_value;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	if(htim==stepper.Timer)
	{
		stepper.position+=stepper.direction;
		if(stepper.state==1)
		{
			if(stepper.position==stepper.destination[0])
			{
				HAL_TIM_PWM_Stop(stepper.Timer, stepper.Channel);
				HAL_TIM_Base_Stop_IT(stepper.Timer);
				stepper.is_working=0;
				stepper.state=0;
			}
		}
		else if(stepper.state==2)
		{
			if(stepper.position==stepper.destination[2])
			{
				stepper.speed=stepper.speed/2;
				__HAL_TIM_SET_PRESCALER(stepper.Timer, stepper.speed);
			}
			else if(stepper.position==stepper.destination[1])
			{
				stepper.speed=stepper.speed*2;
				__HAL_TIM_SET_PRESCALER(stepper.Timer, stepper.speed);
			}
			else if(stepper.position==stepper.destination[0])
			{
				HAL_TIM_PWM_Stop(stepper.Timer, stepper.Channel);
				HAL_TIM_Base_Stop_IT(stepper.Timer);
				stepper.is_working=0;
				stepper.state=0;
				stepper.speed=stepper.speed/2;
				__HAL_TIM_SET_PRESCALER(stepper.Timer, stepper.speed);
			}
		}
	}
}

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
