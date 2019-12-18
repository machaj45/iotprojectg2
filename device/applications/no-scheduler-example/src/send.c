#include "no-scheduler-example.h"
#include "send.h"
//#include "other.h"

extern uint16_t            LoRaWAN_Counter;
extern uint8_t      murata_init;
extern uint64_t short_UID;
uint16_t            DASH7_Counter   = 0;
uint8_t             lastsendlora    = 0;
uint8_t             lastsenddash    = 1;
extern volatile uint8_t    murata_successful;
UART_HandleTypeDef *murata_uart;

extern _Bool Print_SERIAL;


void LorawanInit() {
  short_UID               = get_UID();
  murata_init             = Murata_Initialize(short_UID, 0);
  struct OCTA_header temp = platform_getHeader(MURATA_CONNECTOR);
  // platform_initialize_I2C(temp);  // it was init I2C
  if (temp.uartHandle == NULL) {
    if(Print_SERIAL)
      printf("Uart not avalable");
  } else {
    murata_uart = temp.uartHandle;
  }

  if (murata_init) {
    if (Print_SERIAL)
    printf("Murata dualstack module init OK\r\n\r\n");
 //   printOCTAID();
 //   HAL_Delay(1000);
    //    Murata_LoRaWAN_Join();
  } else {
    if (Print_SERIAL)
    printf("ERROR murata is not initialized properly\r\n\r\n");
  }
}

void LoRaWAN_send(uint8_t *loraMessage, uint8_t size) {
  if (murata_init) {
    lastsendlora = 1;
 //   osMutexWait(txMutexId, osWaitForever);
    if (!Murata_LoRaWAN_Send(loraMessage, size)) {
      if (Print_SERIAL)
      printf("Unsuccessful LoRaWAN send\n\r");
    }
  //  osMutexRelease(txMutexId);
  } else {
    if (Print_SERIAL)
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
    //  osMutexWait(txMutexId, osWaitForever);
      if (!Murata_LoRaWAN_Send(loraMessage, size)) {
        if (Print_SERIAL)
        printf("Unsuccessful LoRaWAN send\n\r");
      }
    //  osMutexRelease(txMutexId);
    } else {
      if (Print_SERIAL)
      printf("murata not initialized, not sending\r\n");
    }
  }
}

void Dash7_send(uint8_t *dash7Message, uint8_t size) {
  lastsenddash = 1;
  if (lastsendlora == 1) {
    lastsendlora = 0;
    if (murata_init) {
  //    osMutexWait(txMutexId, osWaitForever);
      if (!Murata_Dash7_Send(dash7Message, size)) {
        if (Print_SERIAL)
        printf("Unsuccessful DASH7 send\n\r");
      }
      DASH7_Counter++;
    //  osMutexRelease(txMutexId);
    } else {
      if (Print_SERIAL)
      printf("murata not initialized, not sending\r\n");
    }
    murata_successful = 1;
/*     while (murata_successful == 1) {
      HAL_Delay(50);
    }; */

    
    if (murata_init) {
      //osMutexWait(txMutexId, osWaitForever);
      if (!Murata_Dash7_Send(dash7Message, size)) {
        if (Print_SERIAL)
        printf("Unsuccessful DASH7 send\n\r");
      }
      DASH7_Counter++;
     // osMutexRelease(txMutexId);
    } else {
      if (Print_SERIAL)
      printf("murata not initialized, not sending\r\n");
    }
  } else {
    if (murata_init) {
 //     osMutexWait(txMutexId, osWaitForever);
      if (!Murata_Dash7_Send(dash7Message, size)) {
        if (Print_SERIAL)
        printf("Unsuccessful DASH7 send\n\r");
      }
      DASH7_Counter++;
   //   osMutexRelease(txMutexId);
    } else {
      if (Print_SERIAL)
      printf("murata not initialized, not sending\r\n");
    }
  }
}

// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
//   if (huart == murata_uart) {
//     Murata_rxCallback();
//     //murata_data_ready = 1;
//   }
//   // if (huart == &BLE_UART) {
//   //   button                 = 6;
//   //   datainble[charCounter] = data[0];
//   //   HAL_UART_Receive_IT(&BLE_UART, data, sizeof(data));
//   // }
// }
