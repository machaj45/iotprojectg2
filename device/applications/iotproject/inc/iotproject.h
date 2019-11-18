#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif
// System
#include <string.h>
#include <stdio.h>
// From school
#include "platform.h"
#include "murata.h"
#include "LSM303AGRSensor.h"
#include "sht31.h"
// Me
#define IWDG_INTERVAL 50           // seconds
#define LORAWAN_INTERVAL 30        // seconds
#define MODULE_CHECK_INTERVAL 500  // seconds
// OS
osThreadId defaultTaskHandle;
osThreadId GSPmeasuerTaskHandle;
osThreadId murata_rx_processing_handle;
osTimerId  loraWANTimId;
osTimerId  moduleCheckTimId;
osTimerId  GSPTimId;
osMutexId  i2cMutexId;
osMutexId  txMutexId;
osMutexId  murata_rx_process_mutex_id;
osTimerId  iwdgTimId;

volatile uint8_t acc_int;
float            SHTData[2];
uint8_t          lora_init = 0;
// Globals
#ifdef __cplusplus
}
#endif
#endif
