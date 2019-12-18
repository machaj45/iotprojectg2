#include "no-scheduler-example.h"
#include "other.h"
#include "sgp30.h"
volatile uint8_t acc_int = 0;


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
uint8_t uint162byte(uint16_t input, uint8_t* output, uint8_t offset) {
  uint16_t* floatPtr    = &input;
  uint8_t*  dataPointer = (uint8_t*)floatPtr;
  output[0 + offset]    = *dataPointer;
  output[1 + offset]    = *(dataPointer + 1);
}

uint16_t byte2uint16(uint8_t* input, uint8_t offset) {
  uint16_t output;
  output = input[0+offset] + input[1+offset] * 256;
  return output;
}

uint32_t byte2uint32(uint8_t* input, uint8_t offset) {
  uint32_t output;
  output = input[0+offset] + input[1+offset] * 256 + input[2+offset] * 256 * 256 + input[3+offset] * 256 * 256 * 256;
  return output;
}


void setUpDefaultValuesforTresholds() {
  uint8_t data[16];
  data[1]  = 0x00;
  data[0]  = 0x12;
  data[3]  = 0x00;
  data[2]  = 0x1E;
  data[4]  = 0x00;
  data[5]  = 0x00;
  data[7]  = 0x00;
  data[6]  = 0x38;
  data[8]  = 0x00;
  data[9]  = 0x00;
  data[11] = 0x02;
  data[10] = 0x58;
  data[12] = 0x00;
  data[13] = 0x00;
  data[15] = 0x00;
  data[14] = 0x32;
  writeInFlash(0, data, sizeof(data));
  while (S25FL256_isWriteInProgress()) {
    HAL_Delay(51);
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
  for (int i = addres; i < size+addres; i++) {
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
  for (int i = addres; i < size+addres; i++) {
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

