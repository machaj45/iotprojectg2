/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
RTC_HandleTypeDef hrtc;


/* Private variables ---------------------------------------------------------*/
osThreadId defaultTaskHandle;
osThreadId murata_rx_processing_handle;
/* USER CODE BEGIN PV */
osTimerId iwdgTimId;
osTimerId loraWANTimId;
osTimerId dash7TimId;
osTimerId moduleCheckTimId;

osMutexId txMutexId;
osMutexId murata_rx_process_mutex_id;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void vApplicationIdleHook();

/* USER CODE BEGIN PFP */
void printWelcome(void);
void IWDG_feed(void const *argument);
void LoRaWAN_send(void const *argument);
void Dash7_send(void const *argument);
void check_modules(void const *argument);
void murata_process_rx_response(void const *argument);

/* USER CODE END PFP */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/