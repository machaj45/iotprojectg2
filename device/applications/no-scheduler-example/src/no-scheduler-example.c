#include "no-scheduler-example.h"
#include <stdio.h>
#include "murata.h"

#include "send.h"
#include "other.h"

//#include "platform.h"
#include "LSM303AGRSensor.h"
#include "sht31.h"

// GAS sensor
#include "sensirion_common.h"
#include "sgp30.h"

#define HAL_EXTI_MODULE_ENABLED

#define SIZEOFBLEBUFFER 100
#define temp_hum_timer    3

osTimerId temp_hum_timer_id;
float SHTData[2];
volatile _Bool interruptFlag=0; 
volatile uint8_t interruptFlagBle=0; 
uint16_t LoRaWAN_Counter = 0;
//uint8_t lora_init = 0;
uint64_t short_UID;
volatile uint8_t safeCounter = 0;
volatile uint8_t DangerCounter = 0;
volatile uint8_t EmergencyCounter = 0;
volatile _Bool NormalMode = 1;

uint8_t murata_init = 0;

volatile activeSending = 0;

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


uint8_t datainble[SIZEOFBLEBUFFER];
uint8_t data[1];
uint8_t charCounter;



 uint16_t i = 0;
    int16_t err;
    uint16_t tvoc_ppb, co2_eq_ppm;
    uint32_t iaq_baseline;
    uint16_t scaled_ethanol_signal, scaled_h2_signal;
    volatile _Bool danger;
    uint8_t buffer[14] = {};
    uint8_t Message_Counter  = 0;




    uint8_t murata_data_ready = 0;




//RTC 
RTC_HandleTypeDef hrtc;

// static void SystemPower_Config(void);
static void MX_RTC_Init(void);



void sleep(uint16_t time);

void testBlink(void);

void quickBlink(void);

bool calculateDanger();

int main(void)
{
  /* USER CODE BEGIN 1 */
#if USE_BOOTLOADER
  bootloader_SetVTOR();
#endif

  HAL_Init();
 
  SystemClock_Config();

    /* Enable Power Clock */
//  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* Ensure that MSI is wake-up system clock */ 
//  __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_MSI);

  MX_RTC_Init();

  Initialize_Platform();

  //needed for flash
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

 // setUpDefaultValuesforTresholds();

//  LSM303AGR_init();

  printWelcome();

UpdateThresholdsFromFlashBLE();

 
Murata_LoRaWAN_Join(); 


//Dash7_send(data , sizeof(data));


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



        ////////////// while test wake up

/*
        while (1){
              IWDG_feed(NULL); 
            quickBlink();
            printf("wake\r\n");
            printf("going in sleep mode\r\n");
            sleep(fiveSeconds);
        }
*/

        /////////////////////////// end while test wake up
////////////////////////  small while test loop   //////////////////////////////
/*
  HAL_UART_Receive_IT(&BLE_UART, data, sizeof(data));
         while (1){
IWDG_feed(NULL);


    

NormalMode = !NormalMode;
  do_measurement();
  danger = calculateDanger();

  LoadBuffer();

  Murata_toggleResetPin();
  
  //LoRaWAN_send(buffer,sizeof(buffer));LoRaWAN_send
  Dash7_send(buffer, sizeof(buffer));
  WaitSend(5000);

        
} */


//////////////////////// end small while loop   ////////////////////////////


  HAL_UART_Receive_IT(&BLE_UART, data, sizeof(data));
  while (1)
  {
    

      // feed watchdog tmer
      IWDG_feed(NULL); 
    /* USER CODE END WHILE */

/* if(murata_data_ready)
    {
      printf("processing murata fifo\r\n");
      murata_data_ready = !Murata_process_fifo();
    }
  } */


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


      //////////////////  if BLE, commincate  
      //printf("Testing\n\r");
      //printf("INter flag is: %d\n\r",interruptFlagBle);

      if(interruptFlagBle!=0){
       printf("BLE MODE START\r\n"); 
       onBLE();
       interruptFlagBle=0;
       printf("BLE MODE STOP\r\n"); 
      }

      ////////////// end BLE


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
          LoadBuffer();
          printf("enter emergency mode on next wake up\r\n");
        }
        else {
          // TODO: SEND data only on dash 7.
          printf("send Dash7 data\r\n");

          //LOAD data in buffer 
          LoadBuffer();

          Dash7_send(buffer,sizeof(buffer));
         //LoRaWAN_send(buffer, sizeof(buffer));
         WaitSend(5000);
         
        }
      }

      else {
        printf("clear\r\n");
        safeCounter++;
        DangerCounter = 0;

          //check if server needs to update safe values
        if (safeCounter >= maxSafeCounter){
          //reset counter
          safeCounter = 0;
          printf("send safe server LORA update\r\n");
          LoadBuffer();
          LoRaWAN_send(buffer, sizeof(buffer));
          WaitSend(3375);
          
        }

      }      
      printf("safecounter: %d, dangercounter: %d\r\n",safeCounter,DangerCounter);
      sleep(fiveSeconds);
    }

