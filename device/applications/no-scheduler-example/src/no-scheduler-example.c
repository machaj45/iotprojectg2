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


#include "no-scheduler-example.h"
#include <stdio.h>
#include "murata.h"

//#include "send.h"
// #include "other.h"

//#include "platform.h"
#include "LSM303AGRSensor.h"
#include "sht31.h"

// GAS sensor
#include "sensirion_common.h"
#include "sgp30.h"

// Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define HAL_EXTI_MODULE_ENABLED

#define temp_hum_timer    3
//#define LOW_POWER
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
osTimerId temp_hum_timer_id;
float SHTData[2];
volatile _Bool interruptFlag=0; 
uint16_t LoRaWAN_Counter = 0;
//uint8_t lora_init = 0;
uint64_t short_UID;
volatile uint8_t safeCounter = 0;
volatile uint8_t DangerCounter = 0;
volatile uint8_t EmergencyCounter = 0;
volatile _Bool NormalMode = 1;

uint8_t murata_init = 0;

static uint8_t maxSafeCounter = 5;
static uint8_t maxDangerCounter = 5;
static uint8_t maxEmergencyCounter = 5;

static uint16_t NormalSleepCounter = 0x000A;
static uint16_t EmergencySleepCounter = 0x0005;
static uint16_t OneMinute = 0x003C;
static uint16_t fiveMinute = 0x012C;
static uint16_t halfMinute = 0x001E;
static uint16_t tenSeconds = 0x000A;
static uint16_t fiveSeconds = 0x0005;

volatile uint16_t TemperatureTreshold[2];
volatile uint16_t HumidityTreshold[2];
volatile uint16_t CO2Treshold[2];
volatile uint16_t TVOCTreshold[2];





 uint16_t i = 0;
    int16_t err;
    uint16_t tvoc_ppb, co2_eq_ppm;
    uint32_t iaq_baseline;
    uint16_t scaled_ethanol_signal, scaled_h2_signal;
    volatile _Bool danger;

    uint8_t murata_data_ready = 0;




//RTC 
RTC_HandleTypeDef hrtc;

// static void SystemPower_Config(void);
static void MX_RTC_Init(void);



void sleep(uint16_t time);

void testBlink(void);

void quickBlink(void);

bool calculateDanger();

//*/
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

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
 
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

    /* Enable Power Clock */
//  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* Ensure that MSI is wake-up system clock */ 
//  __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_MSI);

  MX_RTC_Init();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize theplatform */
  Initialize_Platform();
  /* USER CODE BEGIN 2 */

  S25FL256_Initialize(&FLASH_SPI);

  // get Unique ID of Octa
  short_UID = get_UID(); 

//init for gas sensor
  SPG30_Initialize();
  P3_I2C_Init();
//LSM303AGR_setI2CInterface(&common_I2C);
  setI2CInterface_SHT31(&common_I2C);
  SHT31_begin(); 

  LorawanInit();

  //setUpDefaultValuesforTresholds();

//  LSM303AGR_init();

  printWelcome();

  readInFlash(TEMP_TH_LOW,TemperatureTreshold,2);
readInFlash(TEMP_TH_HIGH,TemperatureTreshold+1,2);

readInFlash(HUMI_TH_LOW,HumidityTreshold,2);
readInFlash(HUMI_TH_HIGH,HumidityTreshold+1,2);

readInFlash(CO2_TH_LOW,CO2Treshold,2);
readInFlash(CO2_TH_HIGH,CO2Treshold+1,2);

readInFlash(TVOC_TH_LOW,TVOCTreshold,2);
readInFlash(TVOC_TH_HIGH,TVOCTreshold+1,2);



  

// // TX MUTEX ensuring no transmits are happening at the same time
//   osMutexDef(txMutex);
//   txMutexId = osMutexCreate(osMutex(txMutex));
    
//     // pass processing thread handle to murata driver
//   Murata_SetProcessingThread(murata_rx_processing_handle);
  /* USER CODE END 2 */

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */   
uint8_t data[3] = {5,10,15}; 
 
Murata_LoRaWAN_Join(); 

