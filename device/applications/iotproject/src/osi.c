#include "iotproject.h"
#include "func.h"

uint8_t  counter           = 0;
uint16_t LoRaWAN_Counter   = 0;
uint8_t  murata_init       = 0;
uint8_t  murata_data_ready = 0;
uint16_t DASH7_Counter     = 0;
uint8_t  lora_init         = 0;
uint64_t short_UID;
uint8_t button;
void     StartDefaultTask(void const *argument) {
  printf("Start the device!\n\r");
  while (1) {
    if (acc_int == 1) {
    //Dash7_send(NULL);
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
//void EXTI15_10_IRQHandler(void) {
  //HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
//}
void EXTI9_5_IRQHandler(void) {
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
}
void EXTI0_IRQHandler(void) {
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}
void EXTI1_IRQHandler(void) {
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}
void HAL_GPIO_EXTI_Callback(uint16_t gpioPinNumber) {
  if (GPIO_PIN_5 == gpioPinNumber) {
    //printf("PIN5 Int \r\n");
    if (acc_int != 2)
      acc_int = 1;
  }
  if (GPIO_PIN_13 == gpioPinNumber) {
    //printf("PIN13 Int \r\n");
  }
  if (GPIO_PIN_0 == gpioPinNumber) {
    acc_int = 2;
    button=1;
    // printf("PIN0 Int \r\n");
    printf("Send Dash7 \r\n");
  }
  if (GPIO_PIN_1 == gpioPinNumber) {
    printf("PIN1 Int \r\n");
  }
}

void LoRaWAN_send(void const *argument) {
  if (lora_init) {
    uint8_t bytes[8];
    uint8_t loraMessage[8];
    uint8_t i = 0;
    // uint16 counter to uint8 array (little endian)
    // counter (large) type byte
    *(float *)(bytes) = SHTData[0];
    loraMessage[i++]  = bytes[0];
    loraMessage[i++]  = bytes[1];
    loraMessage[i++]  = bytes[2];
    loraMessage[i++]  = bytes[3];
    *(float *)(bytes) = SHTData[1];
    loraMessage[i++]  = bytes[0];
    loraMessage[i++]  = bytes[1];
    loraMessage[i++]  = bytes[2];
    loraMessage[i++]  = bytes[3];
    osMutexWait(txMutexId, osWaitForever);
    if (!Murata_LoRaWAN_Send((uint8_t *)loraMessage, i)) {
      lora_init++;
      if (lora_init == 10)
        lora_init = 0;
    } else {
      lora_init = 1;
    }
    // BLOCK TX MUTEX FOR 3s
    osDelay(3000);
    osMutexRelease(txMutexId);
    LoRaWAN_Counter++;
  } else {
    printf("murata not initialized, not sending\r\n");
  }
}

void check_modules(void const *argument) {
 /* printf("checking the status of the modules\r\n");
  if (!lora_init) {
    // LORAWAN
    lora_init = Murata_Initialize(short_UID, 0);
    Murata_toggleResetPin();
  }
  */
}
void accelerometer_measurement(void) {
  uint16_t accDataRaw[3];
  osMutexWait(i2cMutexId, osWaitForever);
  LSM303AGR_ACC_readAccelerationRawData(accDataRaw);
  osMutexRelease(i2cMutexId);
  print_accelerometer(accDataRaw);
}
void magnetometer_measurement(void) {
  uint16_t magDataRaw[3];
  osMutexWait(i2cMutexId, osWaitForever);
  LSM303AGR_MAG_readMagneticRawData(magDataRaw);
  osMutexRelease(i2cMutexId);
  print_magnetometer(magDataRaw);
}

void print_accelerometer(uint16_t data[]) {
  printf("\r\n");
  printf("Accelerometer data: X: % 06.2f Y: % 06.2f Z: % 06.2f \r\n", (double)(data[0]), (double)(data[1]), (double)(data[2]));
  printf("\r\n");
}
void print_magnetometer(uint16_t data[]) {
  printf("\r\n");
  printf("Magnetometer data: X: %d Y: %d Z: %d \r\n", data[0], data[1], data[2]);
  printf("\r\n");
}
void LorawanInit(uint64_t short_UID) {
  short_UID = get_UID();
  //printf("%lld", (long long)short_UID);
  murata_init = Murata_Initialize(short_UID, 0);

  if (murata_init) {
    printf("Murata dualstack module init OK\r\n\r\n");
  }
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
}
void Dash7_send(void const *argument) {
  if (murata_init) {
    uint8_t dash7Message[5];
    uint8_t i = 0;
    // uint16 counter to uint8 array (little endian)
    // counter (large) type byte
    dash7Message[i++] = 0x14;
    dash7Message[i++] = (uint8_t)(DASH7_Counter >> 8 & 0xFF);
    dash7Message[i++] = (uint8_t)(DASH7_Counter & 0xFF);
    // osMutexWait(txMutexId, osWaitForever);
    if (!Murata_Dash7_Send((uint8_t *)dash7Message, i)) {
      murata_init++;
      if (murata_init == 10)
        murata_init = 0;
    } else {
      murata_init = 1;
    }
    // BLOCK TX MUTEX FOR 3s
    // osDelay(3000);
    // osMutexRelease(txMutexId);
    DASH7_Counter++;
    HAL_GPIO_TogglePin(OCTA_BLED_GPIO_Port, OCTA_BLED_Pin);
    HAL_Delay(200);
    HAL_GPIO_TogglePin(OCTA_BLED_GPIO_Port, OCTA_BLED_Pin);
  } else {
    printf("murata not initialized, not sending\r\n");
  }
}
void Initialize_Sensors(void) {
  LSM303AGR_setI2CInterface(&common_I2C);
  setI2CInterface_SHT31(&common_I2C);
  LSM303AGR_ACC_reset();
  acc_int = 0;
  LSM303AGR_MAG_reset();
  HAL_Delay(50);
  LSM303AGR_init();
  HAL_Delay(50);
  SHT31_begin();
}
void Initialize_OS(void) {

  osMutexDef(txMutex);
  txMutexId = osMutexCreate(osMutex(txMutex));

  osThreadDef(murata_rx_processing, murata_process_rx_response, osPriorityNormal, 0, 512);
  murata_rx_processing_handle = osThreadCreate(osThread(murata_rx_processing), NULL);
  Murata_SetProcessingThread(murata_rx_processing_handle);

  osTimerDef(moduleCheckTim, check_modules);
  moduleCheckTimId = osTimerCreate(osTimer(moduleCheckTim), osTimerPeriodic, NULL);
  osTimerStart(moduleCheckTimId, MODULE_CHECK_INTERVAL * 10000);


  osThreadDef(defaultTask, StartDefaultTask, osPriorityLow, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);
}
void Error_Handler(void) {
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart == &P3_UART) {
    Murata_rxCallback();
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
void print_temp_hum(void) {
  printf("\r\n");
  printf("Temperature: %.2f deg of C\r\n", SHTData[0]);
  printf("Humidity: %.2f relative \r\n", SHTData[1]);
  printf("\r\n");
}

void temp_hum_measurement(void) {
  SHT31_get_temp_hum(SHTData);
  print_temp_hum();
}


#ifdef USE_FULL_ASSERT
void assert_failed(char *file, uint32_t line) {
}
#endif /* USE_FULL_ASSERT */
