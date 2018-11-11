/***********************************************************
	Module that handles the interface with the temperature and
	humidity sensor 'Si7021-A20'
***********************************************************/
#include "stm32f7xx_hal.h"

#ifndef HUM_TEM_SENS_H
#define HUM_TEM_SENS_H

void init_RH_Temp_sens(I2C_HandleTypeDef* hi2c);		// Initializes the sensor

float get_temperature(void); 												 		// Converts and returns temperature mesured by the sensor
float get_humidity(void);														 		// Converts and returns the relative humidity mesured by de sensor 

#endif
