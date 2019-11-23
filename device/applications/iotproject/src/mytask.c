#include "iotproject.h"
#include "mytask.h"
#include "send.h"
#include "other.h"

#define BlueButton 3
#define Button1 1
#define Button2 2

volatile uint8_t murata_data_ready = 0;
volatile uint8_t button;
uint8_t          lora_init;
uint64_t         short_UID;

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

void onBlueButton() {
  printf("Blue Button pressed");
}
void onButton1() {
  Dash7_send(NULL);
}
void onButton2() {
  printOCTAID();
  uint8_t loraMessage[5];
  loraMessage[0] = 0x14;
  loraMessage[1] = 0x00;
  loraMessage[2] = 0x00;
  Murata_LoRaWAN_Send(loraMessage, sizeof(loraMessage));
}
void StartDefaultTask(void const *argument) {
  printf("Start the device!\n\r");
  while (1) {
    if (acc_int == 1) {
      acc_int = 0;
      // temp_hum_measurement();LoRaWAN_send(NULL);
    }

    if (murata_data_ready) {
      printf("processing murata fifo dafasdfasd\r\n");
      murata_data_ready = !Murata_process_fifo();
    }
    /*
    uint8_t data [20];
    HAL_UART_Receive(&BLE_UART, data, sizeof(data),0xFFF);
    printf("%d %d %d %d %d\n\r",data[0],data[1],data[2],data[3],data[4]);
    */

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
    // Wait to be notified that the transmission is complete.  Note the first
    // parameter is pdTRUE, which has the effect of clearing the task's notification
    // value back to 0, making the notification value act like a binary (rather than
    // a counting) semaphore.

    startProcessing = ulTaskNotifyTake(pdTRUE, osWaitForever);
    printf("The transmission ended as expected");
    if (startProcessing == 1) {
      printf("The transmission ended as expected");
      // The transmission ended as expected.
      Murata_process_fifo();
    } else {
    }
    osDelay(1);
  }
  osThreadTerminate(NULL);
}
void vApplicationIdleHook() {
#if LOW_POWER
  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFE);
#endif
}
