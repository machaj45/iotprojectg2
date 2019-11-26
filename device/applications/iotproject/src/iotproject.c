#include "iotproject.h"
#include "mytask.h"
#include "send.h"
#include "other.h"

  uint8_t tx[SIZE];
  uint8_t rx[SIZE];
  uint8_t data[1];
int main(void) {
  HAL_Init();
  SystemClock_Config();
  Initialize_Platform();
  Initialize_OS();
  Initialize_Sensors();
  LorawanInit();
  S25FL256_Initialize(&FLASH_SPI);


  // fill tx array
  for (int n = 0; n < SIZE; n++) {
    tx[n] = n;
  }

  S25FL256_open(BLOCK_ID);

  //S25FL256_write((uint8_t *)tx, SIZE);

  //while (S25FL256_isWriteInProgress()) {
   // HAL_Delay(50);
 // }

  S25FL256_read((uint8_t *)rx, SIZE);
  printf("%d %d %d %d\n\r",rx[0],rx[1],rx[2],rx[3]);

  osKernelStart();
  while (1) {
    printf("ERROR Kernel Meltdown!\n\r");
    HAL_Delay(10000);
  }
}

