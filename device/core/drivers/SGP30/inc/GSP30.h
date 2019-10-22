/*
 * GSP30AccSensor.h
 *
 *  Created on: 22 oct 2019
 */
#include "stm32l4xx_hal.h"
#ifndef GSP30ACCSENSOR_H_
#define GSP30ACCSENSOR_H_

#define GSP30_I2C_ADDRESS 0x58

#define GSP30_ID 0x3682

#define sgp30_iaq_init 0x2003
#define sgp30_measure_iaq 0x2008
#define sgp30_get_iaq_baseline 0x2015
#define sgp30_set_iaq_baseline 0x201e
#define sgp30_set_absolute_humidity 0x2061
#define sgp30_measure_test 10 0x2032
#define sgp30_get_feature_set 0x202f
#define sgp30_measure_raw 0x2050
#define sgp30_get_tvoc_inceptive_baseline 0x20b3
#define sgp30_set_tvoc_baseline 0x2077

uint8_t            GSP30_setting;
I2C_HandleTypeDef *GSP30_hi2c;

void              GSP30_setI2CInterface(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef GSP30_readRegister(uint16_t GSP30_reg, uint8_t GSP30_data);
HAL_StatusTypeDef GSP30_writeRegister(uint16_t GSP30_reg, uint8_t GSP30_data);
void              GSP30_init();
void              GSP30_reset();
void              GSP30_readData();
void              GSP30_measure();
#endif
