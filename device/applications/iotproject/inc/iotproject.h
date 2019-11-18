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
#include "sensirion_common.h"
#include "sgp30.h"
// Me
#define IWDG_INTERVAL 50           // seconds
#define LORAWAN_INTERVAL 30        // seconds
#define MODULE_CHECK_INTERVAL 5000  // seconds
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

extern volatile uint8_t acc_int;
extern volatile uint8_t button;
extern float            SHTData[2];
extern uint8_t          lora_init;
extern uint64_t         short_UID;
// Globals


#ifndef MURATA_CONNECTOR
#define MURATA_CONNECTOR 1 //THIS IS BACKUP IT IS SET UP IN MAKEFILE!!!
#endif

#ifndef SPG30_CONNECTOR
#define SPG30_CONNECTOR 2 //THIS IS BACKUP IT IS SET UP IN MAKEFILE!!!
#endif

#ifdef __cplusplus
}
#endif
#endif
