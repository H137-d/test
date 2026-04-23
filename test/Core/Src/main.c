/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "usart.h"
#include "rtc.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "stm32l4xx_hal_pwr.h"
#include "stm32l4xx_hal_pwr_ex.h"

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
uint8_t lpuart_buf;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Enter_Sleep(void);
void Enter_Stop2(void);
void RTC_Set_Alarm(uint8_t sec);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Enter_Sleep(void){
	
	HAL_SuspendTick();
	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON,PWR_SLEEPENTRY_WFI);
	HAL_ResumeTick();
}

//void Enter_Stop2(void){
//	HAL_SuspendTick();
//	HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
//	HAL_ResumeTick();
////	SystemClock_Config();
//  
//	
//}

void Enter_Stop2(void)
{
    // 1. 关闭 LPUART（停止接收，禁用外设时钟）
    HAL_UART_DeInit(&hlpuart1);          // 彻底复位 LPUART 外设
    __HAL_RCC_LPUART1_CLK_DISABLE();     // 关闭 LPUART 总线时钟（可选，DeInit 已包含）

    // 2. 进入 Stop2 模式
    HAL_SuspendTick();
    HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
    HAL_ResumeTick();

    // 3. 从 Stop2 唤醒后，重新初始化 LPUART
    MX_LPUART1_UART_Init();              // 重新配置 LPUART
    HAL_UART_Receive_IT(&hlpuart1, &lpuart_buf, 1);  // 重新开启接收中断
}

void RTC_Set_Alarm(uint8_t sec){
    RTC_AlarmTypeDef sAlarm={0};
    RTC_TimeTypeDef sTime={0};
    
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    

    uint32_t newSeconds = sTime.Seconds + sec;
    uint32_t newMinutes = sTime.Minutes;
    uint32_t newHours = sTime.Hours;
    
    if (newSeconds >= 60) {
        newMinutes += newSeconds / 60;
        newSeconds %= 60;
    }
    
    if (newMinutes >= 60) {
        newHours += newMinutes / 60;
        newMinutes %= 60;
    }
    
    if (newHours >= 24) {
        newHours %= 24;
    }

    sAlarm.AlarmTime.Hours = newHours;
    sAlarm.AlarmTime.Minutes = newMinutes;
    sAlarm.AlarmTime.Seconds = newSeconds;
    sAlarm.AlarmTime.SubSeconds = 0;
    
    sAlarm.Alarm = RTC_ALARM_A;

    sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY | RTC_ALARMMASK_HOURS | RTC_ALARMMASK_MINUTES;
    
    HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A);
    
    HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN);
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
  MX_LPUART1_UART_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
	HAL_Delay(5000);
	HAL_UART_Receive_IT(&hlpuart1, &lpuart_buf, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		
		// 1. Sleep
		RTC_Set_Alarm(3);
    Enter_Sleep();

    // 3. Stop 2
    RTC_Set_Alarm(3);
    Enter_Stop2();

    // 4. Stop 3
    RTC_Set_Alarm(3);
    Enter_Stop2();

    // 5. Sleep
		RTC_Set_Alarm(3);
    Enter_Sleep();
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 12;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV4;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc){
	
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(huart->Instance == LPUART1)
  {
    // �յ����ݣ�lpuart_buf
    // ���¿�������
    HAL_UART_Receive_IT(&hlpuart1, &lpuart_buf, 1);
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
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
