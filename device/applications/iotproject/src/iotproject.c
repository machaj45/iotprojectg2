#include "iotproject.h"

//#include "ble.h"

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

  HAL_Delay(1000);

  uint8_t bg[4];
  bg[0]=0x04;
  bg[1]=0x00;
  bg[2]=0x00;
  bg[3]=0x60;
  printf("Init of Ble \n\r");
   
  int status,status2;
  status = HAL_UART_Transmit(&BLE_UART, bg, sizeof(bg), HAL_MAX_DELAY);
  decodeError1(status);

  HAL_Delay(1000);

  uint8_t buffer[6];
  buffer[0]=0x04;
  buffer[1]=0x00;
  buffer[2]=0x00;
  buffer[3]=0x78;
  buffer[4]=0x41;
  buffer[5]=0x03;
   
  //decodeError1(status);
  //printf("after transmit start receaving\n\r");
  uint8_t data[8];
  data[0]=0x06;
  data[1]=0x00;
  data[2]=0x01;
  data[3]=0x78;
  data[4]=0x00;
  data[5]=0x00;
  data[6]=0x00;
  data[7]=0x00;
  //printf("after receaving data are %x %x %x %x \n\r",data[2],data[3],data[4],data[5]);
  status = HAL_UART_Transmit(&BLE_UART, buffer, sizeof(buffer), HAL_MAX_DELAY);
  status2 = HAL_UART_Receive(&BLE_UART, data, sizeof(data), 0xFFF);
  decodeError1(status);
  decodeError1(status2);
  printf("after receaving data are %x %x %x %x \n\r",data[2],data[3],data[4],data[5]);

  // osKernelStart();
  while (1) {
    printf("ERROR Kernel Meltdown!\n\r");
    HAL_Delay(10000);
  }
}

