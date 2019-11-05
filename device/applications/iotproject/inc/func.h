#ifndef _FUNC
#define _FUNC

#ifdef __cplusplus
extern "C" {
#endif
void Initialize_Sensors(void);
void Initialize_OS(void);
void StartDefaultTask(void const *argument);
void printWelcome(void);
void LoRaWAN_send(void const *argument);
void check_modules(void const *argument);
void murata_process_rx_response(void const *argument);
void accelerometer_measurement(void);
void magnetometer_measurement(void);
void print_accelerometer(double data[]);
void print_magnetometer(uint16_t data[]);
void Error_Handler(void);
#ifdef __cplusplus
}
#endif
#endif 