while(1){

  IWDG_feed(NULL);

    if(murata_data_ready)
    {
      printf("processing murata fifo\r\n");
      murata_data_ready = !Murata_process_fifo();
    }
  LoRa_send(NULL);
  HAL_Delay(10000);
}
//Dash7_send(data , sizeof(data));

//LoRaWAN_send(data,sizeof(data));
        printf("\r\n");
        HAL_Delay(3000);

    while(sgp_probe() != STATUS_OK)
    {
    	printf("SGP sensor probing faiLed ... check SGP30 I2C connection and power\r\n");
    	HAL_Delay(500); // delay retry
    }
    
    printf("SGP sensor probing successful\r\n");

    /* Read gas signals */
    err = sgp_measure_signals_blocking_read(&scaled_ethanol_signal, &scaled_h2_signal);
    if(err == STATUS_OK)
    {
			// Print ethanol signal with floating point support
			printf("Ethanol signal: %.2f \r\n", scaled_ethanol_signal / 512.0);

			// Print H2 signal with floating point support
			printf("H2 signals: %.2f \r\n",scaled_h2_signal / 512.0);

    }
    else
    {
    	printf("error reading Ethanol and H2 signals\r\n");
    }
        err = sgp_iaq_init();

uint8_t lora_Mycounter  = 0;


  while (1)
  {
    


    IWDG_feed(NULL); 
    /* USER CODE END WHILE */
uint8_t          buffer[11];
while (1){
     float g[2];
  SHT31_get_temp_hum(g);
  float2byte(g[0], buffer, 0);
  float2byte(g[1], buffer, 4);
 // float2byte(cotlevels, buffer, 8);
 uint162byte(co2_eq_ppm,buffer,8 );

  //printOCTAID();
  buffer[10] = (uint8_t)lora_Mycounter;
  lora_Mycounter++;
  //Dash7_send(buffer, sizeof(buffer));



 //   lorawan_send(buffer, sizeof(buffer))
    HAL_Delay(10000);

}
    //de interrupt zal zorgen dat de flag op 1 staat, dan doen we een measurement van temp
    if (interruptFlag==1) {
          printf("\033[2J");
          printf("\033[H");
        printf("Back awake\r\n"); 
        printf("Session %d\r\n",safeCounter);
      interruptFlag=0; 
    }

///////////////////////////  NORMAL MODE  ////////////////////////////////////
    if (NormalMode){
      
      quickBlink();

      do_measurement();

      danger =  calculateDanger();

      if (danger){
        printf("DANGER\r\n");
        //update danger counter & reset safe counter
        DangerCounter++;
        safeCounter = 0;

        // check if in danger for too long -> if so, enable emergency mode
        if (DangerCounter >= maxDangerCounter){
          //reset counter
          DangerCounter = 0;

          //enter emergency mode on next wake up
          NormalMode = 0;
          printf("enter emergency mode on next wake up\r\n");
        }
        else {
          // TODO: SEND data only on dash 7.
          printf("send danger data\r\n");
        }
      }

      else {
        printf("clear\r\n");
        // update safe counter & reset danger counter
        safeCounter++;
        DangerCounter = 0;

          //check if server needs to update safe values
        if (safeCounter >= maxSafeCounter){
          //reset counter
          safeCounter = 0;
          printf("send safe server update\r\n");
          
        }

      }      
      printf("safecounter: %d, dangercounter: %d\r\n",safeCounter,DangerCounter);
      sleep(tenSeconds);
  // // operating in normal mode
  // if (NormalMode){
  //   //check for BLE config

  //   //do environment measurements & check for danger


  //   // if no danger:
  //   ++safeCounter;

  //   if (safeCounter == maxSafeCounter){
  //     //send data
  //     printf("safe update to the server \r\n");
  //     safeCounter = 0;
  //   }

    
  // }
  // // operating in emergency mode
  // else {

  // }
    }

///////////////////////  EMERGENCY MODE /////////////////////////////
    else{

      //update emergency counter
      EmergencyCounter++;

      if (EmergencyCounter >= maxEmergencyCounter){
        EmergencyCounter = 0;

        //do measurements & calculate danger.
        //if there is still is danger, remain in emergency mode

        do_measurement();
        danger = calculateDanger();

        if (danger){
          // TODO: send data on lora & DASH7
          printf("recalculated emergengy data is dangerous\r\n");
        } 
        else{
          NormalMode = 1;
        }

      }

      else{
        // TODO: send data on lora & dash7

        printf("SEND STORED EMERGENCY DATA\r\n");
      }

    printf("emergency counter: %d\r\n",EmergencyCounter);
    sleep(fiveSeconds);

    }

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

void LoRa_send(void const *argument)
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



bool calculateDanger(){
  //SHT 0 = temp, SHT1 = humid
  // tvoc_ppb , co2_ppm

    if ((SHTData[0] < TemperatureTreshold[0]) || (SHTData[0] > TemperatureTreshold[1]) ){
      printf("temp\r\n");
      return true;
    }

    if ((SHTData[1] < HumidityTreshold[0]) || (SHTData[1] > HumidityTreshold[1]) ){
            printf("humid\r\n");
             return true;
    }

    if ((tvoc_ppb < TVOCTreshold[0]) || (tvoc_ppb > TVOCTreshold[1]) ){
     
           printf("TVOC\r\n");
           return true;
    }

    if ((co2_eq_ppm < CO2Treshold[0]) || (co2_eq_ppm > CO2Treshold[1]) ){
            printf("CO2\r\n");
            return true;
    }

    return false;

    
}

void printWelcome(void)
{
  printf("\033[2J");
  printf("\033[H");
  printf("\r\n");
  printf("*****************************************\r\n");
  printf("noo scheduler example\r\n");
  printf("*****************************************\r\n");
  printf("\r\n");
  HAL_GPIO_TogglePin(OCTA_BLED_GPIO_Port, OCTA_BLED_Pin);
  HAL_Delay(2000);
  HAL_GPIO_TogglePin(OCTA_BLED_GPIO_Port, OCTA_BLED_Pin);
}

/**
  * @brief  System Power Configuration
  *         The system Power is configured as follow :
  *            + RTC Clocked by LSI
  *            + VREFINT OFF, with fast wakeup enabled
  *            + No IWDG
  *            + Automatic Wakeup using RTC clocked by LSI (after ~4s)
  * @param  None
  * @retval None
  */


 /* RTC init function */
static void MX_RTC_Init(void)
{
  /* USER CODE BEGIN RTC_Init 0 */
  /* USER CODE END RTC_Init 0 */
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;
  /* USER CODE BEGIN RTC_Init 1 */
  /* USER CODE END RTC_Init 1 */
    /**Initialize RTC Only 
    */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
    /**Initialize RTC and set the Time and Date 
    */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;
  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
}

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hrtc);
  interruptFlag = 1; 

}

