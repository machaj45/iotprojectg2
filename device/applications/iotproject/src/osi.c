#include "iotproject.h"
#include "osi.h"

void Initialize_OS(void) {

  osMutexDef(txMutex);
  txMutexId = osMutexCreate(osMutex(txMutex));

  osThreadDef(murata_rx_processing, murata_process_rx_response, osPriorityNormal, 0, 512);
  murata_rx_processing_handle = osThreadCreate(osThread(murata_rx_processing), NULL);
  Murata_SetProcessingThread(murata_rx_processing_handle);

  osTimerDef(moduleCheckTim, check_modules);
  moduleCheckTimId = osTimerCreate(osTimer(moduleCheckTim), osTimerPeriodic, NULL);
  osTimerStart(moduleCheckTimId, MODULE_CHECK_INTERVAL * 10000);


  osThreadDef(defaultTask, StartDefaultTask, osPriorityLow, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);
}
