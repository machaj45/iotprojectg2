#include "iotproject.h"
#include "func.h"

uint8_t  counter           = 0;
uint16_t LoRaWAN_Counter   = 0;
uint8_t  murata_init       = 0;
uint8_t  murata_data_ready = 0;
uint16_t DASH7_Counter     = 0;
uint8_t  lora_init         = 0;
uint64_t short_UID;
uint8_t  button;

void StartDefaultTask(void const *argument) {
  printf("Start the device!\n\r");
  while (1) {
    if (acc_int == 1) {
      // Dash7_send(NULL);
      acc_int = 0;
      // temp_hum_measurement();
      // LoRaWAN_send(NULL);
    }
    if (button == 1) {
      Dash7_send(NULL);
      button = 0;

      // temp_hum_measurement();
      // LoRaWAN_send(NULL);
    }
    IWDG_feed(NULL);

    if (murata_data_ready) {
      printf("processing murata fifo\r\n");
      murata_data_ready = !Murata_process_fifo();
    }

    // SEND 5 D7 messages, every 10 sec.
    // Afterwards, send 3 LoRaWAN messages, every minute
    /*
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
    */
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
