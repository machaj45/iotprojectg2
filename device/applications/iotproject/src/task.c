#include "iotproject.h"
#include "task.h"
#include "send.h"

uint8_t          murata_data_ready = 0;
volatile uint8_t button;
uint8_t          lora_init;
uint64_t         short_UID;

void StartDefaultTask(void const *argument) {
  printf("Start the device!\n\r");
  while (1) {
    if (acc_int == 1) {
      acc_int = 0;
      // temp_hum_measurement();LoRaWAN_send(NULL);
    }
    if (button == 1) {
      Dash7_send(NULL);
      button = 0;
    }
    IWDG_feed(NULL);

    if (murata_data_ready) {
      printf("processing murata fifo\r\n");
      murata_data_ready = !Murata_process_fifo();
      murata_process_rx_response(NULL);
    }
    HAL_Delay(200);
    modem_reinit();
  }
}

void check_modules(void const *argument) {
  printf("checking the status of the modules\r\n");
  if (!lora_init) {
    lora_init = Murata_Initialize(short_UID, 0);
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
    IWDG_feed(NULL);
    startProcessing = ulTaskNotifyTake(pdTRUE, osWaitForever);
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