void sleep(uint16_t timer)
{
  printf("going in sleep mode\r\n");
  if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, timer,RTC_WAKEUPCLOCK_CK_SPRE_16BITS) != HAL_OK)
  {
    Error_Handler();
  }
  __HAL_RTC_WAKEUPTIMER_EXTI_ENABLE_IT();

  HAL_SuspendTick();
  HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
  HAL_ResumeTick();
  SystemClock_Config();
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

//See platform>octa>stm32l4xx_it.c there we placed exti15_10 function to capture interrupts on one of the pins 10-15 on connector B,C,D
// (see slides of Mr weyn for the exact function name). In this function we call the "HAL_GPIO_EXTI_IRQHandler" function in the stm32l4xx_hal_gpio.c
// file. In this function the "HAL_GPIO_EXTI_Callback" is called, which we define below 
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  if (GPIO_Pin==GPIO_Pin_13) {
  interruptFlag = 1; 
  printf("interrupt accelerometer! \r\n");
  // we work with a flag so as to make sure that we don't stay in the callback for too long. This will cause disruption. 
  // the flag will call the measurement function in de while loop
  } 
}

void print_data(void){
  printf("\r\n");
  printf("Temperature: %.2f %cC  \r\n", SHTData[0],'°');
  printf("Humidity: %.2f %% \r\n", SHTData[1]);
  printf("TVOC: %5d ppb\r\n",tvoc_ppb);
  printf("CO2: %5d ppm\r\n",co2_eq_ppm);
}

