#include "iotproject.h"
#include "other.h"

volatile uint8_t acc_int=0;
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

void temp_hum_measurement(void) {
  SHT31_get_temp_hum(SHTData);
  print_temp_hum();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
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

void print_temp_hum(void) {
  printf("\r\n");
  printf("Temperature: %.2f deg of C\r\n", SHTData[0]);
  printf("Humidity: %.2f relative \r\n", SHTData[1]);
  printf("\r\n");
}

void Error_Handler(void) {
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart == &P1_UART) {
    Murata_rxCallback();
  }
}

void decodeError (int status) {
  switch (status) {
    case 0:
      printf("HAL_OK\n\r");
      break;
    case 1:
      printf("HAL_Error\n\r");
      break;
    case 2:
      printf("HAL_BUSY\n\r");
      break;
    case 3:
      printf("HAL_TIMEOUT\n\r");
      break;
  }
}

