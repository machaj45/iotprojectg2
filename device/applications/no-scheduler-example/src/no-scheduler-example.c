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

const _Bool Print_SERIAL = 1;

osTimerId temp_hum_timer_id;
float SHTData[2];
volatile _Bool interruptFlag=0; 
volatile uint8_t interruptFlagBle=0; 
uint16_t LoRaWAN_Counter = 0;
uint64_t short_UID;
volatile uint8_t safeCounter = 0;
volatile uint8_t DangerCounter = 0;
volatile uint8_t EmergencyCounter = 0;
volatile _Bool NormalMode = 1;

uint8_t murata_init = 0;

volatile activeSending = 0;

static uint8_t maxSafeCounter = 3;
static uint8_t maxDangerCounter = 3;
static uint8_t maxEmergencyCounter = 3;

uint16_t NormalSleepTime = 0x000A;
uint16_t EmergencySleepTime = 0x000A;

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

static void MX_RTC_Init(void);



void sleep(uint16_t time);

void testBlink(void);

void quickBlink(void);

bool calculateDanger();

int main(void)
{
#if USE_BOOTLOADER
  bootloader_SetVTOR();
#endif

  HAL_Init();
 
  SystemClock_Config();

  MX_RTC_Init();

  Initialize_Platform();

  //needed for flash
  S25FL256_Initialize(&FLASH_SPI);

  // get Unique ID of Octa
  short_UID = get_UID(); 

//init for gas sensor
  SPG30_Initialize();
  P3_I2C_Init();
  setI2CInterface_SHT31(&common_I2C); 

  SHT31_begin(); 

  LorawanInit();


  setUpDefaultValuesforTresholds();


if (Print_SERIAL)
  printWelcome();

UpdateThresholdsFromFlashBLE();

 
Murata_LoRaWAN_Join(); 

////////////////////////  small while test loop   //////////////////////////////
/*
  HAL_UART_Receive_IT(&BLE_UART, data, sizeof(data));


  SGP_Init();


         while (1){
IWDG_feed(NULL);


    

NormalMode = !NormalMode;
  do_measurement(4);
  danger = calculateDanger();

  if (!NormalMode){
    danger = true;
  }

  LoadBuffer();

//  Murata_toggleResetPin();
  
 // LoRaWAN_send(buffer,sizeof(buffer));

           
  modem_reinit();
  Dash7_send(buffer, sizeof(buffer));
  WaitSend(5000);

        
}
 */
//////////////////////// end small while loop   ////////////////////////////


  HAL_UART_Receive_IT(&BLE_UART, data, sizeof(data));
  while (1)
  {
    

      // feed watchdog tmer
      IWDG_feed(NULL);  

    //de interrupt will put the flag op 1
    if (interruptFlag==1) {
      if (Print_SERIAL){
          printf("\033[2J");
          printf("\033[H");
        printf("Back awake\r\n"); 
        printf("Session %d\r\n",safeCounter);
      }
      interruptFlag=0; 
    }



///////////////////////////  NORMAL MODE  ////////////////////////////////////
    if (NormalMode){



     if (Print_SERIAL) 
         quickBlink();


      //////////////////  if BLE, commincate   /////////////////////

      if(interruptFlagBle!=0){

        if (Print_SERIAL)
          printf("BLE MODE START\r\n"); 
       onBLE();
       interruptFlagBle=0;

       if (Print_SERIAL)
          printf("BLE MODE STOP\r\n"); 

       UpdateThresholdsFromFlashBLE();

      }

      ////////////// end BLE  /////////////////////////////////

    //wake up SGP-sensor 
    SGP_Init();

    // wait for 15 s and do calibration message
    do_measurement(0x000F);

      //usefull data 
    do_measurement(0x0002);

      danger =  calculateDanger(); 

      if (danger){

        if (Print_SERIAL)
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

          if (Print_SERIAL)
            printf("enter emergency mode on next wake up\r\n");
        }
        else {
          if (Print_SERIAL)
            printf("send Dash7 data\r\n");

          //LOAD data in buffer 
          LoadBuffer();
          modem_reinit();
          Dash7_send(buffer,sizeof(buffer));
         WaitSend(5000);
         
        }
      }

      else {
        if (Print_SERIAL)
          printf("clear\r\n");
        safeCounter++;
        DangerCounter = 0;

          //check if server needs to update safe values
        if (safeCounter >= maxSafeCounter){
          //reset counter
          safeCounter = 0;
          if (Print_SERIAL)
            printf("send safe server LORA update\r\n");
          LoadBuffer();
          LoRaWAN_send(buffer, sizeof(buffer));
          WaitSend(3375);
          
        }

      }     
      if (Print_SERIAL) 
        printf("safecounter: %d, dangercounter: %d\r\n",safeCounter,DangerCounter);
      sleep(NormalSleepTime);
    }

///////////////////////  EMERfGENCY MODE  ////////////////////////////
    else{

   // if (Print_SERIAL)
      printf("Emergency\r\n");


      //update emergency counter
      EmergencyCounter++;

      quickBlink();
      if (EmergencyCounter >= maxEmergencyCounter){
        EmergencyCounter = 0;

        //do measurements & calculate danger.
        //if there is still is danger, remain in emergency mode


        // wake up SGP sensor, 15 s and again 2 s
        SGP_Init();


        do_measurement(0x000F);

        do_measurement(0x0002);

        danger = calculateDanger();

        if (danger){
          if (Print_SERIAL)
            printf("recalculated emergengy data is dangerous DASH7\r\n");
          LoadBuffer();
       } 
        else{
          //put safe counter back on 
          safeCounter = maxSafeCounter;
          NormalMode = 1;
        }

      }

      else{
        if (Print_SERIAL)
          printf("SEND STORED DASH7 EMERGENCY DATA\r\n");

          buffer[13] = (uint8_t)Message_Counter++;
          modem_reinit();
          Dash7_send(buffer,sizeof(buffer));
          WaitSend(5000);


      }
  if (Print_SERIAL)
      printf("emergency counter: %d\r\n",EmergencyCounter);
    sleep(EmergencySleepTime);

    }
  }
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

  readInFlash(EMTM,&EmergencySleepTime,2);
  readInFlash(NORM,&NormalSleepTime,2);

  if(Print_SERIAL){
  
  printf("\033[2J");
  printf("\033[H");
  printf("\r\n");
  printf("************************************************\r\n");
  printf("Tempriture treshlod is between %d and %d\r\n",TemperatureTreshold[0],TemperatureTreshold[1]);
  printf("Humidity treshlod is between %d and %d\r\n",HumidityTreshold[0],HumidityTreshold[1]);
  printf("CO2Treshold treshlod is between %d and %d\r\n",CO2Treshold[0],CO2Treshold[1]);
  printf("TVOCTreshold treshlod is between %d and %d\r\n",TVOCTreshold[0],TVOCTreshold[1]);
  printf("Timer Emergency is %d and Timer Normal %d\r\n",EmergencySleepTime,NormalSleepTime);
  printf("************************************************\r\n");
  printf("\r\n");
  }
  HAL_Delay(1000);
}

