#include "iotproject.h"
#include "inte.h"

// is somewere defined
// void EXTI15_10_IRQHandler(void) {
// HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
//}

void EXTI9_5_IRQHandler(void) {
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
}

void EXTI0_IRQHandler(void) {
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void EXTI1_IRQHandler(void) {
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

void HAL_GPIO_EXTI_Callback(uint16_t gpioPinNumber) {
  if (GPIO_PIN_5 == gpioPinNumber) {
    if (acc_int != 2)
      acc_int = 1;
  }
  if (GPIO_PIN_13 == gpioPinNumber) {
    button  = 3;
  }
  if (GPIO_PIN_0 == gpioPinNumber) {
    acc_int = 2;
    button  = 1;
  }
  if (GPIO_PIN_1 == gpioPinNumber) {
    button = 2;
  }
}
