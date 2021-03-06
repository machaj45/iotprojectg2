#include "octa.h"

void OCTA_NVIC_Initialize() {

  /* EXTI5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(23, 2, 0);
  HAL_NVIC_EnableIRQ(23);  // 23 is interrupt line 5 - 9

  HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 0x0, 0);
  HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);


  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);  // 6 is interrupt line 0

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);  // 7 is interrupt line 1

}
void OCTA_Initialize_Platform(void) {
  // GPIO, IWDG, USB_UART(Logging)
  OCTA_Initialize_Common_Peripherals();
  // P1 UART, I2C, SPI & GPIO handles
  OCTA_Initialize_P1_Peripherals();
  // P2 UART, I2C, SPI & GPIO handles
  OCTA_Initialize_P2_Peripherals();
  // P3 UART, I2C, SPI & GPIO handles
  OCTA_Initialize_P3_Peripherals();
  // Interupts

  OCTA_NVIC_Initialize();
}

void OCTA_Initialize_Common_Peripherals(void) {
  OCTA_GPIO_Init();
  //    OCTA_IWDG_Init();
  FLASH_SPI_Init();
  common_I2C_Init();
  USB_UART_Init(115200);
  BLE_UART_Init(115200);
  //bootloader_initialize(&BLE_UART);
}

void OCTA_Initialize_P1_Peripherals(void) {
  P1_header.number     = 1;
  P1_header.uartHandle = &P1_UART;
  P1_header.i2cHandle  = &P1_I2C;
  P1_header.spiHandle  = &P1_SPI;
  P1_header.DIO1       = &P1_DIO1;
  P1_header.DIO2       = &P1_DIO2;
  P1_header.DIO3       = &P1_DIO3;
  P1_header.DIO4       = &P1_DIO4;
  P1_header.DIO5       = &P1_DIO5;
  P1_header.DIO6       = &P1_DIO6;
}

void OCTA_Initialize_P2_Peripherals(void) {
  P2_header.number = 2;
  //printf("\r\nusing bootloader, P2 UART unavailable\r\n");
  P2_header.uartHandle = NULL;
//  P2_header.uartHandle = &P2_UART;
  P2_header.i2cHandle = &P2_I2C;
  P2_header.spiHandle = &P2_SPI;
  P2_header.DIO1      = &P2_DIO1;
  P2_header.DIO2      = &P2_DIO2;
  P2_header.DIO3      = &P2_DIO3;
  P2_header.DIO4      = &P2_DIO4;
  P2_header.DIO5      = &P2_DIO5;
  P2_header.DIO6      = &P2_DIO6;
}

void OCTA_Initialize_P3_Peripherals(void) {
  P3_header.number     = 3;
  P3_header.uartHandle = &P3_UART;
  P3_header.i2cHandle  = &P3_I2C;
  P3_header.spiHandle  = &P3_SPI;
  P3_header.DIO1       = &P3_DIO1;
  P3_header.DIO2       = &P3_DIO2;
  P3_header.DIO3       = &P3_DIO3;
  P3_header.DIO4       = &P3_DIO4;
  P3_header.DIO5       = &P3_DIO5;
  P3_header.DIO6       = &P3_DIO6;
}

uint64_t get_UID(void) {
  return (*((uint64_t *)(UID_BASE)) << 32) + *((uint64_t *)(UID_BASE + 0x04U));
}

void platform_initialize_UART(struct OCTA_header aHeader, uint32_t aBaudRate) {
  switch (aHeader.number) {
    case 1:
      P1_UART_Init(aBaudRate);
      break;
    case 2:
      P2_UART_Init(aBaudRate);
      break;
    case 3:
      P3_UART_Init(aBaudRate);
      break;
    default:
      break;
  }
}

void platform_initialize_I2C(struct OCTA_header aHeader) {
  switch (aHeader.number) {
    case 1:
      P1_I2C_Init();
      break;
    case 2:
      P2_I2C_Init();
      break;
    case 3:
      P3_I2C_Init();
      break;
    default:
      break;
  }
}

void platform_initialize_SPI(struct OCTA_header aHeader) {
  switch (aHeader.number) {
    case 1:
      P1_SPI_Init();
      break;
    case 2:
      P2_SPI_Init();
      break;
    case 3:
      P3_SPI_Init();
      break;
    default:
      break;
  }
}

struct OCTA_header platform_getHeader(uint8_t connector)
{
  switch (connector) {
    case 1:
      P1_header.active = 1;
      return P1_header;
    case 2:
      P2_header.active = 1;
      return P2_header;
    case 3:
      P3_header.active = 1;
      return P3_header;
    default:
      // invalid connector
      printf("Invalid connector\r\n");
      struct OCTA_header invalid;
      invalid.active = 0;
      return invalid;
  }
}
