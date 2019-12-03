/* Includes ------------------------------------------------------------------*/
#include "murata-dualstack-example-ns.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "murata.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define IWDG_INTERVAL           5    //seconds
#define LORAWAN_INTERVAL        10   //seconds
#define DASH7_INTERVAL          200  //seconds
#define MODULE_CHECK_INTERVAL   3600 //seconds

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint16_t LoRaWAN_Counter = 0;
uint16_t DASH7_Counter = 0;
uint8_t murata_init = 0;
uint64_t short_UID;
uint8_t murata_data_ready = 0;
  RTC_HandleTypeDef hrtc;

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

  /* Initialize the platform, OCTA in this case */
  Initialize_Platform();
  /* USER CODE BEGIN 2 */

  // Get Unique ID of octa
  short_UID = get_UID();

  // Print Welcome Message
  printWelcome();
  
  // LORAWAN
  murata_init = Murata_Initialize(short_UID, 0);

  if (murata_init)
  {
    printf("Murata dualstack module init OK\r\n\r\n");
  }
  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */

  // TX MUTEX ensuring no transmits are happening at the same time

  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the thread(s) */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */

  //feed IWDG every 5 seconds
  IWDG_feed(NULL);

  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */

  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Start scheduler */

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  uint8_t counter = 0;
  uint8_t use_lora = 1;


  /* USER CODE BEGIN WHILE */
  while (1)
  { 
    IWDG_feed(NULL);


          LoRaWAN_send(NULL);

    HAL_GPIO_TogglePin(OCTA_RLED_GPIO_Port, OCTA_RLED_Pin);
    HAL_Delay(1000);
    HAL_GPIO_TogglePin(OCTA_RLED_GPIO_Port, OCTA_RLED_Pin);

    if(murata_data_ready)
    {
      printf("processing murata fifo\r\n");
      murata_data_ready = !Murata_process_fifo();
    }
    
    // SEND 5 D7 messages, every 10 sec.
    // Afterwards, send 3 LoRaWAN messages, every minute
    if(DASH7_Counter<5)
    {
      if(counter==DASH7_INTERVAL)
      {
        Dash7_send(NULL);
        counter = 0;
      }
    }
    else
    { 
      if(LoRaWAN_Counter == 0)
        Murata_LoRaWAN_Join();
      if(LoRaWAN_Counter<3)
      {
        if (counter == LORAWAN_INTERVAL)
        {
          LoRaWAN_send(NULL);
          counter = 0;
        }
      }
      if(LoRaWAN_Counter == 3)
      {
        //reset counters to restart flow
        DASH7_Counter = 0;
        LoRaWAN_Counter = 0;
      }
    }
   
    counter++;
    HAL_Delay(1000);
    

    /* USER CODE END WHILE */
    
    /* USER CODE BEGIN 3 */

  //  HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON,PWR_SLEEPENTRY_WFI);
  }
  /* USER CODE END 3 */
}


void LoRaWAN_send(void const *argument)
{
  if (murata_init)
  {
    uint8_t loraMessage[5];
    uint8_t i = 0;
    //uint16 counter to uint8 array (little endian)
    //counter (large) type byte
    loraMessage[i++] = 0x14;
    loraMessage[i++] = LoRaWAN_Counter;
    loraMessage[i++] = LoRaWAN_Counter >> 8;
    //osMutexWait(txMutexId, osWaitForever);
    if(!Murata_LoRaWAN_Send((uint8_t *)loraMessage, i))
    {
      murata_init++;
      if(murata_init == 10)
        murata_init == 0;
    }
    else
    {
      murata_init = 1;
    }
    //BLOCK TX MUTEX FOR 3s
    //osDelay(3000);
    //osMutexRelease(txMutexId);
    LoRaWAN_Counter++;
  }
  else{
    printf("murata not initialized, not sending\r\n");
  }
}

void Dash7_send(void const *argument)
{
  if (murata_init)
  {
    uint8_t dash7Message[5];
    uint8_t i = 0;
    //uint16 counter to uint8 array (little endian)
    //counter (large) type byte
    dash7Message[i++] = 0x14;
    dash7Message[i++] = DASH7_Counter;
    dash7Message[i++] = DASH7_Counter >> 8;
    //osMutexWait(txMutexId, osWaitForever);
    if(!Murata_Dash7_Send((uint8_t *)dash7Message, i))
    {
      murata_init++;
      if(murata_init == 10)
        murata_init == 0;
    }
    else
    {
      murata_init = 1;
    }
    //BLOCK TX MUTEX FOR 3s
    //osDelay(3000);
    //osMutexRelease(txMutexId);
    DASH7_Counter++;
  }
  else{
    printf("murata not initialized, not sending\r\n");
  }
}

// UART RX CALLBACK
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart == &P1_UART)
  {
    Murata_rxCallback();
    murata_data_ready = 1;
  }
}

void vApplicationIdleHook(){
  #if LOW_POWER
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFE);
  #endif
}

void printWelcome(void)
{
  printf("\r\n");
  printf("*****************************************\r\n");
  printf("Murata Dual Stack example without scheduler\r\n");
  printf("*****************************************\r\n");
  printf("\r\n");
  char UIDString[sizeof(short_UID)];
  memcpy(UIDString, &short_UID, sizeof(short_UID));
  printf("octa ID: ");
  for (const char* p = UIDString; *p; ++p)
    {
        printf("%02x", *p);
    }
  printf("\r\n\r\n");
  HAL_GPIO_TogglePin(OCTA_BLED_GPIO_Port, OCTA_BLED_Pin);
  HAL_Delay(2000);
  HAL_GPIO_TogglePin(OCTA_BLED_GPIO_Port, OCTA_BLED_Pin);
}

/* USER CODE END 4 */




/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
