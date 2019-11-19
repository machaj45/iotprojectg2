#include "iotproject.h"
#include "mytask.h"
#include "send.h"
#include "other.h"

int main(void) {
  HAL_Init();
  SystemClock_Config();
  Initialize_Platform();
  Initialize_OS();
  Initialize_Sensors();
  LorawanInit();
  osKernelStart();
  while (1) {
    printf("ERROR Kernel Meltdown!\n\r");
    HAL_Delay(10000);
  }
}

