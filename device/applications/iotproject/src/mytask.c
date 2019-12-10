#include "iotproject.h"
#include "mytask.h"
#include "send.h"
#include "other.h"
#include <math.h>
#include "S25FL256.h"


#define SIZE 256
#define BLOCK_ID 0
#define BlueButton 3
#define Button1 1
#define Button2 2

volatile uint8_t murata_data_ready = 0;
volatile uint8_t button;
uint8_t          lora_init;
uint64_t         short_UID;
uint8_t          buffer[13];
uint8_t          murata_init;
uint8_t          murata_joined = 0;
uint8_t          data[1];
uint8_t          datainble[SIZEOFBLEBUFFER];
uint8_t          charCounter = 0;
extern float     cotlevels;

uint8_t dash7_Mycounter = 0;
uint8_t lora_Mycounter  = 0;

void Initialize_OS(void) {


  osMutexDef(txMutex);
  txMutexId = osMutexCreate(osMutex(txMutex));

  osThreadDef(murata_rx_processing, murata_process_rx_response, osPriorityNormal, 0, 512);
  murata_rx_processing_handle = osThreadCreate(osThread(murata_rx_processing), NULL);
  Murata_SetProcessingThread(murata_rx_processing_handle);

  osTimerDef(moduleCheckTim, check_modules);
  moduleCheckTimId = osTimerCreate(osTimer(moduleCheckTim), osTimerPeriodic, NULL);
  osTimerStart(moduleCheckTimId, MODULE_CHECK_INTERVAL * 1000);

  osThreadDef(defaultTask, StartDefaultTask, osPriorityLow, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);
}

uint8_t vc = 0;

void onBlueButton() {
  printf("Blue Button pressed\n\r");
  uint8_t data2[16];
  readInFlash(0, data2, sizeof(data2));
  printf("<------ Printing tresholds ------>\n\r");
  printf("Tempriture is between %d and %d\n\r",byte2uint16(data2,TEMP_TH_LOW),byte2uint16(data2,TEMP_TH_HIGH));
  printf("Humidity is between %d and %d\n\r",byte2uint16(data2,HUMI_TH_LOW),byte2uint16(data2,HUMI_TH_HIGH));
  printf("CO2 is between %d and %d\n\r",byte2uint16(data2,CO2_TH_LOW),byte2uint16(data2,CO2_TH_HIGH));
  printf("TOV is between %d and %d\n\r",byte2uint16(data2,TVOC_TH_LOW),byte2uint16(data2,TVOC_TH_HIGH));
  printf("<------        END         ------>\n\r");
}
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
    vc = 0;
    uint32_t newData = 0;

    for (int i = start + 1; i <= stop; i++) {
      printf("%c", datainble[i]);
      uint32_t a = pow(10, stop - i);
      newData += a * (datainble[i] - 48);
    }
    printf("\n\r");
    uint8_t data [4];
    uint322byte(newData,data,0);
    writeInFlash(value,data,sizeof(data));
    
    uint8_t data2 [4];
    readInFlash(value,data2,sizeof(data));
    uint32_t olddata;
    byte2uint32(data2,olddata);
    printf("Data are %d",olddata);
    for (int i = 0; i < SIZEOFBLEBUFFER; i++) {
      datainble[i] = 0;
    }
    charCounter = 0;
  }
}
void onBLE() {
  datainble[charCounter] = data[0];

  HAL_UART_Receive_IT(&BLE_UART, data, sizeof(data));
  printf("Received data from BLE\n\r");
  // printf("Received byte is %d\n\r", datainble[charCounter]);
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
  if (charCounter > SIZEOFBLEBUFFER) {
    charCounter = 0;
  }
}
void onButton1() {
  printf("BTN1 pressed\n\r");
  /*
  float g[2];
  SHT31_get_temp_hum(g);
  float2byte(g[0], buffer, 0);
  float2byte(g[1], buffer, 4);
  float2byte(cotlevels, buffer, 8);
  buffer[12] = (uint8_t)dash7_Mycounter;
  dash7_Mycounter++;
  */
  uint8_t test [1];
  test[0]=dash7_Mycounter;
  dash7_Mycounter+=6;
  //Dash7_send(buffer, sizeof(buffer));
  Dash7_send(test, sizeof(test));
  HAL_GPIO_TogglePin(OCTA_BLED_GPIO_Port, OCTA_BLED_Pin);
  HAL_Delay(2000);
  HAL_GPIO_TogglePin(OCTA_BLED_GPIO_Port, OCTA_BLED_Pin);
  printf("BTN1 end\n\r");
}
void onButton2() {
  printf("BTN2 pressed\n\r");
  /*float g[2];
  SHT31_get_temp_hum(g);
  float2byte(g[0], buffer, 0);
  float2byte(g[1], buffer, 4);
  float2byte(cotlevels, buffer, 8);
  printOCTAID();
  buffer[12] = (uint8_t)lora_Mycounter;
  lora_Mycounter++;
  LoRaWAN_send(buffer, sizeof(buffer));*/
setUpDefaultValuesforTresholds();
  uint8_t test [1];
  test[0]=255;
  Murata_Dash7_Send(test, 1);
  printf("BTN2 end\n\r");
}
void StartDefaultTask(void const *argument) {
  printf("|||||||||||||||||| Start! |||||||||||||||||| \n\r");
  HAL_UART_Receive_IT(&BLE_UART, data, sizeof(data));
  while (1) {
    if (acc_int == 1) {
      acc_int = 0;
      // temp_hum_measurement();LoRaWAN_send(NULL);
    }
    switch (button) {
      case BlueButton:
        onBlueButton();
        button = 0;
        break;
      case Button1:
        onButton1();
        button = 0;
        break;
      case Button2:
        onButton2();
        button = 0;
        break;
      case 6:
        onBLE();
        button = 0;
        break;
    }
    IWDG_feed(NULL);
     SPG30_measure();
    osDelay(1000);
  }
}

void check_modules(void const *argument) {
  printf("checking the status of the modules\r\n");
  if (!lora_init) {
    lora_init = Murata_Initialize(short_UID, 1);
    Murata_toggleResetPin();
  }
}

void murata_process_rx_response(void const *argument) {
  uint32_t startProcessing;
  while (1) {
    startProcessing = ulTaskNotifyTake(pdTRUE, osWaitForever);
    if (startProcessing == 1) {
      printf("The transmission ended as expected\n\r");
      Murata_process_fifo();
    }
    osDelay(1);
    startProcessing = 0;
  }
  osThreadTerminate(NULL);
}
void vApplicationIdleHook() {
#if LOW_POWER
  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFE);
#endif
}
