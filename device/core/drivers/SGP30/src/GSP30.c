#include "GSP30.h"


void GSP30_init() {
//  GSP30_softReset();
  uint16_t addres[1];
  uint8_t data[2];
  for(int i = 0;i<sizeof(data);i++){
    data[i]=0;
  }
  addres[0] = 0x2032;
  printf("READ ID\n\r");
  (addres, data, SGP30_CMD_MEASURE_TEST_DURATION_US);
  HAL_Delay(100);
}
void GSP30_measure() {
  uint8_t  data[9];
  data[0]=0x20;
  data[1]=0x08;
  uint32_t status;
  status = HAL_I2C_Master_Transmit(GSP30_hi2c, 0x58<<1, data,2, 50);
  decodeError1(status);
  HAL_Delay(50);
  data[0]=0x20;
  data[1]=0x08;
  status = HAL_I2C_Master_Receive(GSP30_hi2c,0x58<<1, data,sizeof(data), 500);
  decodeError1(status);


  printf("\t Data Read Out: 0x%x 0x%x 0x%x 0x%x  0x%x  0x%x\n\r", data[0], data[1], data[2], data[3], data[4], data[5]);
  printf("\t CO2: %d\n\r", data[1] * 256 + data[0]);
  printf("\t Air Quality: %d\n\r", data[3] * 256 + data[2]);
}

void GSP30_readData(int32_t *pData) {
}
void GSP30_reset() {
}


void decodeError1(status) {
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
  switch (GSP30_hi2c->ErrorCode) {
    case 0:
      printf("HAL_I2C_ERROR_NONE   \n\r");
      break;
    case 0x1:
      printf("HAL_I2C_ERROR_BERR  \n\r");
      break;
    case 0x2:
      printf("HAL_I2C_ERROR_ARLO \n\r");
      break;
    case 0x4:
      printf("HAL_I2C_ERROR_AF  \n\r");
      break;
    case 0x8:
      printf("HAL_I2C_ERROR_OVR\n\r");
      break;
    case 0x10:
      printf("HAL_I2C_ERROR_DMA\n\r");
      break;
    case 0x20:
      printf("HAL_I2C_ERROR_TIMEOUT\n\r");
      break;
    case 0x40:
      printf("HAL_I2C_ERROR_SIZE  \n\r");
      break;
    case 0x80:
      printf("HAL_I2C_ERROR_DMA_PARAM\n\r");
      break;
    case 0x100:
      printf("HAL_I2C_ERROR_INVALID_CALLBACK\n\r");
      break;
    case 0x200:
      printf("HAL_I2C_ERROR_INVALID_PARAM\n\r");
      break;
    default:
      printf("Error Code is: %x\n\r", GSP30_hi2c->ErrorCode);
      GSP30_hi2c->ErrorCode=0;
  }
}

void GSP30_setI2CInterface(I2C_HandleTypeDef *hi2c) {
  GSP30_hi2c = hi2c;
}
