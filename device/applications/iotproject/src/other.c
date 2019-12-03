#include "iotproject.h"
#include "other.h"
#include "sgp30.h"
volatile uint8_t acc_int = 0;

uint16_t i = 0;
int16_t  err;
uint16_t tvoc_ppb, co2_eq_ppm;
uint32_t iaq_baseline;
uint16_t scaled_ethanol_signal, scaled_h2_signal;
uint32_t measurement_counter = 0;
uint8_t  enable_SGP          = 1;

uint8_t float2byte(float input, uint8_t* output, uint8_t offset) {
  float*   floatPtr    = &input;
  uint8_t* dataPointer = (uint8_t*)floatPtr;
  output[0 + offset]   = *dataPointer;
  output[1 + offset]   = *(dataPointer + 1);
  output[2 + offset]   = *(dataPointer + 2);
  output[3 + offset]   = *(dataPointer + 3);
}

uint8_t uint322byte(uint32_t input, uint8_t* output, uint8_t offset) {
  uint32_t* floatPtr    = &input;
  uint8_t*  dataPointer = (uint8_t*)floatPtr;
  output[0 + offset]    = *dataPointer;
  output[1 + offset]    = *(dataPointer + 1);
  output[2 + offset]    = *(dataPointer + 2);
  output[3 + offset]    = *(dataPointer + 3);
}

uint8_t byte2uint32(uint8_t* input, uint32_t output) {
  output = input[0] + input[1]*256 + input[2]*256*256 + input[3]*256*256*256;
}
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
  if (enable_SGP) {
    HAL_Delay(300);
    SPG30_Initialize();
  }
}
void writeInFlash(uint8_t addres, uint8_t* data, uint8_t size) {
  static uint8_t tx[256];
  static uint8_t rx[256];
  S25FL256_open(0);
  S25FL256_read((uint8_t*)rx, 256);
  for (int i = 0; i < 256; i++) {
    tx[i] = rx[i];
  }
  int j = 0;
  for (int i = addres; i < size; i++) {
    tx[i] = data[j];
    j++;
  }
  S25FL256_eraseSectorFromBlock(0);
  while (S25FL256_isWriteInProgress()) {
    HAL_Delay(51);
  }
  S25FL256_open(0);
  S25FL256_write((uint8_t*)tx, 256);
  while (S25FL256_isWriteInProgress()) {
    HAL_Delay(51);
  }
}
void readInFlash(uint8_t addres, uint8_t* data, uint8_t size) {
  static uint8_t rx[256];
  S25FL256_open(0);
  S25FL256_read((uint8_t*)rx, 256);
  int j = 0;
  for (int i = addres; i < size; i++) {
    data[j] = rx[i];
    j++;
  }
}
/*
uint8_t SPG30_Initialize(void) {
  struct OCTA_header SPG30_Header = platform_getHeader(SPG30_CONNECTOR);
  if (!SPG30_Header.active) {
    printf("Invalid SPG30_CONNECTOR provided in Makefile\r\n");
    return 0;
  } else
    printf("SPG30 sensor on P%d, initializing I2C\r\n", SPG30_CONNECTOR);
  while (sgp_probe() != STATUS_OK) {
    printf("SGP sensor probing failed ... check SGP30 I2C connection and power\r\n");
    HAL_Delay(500);
  }

  printf("SGP sensor probing successful\r\n");
  err = sgp_measure_signals_blocking_read(&scaled_ethanol_signal, &scaled_h2_signal);
  if (err == STATUS_OK) {
    printf("Ethanol signal: %.2f \r\n", scaled_ethanol_signal / 512.0);
    printf("H2 signal: %.2f \r\n", scaled_h2_signal / 512.0);
  } else {
    printf("error reading Ethanol and H2 signals\r\n");
  }
  err = sgp_iaq_init();

  return 1;
}
*/
float cotlevels = 0;

void SPG30_measure(void) {

  if (enable_SGP) {
    err = sgp_measure_iaq_blocking_read(&tvoc_ppb, &co2_eq_ppm);
    if (err == STATUS_OK) {
      measurement_counter++;
      // printf("tVOC Concentration: %5d [ppb]\t CO2eq Concentration: %5d [ppm]\t measurement no.: %10lu \r\n", tvoc_ppb, co2_eq_ppm, measurement_counter);
      cotlevels = co2_eq_ppm;
    } else {
      printf("error reading IAQ values\r\n");
    }
    if (measurement_counter % 600 == 0) {
      err = sgp_get_iaq_baseline(&iaq_baseline);
      if (err == STATUS_OK) {
        printf("actual baseline values: %5lu\r\n", iaq_baseline);
      }
    }
    if (++i % 3600 == 3599) {
      err = sgp_get_iaq_baseline(&iaq_baseline);
      if (err == STATUS_OK) {
        printf("Set baseline values to: %5lu at next startup.\r\n", iaq_baseline);
      }
    }
  }
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

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
}
void printOCTAID() {
  char UIDString[sizeof(short_UID)];
  memcpy(UIDString, &short_UID, sizeof(short_UID));
  printf("octa ID: ");
  for (const char* p = UIDString; *p; ++p) {
    printf("%02x", *p);
  }
  printf("\r\n");
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


void decodeError(int status) {
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

