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
void changeInFlash(uint8_t addres,uint8_t * data,uint8_t size);
void readInFlash(uint8_t addres,uint8_t * data,uint8_t size);
uint8_t uint322byte(uint32_t input, uint8_t* output, uint8_t offset);
uint8_t byte2uint32(uint8_t* input, uint32_t output);
void IWDG_feed(void const *argument);
uint8_t float2byte(float input, uint8_t* output, uint8_t offset);
#ifdef __cplusplus
}
#endif
#endif 
