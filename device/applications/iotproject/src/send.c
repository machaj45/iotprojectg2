#include "iotproject.h"
#include "send.h"
#include "other.h"

uint16_t            LoRaWAN_Counter = 0;
extern uint8_t      murata_init     = 0;
uint16_t            DASH7_Counter   = 0;
uint8_t             lastsendlora    = 0;
uint8_t             lastsenddash    = 1;
volatile uint8_t    murata_successful;
UART_HandleTypeDef *murata_uart;

void LorawanInit() {
  short_UID               = get_UID();
  murata_init             = Murata_Initialize(short_UID, 1);
  struct OCTA_header temp = platform_getHeader(MURATA_CONNECTOR);
  // platform_initialize_I2C(temp);  // it was init I2C
  if (temp.uartHandle == NULL) {
    printf("Uart not avalable");
  } else {
    murata_uart = temp.uartHandle;
  }

  if (murata_init) {
    //    printf("Murata dualstack module init OK\r\n\r\n");
    //   printOCTAID();
    HAL_Delay(1000);
    //    Murata_LoRaWAN_Join();
  } else {
    printf("ERROR murata is not initialized properly\r\n\r\n");
  }
}

void LoRaWAN_send(uint8_t *loraMessage, uint8_t size) {
  if (murata_init) {
    lastsendlora = 1;
    osMutexWait(txMutexId, osWaitForever);
    if (!Murata_LoRaWAN_Send(loraMessage, size)) {
      printf("Unsuccessful LoRaWAN send\n\r");
    }
    osMutexRelease(txMutexId);
  } else {
    printf("murata not initialized, not sending\r\n");
  }
  if (lastsenddash == 1) {
    lastsenddash      = 0;
    murata_successful = 1;
    // while (murata_successful == 1) {
    //   HAL_Delay(50);
    // };

    HAL_Delay(2000);

    if (murata_init) {
      lastsendlora = 1;
      osMutexWait(txMutexId, osWaitForever);
      if (!Murata_LoRaWAN_Send(loraMessage, size)) {
        printf("Unsuccessful LoRaWAN send\n\r");
      }
      osMutexRelease(txMutexId);
    } else {
      printf("murata not initialized, not sending\r\n");
    }
  }
}


void Dash7_send(uint8_t *dash7Message, uint8_t size) {
  uint8_t times   = 0;
  uint8_t dotimes = 6;
  while (times<dotimes) {
    dash7Message[0]+=times;
    times++;
      murata_successful = 1;
    lastsenddash = 1;
    if (lastsendlora == 1) {
      lastsendlora = 0;
      if (murata_init) {
        osMutexWait(txMutexId, osWaitForever);
        if (!Murata_Dash7_Send(dash7Message, size)) {
          printf("Unsuccessful DASH7 send\n\r");
        }
        DASH7_Counter++;
        osMutexRelease(txMutexId);
      } else {
        printf("murata not initialized, not sending\r\n");
      }
      murata_successful = 1;
      while (murata_successful == 1) {
        HAL_Delay(50);
      };
      if (murata_init) {
        osMutexWait(txMutexId, osWaitForever);
        if (!Murata_Dash7_Send(dash7Message, size)) {
          printf("Unsuccessful DASH7 send\n\r");
        }
        DASH7_Counter++;
        osMutexRelease(txMutexId);
      } else {
        printf("murata not initialized, not sending\r\n");
      }
    } else {
      if (murata_init) {
        osMutexWait(txMutexId, osWaitForever);
        if (!Murata_Dash7_Send(dash7Message, size)) {
          printf("Unsuccessful DASH7 send\n\r");
        }
        DASH7_Counter++;
        osMutexRelease(txMutexId);
      } else {
        printf("murata not initialized, not sending\r\n");
      }
    }
      while (murata_successful == 1) {
        HAL_Delay(50);
      };
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart == murata_uart) {
    Murata_rxCallback();
    murata_data_ready = 1;
  }
  if (huart == &BLE_UART) {
    button                 = 6;
    datainble[charCounter] = data[0];
    HAL_UART_Receive_IT(&BLE_UART, data, sizeof(data));
  }
}
