/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "software_timer.h"
#include "led_7seg.h"
#include "button.h"
#include "lcd.h"
#include "picture.h"
#include "ds3231.h"
#include "sensor.h"
#include "buzzer.h"
#include "touch.h"
#include "uart.h"
#include "light_control.h"
#include "snake.h"
#include "UI.h"
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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void system_init();
void test_LedDebug();
void test_LedY0();
void test_LedY1();
void test_7seg();
void test_button();
void test_lcd();
void in_snake_game();
void display_UI();
void display_UI_Snake_Game();
uint16_t status = 0;
uint16_t difficult = 10;
uint8_t id = 0;
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
  MX_TIM2_Init();
  MX_SPI1_Init();
  MX_FSMC_Init();
  MX_I2C1_Init();
  MX_TIM13_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  system_init();
  lcd_Clear(WHITE);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

    while (1)
    {
  	  while(!flag_timer2);
	  flag_timer2 = 0;
	  button_Scan();

	  if(flag_Sensor == 1) {
		  sensor_Read();
		  uartSendSensor();
		  setTimerSendSensor(15000);
	  }

//	  test_button();
//	  test_7seg();
	  switch (status) {
	  case 0:
		  display_UI();
		  if(button_count[0] == 1) {
			  status = 2;
			  lcd_Clear(BLACK);
			  button_count[0] = 0;
		  }
		  break;
	  case 1:

		  wall(id,difficult%3);
		  move();

		  if(button_count[12] == 1) {
			  status = 2;
			  lcd_Clear(BLACK);
			  button_count[12] = 0;
		  }
		  break;
	  case 2:
		  display_UI_Snake_Game();
		  break;
	  }

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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void system_init(){
	  HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin, 0);
	  HAL_GPIO_WritePin(OUTPUT_Y1_GPIO_Port, OUTPUT_Y1_Pin, 0);
	  HAL_GPIO_WritePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin, 0);
	  timer_init();
	  led7_init();
	  button_init();
	  lcd_init();
	  sensor_init();
	  uart_init_esp();
	  setTimer2(50);
	  setTimerSendSensor(15000);
}

uint8_t count_led_debug = 0;
uint8_t count_led_Y0 = 0;
uint8_t count_led_Y1 = 0;

void test_LedDebug(){
	count_led_debug = (count_led_debug + 1)%20;
	if(count_led_debug == 0){
		HAL_GPIO_TogglePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin);
	}
}

void test_LedY0(){
	count_led_Y0 = (count_led_Y0+ 1)%100;
	if(count_led_Y0 > 40){
		HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin, 1);
	} else {
		HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin, 0);
	}
}

void test_LedY1(){
	count_led_Y1 = (count_led_Y1+ 1)%40;
	if(count_led_Y1 > 10){
		HAL_GPIO_WritePin(OUTPUT_Y1_GPIO_Port, OUTPUT_Y1_Pin, 0);
	} else {
		HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y1_Pin, 1);
	}
}

void test_7seg(){
	led7_SetDigit(2, 0, 0);
	led7_SetDigit(3, 1, 0);
	led7_SetDigit(1, 2, 0);
	led7_SetDigit(2, 3, 0);
}
void test_button(){
	for(int i = 0; i < 16; i++){
		if(button_count[i] == 1){
			lcd_ShowIntNum(140, 105, i, 2, BRED, WHITE, 32);
			//di chuyển
		}
	}
}
void test_lcd(){
	snake_init();
	move();
}

void display_UI() {
	lcd_ShowPicture(80,20, 90, 90, gImage_logo);
	lcd_ShowStr(50, 115, "LOGIC DESIGN", RED, WHITE, 24, 1);
	lcd_ShowStr(42, 150, "GVHD: Vu Trong Thien", BLACK, WHITE, 16, 1);
	lcd_ShowStr(42, 168, "--------------------", BLACK, WHITE, 16, 1);
	lcd_ShowStr(48, 190, "Nhom 2", BLACK, WHITE, 16, 1);
	lcd_ShowStr(48, 210, "Huynh Gia Qui", BLACK, WHITE, 16, 1);
	lcd_ShowStr(48, 230, "Dao Duy Thanh", BLACK, WHITE, 16, 1);
	lcd_ShowStr(48, 250, "Pham Dinh Quoc Thai", BLACK, WHITE, 16, 1);
}

uint8_t flag_choose = 0;
void choose_id() {
	if(button_count[3] == 1) {
		id++;
		if(id > 9) {
			id = 9;
		}
	}
	if(button_count[7] == 1) {
		if(id == 0) {
			id = 0;
			return;
		}

		id--;

	}
	if(button_count[10] == 1) {
		flag_choose = 1;
	}
}
void choose_level() {
	if(button_count[3] == 1)
		difficult++;
	if(button_count[7] == 1)
		difficult--;

	switch(difficult%3) {
	case 0:
		lcd_Fill(100, 230, 200, 260, BLACK);
		lcd_ShowStr(100,230,"EASY",WHITE,BLACK,16,0);
		break;
	case 1:
		lcd_Fill(100, 230, 200, 260, BLACK);
		lcd_ShowStr(100,230,"NORMAL",WHITE,BLACK,16,0);
		break;
	case 2:
		lcd_Fill(100, 230, 200, 260, BLACK);
		lcd_ShowStr(100,230,"HARD",WHITE,BLACK,16,0);
		break;
	}
}

void display_UI_Snake_Game() {
	lcd_ShowStr(40,50,"SNAKE ",WHITE,BLACK,32,0);
	lcd_ShowStr(120,100,"GAME",WHITE,BLACK,32,0);
	lcd_ShowStr(50,170,"Nhap ID: ",WHITE,BLACK,16,0);
	lcd_ShowIntNum(120, 170, id, 1, WHITE, BLACK, 16);
	lcd_ShowStr(50,200,"Chon do kho!",WHITE,BLACK,16,0);
	if(flag_choose == 0)
		choose_id();
	if(flag_choose == 1)
		choose_level();
	if(button_count[4] == 1) {
		flag_choose = 0; // về chế độ chọn ID cho lần sau
		status = 1;
		lcd_Clear(WHITE);
		lcd_Fill(0, 0, 240, 100, BLACK);
		snake_init();
		uartSendPlay();
		if(difficult%3 >= 1) {
			createWall();
		}
	}

//	lcd_ShowStr(0,220,"",WHITE,BLACK,16,0);

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
