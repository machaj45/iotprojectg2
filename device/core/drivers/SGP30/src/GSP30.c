#include "GSP30.h"
void GSP30_setI2CInterface(I2C_HandleTypeDef *hi2c) {
  GSP30_hi2c = hi2c;
}


void GSP30_init() {
  uint16_t addres[1];
  uint16_t data[1];
  addres[0] = SGP30_CMD_IAQ_INIT;
  data[0] = SGP30_CMD_IAQ_INIT;
  printf("GSP30 INIT Command: %x\n\r", addres[0]);
  GSP30_writeRegister(addres, data,SGP30_CMD_IAQ_INIT_DURATION_US);
  HAL_Delay(100);
}


void GSP30_measure() {
  uint16_t addres[1];
  uint8_t data[4];
  data[0]=0;
  data[1]=0;
  data[2]=0;
  data[3]=0;
  addres[0] = SGP30_CMD_IAQ_MEASURE;
  data[0] = 0x20;
  data[1] = 0x08;
  printf("Measure Command: %x\n\r", addres[0]);
  //GSP30_writeRegister(addres, data,SGP30_CMD_IAQ_MEASURE_DURATION_US/1000);
  //HAL_Delay(SGP30_CMD_IAQ_MEASURE_DURATION_US/1000);
  GSP30_readRegister(addres, data,SGP30_CMD_IAQ_MEASURE_DURATION_US);
  printf("\t Data Read Out: %x %x\n\r", data[0], data[1]);
  printf("\t CO2: %d\n\r",data[1]*256+data[0]);
  printf("\t Air Quality: %d\n\r",data[3]*256+data[2]);
}

void GSP30_readData(int32_t *pData) {
}
void GSP30_reset() {
}

HAL_StatusTypeDef GSP30_readRegister(uint16_t GSP30_reg, uint8_t GSP30_data,uint32_t Timeout) {

  HAL_I2C_Mem_Read(GSP30_hi2c, GSP30_I2C_ADDRESS, GSP30_reg, I2C_MEMADD_SIZE_16BIT, &GSP30_data, sizeof(GSP30_data), Timeout/1000);

  return HAL_OK;
}

HAL_StatusTypeDef GSP30_writeRegister(uint16_t GSP30_reg, uint8_t GSP30_data,uint32_t Timeout) {
  uint32_t a ;
  a = HAL_I2C_Mem_Write(GSP30_hi2c, GSP30_I2C_ADDRESS, GSP30_reg, I2C_MEMADD_SIZE_16BIT, &GSP30_data, sizeof(GSP30_data), Timeout/1000);
  printf("%d",a);
  return HAL_OK;
}
