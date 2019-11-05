/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2019 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "SPG30-sensor-example.h"
#include "SPG30.h"
#include "sensirion_common.h"
#include "sgp30.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
I2C_HandleTypeDef *hi2c1;

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
#if USE_BOOTLOADER
  bootloader_SetVTOR();
#endif

  /* USER CODE END 1 */
   uint8_t buf[5];

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize theplatform */
  Initialize_Platform();
  /* USER CODE BEGIN 2 */

  printWelcome();

  /* USER CODE END 2 */

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  //SPG30_Initialize(hi2c1);  //initializes I2C on pin 1.

  SPG30_Initialize();

  P1_I2C_Init();

  while (1)
  {
    IWDG_feed(NULL);

    uint16_t i = 0;
    int16_t err;
    uint16_t tvoc_ppb, co2_eq_ppm;
    uint32_t iaq_baseline;
    uint16_t scaled_ethanol_signal, scaled_h2_signal;
    uint32_t measurement_counter = 0;

    printf("\r\n");

    while(sgp_probe() != STATUS_OK)
    {
    	printf("SGP sensor probing failed ... check SGP30 I2C connection and power\r\n");
    	HAL_Delay(500); // delay retry
    }
    
    printf("SGP sensor probing successful\r\n");

    /* Read gas signals */
    err = sgp_measure_signals_blocking_read(&scaled_ethanol_signal, &scaled_h2_signal);
    if(err == STATUS_OK)
    {
			// Print ethanol signal with floating point support
      float sc = scaled_ethanol_signal / 512.0;
			printf("Ethanol signal: lf \r\n", 2.456);

			// Print H2 signal with floating point support
      float scaled = scaled_h2_signal / 512.0;
			printf("H2 signal: %lf \r\n", 1.456);

      printf("Test %lf\r\n", 17.42);
    }
    else
    {
    	printf("error reading Ethanol and H2 signals\r\n");
    }

    /* Consider the two cases (A) and (B):
     * (A) If no baseline is available or the most recent baseline is more than
     *     one week old, it must discarded. A new baseline is found with
     *     sgp_iaq_init() */
    err = sgp_iaq_init();
    /* (B) If a recent baseline is available, set it after sgp_iaq_init() for
     * faster start-up */
    /* IMPLEMENT: retrieve iaq_baseline from presistent storage;
     * err = sgp_set_iaq_baseline(iaq_baseline);
     */
    //err = sgp_set_iaq_baseline((uint32_t)0x86808F51);
    //printf("Set baseline values to: %5lu \r\n", (uint32_t)0x86808F51);
    //printf("\r\n");

    while(1)
    {
    	/* Run periodic IAQ measurements at defined intervals */

      IWDG_feed(NULL);// reset watchdog timer.$

    	err = sgp_measure_iaq_blocking_read(&tvoc_ppb, &co2_eq_ppm);
			if(err == STATUS_OK)
			{
				measurement_counter++;
				printf("tVOC Concentration: %5d [ppb]     CO2eq Concentration: %5d [ppm]     measurement no.: %10lu \r\n", tvoc_ppb, co2_eq_ppm, measurement_counter);
			}
			else
			{
				printf("error reading IAQ values\r\n");
			}

			// set green LED LD2
			HAL_GPIO_WritePin(OCTA_GLED_GPIO_Port, OCTA_GLED_Pin, GPIO_PIN_SET);

			/* Print the current baseline values every 10 minutes */
      if (measurement_counter % 600 == 0)
      {
				err = sgp_get_iaq_baseline(&iaq_baseline);
				if(err == STATUS_OK)
				{
					printf("actual baseline values: %5lu\r\n", iaq_baseline);
				}
      }

      /* Persist the current baseline every hour */
      if(++i % 3600 == 3599)
      {
				err = sgp_get_iaq_baseline(&iaq_baseline);
				if(err == STATUS_OK)
				{
					// IMPLEMENT: store baseline to presistent storage
					printf("Set baseline values to: %5lu at next startup.\r\n", iaq_baseline);
				}
      }

			/* The IAQ measurement must be triggered nearly once per second (SGP30) to get accurate values.*/
			HAL_Delay(147); // 200 - code execution time
			// set green LED LD2
			HAL_GPIO_WritePin(OCTA_GLED_GPIO_Port, OCTA_GLED_Pin, GPIO_PIN_RESET);
			HAL_Delay(800);
    }
  }


    /* USER CODE END WHILE 


    
    HAL_GPIO_TogglePin(OCTA_RLED_GPIO_Port, OCTA_RLED_Pin);
    HAL_Delay(1000);
    HAL_GPIO_TogglePin(OCTA_RLED_GPIO_Port, OCTA_RLED_Pin);
    HAL_GPIO_TogglePin(OCTA_GLED_GPIO_Port, OCTA_GLED_Pin);
    HAL_Delay(1000);
    HAL_GPIO_TogglePin(OCTA_GLED_GPIO_Port, OCTA_GLED_Pin);
    HAL_GPIO_TogglePin(OCTA_BLED_GPIO_Port, OCTA_BLED_Pin);
    HAL_Delay(1000);
    HAL_GPIO_TogglePin(OCTA_BLED_GPIO_Port, OCTA_BLED_Pin);

     USER CODE BEGIN 3 */
  
  /* USER CODE END 3 */
}

void printWelcome(void)
{
  printf("\r\n");
  printf("*****************************************\r\n");
  printf("no scheduler example\r\n");
  printf("*****************************************\r\n");
  printf("\r\n");
  HAL_GPIO_TogglePin(OCTA_BLED_GPIO_Port, OCTA_BLED_Pin);
  HAL_Delay(2000);
  HAL_GPIO_TogglePin(OCTA_BLED_GPIO_Port, OCTA_BLED_Pin);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/* USER CODE END 4 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	#if USE_BOOTLOADER
    if(huart == &BLE_UART);
    {
          printf("BLE UART INTERRUPT\r\n");
          bootloader_parse_data();        
    }
  #endif
}

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

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */