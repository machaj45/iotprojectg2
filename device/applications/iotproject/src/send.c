#include "iotproject.h"
#include "send.h"

uint16_t LoRaWAN_Counter = 0;
uint8_t  murata_init     = 0;
uint16_t DASH7_Counter   = 0;

void LorawanInit(uint64_t short_UID) {
  short_UID   = get_UID();
  murata_init = Murata_Initialize(short_UID, 0);

  if (murata_init) {
    printf("Murata dualstack module init OK\r\n\r\n");
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
    // osDelay(3000);
    // osMutexRelease(txMutexId);
    LoRaWAN_Counter++;
  } else {
    printf("murata not initialized, not sending\r\n");
  }
}

void Dash7_send(void const *argument) {
  if (murata_init) {
    uint8_t dash7Message[5];
    uint8_t i         = 0;
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
