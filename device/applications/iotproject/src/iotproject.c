#include "iotproject.h"


void decodeError1(int status) {
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

int main(void) {
  HAL_Init();
  SystemClock_Config();
  Initialize_Platform();
  Initialize_OS();
  Initialize_Sensors();
  IWDG_feed(NULL);
  HAL_Delay(1000);
  // GSP30_init();
  HAL_Delay(1000);
  LorawanInit();
/*

  uint8_t bg[5];
  bg[0]=0x00;
  bg[1]=0x60;
  bg[2]=0x00;
  bg[3]=0x00;
  bg[4]=0x00;
  printf("Init of Ble \n\r");
   
  int status,status2;
  status = HAL_UART_Transmit(&BLE_UART, bg, sizeof(bg), HAL_MAX_DELAY);
  decodeError1(status);
  HAL_Delay(1000);

  uint8_t buffer[4];
  buffer[0]=0x00;
  buffer[1]=0x78;
  buffer[2]=0x41;
  buffer[3]=0x03;
   
  //decodeError1(status);
  //printf("after transmit start receaving\n\r");
  uint8_t data[6];
  data[0]=0x01;
  data[1]=0x78;
  data[2]=0x00;
  data[3]=0x00;
  data[4]=0x00;
  data[5]=0x00;
  //printf("after receaving data are %x %x %x %x \n\r",data[2],data[3],data[4],data[5]);
  status = HAL_UART_Transmit(&BLE_UART, buffer, sizeof(buffer), HAL_MAX_DELAY);
  status2 = HAL_UART_Receive(&BLE_UART, data, sizeof(data), 0xFFF);
  decodeError1(status);
  decodeError1(status2);
  
  printf("after receaving data are %x %x %x %x \n\r",data[2],data[3],data[4],data[5]);
  uint8_t da[3];
  da[0]=0x00;
  da[1]=0x52;
  da[2]=0x00;
  uint8_t doo[9];
  doo[0]=0x01;
  status = HAL_UART_Transmit(&BLE_UART, da, sizeof(da), HAL_MAX_DELAY);
  status2 = HAL_UART_Receive(&BLE_UART, doo, sizeof(doo), 0xFFF);
  decodeError1(status);
  decodeError1(status2);

  printf("after receaving data are %x %x %x %x \n\r",doo[5],doo[6],doo[7],doo[8]);
*/
  osKernelStart();
  while (1) {
    printf("ERROR Kernel Meltdown!\n\r");
    HAL_Delay(10000);
  }
}

