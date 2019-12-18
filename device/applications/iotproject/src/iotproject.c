#include "iotproject.h"
#include "mytask.h"
#include "send.h"
#include "other.h"


uint8_t           data[1];
RTC_HandleTypeDef hrtc;

int main(void) {

  HAL_Init();
  SystemClock_Config();
  Initialize_Platform();
  Initialize_OS();
  Initialize_Sensors();
  LorawanInit();
  HAL_Delay(100);//MANDATORY!!!!
  S25FL256_Initialize(&FLASH_SPI);
  osKernelStart();
  while (1) {
    printf("ERROR Kernel Meltdown!\n\r");
    HAL_Delay(10000);
  }
}