///////////////////////  EMERfGENCY MODE /////////////////////////////
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
          printf("recalculated emergengy data is dangerous DASH7\r\n");
          LoadBuffer();
       } 
        else{
          NormalMode = 1;
        }

      }

      else{
        // TODO: send data on lora & dash7

        printf("SEND STORED DASH7 EMERGENCY DATA\r\n");

          buffer[13] = (uint8_t)Message_Counter++;
          Dash7_send(buffer,sizeof(buffer));
          //LoRaWAN_send(buffer, sizeof(buffer));
          WaitSend(5000);


      }

    printf("emergency counter: %d\r\n",EmergencyCounter);
    sleep(fiveSeconds);

    }

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}




void LoadBuffer(void){
  float2byte(SHTData[0], buffer, 0);
  float2byte(SHTData[1], buffer, 4);
  uint162byte(co2_eq_ppm,buffer,8 );
  uint162byte(tvoc_ppb,buffer,10 );
  buffer[12] = (buffer[12] & 0x0E )| danger;

  buffer[12] = (buffer[12] & 0x0D) | (!NormalMode) << 1;
  
  
  buffer[13] = (uint8_t)Message_Counter++;
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

void WaitSend(int miliseconds){
  // trial and error defined delay needed for Lora to send coorectly
    HAL_Delay(miliseconds);
if(murata_data_ready)
    {
      printf("processing murata fifo\r\n");
      murata_data_ready = !Murata_process_fifo();
    }
}

void UpdateThresholdsFromFlashBLE(void)
{    
  readInFlash(TEMP_TH_LOW,TemperatureTreshold,2);
  readInFlash(TEMP_TH_HIGH,TemperatureTreshold+1,2);

  readInFlash(HUMI_TH_LOW,HumidityTreshold,2);
  readInFlash(HUMI_TH_HIGH,HumidityTreshold+1,2);

  readInFlash(CO2_TH_LOW,CO2Treshold,2);
  readInFlash(CO2_TH_HIGH,CO2Treshold+1,2);

  readInFlash(TVOC_TH_LOW,TVOCTreshold,2);
  readInFlash(TVOC_TH_HIGH,TVOCTreshold+1,2);
  
  printf("\033[2J");
  printf("\033[H");
  printf("\r\n");
  printf("************************************************\r\n");
  printf("Tempriture treshlod is between %d and %d\r\n",TemperatureTreshold[0],TemperatureTreshold[1]);
  printf("Humidity treshlod is between %d and %d\r\n",HumidityTreshold[0],HumidityTreshold[1]);
  printf("CO2Treshold treshlod is between %d and %d\r\n",CO2Treshold[0],CO2Treshold[1]);
  printf("TVOCTreshold treshlod is between %d and %d\r\n",TVOCTreshold[0],TVOCTreshold[1]);
  printf("************************************************\r\n");
  printf("\r\n");
  HAL_Delay(1000);
}

void printWelcome(void)
{
  printf("\033[2J");
  printf("\033[H");
  printf("\r\n");
  printf("*****************************************\r\n");
  printf("************ Ultimate Gard **************\r\n");
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
  HAL_UART_Receive_IT(&BLE_UART, data, sizeof(data));
  HAL_Delay(200);
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
  if(huart == &BLE_UART){
    interruptFlagBle++;
    //bootloader_parse_data();        
  }
  if (huart == &P1_UART) {
    Murata_rxCallback();
    murata_data_ready = 1;
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */

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
uint8_t  vc    = 0;
uint32_t value = 0;

void validCommand(uint8_t start, uint8_t stop) {
  printf("Valid Command: ");
  for (int i = start + 1; i <= stop; i++) {
    printf("%c", datainble[i]);
    uint32_t a = pow(10, stop - i);
    value += a * (datainble[i] - 48);
  }
  printf("\n\r");
  for (int i = 0; i < SIZEOFBLEBUFFER; i++) {
    datainble[i] = 0;
  }
  charCounter = 0;
  vc          = 1;
}
void validCommandg(uint8_t start, uint8_t stop) {
  if (vc == 1) {
    vc               = 0;
    uint32_t newData = 0;
    printf("\r\n");
    for (int i = start + 1; i <= stop; i++) {
      printf("%c", datainble[i]);
      uint16_t a = pow(10, stop - i);
      newData += a * (datainble[i] - 48);
    }
    printf("\r\n");
    uint8_t data[2];
    uint162byte(newData, data, 0);
    printf("New data %d has been written to addres %d\r\n",newData,value);
    writeInFlash(value*sizeof(data), data, sizeof(data));
    for (int i = 0; i < SIZEOFBLEBUFFER; i++) {
      datainble[i] = 0;
    }
    charCounter = 0;
  }
}
void onBLE() {
  
  uint8_t ack[2];
  ack[0]=0x70;
  ack[1]=0x0A;
  HAL_UART_Transmit(&BLE_UART, ack, sizeof(ack),0xFF);
  printf("Sending ACK\r\n");
  charCounter = 0;
  HAL_UART_Receive_IT(&BLE_UART, data, sizeof(data));
  while ( data[0]!=113) {
    IWDG_feed(NULL);
    if (interruptFlagBle != 0) {
      interruptFlagBle       = 0;
      datainble[charCounter] = data[0];
    HAL_UART_Receive_IT(&BLE_UART, data, sizeof(data));
    HAL_GPIO_TogglePin(OCTA_GLED_GPIO_Port, OCTA_GLED_Pin);
    HAL_Delay(5);
    HAL_GPIO_TogglePin(OCTA_GLED_GPIO_Port, OCTA_GLED_Pin);
    HAL_Delay(50);
      printf("Data has been receivedi 0x%d\r\n",data[0]);
      charCounter++;
      for (int i = 0; i < charCounter; i++) {
        uint8_t correctCount = i;
        switch (datainble[i]) {
          case 115:
            for (int j = 1 + i; j < charCounter; j++) {
              if (datainble[j] >= 48 && datainble[j] <= 57) {
                correctCount++;
              }
              if (correctCount == j - 1 && datainble[j] == 115)
                validCommand(i, correctCount);
              // printf("j-1 is %d and datainble[j] is %d\n\r",j-1,datainble[j]);
            };
            break;
          case 103:
            for (int j = 1 + i; j < charCounter; j++) {
              if (datainble[j] >= 48 && datainble[j] <= 57) {
                correctCount++;
              }
              if (correctCount == j - 1 && datainble[j] == 103)
                validCommandg(i, correctCount);
              // printf("j-1 is %d and datainble[j] is %d\n\r",j-1,datainble[j]);
            };
            break;
        }
      }
    }
  }
  if (charCounter >= SIZEOFBLEBUFFER) {
    charCounter = 0;
  }
}