void do_measurement(void){

  SHT31_get_temp_hum(SHTData);
  err = sgp_measure_iaq_blocking_read(&tvoc_ppb, &co2_eq_ppm);
  if (err == STATUS_OK){
    print_data();
  }
  else{
    printf("error while reading data\r\n");
  }
}


/* USER CODE END 4 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart == &P1_UART) {
    Murata_rxCallback();
    murata_data_ready = 1;
  }
	#if USE_BOOTLOADER
    if(huart == &BLE_UART);
    {
          printf("BLE UART INTERRUPT\r\n");
          bootloader_parse_data();        
    }
  #endif
}




void LoRaWAN_send_self()
{
  if (murata_init)
  {
    uint8_t loraMessage[5];
    uint8_t i = 0;
    //uint16 counter to uint8 array (little endian)
    //counter (large) type byte

    //first data transfer works, but afterwards it stays in a kind of 'transmitted' loop. Find where we can reset the flag.
    loraMessage[i++] = SHTData[0];
    loraMessage[i++] = SHTData[1];
    loraMessage[i++] = LoRaWAN_Counter >> 8;
   // osMutexWait(txMutexId, osWaitForever);
    if(!Murata_LoRaWAN_Send((uint8_t *)loraMessage, i))
    {
      printf("tis ni gelukt :( ");
      murata_init++;
      if(murata_init == 10)
        murata_init == 0;
    }
    else
    {
      murata_init = 1;
    }
    //BLOCK TX MUTEX FOR 3s
    // osDelay(3000);
    // osMutexRelease(txMutexId);
    LoRaWAN_Counter++;
  }
  else{
    printf("murata not initialized, not sending\r\n");
  }
}



/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */


void testBlink(void){
  HAL_GPIO_TogglePin(OCTA_RLED_GPIO_Port, OCTA_RLED_Pin);
    printf("RED\r\n");
    HAL_Delay(1000);
    HAL_GPIO_TogglePin(OCTA_RLED_GPIO_Port, OCTA_RLED_Pin);
    HAL_GPIO_TogglePin(OCTA_GLED_GPIO_Port, OCTA_GLED_Pin);
    printf("GREEN\r\n");
    HAL_Delay(1000);
    HAL_GPIO_TogglePin(OCTA_GLED_GPIO_Port, OCTA_GLED_Pin);
    HAL_GPIO_TogglePin(OCTA_BLED_GPIO_Port, OCTA_BLED_Pin);
    printf("BLUE\r\n");
    HAL_Delay(1000);
    HAL_GPIO_TogglePin(OCTA_BLED_GPIO_Port, OCTA_BLED_Pin); 
    HAL_Delay(1000);

    HAL_GPIO_TogglePin(OCTA_BLED_GPIO_Port, OCTA_BLED_Pin);
    printf("BLUE\r\n");
    HAL_Delay(1000);
    HAL_GPIO_TogglePin(OCTA_BLED_GPIO_Port, OCTA_BLED_Pin);

    HAL_Delay(1000);
}

void quickBlink(void){
  HAL_GPIO_TogglePin(OCTA_RLED_GPIO_Port, OCTA_RLED_Pin);
 //   printf("\33[2K");
    HAL_Delay(500);
    HAL_GPIO_TogglePin(OCTA_RLED_GPIO_Port, OCTA_RLED_Pin);
}


void murata_process_rx_response(void)
{
  uint32_t startProcessing;
  while (1)
  {
    // Wait to be notified that the transmission is complete.  Note the first
    //parameter is pdTRUE, which has the effect of clearing the task's notification
    //value back to 0, making the notification value act like a binary (rather than
    //a counting) semaphore.
    startProcessing = ulTaskNotifyTake(pdTRUE, osWaitForever);
    if (startProcessing == 1)
    {
      // The transmission ended as expected.
      Murata_process_fifo();
    }
    else
    {
    }
    osDelay(1);
  }
  osThreadTerminate(NULL);
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
