#include "GSP30.h"
void GSP30_setI2CInterface(I2C_HandleTypeDef *hi2c) {
  GSP30_hi2c = hi2c;
}

HAL_StatusTypeDef GSP30_readRegister(uint16_t GSP30_reg, uint8_t GSP30_data) {

  HAL_I2C_Mem_Read(GSP30_hi2c, GSP30_I2C_ADDRESS, GSP30_reg, I2C_MEMADD_SIZE_16BIT, &GSP30_data, sizeof(GSP30_data), HAL_MAX_DELAY);

  return HAL_OK;
}

HAL_StatusTypeDef GSP30_writeRegister(uint16_t GSP30_reg, uint8_t GSP30_data) {

  HAL_I2C_Mem_Write(GSP30_hi2c, GSP30_I2C_ADDRESS, GSP30_reg, I2C_MEMADD_SIZE_16BIT, &GSP30_data, sizeof(GSP30_data), HAL_MAX_DELAY);

  return HAL_OK;
}

void GSP30_init() {

  uint16_t addres[1];
  uint8_t data[6];
  addres[0] = 0x2003;
  printf("Init Command: %x\n\r", addres[0]);
  GSP30_writeRegister(addres, data);

  HAL_Delay(1000);
  addres[0] = 0x3682;
  printf("Read ID\n\r");
  GSP30_writeRegister(addres, data);
  printf("answer with write: %x %x %x %x %x %x\n\r", data[0], data[1],data[2],data[3],data[4],data[5]);
  GSP30_readRegister(addres, data);
  printf("answer with read: %x %x %x %x %x %x\n\r", data[0], data[1],data[2],data[3],data[4],data[5]);
  HAL_Delay(100);
}

/**
 * Resets the GSP30 accelerometer
 */
void GSP30_reset() {
  // GSP30_setting = GSP30_ACC_BOOT;
  // GSP30_writeRegister(GSP30_ACC_CTRL_REG5, GSP30_setting, 0);
}

void GSP30_measure() {
  uint16_t addres[1];
  uint8_t data[6];
  uint8_t data1[1];
  addres[0] = 0x2008;
  GSP30_writeRegister(addres, data1);
  HAL_Delay(12);
  GSP30_readRegister(addres, data);
  printf("Data Read Out: %x %x %x %x %x %x\n\r", data[0], data[1],data[2],data[3],data[4],data[5]);
  addres[0]=data[1]*256+data[0];
  printf("Air Quality: %d\n\r",addres[0]);
}

void GSP30_readData(int32_t *pData) {

  // HAL_I2C_Mem_Read(GSP30_hi2c, GSP30_ACC_I2C_ADDRESS,	GSP30_ACC_MULTI_READ, I2C_MEMADD_SIZE_8BIT, accRegisterData,sizeof(accRegisterData),
  // HAL_MAX_DELAY);

  // accRawData[0] = (accRegisterData[1] << 8)	| accRegisterData[0];
  // accRawData[1] = (accRegisterData[3] << 8)| accRegisterData[2];
  // accRawData[2] = (accRegisterData[5] << 8)| accRegisterData[4];

  /* Apply proper shift and sensitivity */
  // Normal mode 10-bit, shift = 6 and FS = 2
  // pData[0] = (int32_t) (((accRawData[0] >> 6) * 3900 + 500) / 1000);
  // pData[1] = (int32_t) (((accRawData[1] >> 6) * 3900 + 500) / 1000);
  // pData[2] = (int32_t) (((accRawData[2] >> 6) * 3900 + 500) / 1000);
}

