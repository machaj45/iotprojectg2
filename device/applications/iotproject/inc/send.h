#ifndef _SEND
#define _SEND

#ifdef __cplusplus
extern "C" {
#endif
void LorawanInit();
void LoRaWAN_send(void const *argument);
void Dash7_send(void const *argument);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
#ifdef __cplusplus
}
#endif
#endif 
