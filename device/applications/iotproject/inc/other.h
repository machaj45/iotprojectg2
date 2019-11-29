#ifndef _OTHER
#define _OTHER

#ifdef __cplusplus
extern "C" {
#endif
void Initialize_Sensors(void);
void accelerometer_measurement(void);
void magnetometer_measurement(void);
void print_accelerometer(uint16_t data[]);
void print_magnetometer(uint16_t data[]);
void print_temp_hum(void);
void printOCTAID();
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void Error_Handler(void);
void decodeError (int status);
void SPG30_measure(void);
void IWDG_feed(void const *argument);
uint8_t float2byte(float input, uint8_t* output, uint8_t offset);
#ifdef __cplusplus
}
#endif
#endif 
