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
uint8_t          buffer[30];
uint8_t          murata_init;
uint8_t          murata_joined = 0;
uint8_t          data[1];

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
  printf("Blue Button pressed\n\r");
  /*
  if (murata_init) {
    if (murata_joined == 0) {
      printf("Command Join\r\n");
      Murata_LoRaWAN_Join();
    } else {
      printf("Device is already joined!\r\n");
    }
  }
  */
}
void onButton1() {
  printf("BTN1 pressed\n\r");
  buffer[2]++;
  Dash7_send(buffer, sizeof(buffer));
  printf("BTN1 end\n\r");
}
void onButton2() {
  printf("BTN2 pressed\n\r");
  LoRaWAN_send(buffer, sizeof(buffer));
  printf("BTN2 end\n\r");
}
void StartDefaultTask(void const *argument) {
  printf("Start the device!\n\r");
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
        printf("Received data from BLE\n\r");
        printf("Received byte is %d\n\r", data[0]);
        button = 0;
        uint8_t adata[2];
        adata[0] = 0x00;
        adata[1] = 0x60;
        HAL_UART_Transmit(&BLE_UART, adata, sizeof(adata), 0xFFF);
        HAL_UART_Receive_IT(&BLE_UART, data, sizeof(data));
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
