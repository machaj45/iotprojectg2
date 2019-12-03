#ifndef _SEND
#define _SEND

#ifdef __cplusplus
extern "C" {
#endif
void LorawanInit();
void LoRaWAN_send(uint8_t * loraMessage, uint8_t size);
void Dash7_send(uint8_t * dash7Message, uint8_t size);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
#ifdef __cplusplus
}
#endif
#endif 