void printWelcome(void)
{
  printf("\033[2J");
  printf("\033[H");
  printf("\r\n");
  printf("*****************************************\r\n");
  printf("************ Ultimate Guard **************\r\n");
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

  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;

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
  if (Print_SERIAL)
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

  if (htim->Instance == TIM1)
  {
    HAL_IncTick();
  }

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  if (GPIO_Pin==GPIO_Pin_13) {
  interruptFlag = 1; 
  printf("interrupt accelerometer! \r\n");
 } 
}

void print_data(void){
  printf("\r\n");
  printf("Temperature: %.2f %cC  \r\n", SHTData[0],'°');
  printf("Humidity: %.2f %% \r\n", SHTData[1]);
  printf("TVOC: %5d ppb\r\n",tvoc_ppb);
  printf("CO2: %5d ppm\r\n",co2_eq_ppm);
}

void do_measurement(uint16_t delay){


if (delay > 11){
  printf("CALIBRATION MEASUREMENT START\r\n");
}
if (delay < 5){
  SHT31_get_temp_hum(SHTData);
}


  sleep (delay);


  err = sgp_measure_iaq_blocking_read(&tvoc_ppb, &co2_eq_ppm);
  if ((err == STATUS_OK) && Print_SERIAL ){
    if (delay < 10){
      print_data();
    }
  }
  else{
    if (Print_SERIAL)
      printf("error while reading data\r\n");
  }



  if (delay < 5){
       SGP_Sleep();
      Flash();
  } 
  else {
    printf("CALIBRATION MEASUREMENT END\r\n");
  }

}


/* USER CODE END 4 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart == &BLE_UART){
    interruptFlagBle++;
    printf("BLE INT\r\n");
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
    HAL_Delay(500);
    HAL_GPIO_TogglePin(OCTA_RLED_GPIO_Port, OCTA_RLED_Pin);
}

void Flash(void){
  HAL_GPIO_TogglePin(OCTA_BLED_GPIO_Port, OCTA_BLED_Pin);
    HAL_Delay(100);
    HAL_GPIO_TogglePin(OCTA_BLED_GPIO_Port, OCTA_BLED_Pin);
    HAL_Delay(10);
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
  
  uint8_t ack[22];
  ack[0]=0x00;

  readInFlash(0,ack+1,sizeof(ack));
  ack[21]=0x0A;
  HAL_UART_Transmit(&BLE_UART, ack, sizeof(ack),0xFF);
  printf("Sending ACK\r\n");

  value=0;
  charCounter = 0;
  for(int i =0;i<SIZEOFBLEBUFFER;i++){
    datainble[i]=0;
  }
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

void SGP_Sleep(void){
          uint8_t data[1] = {0x06};
        sensirion_i2c_write(0x00,data,sizeof(data));
}

void SGP_Init(void){

  //  Flash(); 
     while(sgp_probe() != STATUS_OK)
    {
      if (Print_SERIAL)
    	  printf("SGP sensor probing faiLed ... check SGP30 I2C connection and power\r\n");
    	HAL_Delay(500); // delay retry
    }
    if (Print_SERIAL)
      printf("SGP sensor probing successful\r\n");

    // * Read gas signals 
    err = sgp_measure_signals_blocking_read(&scaled_ethanol_signal, &scaled_h2_signal);
    if((err == STATUS_OK) && (Print_SERIAL))
    {
      
			// Print ethanol signal with floating point support
			printf("Ethanol signal: %.2f \r\n", scaled_ethanol_signal / 512.0);

			// Print H2 signal with floating point support
			printf("H2 signals: %.2f \r\n",scaled_h2_signal / 512.0);

    }
    else
    {
      if (Print_SERIAL)
    	  printf("error reading Ethanol and H2 signals\r\n");
    }
        err = sgp_iaq_init();
 
        
}
