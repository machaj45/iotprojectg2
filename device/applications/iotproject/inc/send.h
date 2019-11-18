#ifndef _SEND
#define _SEND

#ifdef __cplusplus
extern "C" {
#endif
void LorawanInit(uint64_t short_UID);
void LoRaWAN_send(void const *argument);
void Dash7_send(void const *argument);
#ifdef __cplusplus
}
#endif
#endif 
