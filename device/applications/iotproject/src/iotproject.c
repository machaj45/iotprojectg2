#include "iotproject.h"
int main(void) {
  HAL_Init();
  SystemClock_Config();
  Initialize_Platform();
  Initialize_OS();
  Initialize_Sensors();
  IWDG_feed(NULL);
  GSP30_setI2CInterface(&P3_I2C);
  HAL_Delay(1000);
  GSP30_init();
  HAL_Delay(1000);
  LorawanInit();
  osKernelStart();
  while (1) {
    printf("ERROR Kernel Meltdown!\n\r");
  }
}

